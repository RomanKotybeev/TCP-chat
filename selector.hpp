#ifndef SESSION_SELECTOR_HPP_ENTRY
#define SESSION_SELECTOR_HPP_ENTRY
#include <map>
#include <vector>

#include "fdobj.hpp"

enum {
	INIT_ARR_SIZE = 16,
};

class SessionSelector {
	friend class Server;
	std::map<int, FdObj *> fdmap;
	std::vector<int> fd_to_remove;
	int max_fd;
public:
	SessionSelector() : max_fd(-1) {}
	~SessionSelector() {}
	void Run();
	void Add(FdObj *fdobj, int sockfd);
	void Remove(FdObj *fdobj);
};

#endif
