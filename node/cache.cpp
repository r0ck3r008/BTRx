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
#include"node/cache.h"

using node::Cache;
using node::Access;
using logger::Logger;
using logger::LogMsgT;
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

Cache :: Cache(int pcsz, int npcs)
{
	this->pcsz = pcsz;
	this->npcs = npcs;
	this->maxsz = CACHESZ/npcs;
}

Cache :: ~Cache()
{
	close(this->fd);
}

int Cache :: file_open(string fname)
{
	if((this->fd = open(fname.c_str(), O_RDWR)) < 0) {
		lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Open: %s",
					strerror(errno));
		_exit(1);
	}
}

int Cache :: get_pos(int pcno)
{
	/* Converts the piece number to its starting position in file */
	return this->pcsz * pcno;
}

void Cache :: update_cache(int pos, char *buf)
{
	auto itr = this->cvec.end();
	Access *acc = new Access(pos, buf),
		*acc_old = *itr;
	if(this->cvec.size() == this->maxsz) {
		/* erase Least Freq used */
		this->cvec.erase(itr);
		auto itr_map = this->cmap.find(acc_old->pos);
		this->cmap.erase(itr_map);
		delete acc_old;
	}
	/* Insert new Access */
	this->cvec.push_front(acc);
	auto itr_new = this->cvec.begin();
	this->cmap.insert(pair<int, deque<Access *>::iterator>(pos, itr_new));
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
			lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Lseek: %s",
					strerror(errno));
			return 0;
		}
		store = new char[sizeof(char) * this->pcsz];
		if(read(this->fd, store, sizeof(char) * this->pcsz) < 0) {
			lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Read: %s",
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
		lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Lseek: %s",
				strerror(errno));
		return 0;
	}
	if(write(this->fd, piece, sizeof(char) * this->pcsz) < 0) {
		lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Write: %s",
				strerror(errno));
		return 0;
	}

	return 1;
}
