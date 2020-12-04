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

/* PktMsg */
void to_json(json &j, const PktMsg &pkt)
{
        switch(pkt.type) {
                case Handshake:
                        j = json{{"type", pkt.type}, {"hshake", pkt.hshake}};
                        break;
                case Have:
                        j = json{{"type", pkt.type}, {"have", pkt.have}};
                        break;
                case BitField:
                        j = json{{"type", pkt.type}, {"bfield", pkt.bfield}};
                        break;
                case Request:
                        j = json{{"type", pkt.type}, {"req", pkt.req}};
                        break;
                case Piece:
                        j = json{{"type", pkt.type}, {"piece", pkt.piece}};
                        break;
                default:
                        j = json{{"type", pkt.type}};
        }
}

void from_json(const json &j, PktMsg &pkt)
{
        j.at("type").get_to(pkt.type);

        if(pkt.type == Handshake)
                j.at("hshake").get_to(pkt.hshake);
        else if (pkt.type == Have)
                j.at("have").get_to(pkt.have);
        else if (pkt.type == BitField)
                j.at("bfield").get_to(pkt.bfield);
        else if (pkt.type == Request)
                j.at("req").get_to(pkt.req);
        else if (pkt.type == Piece)
                j.at("piece").get_to(pkt.piece);
}
