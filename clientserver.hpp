#ifndef CLIENT_HPP_ENTRY
#define CLIENT_HPP_ENTRY

#include <stdio.h>

#include "selector.hpp"

enum {
	BUF_LEN = 1024,
	QLEN_LISTEN = 16,
	MAX_CLINETS = 100,
};

class Client;

// LinkedList
struct Item {
	Client *client;
	Item *next;
};

// Binary Search Tree
struct NameNode {
	char *name;
	NameNode *left;
	NameNode *right;
	NameNode(char *name_, NameNode *l, NameNode *r)
		: name(name_), left(l), right(r) {}
};

class Server : public FdObj {
	SessionSelector *selector;
	Item *head;
	int n_clients;
	NameNode *names;
	FILE *log;

	/* Add a server to `fdarr` of SessionSelector.
	 * `max_fd` will be equal to the listen socket number.
	 */
	Server(SessionSelector *sel, FILE *f, int sockfd, struct sockaddr_in& addr);

	/* Push to LinkedList */
	Client *PushClient(int sockfd, struct sockaddr_in& addr);

	/* Insert to BST. If BST has this name return false */
	bool InsertName(NameNode **root, char *name);
	void DeleteName(NameNode *root, char *name);
public:
	FILE *GetLog() { return log; }
	int GetNumberClinets() { return n_clients; };

	/* Start server. It initializes the listen socket */
	static Server *Start(SessionSelector *sel, int port, FILE *f);

	virtual ~Server();

	/* Accept client. New Client will be created.
	 * He also will be added to `fdarr` of SessionSelector
	 */
	virtual void Handle();

	void Send(FdObj *fdobj, const char *msg);
	void SendAll(const char *msg, Client *except = 0);

	/* Client will call this function to insert name to BST */
	bool InsertNameAndCheck(char *name);
		
	/* Remove client from LinkedList.
	 * Delete a node with his name.
	 */
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
	/* Initialize the name OR run a command OR send message */
	virtual void Handle();
};

#endif
