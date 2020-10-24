#ifndef LOGGER_H
#define LOGGER_H

#include<vector>

using std::string;
using std::vector;

namespace logger {
	class Logger {
		int sock;
		LogMsgT::LogLvlT max_lvl;
	public:
		Logger(string, LogMsgT::LogLvlT);
		~Logger();
		void write_msg(LogMsgT :: LogLvlT,
				char *, ...);
	};
}

#endif
