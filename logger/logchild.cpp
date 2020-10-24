#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"logger/logmsg.pb.h"
#include"logger/logchild.h"

using logger::LogChild;
using logger::LogMsgT;

LogChild :: LogChild(int sock, FILE *outf, LogMsgT::LogLvlT lvl)
{
	this->sock=sock;
	this->lvl=lvl;
	this->outf=outf;
}

LogChild :: ~LogChild()
{
	close(this->sock);
	if(this->outf!=stdout)
		fclose(this->outf);
}

void LogChild :: srvloop()
{
	/*TODO
	 * Demarshal the LogLvlT and pass that as a class to the pretty_p
	 * function.
	 **/
	/*
	while(1) {
		char buf[1024];
		int stat=1;
		if((stat=read(this->sock, buf, sizeof(char)*1024))<0) {
			fprintf(this->outf, "[-]LOGCHILD: Read: %s\n",
							strerror(errno));
			break;
		} else if(!stat) {
			fprintf(this->outf,
				"[-]LOGCHILD: READ: Empty read\n");
			break;
		}
		if(!strcmp("EXIT", buf))
			break;
		else if(!this->pretty_p(buf))
			break;
	}
	*/
}

int LogChild :: pretty_p(LogMsgT *lmsg)
{
	/*
	switch(loglvl) {
	case LOG_TCP:
		fprintf(this->outf, "[TCPCONNECTION] ");
		break;
	case LOG_CPN:
		fprintf(this->outf, "[CHANGEOFPREFERREDNEIGHBORS] ");
		break;
	case LOG_COUN:
		fprintf(this->outf, "[CHANGEOFOPTIMISTICALLYUNCHOKEDNEIGHBOR] ");
		break;
	case LOG_UC:
		fprintf(this->outf, "[UNCHOKING] ");
		break;
	case LOG_C:
		fprintf(this->outf, "[CHOKING] ");
		break;
	case LOG_HMSSG:
		fprintf(this->outf, "[RECEIVINGHAVEMESSAGE] ");
		break;
	case LOG_IMSSG:
		fprintf(this->outf, "[RECEIVINGINTERESTEDMESSAGE] ");
		break;
	case LOG_NIMSSG:
		fprintf(this->outf, "[RECEIVINGNOTINTERESTEDMESSAGE] ");
		break;
	case LOG_PD:
		fprintf(this->outf, "[DOWLOADINGAPIECE] ");
		break;
	case LOG_CD:
		fprintf(this->outf, "[COMPLETIONOFDOWNLOAD] ");
		break;
	default:
		fprintf(this->outf, "LOGCHILD: Unknown log level!\n");
		return 0;
	}
	fprintf(this->outf, "%s\n", strtok(NULL, "$"));

	*/
	return 1;
}

