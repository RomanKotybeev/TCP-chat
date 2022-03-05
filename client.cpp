#include <unistd.h>
#include <string.h>

#include "clientserver.hpp"

char entered_chat[] = " has entered the chat";


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
			buf_used = buf_used - i - 1;
			memmove(buf, buf+i+1, buf_used);

			char *msg = new char[name_len + sizeof(entered_chat) + 2];
			sprintf(msg, "%s%s\n", name, entered_chat);
			serv_master->SendAll(msg, this);
			delete[] msg;
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
	for (int i = 0; i < buf_used; i++)
		buf[i] = 0;
	buf_used = 0;
	delete[] msg;
}

Client::~Client()
{
	if (name != 0)
		delete[] name;
}
