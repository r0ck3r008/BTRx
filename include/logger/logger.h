#ifndef LOGGER_H
#define LOGGER_H

#include<stdarg.h>

using std::string;

typedef enum LOG_LVL {
	LOG_DBG = 1 << 1,
	LOG_CXNT = 1 << 2,
	LOG_CXNR = 1 << 3,
	LOG_NBR = 1 << 4,
	LOG_OUCHK = 1 << 5,
	LOG_UCHK = 1 << 6,
	LOG_CHK = 1 << 7,
	LOG_HV = 1 << 8,
	LOG_INT = 1 << 9,
	LOG_UINT = 1 << 10,
	LOG_DWL = 1 << 11,
	LOG_DWLD = 1 << 12,
	LOG_ERR = 1 << 13
} LOG_LVL;

class Logger {
	int sock;
public:
	Logger(string);
	~Logger();
	void write_msg(LOG_LVL, int *, int);
};

#endif
