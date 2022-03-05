#ifndef FDOBJ_HPP_ENTRY
#define FDOBJ_HPP_ENTRY

#include <unistd.h>
#include <arpa/inet.h>

class FdObj {
	int fd;
	struct sockaddr_in addr;
public:
	FdObj(int fd_, struct sockaddr_in& addr_)
		: fd(fd_)
		, addr(addr_)
	{} 
	virtual ~FdObj() { close(fd); };

	virtual void Handle() = 0;

	struct sockaddr_in GetAddr() const { return addr; }
	int GetFd() const { return fd; }

	void SetFd(int& _fd) { fd = _fd; }
	void SetAddr(struct sockaddr_in& _addr) { addr = _addr; }

	char *AddrToStr() { return inet_ntoa(addr.sin_addr); }
};

#endif
