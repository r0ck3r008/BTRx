#include<iostream>
#include<vector>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#include"pkts/pkts.h"
#include"logger/logger.h"
#include"handler/handler.h"

using std::vector;
using logger::LogLvlT;
using logger::Logger;

extern Logger *lvar;

void snd(int sock, json &j)
{
        vector<uint8_t> bson = json::to_bson(j);
        uint8_t cmds[bson.size()];
        memset(cmds, 0, bson.size());
        copy(bson.begin(), bson.end(), cmds);

        if(send(sock, cmds, bson.size(), 0) < 0) {
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

void send_interested(int sock)
{
        PktMsg pkt = {.type = Interested};
        json j = pkt;
        snd(sock, j);
}

void send_not_interested(int sock)
{
        PktMsg pkt = {.type = NotInterested};
        json j = pkt;
        snd(sock, j);
}

void send_requests(int sock, unordered_map<uint32_t, bool> &reqs)
{
        for(auto pcno: reqs) {
                PktMsg pkt = {.type = Request, .req = {pcno.first}};
                json j = pkt;
                snd(sock, j);
        }
}

void send_piece(int sock, uint32_t pcno, ObjStore *ost)
{
        char buf[ost->pcsz];
        ost->get_piece(pcno, buf);
        string piece(buf);

        PktMsg pkt = {.type = Piece, .piece = {.pcno = pcno, .payload = piece}};
        json j = pkt;
        snd(sock, j);
}

void send_choke_status(int sock, bool choke)
{
        PktMsg pkt;
        if(choke)
                pkt = {.type = Choke};
        else
                pkt = {.type = UnChoke};

        json j = pkt;
        snd(sock, j);
}

void send_haves(int sock, ObjStore *ost, vector<uint8_t> &bfield_peer)
{
        uint32_t pcno;
        uint8_t mask;
        for(uint32_t i=0; i<bfield_peer.size(); i++) {
                for(uint32_t j=0; j<8; j++) {
                        mask = (uint8_t)1 << j;
                        pcno = i*8 + j;
                        if((bfield_peer[i] & mask) == mask && ost->exists(pcno)) {
                                PktMsg pkt = {.type = Have, .have = {pcno}};
                                json j = pkt;
                                snd(sock, j);
                        }
                }
        }
}
