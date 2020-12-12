#include<iostream>
#include<vector>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#include"pkts/pkts.h"
#include"json/json.hpp"
#include"logger/logger.h"
#include"handler/handler.h"

using std::vector;
using logger::LogLvlT;
using logger::Logger;

extern Logger *lvar;

bool rcv_sz(int sock, uint32_t *sz)
{
        uint8_t cmdr[4] = {0};
        int stat = recv(sock, cmdr, 4, MSG_PEEK);
        if(stat == EWOULDBLOCK) {
                *sz = 0;
                return true;
        } else if(stat == 0) {
                return false;
        } else if(stat < 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Recv: %s",
                                strerror(errno));
                _exit(1);
        }

        *sz = *((uint32_t *)cmdr);

        return true;
}

void rcv(int sock, uint8_t *cmdr, uint32_t sz)
{
        if(recv(sock, cmdr, sz , 0) < 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Recv: %s",
                                strerror(errno));
                _exit(1);
        }
}

void handle_piece(PktMsg *pkt, ObjStore *ost, unordered_map<uint32_t, bool> &reqs)
{
        uint32_t pcno = pkt->piece.pcno;
        size_t pcsz = ost->pcsz * sizeof(char);
        char piece[pcsz];
        snprintf(piece, pcsz, "%s", pkt->piece.payload.c_str());

        ost->add_piece(pcno, piece);

        if(reqs.find(pcno) != reqs.end())
                reqs.erase(pcno);
}

void handle_have(int sock, PktMsg *pkt, unordered_map<uint32_t, bool> &reqs)
{
        auto itr = reqs.find(pkt->have.pcno);
        if(itr != reqs.end()) {
                send_interested(sock);
                PktMsg pkt_out = {.type = Request, .req = {pkt->have.pcno}};
                json j = pkt_out;
                snd(sock, j);
        }
}
