#ifndef CACHE_H
#define CACHE_H

#include<deque>
#include<unordered_map>
#include<pthread.h>

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
		unordered_map<int, int> cmap;
		deque<Access *> cvec;
		int fd, pcsz, npcs, maxsz;
                pthread_rwlock_t rwlock;
	private:
		int get_pos(int);
		void update_cache(int, char *);
                void RdLock();
                void WrLock();
                void UnLock();
	public:
		Cache();
		Cache(int, int);
		~Cache();
		int file_open(string);
		void put(int, char *);
		void get(int, char *);
	};
}

#endif
