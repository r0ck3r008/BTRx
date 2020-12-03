#ifndef NBRMAP
#define NBRMAP

#include<unordered_map>
#include<pthread.h>
#include<atomic>
#include<thread>

using std::unordered_map;
using std::thread;
using std::atomic;

namespace nbrmap {

        struct Nbr {
                atomic<bool> choked;
                atomic<uint32_t> requests;
        };

        class NbrMap {
                pthread_mutex_t mut;
                atomic<uint32_t> npeers;
                vector<int> peers;
                unordered_map<int, Nbr *> peerinfo;
                vector<thread> handlers;

        private:
                void Lock();
                void Unlock();

        public:
                NbrMap(uint32_t);
                ~NbrMap();
                Nbr *register_cli(int);
                void opt_unchoke();
                void select_unchoked(int);
        };

        void opt_unchoke_handler(NbrMap *, int);
        void unchoke_handler(NbrMap *, int, int);
}

#endif
