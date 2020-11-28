#include<iostream>
#include<deque>
#include<unordered_map>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include"logger/logger.h"
#include"defs.h"
#include"objstore/cache.h"

using objstore::Cache;
using objstore::Access;
using logger::Logger;
using logger::LogLvlT;
using std::deque;
using std::unordered_map;
using std::pair;

extern Logger *lvar;

Access :: Access(int pos, char *buf)
{
	this->pos = pos;
	this->buf = buf;
}

Access :: ~Access()
{
	delete[] this->buf;
}

Cache :: Cache() {}

Cache :: Cache(int pcsz, int npcs)
{
	this->pcsz = pcsz;
	this->npcs = npcs;
	this->maxsz = CACHESZ/npcs;
        this->cvec = deque<Access *>();
        this->cmap = unordered_map<int, int>();
        this->mut = PTHREAD_MUTEX_INITIALIZER;
        int stat = 0;
        if((stat = pthread_mutex_init(&(this->mut), NULL)) != 0) {
                lvar->write_msg(LogLvlT::LOG_DBG, "CACHE: Lock Init: %s",
                                strerror(stat));
                _exit(1);
        }
}

Cache :: ~Cache()
{
        int stat = 0;
        if((stat = pthread_mutex_destroy(&(this->mut))) != 0) {
                lvar->write_msg(LogLvlT::LOG_DBG, "CACHE: Lock Init: %s",
                                strerror(stat));
                _exit(1);
        }
	close(this->fd);
}

void Cache :: Lock()
{
        int stat = 0;
        if((stat = pthread_mutex_lock(&(this->mut))) != 0) {
                lvar->write_msg(LogLvlT::LOG_DBG, "CACHE: Lock: %s",
                                strerror(stat));
                _exit(1);
        }
}

void Cache :: UnLock()
{
        int stat = 0;
        if((stat = pthread_mutex_unlock(&(this->mut))) != 0) {
                lvar->write_msg(LogLvlT::LOG_DBG, "CACHE: UnLock: %s",
                                strerror(stat));
                _exit(1);
        }
}

int Cache :: file_open(string fname)
{
	if((this->fd = open(fname.c_str(), O_RDWR)) < 0) {
		lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Open: %s",
					strerror(errno));
		return 0;
	}
	return 1;
}

int Cache :: get_pos(int pcno)
{
	/* Converts the piece number to its starting position in file */
	return this->pcsz * pcno;
}

void Cache :: update_cache(int pos, char *buf)
{
        auto itr = this->cmap.find(pos);
        if(itr != this->cmap.end()) {
                Access *old_acc = this->cvec.front();
                /* Already exists */
		/* Remove from current position */
		this->cvec.erase(this->cvec.begin() + itr->second);
		this->cmap.erase(itr);
		/* Push to back */
		this->cmap.insert(pair<int, int>(pos, this->cvec.size()));
		this->cvec.push_back(old_acc);
        } else {
                // Not found
                Access *new_acc = new Access(pos, buf);
                if(this->cvec.size() == (uint32_t)this->maxsz) {
                        // Evict the least used
                        Access *old_acc = this->cvec.front();
                        this->cvec.pop_front();
                        this->cmap.erase(old_acc->pos);
                }
                this->cmap.insert(pair<int, int>(pos, this->cvec.size()));
                this->cvec.push_back(new_acc);
        }
}

int Cache :: get(int pcno, char *buf)
{
	int pos = this->get_pos(pcno), ret = 1;

        this->Lock();
	auto itr =  this->cmap.find(pos);
	char *store;
	if(itr != this->cmap.end()) {
		/* Cache Hit */
		Access *acc = this->cvec[itr->second];
		store = acc->buf;
	} else {
		/* Cache Miss */
		if(lseek(this->fd, pos, SEEK_SET) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Lseek: %s",
					strerror(errno));
                        ret = 0;
                        goto unlock;
		}
		store = new char[sizeof(char) * this->pcsz];
		if(read(this->fd, store, sizeof(char) * this->pcsz) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Read: %s",
					strerror(errno));
                        ret = 0;
                        goto unlock;
		}
	}
        this->update_cache(pos, store);
        strncpy(buf, store, sizeof(char) * this->pcsz);

unlock:
        this->UnLock();
	return ret;
}

int Cache :: put(int pcno, char *piece)
{
	int pos = this->get_pos(pcno), ret = 1;

        this->Lock();
	if(lseek(this->fd, pos, SEEK_SET) < 0) {
		lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Lseek: %s",
				strerror(errno));
                ret = 0;
                goto unlock;
	}
	if(write(this->fd, piece, sizeof(char) * this->pcsz) < 0) {
		lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Write: %s",
				strerror(errno));
                ret = 0;
                goto unlock;
	}
        this->update_cache(pos, piece);

unlock:
        this->UnLock();
	return ret;
}
