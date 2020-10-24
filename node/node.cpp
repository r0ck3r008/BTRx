#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logger.h"
#include"node/node.h"

using node::Node;
using logger::LogMsgT;
using logger::Logger;

extern Logger *lvar;

Node :: Node(int peerid, char *addr, int port)
{
	if((this->sock=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		lvar->write_msg(LogMsgT::LOG_DBG, "NODE: Socket: %s",
				strerror(errno));
		_exit(1);
	}

	struct sockaddr_in saddr;
	explicit_bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(addr);
	saddr.sin_family = AF_INET;

	if(bind(this->sock, (struct sockaddr *)&saddr,
			sizeof(struct sockaddr_in)) < 0) {
		lvar->write_msg(LogMsgT::LOG_DBG, "NODE: Bind: %s",
				strerror(errno));
		_exit(1);
	}
	if(listen(this->sock, 5) < 0) {
		lvar->write_msg(LogMsgT::LOG_DBG, "NODE: Listen: %s",
				strerror(errno));
		_exit(1);
	}

	this->peerid = peerid;
}

void Node :: srvloop()
{

}

Node :: ~Node()
{
	close(this->sock);
}
