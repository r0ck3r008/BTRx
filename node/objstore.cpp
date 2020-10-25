#include<iostream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"logger/logger.h"
#include"node/objstore.h"

using logger::Logger;
using logger::LogMsgT;
using node::ObjStore;
using std::vector;
using std::string;

extern Logger *lvar;

ObjStore :: ObjStore(int fsz, int pcsz, string fname)
{
	this->fsz = fsz;
	this->pcsz = pcsz;
	this->npcs = (fsz % pcsz) ? ((fsz/pcsz) + 1) : (fsz/pcsz);
	this->fname = fname;
}

ObjStore :: ~ObjStore()
{
}

void ObjStore :: bfield_init(bool hasfile)
{
	if(hasfile) {
		int wholes = this->npcs / 64;
		int lftovr = this->npcs % 64;
		/*TODO:
		*Make sure that memory allocated for this vector
		*Doesn't go out of scope once this function ends.
		*/
		this->bfield = vector<uint64_t>(wholes, 1);
		if(lftovr) {
			uint64_t mask = 0;
			int max = (64 - lftovr);
			for(int i=63; i>=max; i--)
				mask ^ ((uint64_t)1<<i);
			this->bfield.push_back(mask);
		}
	} else {
		char cmd[512];
		sprintf(cmd, "/usr/bin/head -c %d /dev/zero > %s",
					this->fsz, this->fname.c_str());
		system(cmd);
	}
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
