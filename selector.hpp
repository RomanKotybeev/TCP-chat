#ifndef SESSION_SELECTOR_HPP_ENTRY
#define SESSION_SELECTOR_HPP_ENTRY

#include "fdobj.hpp"

enum {
	INIT_ARR_SIZE = 16,
};

class SessionSelector {
	FdObj **fdarr;
	int max_fd;
private:
	void InitFdArr(int sockfd);
	void ExpandFdArr(int new_max_fd);
public:
	SessionSelector() : max_fd(-1) {}
	~SessionSelector() { delete[] fdarr; }
	void Run();
	void Add(FdObj *fdobj);
	void Remove(FdObj *fdobj);
};

#endif
