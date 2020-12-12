#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>

#include"logger/logger.h"
#include"objstore/bfield.h"
#include"handler/handler.h"

using std::cout;
using std::endl;
using objstore::Bfield;
using nbrmap::Nbr;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

void choke_status_check(int sock, Nbr *nbr, bool choked_peer)
{
        /* choked tmp helps in setting getting and setting
         * choked status from nbr->choked in single read, which
         * is efficient since nbr->choked is a mutex operation */
        if(nbr != NULL) {
                bool choked_tmp = choked_peer;
                choked_peer = nbr->choked;
                if(choked_tmp != choked_peer)
                        send_choke_status(sock, choked_peer);
        }
}

void diff_to_reqs(unordered_map<uint32_t, bool> &reqs, vector<uint8_t> &diff)
{
        for(uint32_t i=0; i<diff.size(); i++) {
                for(uint32_t j=0; j<8; j++) {
                        uint8_t mask = (uint8_t)1 << j;
                        if((diff[i] & mask) == mask)
                                reqs[i*8 + j] = true;
                }
        }
}

/* TODO:
 * 1. Replace this with appropriate logger statements
 * 2. HAVE handler, possibly by mantaining a remote peer's bitfield and frequently checking
 *    if that piece becomes available.
 * 3. Absolutely check if bitfield makes sense.
 */
void cli_handler(int sock, int peerid_self, int peerid_peer, ObjStore *ost, NbrMap *nmap)
{
        bool client;
        if(peerid_peer == 0) {
                /* This is a server */
                cout << peerid_self << " connection accepted from " << peerid_peer << endl;
                client = true;
        } else {
                /* This is a client */
                cout << peerid_self << "connected to: " << peerid_peer << endl;
                client = false;
                send_handshake(sock, peerid_self);
                send_bfield(sock, ost);
        }

        uint32_t sz, peerid, ntimeouts = 0;
        bool interested, choked_node = false, choked_peer = true;
        Nbr *nbr = NULL;
        vector<uint8_t> diff, bfield_peer;
        unordered_map<uint32_t, bool> reqs;
        while(true) {
                if(!rcv_sz(sock, &sz)) {
                        close(sock);
                        break;
                } else if(sz == 0) {
                        /* timeout */
                        send_requests(sock, reqs);
                        ntimeouts++;
                        if(ntimeouts%2 == 0) {
                                /* Wait 2 times the timeout for HAVE check */
                                send_haves(sock, ost, bfield_peer);
                        } else if(ntimeouts == 5) {
                                break;
                        }
                        continue;
                }
                uint8_t cmdr[sz];
                rcv(sock, cmdr, sz);
                vector<uint8_t> bson = vector<uint8_t>(cmdr, cmdr + sz);
                json j = json::from_bson(bson);
                PktMsg pkt = j.get<PktMsg>();
                choke_status_check(sock, nbr, choked_peer);
                peerid = pkt.hshake.peerid;
                switch(pkt.type) {
                        case Handshake:                                
                                lvar->write_msg(LogLvlT::LOG_TCP_INIT, peerid_self, peerid);
                                send_handshake(sock, peerid_self);
                                nbr = nmap->register_cli(peerid);
                                lvar->write_msg(LogLvlT::LOG_TCP_FIN, peerid_self, peerid)
                                break;
                        case Choke:
                                choked_node = true;
                                lvar->write_msg(LogLvlT::LOG_CHK, peerid_self, peerid);
                                break;
                        case UnChoke:
                                choked_node = false;
                                lvar->write_msg(LogLvlT::LOG_UCHK, peerid_self, peerid);
                                send_requests(sock, reqs);
                                break;
                        case Interested:
                                nbr->interested = true;
                                lvar->write_msg(LogLvlT::LOG_INT, peerid_self, peerid);
                                break;
                        case NotInterested:
                                nbr->interested = false;
                                lvar->write_msg(LogLvlT::LOG_UINT, peerid_self, peerid);
                                break;
                        case Have:
                                lvar->write_msg(LogLvlT::LOG_UINT, peerid_self, peerid, pkt.piece.pcno);
                                handle_have(sock, &pkt, reqs);
                                break;
                        case BitField:
                                bfield_peer = vector<uint8_t>(pkt.bfield.bfield);
                                send_bfield(sock, ost);
                                if((interested = nmap->earmark(ost->bfield, bfield_peer,
                                                                                diff))) {
                                        send_interested(sock);
                                        diff_to_reqs(reqs, diff);
                                        if(!client && !choked_node)
                                                /* Send requests first if this is a client */
                                                send_requests(sock, reqs);
                                } else {
                                        send_not_interested(sock);
                                }
                                break;
                        case Request:
                                nbr->requests++;
                                if(!choked_peer)
                                        send_piece(sock, pkt.req.pcno, ost);
                                /* Send requests after sending pieces if not client */
                                if(!choked_node)
                                        send_requests(sock, reqs);
                                break;
                        case Piece:
                                lvar->write_msg(LogLvlT::LOG_UINT, peerid_self, peerid, pkt.piece.pcno, ost->npcs);
                                handle_piece(&pkt, ost, reqs);
                                break;
                        default:
                                _exit(1);
                }
        }

        /* exit here */
        nbr->done = true;
        close(sock);
}
