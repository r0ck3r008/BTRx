#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"logchild.h"

Logchild *logchild_init(int sock, char *path, LOG_LVL lvl)
{
	Logchild *lchild=calloc(1, sizeof(Logchild));
	if(lchild==NULL) {
		fprintf(stderr, "[-]LOGCHILD: Error in allocating memory!\n");
		return NULL;
	}
	lchild->sock=sock;
	lchild->lvl=lvl;
	if(!strcmp("-", path)) {
		lchild->outf=stdout;
	} else if((lchild->outf=fopen(path, "w"))==NULL) {
		fprintf(stderr, "[-]LOGCHILD: Fopen: %s\n", strerror(errno));
		return NULL;
	}

	return lchild;
}

int logchild_pretty_p(Logchild *lchild, char *buf)
{
	LOG_LVL loglvl=(LOG_LVL)strtol(strtok(buf, "$"), NULL, 10);
	if(loglvl < lchild->lvl)
		return 1;
	switch(loglvl) {
	case LOG_TCP:
		fprintf(lchild->outf, "[TCPCONNECTION] ");
		break;
	case LOG_CPN:
		fprintf(lchild->outf, "[CHANGEOFPREFERREDNEIGHBORS] ");
		break;
	case LOG_COUN:
		fprintf(lchild->outf, "[CHANGEOFOPTIMISTICALLYUNCHOKEDNEIGHBOR] ");
		break;
	case LOG_UC:
		fprintf(lchild->outf, "[UNCHOKING] ");
		break;
	case LOG_C:
		fprintf(lchild->outf, "[CHOKING] ");
		break;
	case LOG_HMSSG:
		fprintf(lchild->outf, "[RECEIVINGHAVEMESSAGE] ");
		break;
	case LOG_IMSSG:
		fprintf(lchild->outf, "[RECEIVINGINTERESTEDMESSAGE] ");
		break;
	case LOG_NIMSSG:
		fprintf(lchild->outf, "[RECEIVINGNOTINTERESTEDMESSAGE] ");
		break;
	case LOG_PD:
		fprintf(lchild->outf, "[DOWLOADINGAPIECE] ");
		break;
	case LOG_CD:
		fprintf(lchild->outf, "[COMPLETIONOFDOWNLOAD] ");
		break;
	default:
		fprintf(lchild->outf, "LOGCHILD: Unknown log level!\n");
		return 0;
	}
	fprintf(lchild->outf, "%s\n", strtok(NULL, "$"));

	return 1;
}

void logchild_exec(Logchild *lchild)
{
	while(1) {
		char buf[1024];
		int stat=1;
		if((stat=read(lchild->sock, buf, sizeof(char)*1024))<0) {
			fprintf(lchild->outf, "[-]LOGCHILD: Read: %s\n",
							strerror(errno));
			break;
		} else if(!stat) {
			fprintf(lchild->outf,
				"[-]LOGCHILD: READ: Empty read\n");
			break;
		}
		if(!strcmp("EXIT", buf))
			break;
		else if(!logchild_pretty_p(lchild, buf))
			break;
	}

	logchild_deinit(lchild);
}

void logchild_deinit(Logchild *lchild)
{
	close(lchild->sock);
	if(lchild->outf!=stdout)
		fclose(lchild->outf);
	free(lchild);
}