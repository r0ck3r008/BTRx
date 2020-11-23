#ifndef OBJSTORE_H
#define OBJSTORE_H

#include<vector>

#include"objstore/cache.h"

using objstore::Cache;
using std::vector;
using std::string;

namespace objstore {
	class ObjStore {
		int fsz, npcs;
		string fname;
		Cache cache;
	public:
		vector<uint8_t> bfield;
	private:
		int get_pos(int);
	public:
		ObjStore();
		ObjStore(int, int, string);
		~ObjStore();
		/* BitField Related */
		void bfield_init(bool);
		void bfield_diff(vector<uint8_t>&,
				vector<uint8_t>&);
		bool bfield_exists(int);
		void bfield_flip(int);
		/* File Related */
		int add_piece(int, char *);
		int get_piece(int, char **);
	};
}

#endif
