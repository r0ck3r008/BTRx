#ifndef LOGCHILD_H
#define LOGCHILD_H

#include<stdio.h>

using logger::LogMsgT;

namespace logger {
	class LogChild {
		int sock;
		LogMsgT::LogLvlT lvl;
		FILE *outf;
	public:
		LogChild(int, FILE *,
				LogMsgT::LogLvlT);
		~LogChild();
		void srvloop();
		int pretty_p(LogMsgT *);
	};
}


#endif
