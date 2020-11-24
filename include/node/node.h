#ifndef NODE_H
#define NODE_H

#include<vector>

#include"objstore/objstore.h"

using objstore::ObjStore;
using std::vector;

int sock_create(const char *, int);

namespace node {
	class Node {
                vector<pair<pthread_t,
                        HandlerArgs *>> hargs;
		ObjStore ostore;
		int peerid;
	private:
		void connback(vector<char *>&);
		void acceptloop(int);
	public:
		Node(int, string, int, string,
			vector<char *>&, vector<int>&);
		~Node();
	};
}

#endif
