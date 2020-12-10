#ifndef HANDLER_H
#define HANDLER_H

#include<netinet/in.h>

#include"objstore/objstore.h"
#include"pkts/pkts.h"
#include"nbrmap/nbrmap.h"

using objstore::ObjStore;
using nbrmap::NbrMap;

void cli_handler(int, int, struct sockaddr_in *,
                        ObjStore *, NbrMap *, bool);
/* Outhelper */
void send_handshake(int, int);
void send_bfield(int, ObjStore *);
void send_interested(int);
void send_not_interested(int);
void send_requests(int, vector<uint8_t> &);
void send_pieces(vector<uint32_t> &);
void send_choke_status(int, bool);

/* Inhelper */
uint32_t rcv_sz(int);
void rcv(int, uint8_t *, uint32_t);

#endif
