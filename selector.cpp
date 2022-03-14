#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "selector.hpp"

void SessionSelector::InitFdArr(int sockfd)
{
	fdarr = new FdObj*[sockfd * 2];
	max_fd = sockfd;
}

void SessionSelector::CopyFds(int new_max_fd)
{
	FdObj **new_fdarr = new FdObj*[new_max_fd * 2];
	for (int i = 0; i <= new_max_fd; i++)
		new_fdarr[i] = i <= max_fd ? fdarr[i] : 0; 
	delete[] fdarr;
	fdarr = new_fdarr;
	max_fd = new_max_fd;
}

void SessionSelector::Add(FdObj *fdobj)
{
	int sockfd = fdobj->GetFd();
	if (max_fd == -1)
		InitFdArr(sockfd);
	if (sockfd > max_fd)
		CopyFds(sockfd);
	fdarr[sockfd] = fdobj;
}

void SessionSelector::Remove(FdObj *fdobj)
{
	int sockfd = fdobj->GetFd();
	fdarr[sockfd] = 0;
	close(sockfd);
}

void SessionSelector::Run()
{
	int sret;
	fd_set readfds;
	for (;;) {
		FD_ZERO(&readfds);
		for (int i = 0; i <= max_fd; i++) {
			if (fdarr[i] != 0) {
				FD_SET(i, &readfds);
			}
		}
		sret = select(max_fd+1, &readfds, 0, 0, 0);
		if (sret == -1) {
			break;
		} else if (sret > 0) {
			for (int i = 0; i <= max_fd; i++) {
				if (fdarr[i] == 0)
					continue;
				if (FD_ISSET(i, &readfds))
					fdarr[i]->Handle();
			}
		}
	}
}
