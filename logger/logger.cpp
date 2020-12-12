#include<iostream>
#include<string.h>
#include<stdarg.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logger.h"

using std::endl;
using std::string;
using std::cerr;
using namespace logger;

Logger :: Logger(string& fname, LogLvlT lvl_max)
{
	FILE *f = NULL;
	if(fname == "stdout") {
		f = stdout;
	} else if((f = fopen(fname.c_str(), "w")) == NULL) {
		cerr << "LOGGER: Fopen: Error in opening log file!" << endl;
		_exit(-1);
	}

        this->f = f;
        this->max_lvl = lvl_max;
        this->mut = PTHREAD_MUTEX_INITIALIZER;
        int stat = 0;
        if((stat = pthread_mutex_init(&(this->mut), NULL)) == 0) {
                fprintf(stderr, "[-]LOGGER: Mutex Init: %s\n", strerror(errno));
                _exit(1);
        }
}

Logger :: ~Logger()
{
        int stat = 0;
        if((stat = pthread_mutex_destroy(&(this->mut))) != 0) {
                fprintf(stderr, "[-]LOGGER: Mutex Deinit: %s\n", strerror(errno));
                _exit(1);
        }
        fclose(f);
}

void Logger :: Lock()
{
        int stat = 0;
        if((stat = pthread_mutex_lock(&(this->mut))) != 0) {
                fprintf(stderr, "NBRMAP: Lock: %s", strerror(stat));
                _exit(1);
        }
}

void Logger :: UnLock()
{
        int stat = 0;
        if((stat = pthread_mutex_unlock(&(this->mut))) != 0) {
                fprintf(stderr, "NBRMAP: Unlock: %s", strerror(stat));
                _exit(1);
        }
}

/* Good for log_{dbg,wrn,err}
 * Overload this
 */
void Logger :: write_msg(LogLvlT log_lvl)
{
	if(log_lvl > (this->max_lvl))
		return;
}
