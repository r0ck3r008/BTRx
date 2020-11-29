#include<iostream>
#include<string.h>
#include<unistd.h>

#include"logger/logger.h"
#include"node/pkts.h"

using logger::Logger;
using logger::LogLvlT;
using node::PktHandshake;
using node::PktMessage;
using node::MessageType;

extern Logger *lvar;

PktHandshake :: PktHandshake(uint32_t peerid)
{
        sprintf(this->header, "P2PFILESHARINGPROJ");
        explicit_bzero(this->zero, sizeof(char) * 10);
        this->peerid = peerid;
}

PktHandshake :: ~PktHandshake() { }

void to_json(json &j, const PktHandshake &pkt)
{
        j = json{{"header", pkt.header}, {"zero", pkt.zero}, {"peerid", pkt.peerid}};
}

void from_json(const json &j, PktHandshake &pkt)
{
        j.at("header").get_to(pkt.header);
        j.at("zero").get_to(pkt.zero);
        j.at("peerid").get_to(pkt.peerid);
}

PktMessage :: PktMessage(uint32_t len, MessageType type)
{
        this->len = len+1;
        this->type = type;
        //this->payload = strndup(payload, sizeof(char) * len);
        //if(this->payload == NULL) {
        //        lvar->write_msg(LogLvlT::LOG_ERR, "PktMessage: Malloc");
        //        _exit(1);
        //}
}

PktMessage :: ~PktMessage()
{
        //free(this->payload);
}

void to_json(json &j, const PktMessage &pkt)
{
        j = json{{"len", pkt.len}, {"type", pkt.type}};
}

void from_json(const json &j, PktMessage &pkt)
{
        j.at("len").get_to(pkt.len);
        j.at("type").get_to(pkt.type);
}
