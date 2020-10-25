#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<vector>

#include"node/bitfield.h"
#include"logger/logger.h"

using logger::Logger;
using logger::LogMsgT;
using node::BitField;
using std::vector;
using std::string;

extern Logger *lvar;

BitField :: BitField(bool hasfile, int npcs, int fsz, const char *fname)
{
	if(hasfile) {
		int wholes = npcs / 64;
		int lftovr = npcs % 64;
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
		sprintf(cmd, "/usr/bin/head -c %d /dev/zero > %s", fsz, fname);
		system(cmd);
	}
}

void BitField :: diff(vector<uint64_t>& right, vector<uint64_t>& diff)
{
	vector<uint64_t> left = this->bfield;
	for(int i=0; i<left.size(); i++) {
		uint64_t mask = left[i] ^ (left[i] | right[i]);
		diff.push_back(mask);
	}
}

bool BitField :: exists(int pos)
{
	int wholes = pos / 64;
	int lftovr = pos % 64;
	uint64_t mask = 1<<lftovr;
	if(this->bfield[wholes] & mask)
		return true;
	else
		return false;
}

void BitField :: flip(int pos)
{
	int wholes = pos / 64;
	int lftovr = pos % 64;
	uint64_t mask = 1<<lftovr;
	this->bfield[wholes] ^= mask;
}
