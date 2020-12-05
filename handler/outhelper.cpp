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

void snd(int sock, json &j)
{
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

void send_handshake(int sock, int peerid)
{
        PktMsg pkt = {.type = Handshake, .hshake = {"P2PFILESHARINGPROJ", {0}, (uint32_t)peerid}};
        json j = pkt;
        snd(sock, j);
}

void send_bfield(int sock, ObjStore *ost)
{
        vector<uint8_t> vec;
        ost->bfield->clone(vec);
        PktMsg pkt = {.type = BitField, .bfield = {vec}};
        json j = pkt;
        snd(sock, j);
}
