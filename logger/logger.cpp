#include<iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
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
using std::begin;
using std::end;
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
void Logger :: write_msg(LogLvlT log_lvl, string msg, ...)
{
	if(log_lvl > (this->max_lvl))
		return;
	va_list args;
	va_start(args, msg);
	vfprintf(this->f, msg.c_str(), args);
}


void Logger :: write_msg(LogLvlT log_lvl, int peer_id_1, int peer_id_2)
{
	if(log_lvl > (this->max_lvl))
		return;

        time_t now = time(0);
   	char* date_time = ctime(&now);

        Lock();
	switch(log_lvl) {
		case LOG_TCP_INIT:
			fprintf(this->f, "[%s]: Peer  %d makes a connection to Peer %d. \n", date_time, peer_id_1, peer_id_2);
			break;
                case LOG_TCP_FIN:
			fprintf(this->f, "[%s]: Peer %d is connected from Peer %d. \n", date_time, peer_id_1, peer_id_2);
			break;
                case LOG_UCHK:
			fprintf(this->f, "[%s]: Peer %d is unchoked by %d. \n", date_time, peer_id_1, peer_id_2);
			break;
		case LOG_CHK:
			fprintf(this->f, "[%s]: Peer %d is choked by %d. \n", date_time, peer_id_1, peer_id_2);
			break;
		case LOG_INT:
			fprintf(this->f, "[%s]: Peer %d received the 'interested' message from %d. \n", date_time, peer_id_1, peer_id_2);
			break;
		case LOG_UINT:
			fprintf(this->f, "[%s]: Peer %d received the 'not interested' message from %d. \n", date_time, peer_id_1, peer_id_2);
			break;
                default: 
                        fprintf(this->f, "LOGCHILD: Unknown log level!\n");
        }
        UnLock();
}

void Logger :: write_msg(LogLvlT log_lvl, int peer_id_1, int peer_id_2, uint32_t pcno)
{
	if(log_lvl > (this->max_lvl))
		return;

        time_t now = time(0);
   	char* date_time = ctime(&now);
        this->Lock();
        fprintf(this->f, "[%s]: Peer %d received the 'have' message from %d for the piece %d. \n", date_time, peer_id_1, peer_id_2, (uint32_t)pcno);
        this->UnLock();
}

void Logger :: write_msg(LogLvlT log_lvl, int peer_id_1, int peer_id_2, uint32_t pcno, int npcs)
{
	if(log_lvl > (this->max_lvl))
		return;
        time_t now = time(0);
   	char* date_time = ctime(&now);
        this->Lock();
        fprintf(this->f, "[%s]: Peer %d has downloaded the piece %d from %d. Now the number of pieces it has is %d. \n", date_time, peer_id_1, (uint32_t)pcno, peer_id_2, npcs);
        this->UnLock();
}


void Logger :: write_msg(LogLvlT log_lvl, int peer_id)
{
	if(log_lvl > (this->max_lvl))
		return;
        time_t now = time(0);
   	char* date_time = ctime(&now);
        this->Lock();
        fprintf(this->f, "[%s]: Peer %d has downloaded the complete file. \n", date_time, peer_id);
        this->UnLock();
}

void Logger :: write_msg(LogLvlT log_lvl, int peer_id, vector<int> peers)
{
        if(log_lvl > (this->max_lvl))
		return;
        time_t now = time(0);
   	char* date_time = ctime(&now);

        std::ostringstream oss;
        std::copy(peers.begin(), peers.end(), std::ostream_iterator<int>(oss, ";"));
        this->Lock();
        fprintf(this->f, "[%s]: Peer %id has the preferred neighbors %s. \n", date_time, peer_id, oss.str());
        this->UnLock();
}

