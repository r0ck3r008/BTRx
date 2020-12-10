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

void cli_handler(int sock, int peerid_self, struct sockaddr_in *addr, ObjStore *ost,
                                                NbrMap *nmap, bool client)
{
        /* TODO: Replace this with appropriate logger statements */
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
        bool interested, choked_node = false, choked_peer = true, choked_tmp = choked_peer;
        Nbr *nbr;
        Bfield bfield;
        vector<uint8_t> diff;
        vector<uint32_t> reqs;
        while(true) {
                sz = rcv_sz(sock);
                uint8_t cmdr[sz];
                rcv(sock, cmdr, sz);
                vector<uint8_t> bson = vector<uint8_t>(cmdr, cmdr + sz);
                json j = json::from_bson(bson);
                PktMsg pkt = j.get<PktMsg>();
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
                                        if(!client && !choked_node)
                                                /* Send requests first if this is a client */
                                                send_requests(sock, diff);
                                } else {
                                        send_not_interested(sock);
                                }
                                break;
                        case Request:
                                nbr->requests += pkt.req.pcs.size();
                                /* choked tmp helps in setting getting and setting
                                 * choked status from nbr->choked in single read, which
                                 * is efficient since nbr->choked is a mutex operation */
                                choked_tmp = choked_peer;
                                choked_peer = nbr->choked;
                                if(choked_tmp != choked_peer)
                                        send_choke_status(sock, choked_peer);
                                if(!choked_peer)
                                        send_pieces(pkt.req.pcs);
                                /* Send requests after sending pieces if not client */
                                if(!choked_node)
                                        send_requests(sock, diff);
                                break;
                        case Piece:
                                break;
                        default:
                                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Unknown msg!");
                                _exit(1);
                }
        }
}
