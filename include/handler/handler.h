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

/* Inhelper */
uint32_t rcv_sz(int);
void rcv(int, uint8_t *, uint32_t);

#endif
