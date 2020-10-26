#include<iostream>
#include<vector>
#include<unordered_map>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logger.h"
#include"node/objstore.h"

using logger::Logger;
using logger::LogMsgT;
using node::ObjStore;
using std::unordered_map;
using std::vector;
using std::string;

extern Logger *lvar;

ObjStore :: ObjStore(int fsz, int pcsz, string fname)
{
	this->fsz = fsz;
	this->pcsz = pcsz;
	this->npcs = (fsz % pcsz) ? ((fsz/pcsz) + 1) : (fsz/pcsz);
	this->fname = fname;
	this->cmap = unordered_map<int, vector<char *>::iterator&>();
	this->cvec = vector<char *>();
}

ObjStore :: ~ObjStore()
{
	close(this->fd);
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
			mask ^ ((uint64_t)1<<i);
		this->bfield.push_back(mask);
	} else if(lftovr) {
		this->bfield.push_back((uint64_t)0);
		char cmd[512];
		sprintf(cmd, "/usr/bin/head -c %d /dev/zero > %s",
					this->fsz, this->fname.c_str());
		system(cmd);
	}

	if((this->fd = open(this->fname.c_str(), O_RDWR)) < 0) {
		lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Open: %s",
					strerror(errno));
		_exit(1);
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

int ObjStore :: get_pos(int pcno)
{
	/* Converts the piece number to its starting position in file */
	return this->pcsz * pcno;
}

int ObjStore :: add_piece(int pcno, char *piece)
{
	if(this->bfield_exists(pcno))
		/* Piece already exists, return err */
		return 0;
	int pos = this->get_pos(pcno);
	if(lseek(this->fd, pos, SEEK_SET) < 0) {
		lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Lseek: %s",
				strerror(errno));
		_exit(1);
	}
	if(write(this->fd, piece, sizeof(char) * this->pcsz) < 0) {
		lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Write: %s",
				strerror(errno));
		_exit(1);
	}
	this->bfield_flip(pcno);
	/* TODO: Add to cache */

	return 1;
}

int ObjStore :: get_piece(int pcno, char *buf)
{
	if(this->bfield_exists(pcno))
		/* Piece cannot exist, return err */
		return 0;
	int pos = this->get_pos(pcno);
	auto itr = this->cmap.find(pos);
	if(itr != this->cmap.end()) {
		/* Cache hit */
	} else {
		/* Cache Miss */
		if(lseek(this->fd, pos, SEEK_SET) < 0) {
			lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Lseek: %s",
					strerror(errno));
			_exit(1);
		}
		if(read(this->fd, buf, sizeof(char) * this->pcsz) < 0) {
			lvar->write_msg(LogMsgT::LOG_ERR, "OBJSTORE: Read: %s",
					strerror(errno));
			_exit(1);
		}
		char *store = strndup(buf, sizeof(char) * this->pcsz);
		if(store == NULL) {
			lvar->write_msg(LogMsgT::LOG_ERR,
			"OBJSTORE: Malloc: Error in allocating memory!");
			_exit(1);
		}
		/* TODO: Add to cache */
	}

	return 1;
}
