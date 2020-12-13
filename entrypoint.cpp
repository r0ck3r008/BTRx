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

int read_peer_info(string &fname, int pid, vector<int> &peer_ids, vector<string>& peer_hosts,
                                vector<string> &peer_ports, vector<bool> &peer_hasfile)
{
	FILE *f = NULL;
	if((f = fopen(fname.c_str(), "r")) == NULL) {
		lvar->write_msg(LogLvlT::LOG_ERR, "ENTRYPOINT: Fopen: %s",
					fname.c_str());
		_exit(1);
	}

	char *line = NULL, buf[512] = {0};
	size_t n = 0, csz = sizeof(char) * 512;
        int ret = 0;
	while(getline(&line, &n, f) > 0) {
                strncpy(buf, line, csz);
		int peerid = strtol(strtok(line, " "), NULL, 10);
                peer_ids.push_back(peerid);
		peer_hosts.push_back(string(strtok(NULL, " ")));
		char *port = strtok(NULL, " ");
                if(peerid == pid)
                        ret = strtol(port, NULL, 10);
                peer_ports.push_back(string(port));
		peer_hasfile.push_back((bool)strtol(strtok(NULL, " "), NULL, 10));

                free(line);
		line = NULL;
		n = 0;
                memset(buf, 0, csz);
	}

        return ret;
}

void read_conf_file(string& fname, char *peerid, string& sh_fname, vector<int>& vals)
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
			sh_fname = string(peerid) + string("/") + string(strtok(NULL, " "));
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

	string fname = string(argv[1]) + "/log_peer_" + string(argv[1]);
	lvar = new Logger(fname, LogLvlT::LOG_DBG);

        int peerid = strtol(argv[1], NULL, 10);

	vector<int> vals;
	string sh_fname;
	fname = "./Common.cfg";
	read_conf_file(fname, argv[1], sh_fname, vals);

        vector<int> peer_ids;
	vector<string> peer_hosts, peer_ports;
        vector<bool> peer_hasfile;
	fname = "PeerInfo.cfg";
	int port = read_peer_info(fname, peerid, peer_ids, peer_hosts, peer_ports, peer_hasfile);

        n_pref_peers = vals[0];
        uchoke_ival = vals[1];
        opuchoke_ival = vals[2];
	Node *n = new Node(peerid, "127.0.0.1", port, sh_fname, peer_ids, peer_hosts,
                                                        peer_ports, peer_hasfile, vals);

        delete n;
	delete lvar;
}
