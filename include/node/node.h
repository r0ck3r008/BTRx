#ifndef NODE_H
#define NODE_H

int sock_create(char *, int);

namespace node {
	class Node {
		int sock;
		int peerid;
	public:
		Node(int, char *, int, vector<string>&);
		~Node();
		void srvloop();
	};
}

#endif
