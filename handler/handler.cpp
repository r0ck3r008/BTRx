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
                for(int j=7; j>=0; j--) {
                        uint8_t mask = (uint8_t)1 << j;
                        if((diff[i] & mask) == mask)
                                reqs[i*8 + j] = true;
                }
        }
}
/* TODO:
 * 1. Replace this with appropriate logger statements
 * 2. Add a timeout in rcv_sz, which when fails, its turn for other peer to send data
 * 3. Absolutely check if bitfield makes sense.
 */

void cli_handler(int sock, int peerid_self, struct sockaddr_in *addr, ObjStore *ost,
                                                NbrMap *nmap, bool client)
{
        if(client) {
                /* This is a server */
                cout << "Connection accepted from: " << inet_ntoa(addr->sin_addr) << endl;
        } else {
                /* This is a client */
                cout << "Connected to: " << inet_ntoa(addr->sin_addr) << endl;
                send_handshake(sock, peerid_self);
                send_bfield(sock, ost);
        }

        uint32_t sz, peerid;
        bool interested, choked_node = false, choked_peer = true;
        Nbr *nbr = NULL;
        Bfield bfield;
        vector<uint8_t> diff;
        unordered_map<uint32_t, bool> reqs;
        while(true) {
                sz = rcv_sz(sock);
                uint8_t cmdr[sz];
                rcv(sock, cmdr, sz);
                vector<uint8_t> bson = vector<uint8_t>(cmdr, cmdr + sz);
                json j = json::from_bson(bson);
                PktMsg pkt = j.get<PktMsg>();
                choke_status_check(sock, nbr, choked_peer);
                switch(pkt.type) {
                        case Handshake:
                                peerid = pkt.hshake.peerid;
                                send_handshake(sock, peerid_self);
                                nbr = nmap->register_cli(peerid);
                                break;
                        case Choke:
                                choked_node = true;
                                break;
                        case UnChoke:
                                choked_node = false;
                                send_requests(sock, reqs);
                                break;
                        case Interested:
                                nbr->interested = true;
                                break;
                        case NotInterested:
                                nbr->interested = false;
                                break;
                        case Have:
                                break;
                        case BitField:
                                bfield = Bfield(pkt.bfield.bfield);
                                send_bfield(sock, ost);
                                if((interested = nmap->earmark(&bfield, ost->bfield, diff))) {
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
                                handle_piece(&pkt, ost, reqs);
                                break;
                        default:
                                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Unknown msg!");
                                _exit(1);
                }
        }
}
