#ifndef OBJSTORE_H
#define OBJSTORE_H

#include<vector>
#include<pthread.h>

#include"objstore/cache.h"
#include"objstore/bfield.h"

using objstore::Cache;
using objstore::Bfield;
using std::vector;
using std::string;

namespace objstore {
	class ObjStore {
		int fsz, npcs;
		string fname;
		Cache cache;

		int get_pos(int);
	public:
                Bfield *bfield;
                /* This exists as client thread
                 * needs this in order to form a
                 * buffer to fetch the piece */
                int pcsz;

		ObjStore();
		ObjStore(int, int, int, string);
		~ObjStore();
                void bfield_init(bool);
		/* File Related */
		int add_piece(int, char *);
		int get_piece(int, char *);
	};
}

#endif
