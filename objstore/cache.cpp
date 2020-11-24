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
}

Cache :: ~Cache()
{
	close(this->fd);
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
                if(new_acc == NULL) {
                        lvar->write_msg(LogLvlT::LOG_ERR, "CACHE: Malloc");
                        _exit(1);
                }
                if(this->cvec.size() == this->maxsz) {
                        // Evict the least used
                        Access *old_acc = this->cvec.front();
                        this->cvec.pop_front();
                        this->cmap.erase(old_acc->pos);
                }
                this->cmap.insert(pair<int, int>(pos, this->cvec.size()));
                this->cvec.push_back(new_acc);
        }
}

int Cache :: get(int pcno, char **buf)
{
	int pos = this->get_pos(pcno);
	auto itr =  this->cmap.find(pos);
	char *store;
	if(itr != this->cmap.end()) {
		/* Cache Hit */
		Access *acc = *(itr->second);
		store = acc->buf;
		/* Remove from current position */
		this->cvec.erase(itr->second);
		this->cmap.erase(itr);
		/* Push to front */
		this->cvec.push_front(acc);
		this->cmap.insert(pair<int, deque<Access *>::iterator>
						(pos, this->cvec.begin()));
	} else {
		/* Cache Miss */
		if(lseek(this->fd, pos, SEEK_SET) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Lseek: %s",
					strerror(errno));
			return 0;
		}
		store = new char[sizeof(char) * this->pcsz];
		if(read(this->fd, store, sizeof(char) * this->pcsz) < 0) {
			lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Read: %s",
					strerror(errno));
			return 0;
		}
		this->update_cache(pos, store);
	}
	*buf = store;
	return 1;
}

int Cache :: put(int pcno, char *piece)
{
	int pos = this->get_pos(pcno);
	if(lseek(this->fd, pos, SEEK_SET) < 0) {
		lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Lseek: %s",
				strerror(errno));
		return 0;
	}
	if(write(this->fd, piece, sizeof(char) * this->pcsz) < 0) {
		lvar->write_msg(LogLvlT::LOG_ERR, "OBJSTORE: Write: %s",
				strerror(errno));
		return 0;
	}

	return 1;
}
