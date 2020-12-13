#include<iostream>
#include<string.h>
#include<unistd.h>

#include"objstore/bfield.h"
#include"logger/logger.h"

using objstore::Bfield;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

/* Default constructor */
Bfield :: Bfield()
{
        this->needlock = false;
        this->bfield = vector<uint8_t>();
}

/* Used in handler.cpp */
Bfield :: Bfield(vector<uint8_t> &field)
{
        this->bfield = vector<uint8_t>(field);
        this->needlock = false;
}

/* Used in nbrmap.cpp */
Bfield :: Bfield(bool needlock, int npcs)
{
        this->bfield = vector<uint8_t>(npcs, 0);
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

/*
* 1. Used in objmap.cpp after call from Node::connback in node.cpp
* 2. Higher bit is high piece number in the bitfield
* 8 7 6 5 4 3 2 1 (a single uint8_t bfield)
* 0 0 0 0 1 1 1 1
*/
Bfield :: Bfield(bool hasfile, bool needlock, int npcs)
{
	int wholes = npcs / 8;
	int lftovr = npcs % 8;
	this->bfield = vector<uint8_t>(wholes, (uint8_t)hasfile);
        if(lftovr) {
                if(hasfile) {
                        uint8_t mask = 0;
                        for(int i=0; i<8; i++)
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
	vector<uint8_t> left = vector<uint8_t>(this->bfield);
        this->UnLock();

	for(uint32_t i=0; i<left.size(); i++) {
		uint8_t mask = left[i] ^ (left[i] | right[i]);
		diff.push_back(mask);
	}
}

bool Bfield :: exists(int pos)
{
	/* TODO: Make absolutely sure this method works */
	int wholes = pos / 8;
	int lftovr = pos % 8;
	uint8_t mask = 1<<lftovr;
        bool ret = false;

        this->RdLock();
	if(this->bfield[wholes] & mask)
                ret=true;
        this->UnLock();

        return ret;
}

/* Used by NbrMap earmark, this helps avoid multiple divisions during earmark process */
bool Bfield :: flip(int wholes, int lftover)
{
	uint8_t mask = (uint8_t)1<<lftover;
        bool ret = false;

        this->WrLock();
        if((this->bfield[wholes] & mask) != mask) {
                this->bfield[wholes] ^= mask;
                ret = true;
        }
        this->UnLock();

        return ret;
}

/* Used by objstore.cpp in add_piece function */
void Bfield :: flip(int pcno)
{
        int wholes = pcno / 8;
        int lftover = pcno % 8;
	uint8_t mask = 1<<lftover;

        this->WrLock();
        if((this->bfield[wholes] & mask) != mask)
                this->bfield[wholes] ^= mask;
        this->UnLock();
}

void Bfield :: clone(vector<uint8_t> &vec)
{
        this->RdLock();
        vec = vector<uint8_t>(this->bfield);
        this->UnLock();
}
