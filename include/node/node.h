#ifndef NODE_H
#define NODE_H

#include<vector>

using std::vector;

int sock_create(char *, int);

namespace node {
	class Node {
		vector<int> sock;
		int peerid;
	private:
		void connback(vector<char *>&);
	public:
		Node(int, char *, int, vector<char *>&);
		~Node();
		void srvloop();
	};
}

#endif
