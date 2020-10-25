#ifndef BFIELD_H
#define BFIELD_H

#include<vector>
#include<stdint.h>

using std::vector;

namespace node {
	class BitField {
	public:
		vector<uint64_t> bfield;
		BitField(bool, int, int, const char *);
		~BitField();
		void diff(vector<uint64_t>&,
				vector<uint64_t>&);
		bool exists(int);
		void flip(int);
	};
}

#endif
