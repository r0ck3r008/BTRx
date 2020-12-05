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

void send_handshake(int sock)
{
        PktMsg pkt = {.type = Handshake, .hshake = {"P2PFILESHARINGPROJ", {0}, 1234}};
        json j = pkt;
        vector<uint8_t> bson = json::to_bson(j);
        uint8_t cmds[bson.size() + 1];
        explicit_bzero(cmds, bson.size() + 1);
        copy(bson.begin(), bson.end(), cmds);

        if(send(sock, cmds, bson.size() + 1, 0) < 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "HANDLER: Send: %s",
                                strerror(errno));
                _exit(1);
        }
}

