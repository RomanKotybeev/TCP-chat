#ifndef CLIENT_HPP_ENTRY
#define CLIENT_HPP_ENTRY

#include <stdio.h>
#include <string.h>
#include <set>

#include "selector.hpp"

enum {
	BUF_LEN = 512,
	QLEN_LISTEN = 8,
};

class Client;

struct CharPtrComp {
    bool operator()(const char *l, const char *r) const
    {
        return strcmp(r, l) > 0;
    }
};

class Server : public FdObj {
	SessionSelector *selector;
	int n_clients;
	std::set<char *, CharPtrComp> names;
	FILE *log;
private:
	// Add a server to `fdobj_map` of SessionSelector.
	// At this moment `max_fd` will be equal to the listen socket number.
	Server(SessionSelector *sel, FILE *f, int sockfd, sockaddr_in& addr);

	void DeleteName(char *name);
public:
	// Start server. It initializes the listen socket.
	// It returns nullptr if socket creation failed.
	static Server *Start(SessionSelector *sel, int port, FILE *f);

	void WriteLog(const char *err_msg);
	int GetNClinets() { return n_clients; };

	~Server() = default;

	// Accept client. It will create new `Client`.
	virtual void Handle();

	void Send(FdObj *fdobj, const char *msg);
	void SendAll(const char *msg, Client *except = 0);

	// Client will call this function to insert name to `names`
	bool IsNameUnqiue(char *name);
		
	// Delete a node with his name.
	void CloseClientSession(Client *fdobj);
};

/* --------------------------------------------------------------------------*/
/* Client */
/* --------------------------------------------------------------------------*/

class Client : public FdObj {
	friend class Server;
	char *name;
	int name_len;
	char buf[BUF_LEN];
	int buf_used;

	Server *serv_master;
private:
	void SendInfoToChat(const char *str);
	void CleanBuffer();
	void CheckLine();

	void ProcessName();
	void ProcessMsg();
	void ProcessCmd();
public:
	Client(Server *s, int sockfd, sockaddr_in& addr)
		: FdObj(sockfd, addr)
		, buf_used(0)
		, serv_master(s)
	{}
	~Client();
	// Initialize the name OR run a command OR send a message.
	virtual void Handle();
};

#endif
