#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>

#include "selector.hpp"

void SessionSelector::Add(FdObj *fdobj)
{
	int sockfd = fdobj->GetFd();
	if (sockfd > max_fd)
		max_fd = sockfd;
	fds.push_back(sockfd);
	fdobj_map[sockfd] = fdobj;
}

void SessionSelector::Remove(FdObj *fdobj)
{
	int sockfd = fdobj->GetFd();
	fds.erase(std::remove(fds.begin(), fds.end(), sockfd), fds.end());
	fdobj_map.erase(sockfd);
}

void SessionSelector::Run()
{
	int sret;
	fd_set readfds;
	for (;;) {
		FD_ZERO(&readfds);
		for (auto fd : fds)
			FD_SET(fd, &readfds);
		
		sret = select(max_fd+1, &readfds, 0, 0, 0);
		if (sret == -1) {
			break;
		}
		if (sret > 0) {
			for (auto fd : fds) {
				if (FD_ISSET(fd, &readfds))
					fdobj_map[fd]->Handle();
			}
		}
	}
}
