#ifndef HANDLER_H
#define HANDLER_H

#include<unordered_map>
#include<netinet/in.h>

#include"objstore/objstore.h"
#include"json/json.hpp"
#include"pkts/pkts.h"
#include"nbrmap/nbrmap.h"

using std::unordered_map;
using json = nlohmann::json;
using objstore::ObjStore;
using nbrmap::NbrMap;

void cli_handler(int, int, int, ObjStore *, NbrMap *);
/* Outhelper */
void snd(int, json &);
void send_handshake(int, int);
void send_bfield(int, ObjStore *);
void send_interested(int);
void send_not_interested(int);
void send_requests(int,
                unordered_map<uint32_t, bool> &);
void send_piece(int, uint32_t, ObjStore *);
void send_choke_status(int, bool);
void send_haves(int, ObjStore *, vector<uint8_t> &);

/* Inhelper */
bool rcv_sz(int, uint32_t *);
void rcv(int, uint8_t *, uint32_t);
void handle_piece(PktMsg *, ObjStore *,
                unordered_map<uint32_t, bool> &);
void handle_have(int, PktMsg *,
                unordered_map<uint32_t, bool> &);

#endif
