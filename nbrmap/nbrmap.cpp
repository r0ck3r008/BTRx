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

NbrMap :: NbrMap(uint32_t npeers, int npcs)
{
        this->peers = vector<int>();
        this->peerinfo = unordered_map<int, Nbr *>();
        this->mut = PTHREAD_MUTEX_INITIALIZER;
        this->npeers = npeers;
        this->bfield = Bfield(true, npcs);
        int stat = 0;
        if((stat = pthread_mutex_init(&(this->mut), NULL)) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: Lock Initialize: %s",
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
        /* TODO
         * Check if it will block here during shutdown
         */
        for(auto &thr: this->handlers)
                thr.join();

        if((stat = pthread_mutex_destroy(&(this->mut))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: Lock Destroy: %s",
                                strerror(stat));
                _exit(1);
        }
}

void NbrMap :: Lock()
{
        int stat = 0;
        if((stat = pthread_mutex_lock(&(this->mut))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: Lock: %s",
                                strerror(stat));
                _exit(1);
        }
}

void NbrMap :: Unlock()
{
        int stat = 0;
        if((stat = pthread_mutex_unlock(&(this->mut))) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NBRMAP: Unlock: %s",
                                strerror(stat));
                _exit(1);
        }
}

Nbr *NbrMap :: register_cli(int peerid)
{
        Nbr *nbr = new Nbr;
        nbr->choked = false;
        nbr->done = false;
        nbr->interested = false;
        nbr->requests = 0;

        this->Lock();
        this->peers.push_back(peerid);
        this->peerinfo.insert(pair<int, Nbr *>(peerid, nbr));
        this->Unlock();
        this->npeers--;

        return nbr;
}

bool NbrMap :: opt_unchoke()
{
        /* Atomic Operation */
        if(this->npeers)
                /* Since all the clients have not been registered yet, return */
                return true;

        int size;
        srand(time(NULL));
        Nbr *nbr;
        bool ret = false;
        while(true) {
                size = this->peers.size();
                nbr = this->peerinfo[rand()%size];

                /* Atomic Operation */
                if(nbr->choked && nbr->interested && !nbr->done) {
                        nbr->choked = false;
                        ret = true;
                        break;
                }
        }

        return ret;
}

bool NbrMap :: select_unchoked(int n_pref_peers)
{
        /* Atomic Operation */
        if(this->npeers)
                /* Since all the clients have not been registered yet, return, but
                 * return true since it is yet not time for the thread to exit
                 */
                return true;

        map<uint32_t, Nbr *, greater<uint32_t>> requests;
        for(auto &peer: this->peers) {
                Nbr *nbr = this->peerinfo[peer];
                if(!nbr->done)
                        requests.insert(pair<uint32_t, Nbr *>(nbr->requests, nbr));
        }

        if(requests.empty())
                return false;

        int j = 0;
        for(auto &itr: requests) {
                if(itr.second->choked && itr.second->interested) {
                        itr.second->choked = false;
                        j++;
                }
                if(j==n_pref_peers)
                        break;
        }

        return true;
}

bool NbrMap :: earmark(Bfield *bfield_node, vector<uint8_t> &bfield_peer,
                                                        vector<uint8_t> &diff)
{
        bfield_node->diff(bfield_peer, diff);
        bool ret = false;
        for(uint32_t i=0; i<diff.size(); i++) {
                for(uint32_t j=0; j<8; j++) {
                        uint8_t mask = (uint8_t)1 << j;
                        if((diff[i] & mask) == mask) {
                                if((ret = this->bfield.flip(i, j)) == false)
                                        /* If incase earmark fails, remove the bit
                                         * from the diff so that the node does not request it
                                         */
                                        diff[i] ^= mask;
                        }
                }
        }

        return ret;
}

void nbrmap::opt_unchoke_handler(NbrMap *nbrmap, int opuchoke_ival)
{
        std::chrono::milliseconds timespan(opuchoke_ival * 1000);
        while(1) {
                std::this_thread::sleep_for(timespan);
                if(!nbrmap->opt_unchoke())
                        break;
        }
}

void nbrmap::unchoke_handler(NbrMap *nbrmap, int uchoke_ival, int n_pref_peers)
{
        std::chrono::milliseconds timespan(uchoke_ival * 1000);
        while(1) {
                std::this_thread::sleep_for(timespan);
                if(!nbrmap->select_unchoked(n_pref_peers))
                        break;
        }
}
