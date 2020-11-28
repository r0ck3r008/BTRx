#ifndef HANDLER_H
#define HANDLER_H

#include<netinet/in.h>

void cli_handler(int, struct sockaddr_in *, bool);

#endif
