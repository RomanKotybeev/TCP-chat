#include <unistd.h>
#include <string.h>

#include "clientserver.hpp"

char entered_chat[] = " has entered the chat";
char leaved_chat[] = " has leaved the chat";
char conn_again[] = " is used, try another name";
char info_chat[] = "Online: ";

char QUITLONG_CMD[] = "\\quit";
char QUITSHORT_CMD[] = "\\q";
char INFOCHAT_CMD[] = "\\info";


void Client::Handle()
{
	int fd = GetFd();
	int n = read(fd, buf, sizeof(buf));
	if (n < 0) {
		fprintf(serv_master->GetLog(), "[%s] couldn't read data\n",
				AddrToStr());
		return ;
	}
	if (n == 0) {
		serv_master->CloseClientSession(this);
		return ;
	}
	buf_used += n;

	if (name == 0) {
		CheckLine();
		ProcessName();
	}
	else if (buf[0] == '\\') {
		CheckLine();
		ProcessCmd();
	}
	else {
		ProcessMsg();
	}
	CleanBuffer();
}

void Client::CheckLine()
{
	for (int i = 0; i < buf_used; i++) {
		if (buf[i] == '\n') {
			if (i > 0 && buf[i-1] == '\r') {
				buf[i] = 0;
				i--;
			}
			buf[i] = 0;
			buf_used = i;
			return;
		}
	}
}

void Client::ProcessName()
{
	name = new char[buf_used];
	name_len = buf_used;
	memcpy(name, buf, buf_used);
	bool unique = serv_master->InsertNameAndCheck(name);
	if (!unique) {
		char *msg = new char[sizeof(info_chat) + name_len + 1];
		sprintf(msg, "%s%s\n", name, conn_again);
		serv_master->Send(this, msg);
		delete[] msg;
		delete[] name;
		name = 0;
		return;
	}
	SendInfoToChat(entered_chat);
}

void Client::ProcessMsg()
{
	int rest_chars = 5;
	char *msg = new char[buf_used + name_len + rest_chars];
	sprintf(msg, "[%s] %s", name, buf);
	serv_master->SendAll(msg);
	delete[] msg;
}

void Client::ProcessCmd()
{
	if (strcmp(buf, QUITSHORT_CMD) == 0 || strcmp(buf, QUITLONG_CMD) == 0) {
		SendInfoToChat(leaved_chat);
		serv_master->CloseClientSession(this);
	} else if (strcmp(buf, INFOCHAT_CMD) == 0) {
		int n_digits = 3;
		char *msg = new char[sizeof(info_chat) + n_digits + 2];
		sprintf(msg, "%s%d\n", info_chat, serv_master->GetNumberClinets());
		serv_master->Send(this, msg);
		delete[] msg;
	}
}

void Client::SendInfoToChat(char *str)
{
	char *msg = new char[name_len + sizeof(str) + 2];
	sprintf(msg, "%s%s\n", name, str);
	serv_master->SendAll(msg, this);
	delete[] msg;
}

void Client::CleanBuffer()
{
	if (!name)
		serv_master->CloseClientSession(this);
	for (int i = 0; i < buf_used; i++)
		buf[i] = 0;
	buf_used = 0;
}

Client::~Client()
{
	if (name)
		delete[] name;
}
