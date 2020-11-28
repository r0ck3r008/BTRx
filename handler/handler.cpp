#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include"logger/logger.h"
#include"handler/sendloop.h"
#include"handler/recvloop.h"
#include"handler/handler.h"

using std::cout;
using std::endl;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

void cli_handler(int sock, struct sockaddr_in *addr, bool client)
{
        /* TODO: Replace this with appropriate logger statements */
        if(client)
                cout << "Connection accepted from: ";
        else
                cout << "Connected to: ";

        cout << inet_ntoa(addr->sin_addr) << endl;
}
