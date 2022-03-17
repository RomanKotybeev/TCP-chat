#ifndef SESSION_SELECTOR_HPP_ENTRY
#define SESSION_SELECTOR_HPP_ENTRY
#include <map>
#include <list>

#include "fdobj.hpp"

enum {
	INIT_ARR_SIZE = 16,
};

class SessionSelector {
	friend class Server;
	std::map<int, FdObj*> fdobj_map;
	std::list<int> fds;
	int max_fd;
public:
	SessionSelector() : max_fd(-1) {}
	~SessionSelector() {}
	void Run();
	void Add(FdObj *fdobj, int sockfd);
	void Remove(FdObj *fdobj);
};

#endif
