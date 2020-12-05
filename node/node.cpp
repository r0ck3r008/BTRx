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
#include"objstore/objstore.h"
#include"handler/handler.h"
#include"node/node.h"

using node::Node;
using objstore::ObjStore;
using logger::LogLvlT;
using logger::Logger;

extern Logger *lvar;

int sock_create(const char *addr, int port)
{
	int sock = 0;
	if((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: Socket: %s",
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
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: Bind: %s",
				strerror(errno));
		close(sock);
		return -1;
	}
	if(listen(sock, 5) < 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: Listen: %s",
				strerror(errno));
		close(sock);
		return -1;
	}
	return sock;
}

Node :: Node(int peerid, string addr, int port, string sh_fname,
		vector<char *>& peer, vector<int>& vals)
{
	int listen_sock = 0;
	if((listen_sock = sock_create(addr.c_str(), port)) < 0)
		_exit(1);
        this->threads = vector<thread>();

	this->peerid = peerid;

	int fsz = vals[3], pcsz = vals[4],
                npcs = (fsz % pcsz) ? ((fsz/pcsz) + 1) : (fsz/pcsz);

	this->ostore = new ObjStore(fsz, pcsz, npcs, sh_fname);

        /* NbrMap is initialized on heap as it CANNOT have a default constructor
         * since its constructor initializes 2 threads */
        this->nbrmap = new NbrMap(peer.size(), npcs);

	this->connback(peer);

        this->acceptloop(listen_sock);
}

Node :: ~Node()
{
        for(auto &thr: this->threads)
                thr.join();

        delete this->ostore;
        delete this->nbrmap;
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

	for(uint32_t i=0; i<peers.size(); i++) {
		strncpy(line, peers[i], 512 * sizeof(char));
		int peerid = strtol(strtok(line, " "), NULL, 10);
		char *hname = strtok(NULL, " ");
		char *port = strtok(NULL, " ");
		bool hasfile = (bool)strtol(strtok(NULL, " "), NULL, 10);

		if(this->peerid == peerid) {
			this->ostore->bfield_init(hasfile);
			break;
		}
		if(getaddrinfo(hname, port, &hints, &res) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Getaddrinfo: %s",
					strerror(errno));
			_exit(1);
		}
		if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Sock: %s",
					strerror(errno));
			_exit(1);
		}
		struct addrinfo *curr;
		for(curr=res; curr != NULL; curr = curr->ai_next) {
			if(connect(sock, curr->ai_addr, len) == 0) {
                                this->make_thread(sock, (struct sockaddr_in *)curr->ai_addr,
                                                false);
				break;
			}
		}
		explicit_bzero(line, 512 * sizeof(char));
		free(peers[i]);
	}
}

void Node :: acceptloop(int listen_sock)
{
        socklen_t len = sizeof(struct sockaddr_in);
        while(1) {
                struct sockaddr_in addr;
                int clisock;
                if((clisock=accept(listen_sock, (struct sockaddr *)&addr, &len)) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Accept: %s",
					strerror(errno));
			_exit(1);
                }
                this->make_thread(clisock, &addr, true);
        }
}

void Node :: make_thread(int sock, struct sockaddr_in *_addr, bool client)
{
        struct sockaddr_in *addr = new struct sockaddr_in;
        addr->sin_port = _addr->sin_port;
        addr->sin_addr.s_addr = inet_addr(inet_ntoa(_addr->sin_addr));
        addr->sin_family = _addr->sin_family;

        this->threads.push_back(thread(cli_handler, sock, addr, this->ostore, this->nbrmap,
                                        client));
}
