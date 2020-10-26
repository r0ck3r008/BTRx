#include<iostream>
#include<vector>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include"logger/logger.h"
#include"node/cache.h"
#include"node/objstore.h"

using logger::Logger;
using logger::LogMsgT;
using node::ObjStore;
using node::Cache;
using std::vector;
using std::string;

extern Logger *lvar;

ObjStore :: ObjStore(int fsz, int pcsz, string fname)
{
	this->fsz = fsz;
	this->npcs = (fsz % pcsz) ? ((fsz/pcsz) + 1) : (fsz/pcsz);
	this->cache = new Cache(pcsz, this->npcs);
}

ObjStore :: ~ObjStore()
{
	delete this->cache;
}

void ObjStore :: bfield_init(bool hasfile)
{
	int wholes = this->npcs / 64;
	int lftovr = this->npcs % 64;
	this->bfield = vector<uint64_t>(wholes, (uint64_t)hasfile);
	if(lftovr && hasfile) {
		uint64_t mask = 0;
		int max = (64 - lftovr);
		for(int i=63; i>=max; i--)
			mask ^= ((uint64_t)1<<i);
		this->bfield.push_back(mask);
	} else if(lftovr) {
		this->bfield.push_back((uint64_t)0);
		char cmd[512];
		sprintf(cmd, "/usr/bin/head -c %d /dev/zero > %s",
					this->fsz, this->fname.c_str());
		system(cmd);
	}
	if(!(this->cache->file_open(this->fname)))
		_exit(1);
}

void ObjStore :: bfield_diff(vector<uint64_t>& right, vector<uint64_t>& diff)
{
	vector<uint64_t> left = this->bfield;
	for(int i=0; i<left.size(); i++) {
		uint64_t mask = left[i] ^ (left[i] | right[i]);
		diff.push_back(mask);
	}
}

bool ObjStore :: bfield_exists(int pos)
{
	/* TODO: Make absolutely sure this method works */
	int wholes = pos / 64;
	int lftovr = pos % 64;
	uint64_t mask = 1<<lftovr;
	if(this->bfield[wholes] & mask)
		return true;
	else
		return false;
}

void ObjStore :: bfield_flip(int pos)
{
	int wholes = pos / 64;
	int lftovr = pos % 64;
	uint64_t mask = 1<<lftovr;
	this->bfield[wholes] ^= mask;
}

int ObjStore :: add_piece(int pcno, char *piece)
{
	if(this->bfield_exists(pcno))
		/* Piece already exists, return err */
		return 0;

	if(!this->cache->put(pcno, piece))
		_exit(1);

	this->bfield_flip(pcno);
	return 1;
}

int ObjStore :: get_piece(int pcno, char **buf)
{
	if(this->bfield_exists(pcno))
		/* Piece cannot exist, return err */
		return 0;

	if(!this->cache->get(pcno, buf))
		_exit(1);

	return 1;
}
