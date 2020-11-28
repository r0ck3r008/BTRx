#ifndef NODE_H
#define NODE_H

#include<vector>
#include<thread>

#include"objstore/objstore.h"

using objstore::ObjStore;
using std::vector;
using std::pair;
using std::thread;

int sock_create(const char *, int);

namespace node {
	class Node {
                vector<thread> threads;
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
