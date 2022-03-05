#ifndef SESSION_SELECTOR_HPP_ENTRY
#define SESSION_SELECTOR_HPP_ENTRY

#include "fdobj.hpp"

enum {
	INIT_ARR_SIZE = 16,
};

class SessionSelector {
	FdObj **fdarr;
	int max_fd;
public:
	SessionSelector();
	~SessionSelector() { delete[] fdarr; }
	void Run();
	void AddClient(int fd, struct sockaddr_in& addr);
	void Add(FdObj *fdobj);
	void CopyFds(int new_max_fd);
	void Remove(FdObj *fdobj);
};

#endif
