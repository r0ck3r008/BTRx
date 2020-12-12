#include<iostream>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<vector>

#include"logger/logger.h"
#include"node/node.h"

using logger::Logger;
using logger::LogLvlT;
using node::Node;
using std::vector;
using std::string;

Logger *lvar;
int n_pref_peers;
int uchoke_ival;
int opuchoke_ival;

void read_peer_info(string& fname, vector<char *>& peers)
{
	FILE *f = NULL;
	if((f = fopen(fname.c_str(), "r")) == NULL) {
		lvar->write_msg(LogLvlT::LOG_ERR, "ENTRYPOINT: Fopen: %s",
					fname.c_str());
		_exit(1);
	}

	char *line = NULL;
	size_t n = 0;
	while(getline(&line, &n, f) > 0) {
		peers.push_back(line);
		line = NULL;
		n = 0;
	}
}

void read_conf_file(string& fname, string& sh_fname, vector<int>& vals)
{
	FILE *f = NULL;
	if((f = fopen(fname.c_str(), "r")) == NULL) {
		lvar->write_msg(LogLvlT::LOG_ERR, "ENTRYPOINT: Fopen: %s",
					fname.c_str());
		_exit(1);
	}

	char *line = NULL;
	size_t n = 0;
	char tmp[512];
	while(getline(&line, &n, f) > 0) {
		strncpy(tmp, line, 512 * sizeof(char));
		char *field = strtok(tmp, " ");
		if(!strcmp(field, "FileName"))
			sh_fname = string(strtok(NULL, " "));
		else
			vals.push_back(strtol(strtok(NULL, " "), NULL, 10));

		free(line);
		line = NULL;
		n = 0;
	}
}

int main(int argc, char **argv)
{
	if(argc != 2) {
		fprintf(stderr, "[-]Usage: ./peerProcess <peer-id>\n");
		_exit(1);
	}

	string fname = "./log_peer_" + string(argv[1]);
	lvar = new Logger(fname, LogLvlT::LOG_DBG);

	vector<int> vals;
	string sh_fname;
	fname = "./Common.cfg";
	read_conf_file(fname, sh_fname, vals);

	vector<char *> peers;
	fname = "./peer_" + string(argv[1]) + "/" + "PeerInfo.cfg";
	read_peer_info(fname, peers);

        n_pref_peers = vals[0];
        uchoke_ival = vals[1];
        opuchoke_ival = vals[2];
	Node *n = new Node(strtol(argv[1], NULL, 10), "127.0.0.1", 6008, sh_fname,
								peers, vals);

        delete n;
	delete lvar;
}
