#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "clientserver.hpp"

char welcome_msg[] = "Enter your name: ";


Server::Server(SessionSelector *sel, FILE *f,
               int sockfd, struct sockaddr_in& addr)
	: FdObj(sockfd, addr)
	, selector(sel)
	, head(0)
	, n_clients(0)
	, names(0)
	, log(f)
{ 
	selector->Add(this);
}

Server::~Server()
{
	while (head != 0) {
		Item *tmp = head;
		head = head->next;
		selector->Remove(tmp->client);
		delete tmp->client;
		delete tmp;
	}
	fclose(log);
}

void Server::Send(FdObj *fdobj, const char *msg)
{
	write(fdobj->GetFd(), msg, strlen(msg));
}

void Server::SendAll(const char *msg, Client *except)
{
	for (Item *tmp = head; tmp != 0; tmp = tmp->next) {
		if (tmp->client != except)
			Send(tmp->client, msg);
	}
}

void Server::Handle()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int sockfd = accept(GetFd(), (struct sockaddr *)&addr, &len);
	if (sockfd == -1) {
		return ;
	}
	Client *c = PushClient(sockfd, addr);
	selector->Add(c);
	Send(c, welcome_msg);
}

Client *Server::PushClient(int sockfd, struct sockaddr_in& addr)
{
	Item *tmp = new Item;
	Client *c = new Client(this, sockfd, addr);
	tmp->client = c;
	tmp->next = head;
	head = tmp;
	n_clients++;
	return c;
}

bool Server::InsertNameAndCheck(char *name)
{
	return InsertName(&names, name);
}

bool Server::InsertName(NameNode **root, char *name)
{
	if (*root == 0) {
		*root = new NameNode(name, 0, 0);
		return true;
	}

	int cmp = strcmp(names->name, name);
	if (cmp == 0)
		return false;
	else if (cmp > 0)
		return InsertName(&(*root)->left, name);
	else
		return InsertName(&(*root)->right, name);
}

void Server::CloseClientSession(Client *c)
{
	Item **pp;
	for (pp = &head; pp != 0; pp = &((*pp)->next)) {
		if ((*pp)->client == c) {
			Item *tmp = *pp;
			*pp = tmp->next;
			selector->Remove(tmp->client);
			delete tmp->client;
			delete tmp;
			n_clients--;
			return ;
		}
	}
}

Server *Server::Start(SessionSelector *sel, int port, FILE *f)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return 0;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
		return 0;

	if (-1 == listen(sockfd, QLEN_LISTEN))
		return 0;

	return new Server(sel, f, sockfd, addr);
}
