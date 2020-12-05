#ifndef HANDLER_H
#define HANDLER_H

#include<netinet/in.h>

#include"objstore/objstore.h"
#include"pkts/pkts.h"
#include"nbrmap/nbrmap.h"

using objstore::ObjStore;
using nbrmap::NbrMap;

void cli_handler(int, struct sockaddr_in *,
                        ObjStore *, NbrMap *, bool);
/* Outhelper */
void send_handshake(int);

#endif
