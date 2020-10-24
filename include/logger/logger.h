#ifndef LOGGER_H
#define LOGGER_H

#include<vector>

#include"logger/logmsg.pb.h"

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
				string, ...);
	};
}

#endif
