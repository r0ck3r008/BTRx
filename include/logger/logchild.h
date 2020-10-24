#ifndef LOGCHILD_H
#define LOGCHILD_H

#include<stdio.h>

#include"logger.h"

typedef struct Logchild
{
	int sock;
	LOG_LVL lvl;
	FILE *outf;
} Logchild;

Logchild *logchild_init(int, char *, LOG_LVL);
int logchild_pretty_p(Logchild *, char *);
void logchild_exec(Logchild *);
void logchild_deinit(Logchild *);

#endif