#ifndef CACHE_H
#define CACHE_H

#include<deque>
#include<unordered_map>

using std::deque;
using std::unordered_map;
using std::string;

namespace objstore {
	struct Access {
		int pos;
		char *buf;
		Access(int, char *);
		~Access();
	};

	class Cache {
		unordered_map<int, deque<Access *>::iterator> cmap;
		deque<Access *> cvec;
		int fd, pcsz, npcs, maxsz;
	private:
		int get_pos(int);
		void update_cache(int, char *);
	public:
		Cache();
		Cache(int, int);
		~Cache();
		int file_open(string);
		int put(int, char *);
		int get(int, char **);
	};
}

#endif
