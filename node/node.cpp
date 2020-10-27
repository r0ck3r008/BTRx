#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logger.h"
#include"node/objstore.h"
#include"node/node.h"

using node::Node;
using node::ObjStore;
using logger::LogMsgT;
using logger::Logger;

extern Logger *lvar;

int sock_create(char *addr, int port)
{
	int sock = 0;
	if((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		lvar->write_msg(LogMsgT::LOG_DBG, "NODE: Socket: %s",
				strerror(errno));
		return -1;
	}

	struct sockaddr_in saddr;
	explicit_bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(addr);
	saddr.sin_family = AF_INET;

	if(bind(sock, (struct sockaddr *)&saddr,
			sizeof(struct sockaddr_in)) < 0) {
		lvar->write_msg(LogMsgT::LOG_DBG, "NODE: Bind: %s",
				strerror(errno));
		close(sock);
		return -1;
	}
	if(listen(sock, 5) < 0) {
		lvar->write_msg(LogMsgT::LOG_DBG, "NODE: Listen: %s",
				strerror(errno));
		close(sock);
		return -1;
	}
	return sock;
}

Node :: Node(int peerid, char *addr, int port, vector<char *>& peer)
{
	int sock = 0;
	if((sock = sock_create(addr, port)) < 0)
		_exit(1);
	this->sock = vector<int>();
	this->sock.push_back(sock);

	this->peerid = peerid;

	this->connback(peer);
}

Node :: ~Node()
{
	for(auto i: this->sock)
		close(i);
}

void Node :: connback(vector<char *>& peers)
{
	struct addrinfo hints, *res;
	explicit_bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol= IPPROTO_TCP;
	size_t len = sizeof(struct sockaddr);
	int sock;
	char line[512];

	for(int i=0; i<peers.size(); i++) {
		strncpy(line, peers[i], 512 * sizeof(char));
		int peerid = strtol(strtok(line, " "), NULL, 10);
		char *hname = strtok(NULL, " ");
		char *port = strtok(NULL, " ");
		bool hasfile = (bool)strtol(strtok(NULL, " "), NULL, 10);

		if(this->peerid == peerid) {
			if(hasfile)
				/* TODO: Read in File */
			break;
		}
		if(getaddrinfo(hname, port, &hints, &res) < 0) {
			lvar->write_msg(LogMsgT::LOG_ERR, "NODE: Getaddrinfo: %s",
					strerror(errno));
			_exit(1);
		}
		struct addrinfo *curr = res;
		if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			lvar->write_msg(LogMsgT::LOG_ERR, "NODE: Sock: %s",
					strerror(errno));
			_exit(1);
		}
		for(curr; curr != NULL; curr = curr->ai_next) {
			if(connect(sock, curr->ai_addr, len) == 0) {
				this->sock.push_back(sock);
				break;
			}
		}
		explicit_bzero(line, 512 * sizeof(char));
		free(peers[i]);
	}
}

void Node :: srvloop()
{

}
