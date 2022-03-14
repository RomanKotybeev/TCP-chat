#include <stdio.h>

#include "fdobj.hpp"
#include "selector.hpp"
#include "clientserver.hpp"

int main()
{
	int port = 6666;
	SessionSelector *selector = new SessionSelector;
	FILE *log = fopen("log.txt", "w");
	Server *server = Server::Start(selector, port, log);
	if (server == 0) {
		perror("server");
		return 1;
	}
	selector->Run();
	delete selector;
	delete server;
	return 0;
}
