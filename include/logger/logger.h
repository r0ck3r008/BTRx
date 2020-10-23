#ifndef LOGGER_H
#define LOGGER_H

#include<stdarg.h>

#include"logger/logmsg.pb.h"

using std::string;

namespace logger {
	class Logger {
		int sock;
		LogLvlT max_lvl;
	public:
		Logger(string, LogLvlT);
		~Logger();
		void write_msg(LogLvlT, int *, int);
	};
}

#endif
