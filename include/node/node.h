#ifndef NODE_H
#define NODE_H

namespace node {
	class Node {
		int sock;
		int peerid;
	public:
		Node(int, char *, int);
		~Node();
		void srvloop();
	};
}

#endif
