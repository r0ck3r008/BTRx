#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>

#include"logger/logger.h"
#include"pkts/pkts.h"
#include"handler/handler.h"

using std::cout;
using std::endl;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

uint32_t rcv_sz(int sock)
{
        uint8_t cmdr[4] = {0};
        if(recv(sock, cmdr, 4 , 0) < 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Recv: %s",
                                strerror(errno));
                _exit(1);
        }

        uint32_t sz = *((uint32_t *)cmdr);
        return sz;
}

void rcv(int sock, uint8_t *cmdr, uint32_t sz)
{
        if(recv(sock, cmdr, sz , 0) < 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Recv: %s",
                                strerror(errno));
                _exit(1);
        }
}

void cli_handler(int sock, struct sockaddr_in *addr, ObjStore *objstore,
                                                NbrMap *nmap, bool client)
{
        /* TODO: Replace this with appropriate logger statements */
        if(client)
                cout << "Connection accepted from: ";
        else
                cout << "Connected to: ";

        uint32_t sz;
        while(true) {
                sz = rcv_sz(sock);
                uint8_t cmdr[sz];
                rcv(sock, cmdr, sz);
                vector<uint8_t> bson = vector<uint8_t>(cmdr, cmdr + sz);
                json j = json::from_bson(bson);
                PktMsg pkt = j.get<PktMsg>();
                switch(pkt.type) {
                        case Handshake:
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
