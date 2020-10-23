#ifndef LOGGER_H
#define LOGGER_H

#include<stdarg.h>

using std::string;

typedef enum LOG_LVL: uint32_t {
	LOG_TCP = 1 << 12,
	LOG_CPN = 1 << 13,
	LOG_COUN = 1 << 14,
	LOG_UC = 1 << 15,
	LOG_C = 1 << 16,
	LOG_HMSSG = 1 << 17,
	LOG_IMSSG = 1 << 18,
	LOG_NIMSSG = 1 << 19,
	LOG_DP = 1 << 20,
	LOG_CD = 1 << 21
} LOG_LVL;

typedef enum LOG_TYPE: uint32_t {
	LOG_CXNT = (1 << 1) | LOG_WRN,
	LOG_CXNR = (1 << 2) | LOG_WRN,
	LOG_NBR = (1 << 3) | LOG_WRN,
	LOG_OUCHK = (1 << 4) | LOG_WRN,
	LOG_UCHK = (1 << 5) | LOG_WRN,
	LOG_CHK = (1 << 6) | LOG_WRN,
	LOG_HV = (1 << 7) | LOG_WRN,
	LOG_INT = (1 << 8) | LOG_WRN,
	LOG_UINT = (1 << 9) | LOG_WRN,
	LOG_DWL = (1 << 10) | LOG_WRN,
	LOG_DWLD = (1 << 11) | LOG_WRN,
} LOG_TYPE;

class Logger {
	int sock;
	LOG_LVL max_lvl;
public:
	Logger(string, LOG_LVL);
	~Logger();
	void write_msg(LOG_LVL, int *, int);
};

#endif
