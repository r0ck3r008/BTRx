#include<iostream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"logger/logger.h"
#include"node/bitfield.h"
#include"node/objstore.h"

using logger::Logger;
using logger::LogMsgT;
using node::ObjStore;
using node::BitField;
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

void ObjStore :: bfield_init(bool hasfile)
{
	this->bfield = new BitField(hasfile, this->npcs, this->fsz,
				this->fname.c_str());
}

ObjStore :: ~ObjStore()
{
	delete this->bfield;
}
