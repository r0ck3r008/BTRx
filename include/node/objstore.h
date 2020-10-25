#ifndef OBJSTORE_H
#define OBJSTORE_H

#include<vector>

using std::vector;
using std::string;

namespace node {
	class ObjStore {
		int fsz;
		int pcsz;
		int npcs;
		string fname;
	public:
		vector<uint64_t> bfield;
	public:
		ObjStore(int, int, string);
		~ObjStore();
		void bfield_init(bool);
		void bfield_diff(vector<uint64_t>&,
				vector<uint64_t>&);
		bool bfield_exists(int);
		void bfield_flip(int);
	};
}

#endif
