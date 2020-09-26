#include<iostream>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>

using std::endl;
using std::string;
using std::cerr;

#include"logger/logger.h"

Logger :: Logger(string fname, LOG_LVL lvl_max)
{
	FILE *f = NULL;
	if(fname == "stdout") {
		f = stdout;
	} else if((f = fopen(fname.c_str(), "w")) == NULL) {
		cerr << "LOGGER: Fopen: Error in opening log file!" << endl;
		_exit(-1);
	}

	int sock[2] = {0};
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, sock) < 0) {
		cerr << "LOGGER: Init: Erorr in making socket pair: " <<
			strerror(errno) << endl;
		_exit(-1);
	}

	pid_t child_pid = fork();
	if(!child_pid) {
		/* child */
		close(sock[1]);
		/* TODO */
		_exit(1);
	} else if (child_pid < 0) {
		/* err */
		cerr << "LOGGER: Fork: " << strerror(errno) << endl;
		_exit(-1);
	} else {
		/* parent */
		close(sock[0]);
		this->sock = sock[1];
		this->max_lvl = lvl_max;
	}
}

Logger :: ~Logger()
{
	if(write(this->sock, "EXIT", sizeof(char) * 5) < 0) {
		cerr << "LOGGER: Write: Error writing to child: "
			<< strerror(errno) << endl;
		_exit(-1);
	}

	close(this->sock);
}

void Logger :: write_msg(LOG_LVL lvl, int *nums, int n)
{
	if(lvl >= (this->max_lvl << 1))
		return;

	char cmds[512];
	sprintf(cmds, "%d", lvl);
	for(int i = 0; i < n; i++)
		sprintf(cmds, "%s:%d", cmds, nums[i]);

	if(write(this->sock, cmds, 512 * sizeof(char)) < 0) {
		cerr << "LOGGER: Write: Error writing to child: "
			<< strerror(errno) << endl;
		_exit(-1);
	}
}
