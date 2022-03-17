#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <algorithm>
#include <vector>

#include "selector.hpp"

void SessionSelector::Add(FdObj *fdobj, int fd)
{
	if (fd > max_fd)
		max_fd = fd;
	fdmap[fd] = fdobj;
}

void SessionSelector::Remove(FdObj *fdobj)
{
	int fd = fdobj->GetFd();
	fd_to_remove.push_back(fd);
}

void SessionSelector::Run()
{
	int sret;
	fd_set readfds;
	for (;;) {
		for (auto const& fdobj : fdmap)
			FD_SET(fdobj.first, &readfds);
		
		sret = select(max_fd+1, &readfds, 0, 0, 0);
		if (sret == -1) {
			if (errno == EINTR)
				continue;
			else
				break;
		}
		if (sret > 0) {
			for (auto const& fdobj : fdmap)
				if (FD_ISSET(fdobj.first, &readfds))
					fdobj.second->Handle();
		}

		for (int fd : fd_to_remove) {
			fdmap.erase(fd);
			if (fd == max_fd)
				max_fd = fdmap.rbegin()->first;
		}
		if (fd_to_remove.size() > 0)
			fd_to_remove.clear();
	}
}
