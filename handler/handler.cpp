#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>

#include"logger/logger.h"
#include"handler/handler.h"

using std::cout;
using std::endl;
using std::copy;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

void cli_handler(int sock, struct sockaddr_in *addr, ObjStore *objstore,
                                                NbrMap *nmap, bool client)
{
        /* TODO: Replace this with appropriate logger statements */
        if(client) {
                cout << "Connection accepted from: ";
        } else {
                cout << "Connected to: ";
                send_handshake(sock);
        }

        uint32_t sz, peerid;
        while(true) {
                sz = rcv_sz(sock);
                uint8_t cmdr[sz];
                rcv(sock, cmdr, sz);
                vector<uint8_t> bson = vector<uint8_t>(cmdr, cmdr + sz);
                json j = json::from_bson(bson);
                PktMsg pkt = j.get<PktMsg>();
                switch(pkt.type) {
                        case Handshake:
                                peerid = handshake_handler(pkt);
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
