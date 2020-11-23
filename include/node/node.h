#ifndef NODE_H
#define NODE_H

#include<vector>

#include"objstore/objstore.h"

using objstore::ObjStore;
using std::vector;

int sock_create(const char *, int);

namespace node {
	class Node {
		vector<int> sock;
		ObjStore ostore;
		int peerid;
	private:
		void connback(vector<char *>&);
	public:
		Node(int, string, int, string,
			vector<char *>&, vector<int>&);
		~Node();
		void srvloop();
	};
}

#endif
