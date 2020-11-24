#include<iostream>
#include<vector>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include"objstore/cache.h"
#include"logger/logger.h"
#include"objstore/objstore.h"

using objstore::ObjStore;
using objstore::Cache;
using logger::LogLvlT;
using logger::Logger;
using std::vector;
using std::string;

extern Logger *lvar;

ObjStore :: ObjStore(){}

ObjStore :: ObjStore(int fsz, int pcsz, string fname)
{
	this->fsz = fsz;
	this->npcs = (fsz % pcsz) ? ((fsz/pcsz) + 1) : (fsz/pcsz);
	this->cache = Cache(pcsz, this->npcs);
        this->fname = fname;
        this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
        int stat=0;
        if((stat = pthread_rwlock_init(&(this->rwlock), NULL)) != 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock Init: %s",
				strerror(stat));
                _exit(1);
        }
}

ObjStore :: ~ObjStore()
{
        int stat=0;
        if((stat = pthread_rwlock_destroy(&(this->rwlock))) != 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock Deinit: %s",
				strerror(stat));
                _exit(1);
        }
}

void ObjStore :: RdLock()
{
        int stat = 0;
        if((stat = pthread_rwlock_rdlock(&(this->rwlock))) != 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock RDLock: %s",
				strerror(stat));
                _exit(1);
        }
}

void ObjStore :: WrLock()
{
        int stat = 0;
        if((stat = pthread_rwlock_wrlock(&(this->rwlock))) != 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock WrLock: %s",
				strerror(stat));
                _exit(1);
        }
}

void ObjStore :: UnLock()
{
        int stat = 0;
        if((stat = pthread_rwlock_unlock(&(this->rwlock))) != 0) {
		lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock Unlock: %s",
				strerror(stat));
                _exit(1);
        }
}

void ObjStore :: bfield_init(bool hasfile)
{
	int wholes = this->npcs / 8;
	int lftovr = this->npcs % 8;
	this->bfield = vector<uint8_t>(wholes, (uint8_t)hasfile);
	if(lftovr && hasfile) {
		uint8_t mask = 0;
		int max = (8 - lftovr);
		for(int i=7; i>=max; i--)
			mask ^= ((uint8_t)1<<i);
		this->bfield.push_back(mask);
	} else if(lftovr) {
		this->bfield.push_back((uint8_t)0);
		char cmd[512];
		sprintf(cmd, "/usr/bin/head -c %d /dev/zero > %s",
					this->fsz, this->fname.c_str());
		system(cmd);
	}
	if(!(this->cache.file_open(this->fname)))
		_exit(1);
}

void ObjStore :: bfield_diff(vector<uint8_t>& right, vector<uint8_t>& diff)
{
        this->RdLock();
	vector<uint8_t> left = this->bfield;
	for(uint32_t i=0; i<left.size(); i++) {
		uint8_t mask = left[i] ^ (left[i] | right[i]);
		diff.push_back(mask);
	}
        this->UnLock();
}

/* Absolutely needs to be called with at least read lock set */
bool ObjStore :: bfield_exists(int pos)
{
	/* TODO: Make absolutely sure this method works */
	int wholes = pos / 8;
	int lftovr = pos % 8;
	uint8_t mask = 1<<lftovr;
        bool ret = false;

	if(this->bfield[wholes] & mask)
                ret=true;

        return ret;
}

/* Absolutely needs to be called with write lock set */
void ObjStore :: bfield_flip(int pos)
{
	int wholes = pos / 8;
	int lftovr = pos % 8;
	uint8_t mask = 1<<lftovr;

	this->bfield[wholes] ^= mask;
}

int ObjStore :: add_piece(int pcno, char *piece)
{
        int ret = 1;
        this->WrLock();
	if(this->bfield_exists(pcno)) {
		/* Piece already exists, return err */
                ret = 0;
                goto unlock;
        }

	if(!this->cache.put(pcno, piece)) {
                ret = 0;
                goto unlock;
        }

	this->bfield_flip(pcno);

unlock:
        this->UnLock();
	return ret;
}

int ObjStore :: get_piece(int pcno, char *buf)
{
        int ret = 1;
        this->WrLock();
	if(!this->bfield_exists(pcno)) {
		/* Piece cannot exist, return err */
                ret = 0;
                goto unlock;
        }
	if(!this->cache.get(pcno, buf))
                ret = 0;

unlock:
        this->UnLock();
	return ret;
}
