#ifndef LOGCHILD_H
#define LOGCHILD_H

#include<stdio.h>

#include"logger/logger.h"

using logger::LogLvlT;

namespace logger {
	class LogChild {
		int sock;
		FILE *outf;
	public:
		LogChild(int, FILE *);
		~LogChild();
		void srvloop();
		int pretty_p(char *);
	};
}


#endif
