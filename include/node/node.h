#ifndef NODE_H
#define NODE_H

#include<vector>

#include"objstore/objstore.h"
#include"handler/handler.h"

using objstore::ObjStore;
using handler::HandlerArgs;
using std::vector;
using std::pair;

int sock_create(const char *, int);

namespace node {
	class Node {
                vector<pair<pthread_t,
                        HandlerArgs *>> hargs;
		ObjStore ostore;
		int peerid;
	private:
		void connback(vector<char *> &);
		void acceptloop(int);
                void make_thread(int,
                                struct sockaddr_in *,
                                bool);
	public:
		Node(int, string, int, string,
			vector<char *>&, vector<int>&);
		~Node();
	};
}

#endif
