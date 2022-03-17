#include <unistd.h>
#include <string.h>

#include "clientserver.hpp"

char entered_chat[] = " has entered the chat";
char leaved_chat[] = " has leaved the chat";
char info_chat[] = "Online: ";
char name_is_not_unique_msg[] = " is used, try another name";
char read_err_msg[] = " couldn't read data\n";

char QUITLONG_CMD[] = "\\quit";
char QUITSHORT_CMD[] = "\\q";
char INFOCHAT_CMD[] = "\\info";


void Client::Handle()
{
	int n = read(GetFd(), buf, BUF_LEN);
	if (n < 0) {
		int len_char_addr = 15;
		char *msg = new char[sizeof(read_err_msg) + len_char_addr + 4];
		sprintf(msg, "[%s]%s", AddrToStr(), read_err_msg);
		serv_master->WriteLog(msg);
		delete[] msg;

		serv_master->CloseClientSession(this);
		return;
	}
	buf_used = n;
	CheckLine();

	if (name == 0)
		ProcessName();
	else if (buf[0] == '\\')
		ProcessCmd();
	else
		ProcessMsg();
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
	name_len = buf_used;
	name = new char[name_len + 1];
	memcpy(name, buf, name_len);
	name[name_len] = '\0';
	bool unique = serv_master->IsNameUnqiue(name);
	if (!unique) {
		char *msg = new char[sizeof(info_chat) + name_len + 1];
		sprintf(msg, "%s%s\n", name, name_is_not_unique_msg);
		serv_master->Send(this, msg);
		delete[] msg;
		serv_master->CloseClientSession(this);
		return;
	}
	SendInfoToChat(entered_chat);
}

void Client::ProcessMsg()
{
	int rest_chars = 5;
	char *msg = new char[buf_used + name_len + rest_chars];
	sprintf(msg, "[%s] %s\n", name, buf);
	serv_master->SendAll(msg);
	delete[] msg;
}

void Client::ProcessCmd()
{
	if (strcmp(buf + buf_begin, QUITSHORT_CMD) == 0 ||
		strcmp(buf + buf_begin, QUITLONG_CMD) == 0
	) {
		SendInfoToChat(leaved_chat);
		serv_master->CloseClientSession(this);
	} else if (strcmp(buf + buf_begin, INFOCHAT_CMD) == 0) {
		int n_digits = 3;
		char *msg = new char[sizeof(info_chat) + n_digits + 2];
		sprintf(msg, "%s%d\n", info_chat, serv_master->GetNClinets());
		serv_master->Send(this, msg);
		delete[] msg;
	}
}

void Client::SendInfoToChat(const char *str)
{
	char *msg = new char[name_len + sizeof(str) + 2];
	sprintf(msg, "%s%s\n", name, str);
	serv_master->SendAll(msg, this);
	delete[] msg;
}

Client::~Client()
{
	if (name) {
		delete[] name;
		name = 0;
	}
}
