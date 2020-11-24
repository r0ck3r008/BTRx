#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logger.h"
#include"handler/handler.h"

using std::cout;
using std::endl;
using logger::Logger;
using namespace handler;

extern Logger *lvar;

HandlerArgs :: HandlerArgs(int sock, struct sockaddr_in *addr, bool client)
{
        this->sock = sock;
        this->addr = addr;
        this->client = client;
}

HandlerArgs :: ~HandlerArgs()
{
        close(this->sock);
        delete this->addr;
}

void *cli_handler(void *args)
{
        HandlerArgs *harg = (HandlerArgs *)args;
        if(harg->client)
                cout << "Connection accepted from: ";
        else
                cout << "Connected to: ";

        cout << inet_ntoa(harg->addr->sin_addr) << endl;

        pthread_exit(NULL);
}
