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

uint32_t handshake_handler(PktMsg &pkt)
{
        return pkt.hshake.peerid;
}

