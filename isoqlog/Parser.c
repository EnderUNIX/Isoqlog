#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Data.h"
#include "loadconfig.h"
#include "Parser.h"

extern char hostname[VALSIZE];
extern char outputdir[VALSIZE];
extern int cur_year, cur_month, cur_day;
char *months[13] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};


void lowercase(char *s, int size)
{
	int i = 0;

        while ((*s != '\0') && (i++ < size)) {
		*s = tolower(*s);
		s++;
	}
}


/* These three functions read the logfile and fill the data structure	*/
void readQmailLogFile(char *fn) 
{	

	FILE *fp = NULL;
	char buf[1024];

	if ((fp = fopen(fn, "r")) == NULL) {
		fprintf(stderr, "fopen: %s: %s\n", fn, strerror(errno));
		exit(-1);
	}

	while ((fgets(buf, 1024, fp)) != NULL) {
		if ((strstr(buf, " from ")) != NULL)  /* if this is from line */
			parseQmailFromBytesLine(buf, strlen(buf));
		else if ((strstr(buf, " to remote ")) != NULL)
			parseQmailToRemoteLine(buf, strlen(buf));
		else if ((strstr(buf, " to local ")) != NULL)
			parseQmailToLocalLine(buf, strlen(buf));
	}
	fclose(fp);
}	

void readSendmailLogFile(char *fn) 
{	
	FILE *fp;
	char buf[1024];

	if ((fp = fopen(fn, "r")) == NULL) {
		fprintf(stderr, "fopen: %s: %s\n", fn, strerror(errno));
		exit(-1);
	}
	 /* Get the historical values of last read log file's inode number
	    If both inode numbers are the same, we assume that log file 
	    has not been changed (i.e rotated, removed), so we can 
	    go past offset bytes and continue to read new logs */

	while ((fgets(buf, 1024, fp)) != NULL) {
		if ((check_syslog_date(buf, strlen(buf))) > 0) {
			if ((strstr(buf, " from=")) != NULL)  /* if this is from line */
				parseSendmailFromBytesLine(buf, strlen(buf));
			else if ((strstr(buf, " to=")) != NULL)
				parseSendmailToLine(buf, strlen(buf));
		}
	}

	/* close */
	fclose(fp);
}

int check_syslog_date(char *buf, int buflen)
{
	unsigned char m[4]= {0};
	int day = 0, i = 0, j;

	for (i = 0; buf[i] != ' ' && buf[i] != '\n' && buf[i] != EOF && i < 4; i++)
		m[i] = buf[i];

	m[3] = '\0';

	while(buf[i] ==' ' && i < buflen)
		i++;

	for (j = i++; buf[j] != ' ' && buf[j] != '\n' && buf[j] != EOF && j < buflen; j++)
		day =  10 * day + (buf[j] - '0');

	if ((strncmp(months[cur_month -1], m, 3) == 0) && (day == cur_day ))  
		return 1;

	return -1;
}
/* This function parses one line from log file, checks if any " from " 
 * is matched. If so, returns the mail address */
void parseQmailFromBytesLine(char *str, int lenstr)
{

	char *p = str;
	char tmpbytes[128];
	char domain[128];
	char user[128];
	char email[300];
	int i = 0;
	char *tmp = NULL;

	if (((strstr(str, " from <>")) != NULL) || ((strstr(str, " bytes ")) == NULL)) {
		(general.from_cnt)++;
		return;  /* null sender */
	}
	/* get bytes */
	if ((p = strstr(p, " bytes ")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	p += 7;
	if (strstr(p, "@") == NULL) {
		(general.from_cnt)++;
		return; /*null sender, ignore it */
	}
	lowercase(p, strlen(p));

	for (i = 0; (*p != ' ') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		tmpbytes[i] = *p;
	tmpbytes[i] = 0x0;

	if (strlen(tmpbytes) == 0)
		return;
	if ((p = strstr(p, "from ")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

    	p += 6;
	for (i = 0; (*p != '@') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		user[i] = *p;
	user[i] = 0x0;
	if (strlen(user) == 0)
		return;

	p++;	/* skip pass @	*/
	for (i = 0; (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		domain[i] = *p;
	domain[i] = 0x0;
	if (strlen(domain) == 0)
		return;
	sprintf(email, "%.128s@%.128s", user, domain);
	checkUser(domain, email, FROM_MAIL, atol(tmpbytes));
	(general.from_cnt)++;
	general.from_byte += atol(tmpbytes);
}

void 
parseQmailToRemoteLine(char *str, int lenstr)
{

	char *p = str;
	char domain[128];
	char user[128];
	char email[300];
	int i = 0;
	char *tmp = NULL;

	if ((p = strstr(p, " to remote ")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	p += 11;
	if (strstr(p, "@") == NULL) {
		(general.to_cnt)++;
		return; /*null receiver, ignore it */
	}
	lowercase(p, strlen(p));
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != '@') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		user[i] = *p;
	user[i] = 0x0;
	if (strlen(user) == 0)
		return;

	p++;	/* skip pass @	*/
	for (i = 0; (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		domain[i] = *p;
	domain[i] = 0x0;
	if (strlen(domain) == 0)
		return;
	sprintf(email, "%.128s@%.128s", user, domain);
	checkUser(domain, email, TO_MAIL, 0);
	(general.to_cnt)++;
}
void 
parseQmailToLocalLine(char *str, int lenstr)
{

	char *p = str;
	char domain[128];
	char user[512];
	char email[1024];
	int i = 0;
	char *tmp = NULL;
	char vdomain[150];

	if ((p = strstr(p, " to local ")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	p += 10;
	if (strstr(p, "@") == NULL) {
		(general.to_cnt)++;
		return; /*null receiver, ignore it */
	}
	lowercase(p, strlen(p));
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != '@') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 511) && ((p - str) < lenstr); i++, p++)
		user[i] = *p;
	user[i] = 0x0;
	if (strlen(user) == 0)
		return;

	p++;	/* skip pass @	*/
	for (i = 0; (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		domain[i] = *p;
	domain[i] = 0x0;
	if (strlen(domain) == 0)
		return;
	sprintf(vdomain, "%.128s-", domain); /* If virtual domains enabled	*/
	if (strstr(user, vdomain) != NULL)
		sprintf(email, "%.512s@%.128s", (user + strlen(vdomain)), domain);
	else
		sprintf(email, "%.512s@%.128s", user, domain);

	checkUser(domain, email, TO_MAIL, 0);
	(general.to_cnt)++;
}

/* This function parses one line from log file, checks if any " from " 
 * is matched. If so, returns the mail address */
void 
parseSendmailFromBytesLine(char *str, int lenstr)
{

	char *p = str;
	char domain[128];
	char user[128];
	char email[300];
	int i = 0;
	char *tmp = NULL;
	int domainflg = 0;
	char tmpbytes[128];

	if (((strstr(str, " from=<>")) != NULL) || 
					((strstr(str, " from=,")) != NULL)) {
		(general.from_cnt)++;
		return; /* null sender */
	}

	if ((p = strstr(p, "from=")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	p += 5;
	if (*p == '<')
			p++;
	lowercase(p, strlen(p));
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != ',') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++) {
		if (*p == '@') {
			domainflg = 1;
			break;
		}
		user[i] = *p;
	}
	user[i] = 0x0;

	if (strlen(user) == 0)
		return;

	if (domainflg) {
		p++;	/* skip pass @	*/
		for (i = 0; (*p != ',') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
			domain[i] = *p;
		domain[i] = 0x0;
		if (strlen(domain) == 0)
			return;
	} else
			strncpy(domain, hostname, 127);

	if ((p = strstr(p, " size=")) == NULL)
			return;

	p += 6;
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != ',') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		tmpbytes[i] = *p;
	tmpbytes[i] = 0x0;
	
	sprintf(email, "%.128s@%.128s", user, domain);
	checkUser(domain, email, FROM_MAIL, atol(tmpbytes));
	(general.to_cnt)++;
}

void 
parseSendmailToLine(char *str, int lenstr)			 /* ft = from or to */
{
	char *p = str;
	char domain[128];
	char user[128];
	char email[300];
	int i = 0;
	char *tmp = NULL;
	int domainflg = 0;
	char tmpbytes[128];
	int vdomainflg = 0;

	if ((strstr(str, " to=<>")) != NULL) {
		(general.to_cnt)++;
		return;
	}

	if ((p = strstr(p, " to=")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	/* For Postfix Virtual Domains	*/
	if (strstr(p, " orig_to=") != NULL) {
		p = strstr(p, " orig_to=");
		p += 9;
	} else
		p += 4;

	if (*p == '<')
		p++;

	lowercase(p, strlen(p));
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != ',') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++) {
		if (*p == '@') {
				domainflg = 1;
				break;
		}
		user[i] = *p;
	}
	user[i] = 0x0;

	if (strlen(user) == 0)
		return;

	if (domainflg) {
		p++;	/* skip pass @	*/
		for (i = 0; (*p != ',') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
			domain[i] = *p;
		domain[i] = 0x0;
		if (strlen(domain) == 0)
			return;
	} else
			strncpy(domain, hostname, 128);

	sprintf(email, "%.128s@%.128s", user, domain);
	checkUser(domain, email, TO_MAIL, 0);
	(general.to_cnt)++;
}

/* This function parses one line from log file, checks if any " from " 
 * is matched. If so, returns the mail address 
 *
 *
 * EXIM SUPPORT CODE by Marco Erra <mare@erra.myip.org>
 */
void 
parseEximFromBytesLine(char *str, int lenstr)
{

	char *p = str;
	char domain[128];
	char user[128];
	char email[300];
	int i = 0;
	char *tmp = NULL;
	int domainflg = 0;
	char tmpbytes[128];

	if (((strstr(str, " <= <>")) != NULL) || ((strstr(str, " <=,")) != NULL) ) {
		(general.from_cnt)++;
		return; /* null sender */
	}

	if ((p = strstr(p, " <= ")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	p += 4;
	if (*p == '<')
			p++;
	lowercase(p, strlen(p));
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != ' ') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++) {
		if (*p == '@') {
				domainflg = 1;
				break;
		}
		user[i] = *p;
	}
	user[i] = 0x0;

	if (strlen(user) == 0)
		return;

	if (domainflg) {
		p++;	/* skip pass @	*/
		for (i = 0; (*p != ' ') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
			domain[i] = *p;
		domain[i] = 0x0;
		if (strlen(domain) == 0)
			return;
	} else
			strncpy(domain, hostname, 128);

	if ((p = strstr(p, " s=")) == NULL)
		return;

	p += 3;
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != ' ') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
		tmpbytes[i] = *p;
	tmpbytes[i] = 0x0;
	
	sprintf(email, "%.128s@%.128s", user, domain);
	checkUser(domain, email, FROM_MAIL, atol(tmpbytes));
	(general.to_cnt)++;
}

void 
parseEximToLine(char *str, int lenstr) /* ft = from or to */
{
	char *p = str;
	char domain[128];
	char user[128];
	char email[300];
	int i = 0;
	char *tmp = NULL;
	int domainflg = 0;
	char tmpbytes[128];

	if ((strstr(str, " => <>")) != NULL) {
		(general.to_cnt)++;
		return;
	}

	if ((p = strstr(p, " => ")) == NULL)
		return;
	if ((p - str) > lenstr)
		return;

	p += 4;
	if (*p == '<')
			p++;
	lowercase(p, strlen(p));
	if ((p - str) > lenstr)
		return;

	for (i = 0; (*p != ' ') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++) {
		if (*p == '@') {
				domainflg = 1;
				break;
		}
		user[i] = *p;
	}
	user[i] = 0x0;

	if (strlen(user) == 0)
		return;

	if (domainflg) {
		p++;	/* skip pass @	*/
		for (i = 0; (*p != ' ') && (*p != '>') && (*p != '\0') && (*p != '\r') && (*p != '\n') && (i < 127) && ((p - str) < lenstr); i++, p++)
			domain[i] = *p;
		domain[i] = 0x0;
		if (strlen(domain) == 0)
			return;
	} else
			strncpy(domain, hostname, 128);

	sprintf(email, "%.128s@%.128s", user, domain);
	checkUser(domain, email, TO_MAIL, 0);
	(general.to_cnt)++;
}


void readEximLogFile(char *fn) 
{	
	FILE *fp = NULL;
	char buf[1024];

	if ((fp = fopen(fn, "r")) == NULL) {
		fprintf(stderr, "fopen: %s: %s\n", fn, strerror(errno));
		exit(-1);
	}
	 /* Get the historical values of last read log file's inode number
	    If both inode numbers are the same, we assume that log file 
	    has not been changed (i.e rotated, removed), so we can 
	    go past offset bytes and continue to read new logs */

	while ((fgets(buf, 1024, fp)) != NULL) {
		if ((check_syslog_date_exim(buf, strlen(buf))) > 0) {
			if ((strstr(buf, " <= ")) != NULL){  /* if this is from line */
				parseEximFromBytesLine(buf, strlen(buf));
			}else if ((strstr(buf, " => ")) != NULL)
				parseEximToLine(buf, strlen(buf));
		}
	}
	fclose(fp);
}

int check_syslog_date_exim(char *buf, int buflen)
{
	int ano = 0, mes = 0, day = 0, i = 0, j, k;

	for (k = 0; buf[k] != '-' && buf[k] != '\n' && buf[k] != EOF && k < buflen; k++)
		ano =  10 * ano + (buf[k] - '0');
	k++;
	for (i = k++; buf[i] != '-' && buf[i] != '\n' && buf[i] != EOF && i < buflen; i++)
		mes =  10 * mes + (buf[i] - '0');
	i++;
	for (j = i++; buf[j] != ' ' && buf[j] != '\n' && buf[j] != EOF && j < buflen; j++)
		day =  10 * day + (buf[j] - '0');
	if ((cur_month == mes) && (day == cur_day )){
		return 1;
	}
	return -1;
}
