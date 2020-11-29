#ifndef PKTS_H
#define PKTS_H

#include"json/json.hpp"

using json = nlohmann::json;

namespace node {
        typedef struct PktHandshake {
                char header[18];
                char zero[10];
                uint32_t peerid;

                PktHandshake(uint32_t);
                ~PktHandshake();
        } PktHandshake;
        void to_json(json &, const PktHandshake &);
        void from_json(json &, const PktHandshake &);

        typedef enum MessageType : uint8_t {
                Choke = (uint8_t)1 << 0,
                UnChoke = (uint8_t)1 << 1,
                Interested = (uint8_t)1 << 2,
                NotInterested = (uint8_t)1 << 3,
                Have = (uint8_t)1 << 4,
                BitField = (uint8_t)1 << 5,
                Request = (uint8_t)1 << 6,
                Piece = (uint8_t)1 << 7
        } MessageType;

        typedef struct PktMessage {
                uint32_t len;
                MessageType type;

                PktMessage(uint32_t, MessageType);
                ~PktMessage();
        } PktMessage;
        void to_json(json &, const PktMessage &);
        void from_json(json &, const PktMessage &);
}

#endif
