#ifndef LOADCONFIG_H
#define LOADCONFIG_H

#include "Global.h"

/* Configuration File - Specific Strings */

char outputdir[VALSIZE];
char logtype[VALSIZE];
char logstore[VALSIZE];
char domainsfile[VALSIZE];
char htmldir[VALSIZE];
char langfile[VALSIZE];
char hostname[VALSIZE];
int maxsender;
int maxreceiver;
int maxtotal;
int maxbyte;

int removespaces(char *, int);
void loadconfig(char *);
void readconfig(char *);

#endif
