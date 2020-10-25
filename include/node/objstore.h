#ifndef OBJSTORE_H
#define OBJSTORE_H

#include<vector>

#include"node/bitfield.h"

using node::BitField;
using std::vector;
using std::string;

namespace node {
	class ObjStore {
		int fsz;
		int pcsz;
		int npcs;
		string fname;
		BitField *bfield;
	public:
		ObjStore(int, int, string);
		~ObjStore();
		void bfield_init(bool);
	};
}

#endif
