#ifndef DIR_H
#define DIR_H

#include "Global.h"

#define DIR_MODE 0755


int cur_year, cur_month, cur_day, cur_hour, cur_min, cur_sec;
void openlogdir(char *);
int timeconvert(char *, int, int, int, int, int *);
void readSpecialLogFile(char *fn, int year, int month, int day);
void createdir(char *);

#endif


