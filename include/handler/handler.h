#ifndef HANDLER_H
#define HANDLER_H

#include<unordered_map>
#include<netinet/in.h>

#include"objstore/objstore.h"
#include"pkts/pkts.h"
#include"nbrmap/nbrmap.h"

using std::unordered_map;
using objstore::ObjStore;
using nbrmap::NbrMap;

void cli_handler(int, int, struct sockaddr_in *,
                        ObjStore *, NbrMap *, bool);
/* Outhelper */
void send_handshake(int, int);
void send_bfield(int, ObjStore *);
void send_interested(int);
void send_not_interested(int);
void send_requests(int,
                unordered_map<uint32_t, bool> &);
void send_piece(int, uint32_t, ObjStore *);
void send_choke_status(int, bool);

/* Inhelper */
uint32_t rcv_sz(int);
void rcv(int, uint8_t *, uint32_t);
void handle_piece(PktMsg *, ObjStore *,
                unordered_map<uint32_t, bool> &);

#endif
