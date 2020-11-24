#include<iostream>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>

#include"handler/handler.h"
#include"logger/logger.h"
#include"objstore/objstore.h"
#include"handler/recvloop.h"

using namespace handler;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

void *recvloop(void *args)
{
        HandlerArgs *hargs = (HandlerArgs *)args;

        pthread_exit(NULL);
}
