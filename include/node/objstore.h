#ifndef OBJSTORE_H
#define OBJSTORE_H

#include"defs.h"

#include<vector>
#include<set>
#include<unordered_map>

using std::vector;
using std::string;
using std::multiset;
using std::unordered_map;
using std::less;

namespace node {
	class ObjStore {
		int fsz, pcsz, npcs, fd;
		/* Actual Cache, maps piece number to piece value */
		unordered_map<int, char *> cmap;
		/* Maps piece number to its frequency */
		unordered_map<int, int> fmap;
		/* Min heap to store the frequency, not using an
		 * actual priority queue as we may need to do
		 * random removals
		 */
		multiset<int, std::less<int>> fque;
		string fname;
	public:
		vector<uint64_t> bfield;
	private:
		int get_pos(int);
	public:
		ObjStore(int, int, string);
		~ObjStore();
		/* BitField Related */
		void bfield_init(bool);
		void bfield_diff(vector<uint64_t>&,
				vector<uint64_t>&);
		bool bfield_exists(int);
		void bfield_flip(int);
		/* File Related */
		int add_piece(int, char *);
		int get_piece(int, char *);
	};
}

#endif
