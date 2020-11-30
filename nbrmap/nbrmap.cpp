#include<iostream>
#include<algorithm>
#include<map>
#include<string.h>
#include<unistd.h>

#include"logger/logger.h"
#include"nbrmap/nbrmap.h"

using std::pair;
using std::greater;
using std::map;
using namespace nbrmap;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;
extern int uchoke_ival;
extern int opuchoke_ival;
extern int n_pref_peers;

NbrMap :: NbrMap(uint32_t npeers)
{
        this->peers = vector<int>();
        this->peerinfo = unordered_map<int, Nbr *>();
        this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
        this->npeers = npeers;
        int stat = 0;
        if((stat = pthread_rwlock_init(&(this->rwlock), NULL)) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: RWLock Initialize: %s",
                                strerror(stat));
                _exit(1);
        }

        this->handlers = vector<thread>();
        this->handlers.push_back(thread(opt_unchoke_handler, this, opuchoke_ival));
        this->handlers.push_back(thread(unchoke_handler, this, uchoke_ival, n_pref_peers));
}

NbrMap :: ~NbrMap()
{
        int stat = 0;
        for(auto &thr: this->handlers)
                thr.join();

        if((stat = pthread_rwlock_destroy(&(this->rwlock))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: RWLock Destroy: %s",
                                strerror(stat));
                _exit(1);
        }
}

void NbrMap :: RdLock()
{
        int stat = 0;
        if((stat = pthread_rwlock_rdlock(&(this->rwlock))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: RWLock RdLock: %s",
                                strerror(stat));
                _exit(1);
        }
}

void NbrMap :: WrLock()
{
        int stat = 0;
        if((stat = pthread_rwlock_wrlock(&(this->rwlock))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: RWLock WrLock: %s",
                                strerror(stat));
                _exit(1);
        }
}

void NbrMap :: UnLock()
{
        int stat = 0;
        if((stat = pthread_rwlock_unlock(&(this->rwlock))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: RWLock Unlock: %s",
                                strerror(stat));
                _exit(1);
        }
}

Nbr *NbrMap :: register_cli(int peerid)
{
        Nbr *nbr = new Nbr;
        nbr->choked = true;
        nbr->requests = 0;

        this->WrLock();
        this->peers.push_back(peerid);
        this->peerinfo.insert(pair<int, Nbr *>(peerid, nbr));
        this->UnLock();
        this->npeers--;

        return nbr;
}

void NbrMap :: opt_unchoke()
{
        /* Atomic Operation */
        if(this->npeers)
                /* Since all the clients have not been registered yet, return */
                return;

        int size;
        srand(time(NULL));
        Nbr *nbr;
        while(true) {
                size = this->peers.size();
                nbr = this->peerinfo[rand()%size];

                /* Atomic Operation */
                if(nbr->choked) {
                        nbr->choked = false;
                        break;
                }
        }
}

void NbrMap :: select_unchoked(int n_pref_peers)
{
        /* Atomic Operation */
        if(this->npeers)
                /* Since all the clients have not been registered yet, return */
                return;

        map<uint32_t, Nbr *, greater<uint32_t>> requests;
        for(auto &peer: this->peers) {
                Nbr *nbr = this->peerinfo[peer];
                requests.insert(pair<uint32_t, Nbr *>(nbr->requests, nbr));
        }

        auto itr = requests.begin();
        for(int i=0; i<n_pref_peers; i++, itr++)
                itr->second->choked = false;
}

void opt_unchoke_handler(NbrMap *nbrmap, int opuchoke_ival)
{
        std::chrono::milliseconds timespan(opuchoke_ival * 1000);
        while(1) {
                std::this_thread::sleep_for(timespan);
                nbrmap->opt_unchoke();
        }
}

void unchoke_handler(NbrMap *nbrmap, int uchoke_ival, int n_pref_peers)
{
        std::chrono::milliseconds timespan(uchoke_ival * 1000);
        while(1) {
                std::this_thread::sleep_for(timespan);
                nbrmap->select_unchoked(n_pref_peers);
        }
}
