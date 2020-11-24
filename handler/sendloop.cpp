#include<iostream>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>

#include"objstore/objstore.h"
#include"logger/logger.h"
#include"handler/handler.h"
#include"handler/sendloop.h"

using namespace handler;
using logger::Logger;
using logger::LogLvlT;

extern Logger *lvar;

void *sendloop(void *args)
{
        HandlerArgs *hargs = (HandlerArgs *)args;

        pthread_exit(NULL);
}
