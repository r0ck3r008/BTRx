#include<iostream>
#include<string.h>
#include<unistd.h>

#include"objstore/bfield.h"
#include"logger/logger.h"

using objstore::Bfield;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

Bfield :: Bfield()
{
        this->needlock = false;
        this->bfield = vector<uint8_t>();
}

Bfield :: Bfield(vector<uint8_t> &field)
{
        this->bfield = vector<uint8_t>(field);
        this->needlock = false;
}

Bfield :: Bfield(bool needlock, int npcs)
{
        this->bfield = vector<uint8_t>(npcs, 0);
        this->needlock = needlock;
}

Bfield :: Bfield(bool hasfile, bool needlock, int npcs)
{
	int wholes = npcs / 8;
	int lftovr = npcs % 8;
	this->bfield = vector<uint8_t>(wholes, (uint8_t)hasfile);
        if(lftovr) {
                if(hasfile) {
                        uint8_t mask = 0;
                        int max = (8 - lftovr);
                        for(int i=7; i>=max; i--)
                                mask ^= ((uint8_t)1<<i);
                        this->bfield.push_back(mask);
                } else {
                        this->bfield.push_back((uint8_t)0);
                }
        }
        this->needlock = needlock;
        if(this->needlock) {
                int stat = 0;
                this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
                if((stat = pthread_rwlock_init(&(this->rwlock), NULL)) != 0) {
                        lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock Init: %s",
                                        strerror(stat));
                        _exit(1);
                }
        }
}

Bfield :: ~Bfield()
{
        if(this->needlock) {
                int stat=0;
                if((stat = pthread_rwlock_destroy(&(this->rwlock))) != 0) {
                        lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock Deinit: %s",
                                        strerror(stat));
                        _exit(1);
                }
        }
}

void Bfield :: RdLock()
{
        if(this->needlock) {
                int stat = 0;
                if((stat = pthread_rwlock_rdlock(&(this->rwlock))) != 0) {
                        lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock RDLock: %s",
                                        strerror(stat));
                        _exit(1);
                }
        }
}

void Bfield :: WrLock()
{
        if(this->needlock) {
                int stat = 0;
                if((stat = pthread_rwlock_wrlock(&(this->rwlock))) != 0) {
                        lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock WrLock: %s",
                                        strerror(stat));
                        _exit(1);
                }
        }
}

void Bfield :: UnLock()
{
        if(this->needlock) {
                int stat = 0;
                if((stat = pthread_rwlock_unlock(&(this->rwlock))) != 0) {
                        lvar->write_msg(LogLvlT::LOG_DBG, "NODE: RWLock Unlock: %s",
                                        strerror(stat));
                        _exit(1);
                }
        }
}

void Bfield :: diff(vector<uint8_t>& right, vector<uint8_t>& diff)
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
bool Bfield :: exists(int pos)
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
void Bfield :: flip(int pos)
{
	int wholes = pos / 8;
	int lftovr = pos % 8;
	uint8_t mask = 1<<lftovr;

	this->bfield[wholes] ^= mask;
}
