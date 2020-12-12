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
#include"defs.h"
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
	memset(&saddr, 0, sizeof(struct sockaddr_in));
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

Node :: Node(int peerid, string addr, int port, string sh_fname, vector<int> &peer_ids,
		vector<string> &peer_hosts, vector<string> &peer_ports,
                vector<bool> &peer_hasfile, vector<int>& vals)
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
        this->nbrmap = new NbrMap(peer_ids.size(), npcs);

	uint32_t conns = this->connback(peer_ids, peer_hosts, peer_ports, peer_hasfile);

        this->acceptloop(listen_sock, conns, peer_ids.size());
}

Node :: ~Node()
{
        for(auto &thr: this->threads)
                thr.join();

        delete this->ostore;
        delete this->nbrmap;
}

uint32_t Node :: connback(vector<int> &peer_ids, vector<string>& peer_hosts,
                vector<string> &peer_ports, vector<bool> &peer_hasfile)
{
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol= IPPROTO_TCP;
	int sock = -1;
        uint32_t i;

	for(i=0; i<peer_ids.size(); i++) {
		if(this->peerid == peer_ids[i]) {
			this->ostore->bfield_init(peer_hasfile[i]);
			break;
		}
		if(getaddrinfo(peer_hosts[i].c_str(), peer_ports[i].c_str(),
                                        &hints, &res) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Getaddrinfo: %s",
					strerror(errno));
			_exit(1);
		}
		struct addrinfo *curr;
		for(curr=res; curr != NULL; curr = curr->ai_next) {
                        if((sock = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) < 0) {
                                lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Sock: %s",
                                                strerror(errno));
                                _exit(1);
                        }
			if(connect(sock, curr->ai_addr, curr->ai_addrlen) == 0) {
                                this->make_thread(sock, peer_ids[i]);
				break;
			}
                        close(sock);
                        sock = -1;
		}
                freeaddrinfo(res);
	}

        return i+1;
}

void Node :: acceptloop(int listen_sock, uint32_t clients, uint32_t size)
{
        for(uint32_t i=clients; i<size; i++) {
                int clisock;
                if((clisock=accept(listen_sock, NULL, NULL)) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Accept: %s",
					strerror(errno));
			_exit(1);
                }
                this->make_thread(clisock, 0);
        }

        close(listen_sock);
}

void Node :: make_thread(int sock, int peerid_peer)
{
        struct timeval t;
        t.tv_sec = 0;
        t.tv_usec = RECVTIMEOUT;
        if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timeval)) < 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Setsockopt: %s", strerror(errno));
                _exit(1);
        }

        this->threads.push_back(thread(cli_handler, sock, this->peerid, peerid_peer,
                                        this->ostore, this->nbrmap));
}
