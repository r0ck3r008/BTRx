#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logger.h"
#include"handler/sendloop.h"
#include"handler/recvloop.h"
#include"handler/handler.h"

using std::cout;
using std::endl;
using logger::Logger;
using logger::LogLvlT;
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
        /* TODO: Replace this with appropriate logger statements */
        if(harg->client)
                cout << "Connection accepted from: ";
        else
                cout << "Connected to: ";

        cout << inet_ntoa(harg->addr->sin_addr) << endl;

        pthread_t tid[2];
        int stat = 0;
        if((stat = pthread_create(&tid[0], NULL, sendloop, args)) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Create: %s\n",
                                strerror(stat));
                _exit(1);
        }

        if((stat = pthread_create(&tid[0], NULL, recvloop, args)) != 0) {
                lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Create: %s\n",
                                strerror(stat));
                _exit(1);
        }

        for(int i=0; i<2; i++) {
                if((stat = pthread_join(tid[i], NULL)) != 0) {
                        lvar->write_msg(LogLvlT::LOG_ERR, "NODE: Join: %s\n",
                                        strerror(stat));
                        _exit(1);
                }
        }

        pthread_exit(NULL);
}
