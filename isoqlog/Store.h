#ifndef STORE_H
#define STORE_H

#include "Data.h"

enum {
	MAXMONTHS = 12,
	MAXDAYS = 31,
	MAXYEARS = 250		/* should be enough, huh? ;)	*/
};

typedef struct hist {
	int sent;
	int received;
	double bytes_sent;
} hist;


struct hist mhist[MAXMONTHS];
struct hist dhist[MAXDAYS];

/* Functions	*/
void storeHistData(char *, hist *, size_t);
void getHistData(char *, hist *, size_t);
void storeCurrentDayHistory(char *, domain *);
void storeCurrentMonthHistory(char *, int, int, double);
void storeCurrentYearHistory(char *, int, int, double);


#endif
