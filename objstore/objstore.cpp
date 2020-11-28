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
        this->bfield = NULL;
}

ObjStore :: ~ObjStore()
{
        delete this->bfield;
}

void ObjStore :: bfield_init(bool hasfile)
{
        this->bfield = new Bfield(hasfile, true, this->npcs);
        if(!hasfile) {
                char cmd[512];
                sprintf(cmd, "/usr/bin/head -c %d /dev/zero > %s",
                                        this->fsz, this->fname.c_str());
                system(cmd);
        }

	if(!(this->cache.file_open(this->fname)))
		_exit(1);
}

int ObjStore :: add_piece(int pcno, char *piece)
{
	if(this->bfield->exists(pcno))
		/* Piece already exists, return err */
                return 0;

	if(!this->cache.put(pcno, piece))
                return 0;

	this->bfield->flip(pcno);

        return 1;
}

int ObjStore :: get_piece(int pcno, char *buf)
{
	if(!this->bfield->exists(pcno))
		/* Piece cannot exist, return err */
                return 0;

	if(!this->cache.get(pcno, buf))
                return 0;

        return 1;;
}
