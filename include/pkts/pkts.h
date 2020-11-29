#ifndef PKTS_H
#define PKTS_H

#include<vector>

#include"json/json.hpp"

using json = nlohmann::json;
using std::vector;
using std::string;

struct PktHandshake {
        char header[18];
        char zero[10];
        uint32_t peerid;
};
void to_json(json &, const PktHandshake &);
void from_json(const json &, PktHandshake &);

enum MessageType : uint8_t {
        Choke = (uint8_t)1 << 0,
        UnChoke = (uint8_t)1 << 1,
        Interested = (uint8_t)1 << 2,
        NotInterested = (uint8_t)1 << 3,
        Have = (uint8_t)1 << 4,
        BitField = (uint8_t)1 << 5,
        Request = (uint8_t)1 << 6,
        Piece = (uint8_t)1 << 7
};

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
        vector<uint32_t> pcs;
};
void to_json(json &, const PktMsgRequest &);
void from_json(const json &, PktMsgRequest &);

struct PktMsgPiece {
        uint32_t pcno;
        string payload;
};
void to_json(json &, const PktMsgPiece &);
void from_json(const json &, PktMsgPiece &);

union PktPayload {
        PktMsgHave have;
        PktMsgBfield bfield;
        PktMsgRequest req;
        PktMsgPiece piece;
};
void to_json(json &, const PktPayload &);
void from_json(const json &, PktPayload &);

struct PktMsg {
        uint32_t len;
        MessageType type;
        PktPayload payload;
};
void to_json(json &, const PktMsg &);
void from_json(const json &, PktMsg &);

#endif
