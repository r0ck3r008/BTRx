#include<iostream>
#include<string.h>
#include<unistd.h>

#include"pkts/pkts.h"

/* PktHandshake */
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

/* PktMsgHave */
void to_json(json &j, const PktMsgHave &pkt)
{
        j = json{{"pcno", pkt.pcno}};
}

void from_json(const json &j, PktMsgHave &pkt)
{
        j.at("pcno").get_to(pkt.pcno);
}

/* Bfield */
void to_json(json &j, const PktMsgBfield &pkt)
{
        j = json{{"bfield", pkt.bfield}};
}

void from_json(const json &j, PktMsgBfield &pkt)
{
        j.at("bfield").get_to(pkt.bfield);
}

/* PktMsgRequest */
void to_json(json &j, const PktMsgRequest &pkt)
{
        j = json{{"pcs", pkt.pcs}};
}

void from_json(const json &j, PktMsgRequest &pkt)
{
        j.at("pcs").get_to(pkt.pcs);
}

/* PktMsgPiece */
void to_json(json &j, const PktMsgPiece &pkt)
{
        j = json{{"pcno", pkt.pcno}, {"payload"}, pkt.payload};
}

void from_json(const json &j, PktMsgPiece &pkt)
{
        j.at("pcno").get_to(pkt.pcno);
        j.at("payload").get_to(pkt.payload);
}

/* PktPayload */
void to_json(json &j, const PktPayload &pkt)
{
        j = json{{"have", pkt.have}, {"bfield", pkt.bfield},
                {"req", pkt.req}, {"piece", pkt.piece}};
}

void from_json(const json &j, PktPayload &pkt)
{
        j.at("have").get_to(pkt.have);
        j.at("bfield").get_to(pkt.bfield);
        j.at("req").get_to(pkt.req);
        j.at("piece").get_to(pkt.piece);
}

/* PktMsg */
void to_json(json &j, const PktMsg &pkt)
{
        j = json{{"len", pkt.len}, {"type", pkt.type}};
}

void from_json(const json &j, PktMsg &pkt)
{
        j.at("len").get_to(pkt.len);
        j.at("type").get_to(pkt.type);
}
