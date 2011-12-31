#ifndef HTML_H
#define HTML_H

#include "Data.h"



void addFooter(FILE *fp);
void putMetaTags(FILE *fp);

void createHtml();

void createIndexHtml(char *);

void createDailyHtml(char *, domain *);
void createMonthlyHtml(char *, char *);
void createYearlyHtml(char *, char *);
void createDomainHtml(char *, char *);

void createGeneralDailyHtml(char *);
void createGeneralMonthlyHtml(char *);
void createGeneralYearlyHtml(char *);
void createGeneralHtml(char *);

void getSizeStr(double, char *, double *);

#endif
