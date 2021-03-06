#ifndef PKTS_H
#define PKTS_H

#include<vector>

#include"json/json.hpp"

using json = nlohmann::json;
using std::vector;
using std::string;

enum MessageType : uint16_t {
        Handshake = (uint16_t)1 << 1,
        Choke = (uint16_t)1 << 2,
        UnChoke = (uint16_t)1 << 3,
        Interested = (uint16_t)1 << 4,
        NotInterested = (uint16_t)1 << 5,
        Have = (uint16_t)1 << 6,
        BitField = (uint16_t)1 << 7,
        Request = (uint16_t)1 << 8,
        Piece = (uint16_t)1 << 9
};

struct PktHandshake {
        uint8_t header[19];
        uint8_t zero[10];
        uint32_t peerid;
};
void to_json(json &, const PktHandshake &);
void from_json(const json &, PktHandshake &);

struct PktMsgHave {
        uint32_t pcno;
};
void to_json(json &, const PktMsgHave &);
void from_json(const json &, PktMsgHave &);

struct PktMsgBfield {
        vector<uint8_t> bfield;
};
void to_json(json &, const PktMsgBfield &);
void from_json(const json &, PktMsgBfield &);

struct PktMsgRequest {
        uint32_t pcno;
};
void to_json(json &, const PktMsgRequest &);
void from_json(const json &, PktMsgRequest &);

struct PktMsgPiece {
        uint32_t pcno;
        string payload;
};
void to_json(json &, const PktMsgPiece &);
void from_json(const json &, PktMsgPiece &);

struct PktMsg {
        MessageType type;
        PktHandshake hshake;
        PktMsgHave have;
        PktMsgBfield bfield;
        PktMsgRequest req;
        PktMsgPiece piece;
};
void to_json(json &, const PktMsg &);
void from_json(const json &, PktMsg &);

#endif
