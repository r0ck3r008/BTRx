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
        Nbr *nbr;
        Bfield bfield;
        vector<uint8_t> diff;
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
                                break;
                        case UnChoke:
                                break;
                        case Interested:
                                break;
                        case NotInterested:
                                break;
                        case Have:
                                break;
                        case BitField:
                                bfield = Bfield(pkt.bfield.bfield);
                                send_bfield(sock, ost);
                                nbr->interested = nmap->earmark(&bfield, diff);
                                break;
                        case Request:
                                break;
                        case Piece:
                                break;
                        default:
                                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Unknown msg!");
                                _exit(1);
                }
        }
}
