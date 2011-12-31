#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "Dir.h"
#include "Parser.h"

extern int debug;
extern int cur_year, cur_month, cur_day;
extern char logstore[128];

/*open qmail-send log directory for qmail multilog format */
void 
openlogdir(char *logdir)
{
	DIR *dp = NULL;
	struct dirent *dirp = NULL;
	int fdate, ffirst = 0, state = 0;
	char fname[128];
	char *current = NULL;

	if ((dp = opendir(logdir)) == NULL) {
		printf("Can't open %s dir\n", logdir);
		exit(-1);
	}

        printf("Current %d-%d-%d\n",  cur_year, cur_month, cur_day);

	/* we will call readdir twice, first is to find today's "first created file" 
	 * Second is to find all other files that belong to today excep first created file 
	 * we must find today's first created files, because this file may contains last day's log info and today
	 * so we should not process last day's log */

	while((dirp = readdir(dp)) != NULL) {  /* we are opening directory to find today's first created file */
		if ((dirp->d_name[0] == '@')) {
			if ((timeconvert(dirp->d_name, 128, cur_year, cur_month, cur_day, &fdate)) > 0 ) {
				printf("today file: %.128s  ", dirp->d_name);
				printf("date %d : \n", fdate);
				if(state == 0) {
					ffirst = fdate;
					strncpy(fname, dirp->d_name, 128);
					state = 1;
				}
				else if(fdate < ffirst) {
					ffirst = fdate;
					strncpy(fname, dirp->d_name, 128);
				}
			}
		}
	}
	if ((chdir(logdir)) == -1) {
		fprintf(stderr, "chdir  %s\n",  strerror(errno));
		exit(-1);
	}
	rewinddir(dp); /* rewind directory to read again all files */
	while((dirp = readdir(dp)) != NULL) {
		if (dirp->d_name[0] == '@'){
			if ((timeconvert(dirp->d_name, 128, cur_year, cur_month, cur_day, &fdate)) > 0 ) {
				if ((strncmp(fname, dirp->d_name, 128)) != 0) /* if this file is not our firts created file*/
					readQmailLogFile(dirp->d_name); /* these files are between first create file and current */
			}
		}
	}
	closedir(dp);

	if (ffirst != 0)   /* if we have an file  today*/
		readSpecialLogFile(fname, cur_year, cur_month, cur_day);
	current = (char *)malloc((strlen(logstore) + 10) * sizeof(char));
	sprintf(current, "%s/current", logstore);
	readSpecialLogFile(current, cur_year, cur_month, cur_day);
}

/*to convert multilog log format to human readable format */
int 
timeconvert(char *p, int plen, int year, int month, int day, int *fdate)
{
	int  c;
	unsigned long nanosecs;
	unsigned long u;
 	time_t secs = 0;
	struct tm *t;

	p++;
	while (((c = *p++) != ' ') && (c != '.') && (--plen > 0)) {
		u = c - '0';
		if (u >= 10) {
			u = c - 'a';
			if (u >= 6) 
				break;
			u += 10;
		}
		secs <<= 4;
		secs += nanosecs >> 28;
		nanosecs &= 0xfffffff;
		nanosecs <<= 4;
		nanosecs += u;
  	}
	secs -= 4611686018427387914ULL;
	t = localtime(&secs);
	if ((year == (t->tm_year +1900)) && (month == (t->tm_mon + 1))
					 && (day ==  t->tm_mday )) {
		/* if this file belongs to today, convert to it's created hour, 
		 * minute to second and sum all of them	*/ 
		/* therefore we can find the first created file in today */
     		*fdate = t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec; 

		return 1;
	}
	else
		return -1;
}

void readSpecialLogFile(char *fn, int year, int month, int day)
{
	FILE *fp = NULL;
	char buf[1024];
	int i = 0;

	if ((fp = fopen(fn, "r")) == NULL) {
		fprintf(stderr, "open file: %s %s\n", fn, strerror(errno));
		exit(-1);
	}
	while ((fgets(buf, 1024, fp)) != NULL)
		if ((timeconvert(buf, 1024, year, month, day, &i)) > 0) {
			if ((strstr(buf, " from ")) != NULL)
				parseQmailFromBytesLine(buf, strlen(buf));
			else if ((strstr(buf, " to remote")) != NULL)
				parseQmailToRemoteLine(buf, strlen(buf));
			else if ((strstr(buf, " to local")) != NULL)
				parseQmailToLocalLine(buf, strlen(buf));
			}
	fclose(fp);
}

void createdir(char *dir)
{
	struct stat statbuf;

	if ((stat(dir, &statbuf)) == 0) {
		if (S_ISDIR(statbuf.st_mode) == 0) { /* if it is not a directory */
	   	     printf("dir: %s must be a directory\n", dir);
	   	     exit(-1);
	        }
	}
       	else {
		if (errno == ENOENT) {  /* if dir is not exists */
			if((mkdir(dir, DIR_MODE)) != 0){  /* try to create it */
				fprintf(stderr, "Can't create directory : %s : %s\n", dir, strerror(errno));
				exit(-1);
			}
			printf ("The Created directory : %s\n", dir);
		}
	}
}


