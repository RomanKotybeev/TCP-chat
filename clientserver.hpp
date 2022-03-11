#ifndef CLIENT_HPP_ENTRY
#define CLIENT_HPP_ENTRY

#include <stdio.h>

#include "selector.hpp"

enum {
	BUF_LEN = 1023,
	QLEN_LISTEN = 16,
	MAX_CLINETS = 100,
};

class Client;

class Server : public FdObj {
	SessionSelector *selector;
	struct Item {
		Client *client;
		Item *next;
	};
	Item *head;
	int n_clients;
	FILE *log;

	Server(SessionSelector *sel, FILE *f, int sockfd, struct sockaddr_in& addr);

	Client *PushClient(int sockfd, struct sockaddr_in& addr);
public:
	FILE *GetLog() { return log; }
	int GetNumberClinets() { return n_clients; };

	static Server *Start(SessionSelector *sel, int port, FILE *f);

	virtual ~Server();
	virtual void Handle();

	void Send(FdObj *fdobj, const char *msg);
	void SendAll(const char *msg, Client *except = 0);

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

	void SendInfoToChat(char *str);
	void CleanBuffer();
	void CheckLine();

	void ProcessName();
	void ProcessMsg();
	void ProcessCmd();
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
