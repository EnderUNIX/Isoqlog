#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

typedef struct hist_stat {
	unsigned inode;
        int saved_pos;
} hist_stat;

void lowercase(char *, int);


/* Parser  routines for Qmail */
void readQmailLogFile(char *);
void parseQmailFromBytesLine(char *, int len);
void parseQmailToRemoteLine(char *, int len);
void parseQmailToLocalLine(char *, int len);


/* Parser  routines for Sendmail  and Postfix*/
void readSendmailLogFile(char *);
void parseSendmailFromBytesLine(char *, int);
void parseSendmailToLine(char *, int);
int check_syslog_date(char *, int);

/* Parser  routines for Exim */
void readEximLogFile(char *);
void parseEximFromBytesLine(char *, int);
void parseEximToLine(char *, int);
int check_syslog_date_exim(char *, int);

#endif
