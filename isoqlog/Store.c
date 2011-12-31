
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


#include "Store.h"
#include "Data.h"
#include "Dir.h"


extern char logtype[128];

/* write hist[] array to the specified file	*/
void storeHistData(char *f, hist *h, size_t size)
{
	FILE *fp;

	if (f == NULL || h == NULL)
		return;

	if ((fp = fopen(f, "w")) == NULL) {
		fprintf(stderr, "storeHistData: cannot create history file %s: %s\n", f, strerror(errno));
		exit(-1);
	}
	fwrite(h, size, 1, fp);
	fclose(fp);
}

/* read hist[] array from the specified array	*/
void getHistData(char *f, hist *h, size_t size)
{
	FILE *fp;

	if (f == NULL || h == NULL)
		return;
	if ((fp = fopen(f, "r")) == NULL)
		return;
	fread(h, size, 1, fp);
	fclose(fp);
}

/* Store domain's current day statistics to the history file(monthly)	*/
void storeCurrentDayHistory(char *f, domain *d)
{
	hist h[MAXDAYS];
	int i;

	if (f == NULL || d == NULL)
		return;
	for (i = 0; i < MAXDAYS; i++) {
		h[i].sent = 0;
		h[i].received = 0;
		h[i].bytes_sent = 0;
	}

	getHistData(f, h, sizeof(h));

	h[cur_day - 1].sent = d->from_cnt;
	h[cur_day - 1].received = d->to_cnt;
	h[cur_day - 1].bytes_sent = d->from_byte;

	storeHistData(f, h, sizeof(h));
}

/* Store domain's current month statistics to the history file(yearly) 	*/
void storeCurrentMonthHistory(char *f, int s, int r, double b)
{
	hist h[MAXMONTHS];
	int i;

	if (f == NULL)
		return;
	for (i = 0; i < MAXMONTHS; i++) {
		h[i].sent = 0;
		h[i].received = 0;
		h[i].bytes_sent = 0;
	}

	getHistData(f, h, sizeof(h));

	h[cur_month - 1].sent = s;
	h[cur_month - 1].received = r;
	h[cur_month - 1].bytes_sent = b;

	storeHistData(f, h, sizeof(h));
}

void storeCurrentYearHistory(char *f, int s, int r, double b)
{
	hist h[MAXYEARS];
	int i;

	if (f == NULL)
		return;
	for (i = 0; i < MAXYEARS; i++) {
		h[i].sent = 0;
		h[i].received = 0;
		h[i].bytes_sent = 0;
	}

	getHistData(f, h, sizeof(h));

	h[cur_year - 2002].sent = s;
	h[cur_year - 2002].received = r;
	h[cur_year - 2002].bytes_sent = b;

	storeHistData(f, h, sizeof(h));
}
