#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <algorithm>
#include <vector>

#include "selector.hpp"

void SessionSelector::Add(FdObj *fdobj, int sockfd)
{
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
		std::vector<int>::size_type i;
		std::vector<int>::size_type size = fds.size();

		FD_ZERO(&readfds);
		for (i = 0; i < size; i++)
			FD_SET(fds[i], &readfds);
		
		sret = select(max_fd+1, &readfds, 0, 0, 0);
		if (sret == -1) {
			if (errno == EINTR)
				continue;
			else
				break;
		}
		if (sret > 0) {
			for (i = 0; i < size; i++)
				if (FD_ISSET(fds[i], &readfds))
					fdobj_map[fds[i]]->Handle();
		}
	}
}
