#ifndef LOGCHILD_H
#define LOGCHILD_H

#include<stdio.h>

#include"logger/logmsg.pb.h"

using logger::LogMsgT;

namespace logger {
	class LogChild {
		int sock;
		FILE *outf;
	public:
		LogChild(int, FILE *);
		~LogChild();
		void srvloop();
		int pretty_p(LogMsgT *);
	};
}


#endif
