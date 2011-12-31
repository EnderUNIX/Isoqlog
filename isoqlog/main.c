#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Data.h"
#include "Parser.h"
#include "Dir.h"
#include "Html.h"
#include "LangCfg.h"
#include "loadconfig.h"
#include "Store.h"

extern char langfile[VALSIZE]; 
extern char logtype[VALSIZE]; 
extern char logstore[VALSIZE]; 


int main(int argc, char **argv)
{
	int c, f = 0, error = 0;
	extern char *optarg;
	extern int optind;
	char isoqlogconf[256];


	while (!error && (c = getopt(argc,argv,"f:hv")) != -1) {
        	switch (c) {
        	case 'v':
			puts(VERSION_STRING);
	        	exit(0);
	        	break;
		case 'h':
			puts("Usage: isoqlog [-f isoqlog.conf]");
			exit(0);
			break;
		case 'f':
			strncpy(isoqlogconf, optarg, 256);
			f = 1;
       			break;
		default:
			error = 1;
			puts("Usage: isoqlog [-f isoqlog.conf]");
			exit(-1);
			break;
       		} 
	}

	general.from_cnt = 0;
	general.to_cnt = 0;
	general.from_byte = 0;
	
	if (f == 0) 
		readconfig("/usr/local/etc/isoqlog.conf");
	else 
		readconfig(isoqlogconf);
	loadLang(langfile);

	if (strcmp(logtype, "qmail-multilog") == 0) 
		openlogdir(logstore);
	else
	if (strcmp(logtype, "qmail-syslog") == 0)
		readQmailLogFile(logstore);
	else
	if (strcmp(logtype, "sendmail") == 0)
		readSendmailLogFile(logstore);
	else
	if (strcmp(logtype, "postfix") == 0)
		readSendmailLogFile(logstore);
	else
        if (strcmp(logtype, "exim") == 0)
		readEximLogFile(logstore);

	createHtml();
	return 0;
}
