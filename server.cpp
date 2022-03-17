#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "clientserver.hpp"

char ask_name_msg[] = "Enter your name: ";


Server *Server::Start(SessionSelector *sel, int port, FILE *f)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return nullptr;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (-1 == bind(sockfd, (sockaddr *)&addr, sizeof(addr)))
		return nullptr;

	if (-1 == listen(sockfd, QLEN_LISTEN))
		return nullptr;

	return new Server(sel, f, sockfd, addr);
}

Server::Server(SessionSelector *sel, FILE *f,
               int sockfd, struct sockaddr_in& addr)
	: FdObj(sockfd, addr)
	, selector(sel)
	, n_clients(0)
	, log(f)
{ 
	selector->Add(this);
}

void Server::Handle()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int sockfd = accept(GetFd(), (sockaddr *)&addr, &len);
	if (sockfd == -1) {
		WriteLog(strerror(errno));
		return;
	}
	Client *client = new Client(this, sockfd, addr);
	selector->Add(client);
	n_clients++;
	Send(client, ask_name_msg);
}

void Server::Send(FdObj *fdobj, const char *msg)
{
	write(fdobj->GetFd(), msg, strlen(msg));
}

void Server::SendAll(const char *msg, Client *except)
{
	for (auto fd : selector->fds) {
		FdObj *client = selector->fdobj_map[fd];
		if (client != except && GetFd() != fd)
			Send(client, msg);
	}
}

bool Server::IsNameUnqiue(char *name)
{
	auto res = names.insert(name);
	return res.second;
}

void Server::DeleteName(char *name)
{
	names.erase(name);
}

void Server::CloseClientSession(Client *c)
{
	selector->Remove(c);
	DeleteName(c->name);
	delete c;
	n_clients--;
}

void Server::WriteLog(const char *err_msg)
{
	fprintf(log, "%s\n", err_msg);
	fputc('\n', log);
}
