#include<iostream>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<vector>

#include"logger/logger.h"
#include"node/node.h"

using logger::Logger;
using logger::LogMsgT;
using node::Node;
using std::vector;
using std::string;

Logger *lvar;

void read_peer_info(string& fname, vector<string>& peers)
{
	FILE *f = NULL;
	if((f = fopen(fname.c_str(), "r")) == NULL) {
		lvar->write_msg(LogMsgT::LOG_ERR, "ENTRYPOINT: Fopen: %s",
					fname.c_str());
		_exit(1);
	}

	char *line = NULL;
	size_t n = 0;
	while(getline(&line, &n, f) > 0) {

	}
}

int main(int argc, char **argv)
{
	if(argc != 2) {
		fprintf(stderr, "[-]Usage: ./peerProcess <peer-id>\n");
		_exit(1);
	}

	string fname = "./log_peer_" + string(argv[1]);
	lvar = new Logger(fname, LogMsgT::LOG_DBG);

	vector<string> peers;
	fname = "./peer_" + string(argv[1]) + "/" + "PeerInfo.cfg";
	read_peer_info(fname, peers);

	delete lvar;
}
