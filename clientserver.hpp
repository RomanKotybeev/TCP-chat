#ifndef CLIENT_HPP_ENTRY
#define CLIENT_HPP_ENTRY

#include <stdio.h>

#include "selector.hpp"

enum {
	BUF_LEN = 1023,
	QLEN_LISTEN = 16,
};

class Client;

class Server : public FdObj {
	SessionSelector *selector;
	struct Item {
		Client *client;
		Item *next;
		int n_clients;
	};
	Item *head;
	FILE *log;

	Server(SessionSelector *sel, FILE *f, int sockfd, struct sockaddr_in& addr);
	Client *PushClient(int sockfd, struct sockaddr_in& addr);
public:
	virtual ~Server();
	virtual void Handle();
	static Server *Start(SessionSelector *sel, int port, FILE *f);
	void Send(FdObj *fdobj, const char *msg);
	void SendAll(const char *msg, Client *except = 0);
	FILE *GetLog() { return log; }
	void CloseClientSession(Client *fdobj);
};

/* --------------------------------------------------------------------------*/
/* Client */
/* --------------------------------------------------------------------------*/

class Client : public FdObj {
	char *name;
	int name_len;
	char buf[BUF_LEN+1];
	int buf_used;

	Server *serv_master;

	void ProcessName();
	void ProcessMsg();
public:
	Client(Server *s, int sockfd, struct sockaddr_in& addr)
		: FdObj(sockfd, addr)
		, buf_used(0)
		, serv_master(s)
	{}
	virtual ~Client();
	virtual void Handle();
};

#endif
