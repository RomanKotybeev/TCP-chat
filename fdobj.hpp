#ifndef FDOBJ_HPP_ENTRY
#define FDOBJ_HPP_ENTRY

#include <unistd.h>
#include <arpa/inet.h>

class FdObj {
	int fd;
	sockaddr_in addr;
public:
	FdObj(int fd_, sockaddr_in& addr_)
		: fd(fd_)
		, addr(addr_)
	{} 
	virtual ~FdObj() { close(fd); };

	virtual void Handle() = 0;

	struct sockaddr_in GetAddr() const { return addr; }
	constexpr int GetFd() const { return fd; }

	char *AddrToStr() { return inet_ntoa(addr.sin_addr); }
};

#endif
