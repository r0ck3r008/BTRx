#ifndef HANDLER_H
#define HANDLER_H

#include<netinet/in.h>

namespace handler {
        struct HandlerArgs {
                int sock;
                bool client;
                struct sockaddr_in *addr;

                HandlerArgs(int, struct sockaddr_in *,
                                bool);
                ~HandlerArgs();
        };
}

void *cli_handler(void *);

#endif
