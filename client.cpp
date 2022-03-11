#include <unistd.h>
#include <string.h>

#include "clientserver.hpp"

char entered_chat[] = " has entered the chat";
char leaved_chat[] = " has leaved the chat";

char QUITLONG_CMD[] = "\\quit";
char QUITSHORT_CMD[] = "\\q";


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

	if (name == 0)
		ProcessName();
	else if (buf[0] == '\\')
		ProcessCmd();
	else
		ProcessMsg();
}

void Client::ProcessName()
{
	for (int i = 0; i < buf_used; i++) {
		if (buf[i] == '\n') {
			if (i > 0 && buf[i-1] == '\r')
				buf[i - 1] = 0;
			name = new char[i];
			memcpy(name, buf, i);
			name[i] = 0;
			name_len = i;

			CleanBuffer();

			SendInfoToChat(entered_chat);
			return ;
		}
	}
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
	if (strcmp(buf, QUITSHORT_CMD) || strcmp(buf, QUITLONG_CMD)) {
		SendInfoToChat(leaved_chat);
		serv_master->CloseClientSession(this);
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
	for (int i = 0; i < buf_used; i++)
		buf[i] = 0;
	buf_used = 0;
}

Client::~Client()
{
	if (name != 0)
		delete[] name;
}
