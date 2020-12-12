#ifndef LOGGER_H
#define LOGGER_H

#include<iostream>
#include<vector>
#include<pthread.h>

using std::string;
using std::vector;

namespace logger {
	enum LogLvlT {
		LOG_TCP_INIT = 1,
		LOG_TCP_FIN = 2,
		LOG_PREF_NGBR = 4,
		LOG_OUCHK_NGBR = 8,
		LOG_UCHK = 16,
		LOG_CHK = 32,
		LOG_HV = 64,
		LOG_INT = 128,
		LOG_UINT = 256,
		LOG_DWLD = 512,
		LOG_DWLD_CMPLT = 1024,
		LOG_ERR = 2048,
		LOG_WRN = 4096,
		LOG_INFO = 8192,
		LOG_DBG = 16384,
	};
	class Logger {
                FILE *f;
                pthread_mutex_t mut;
		LogLvlT max_lvl;

                void Lock();
                void UnLock();
	public:
		Logger(string&, LogLvlT);
		~Logger();
		void write_msg(LogLvlT, string, ...);
		void write_msg(LogLvlT, int ,int);
		void write_msg(LogLvlT, int);
		void write_msg(LogLvlT, int, int, uint32_t);
		void write_msg(LogLvlT, int, int, uint32_t, int);
		void write_msg(LogLvlT, int, vector<int>);
	};
}

#endif
