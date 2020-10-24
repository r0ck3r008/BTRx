#include<iostream>
#include<string.h>
#include<stdarg.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logchild.h"
#include"logger/logger.h"

using std::endl;
using std::string;
using std::cerr;
using namespace logger;

Logger :: Logger(string fname, LogMsgT::LogLvlT lvl_max)
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
		LogChild lchild(sock[0], f);
		lchild.srvloop();
		_exit(0);
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

/* Good for log_{dbg,wrn,err} */
void Logger :: write_msg(LogMsgT :: LogLvlT log_lvl, string msg, ...)
{
	if(log_lvl > (this->max_lvl))
		return;

	LogMsgT lmsg;
	char tmp[512] = {0};
	va_list args;
	va_start(args, msg);
	vsprintf(tmp, msg.c_str(), args);
	lmsg.set_logmsg(tmp);
	lmsg.set_loglvl(log_lvl);

	size_t size = lmsg.ByteSizeLong();
	char cmds[size] = {0};
	if(write(this->sock, cmds, size * sizeof(char)) < 0) {
		cerr << "LOGGER: Write: Error writing to child: "
			<< strerror(errno) << endl;
		_exit(-1);
	}
	va_end(args);
}
