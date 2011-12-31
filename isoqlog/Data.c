#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include "Data.h"

unsigned int hash(char *str)
{
	unsigned int h;
	unsigned char *p;

	h = 0;
	for (p = (unsigned char *)str; *p != '\0'; p++)
		h = MULTIPLIER * h + *p;
	return h % MAXUSERS;
}

int addDomain(char *d) 
{
	domain *dm;
	int i;

	/* Check whether this domain was added before	*/
	for (i = 0; i < DomainsTab.nval; i++)
		if (strncmp(DomainsTab.alldomains[i].domain, d, DOMAINSIZ) == 0)
			return -1;
	

	if (DomainsTab.alldomains == NULL) { 
		dm = (domain *)malloc(NVINIT * sizeof(domain));
		if (dm == NULL)
			return -1;
		DomainsTab.max = NVINIT;
		DomainsTab.nval = 0;
		DomainsTab.alldomains = dm;
	} else 
	if (DomainsTab.nval >= DomainsTab.max) {
		dm = (domain *)realloc(DomainsTab.alldomains,
				(NVGROW * DomainsTab.max) * sizeof(domain));
		if (dm == NULL)
			return -1;
		DomainsTab.max *= NVGROW;
		DomainsTab.alldomains = dm;
	}
	strncpy(DomainsTab.alldomains[DomainsTab.nval].domain, d, DOMAINSIZ);
	/* Initializing domain users hash array to NULL, initializing domain stats */
	for (i = 0; i < MAXUSERS; i++)
		DomainsTab.alldomains[DomainsTab.nval].users[i] = NULL;
	DomainsTab.alldomains[DomainsTab.nval].from_cnt = 0;
	DomainsTab.alldomains[DomainsTab.nval].from_byte = 0;
	DomainsTab.alldomains[DomainsTab.nval].to_cnt = 0;
	return DomainsTab.nval++;
}

void checkUserInGeneral(char *m, int d, double b)
{
	int h;
	user *p;
	globuser *gl;

	h = hash(m);

	for (gl = globusers[h]; gl != NULL; gl = gl->next) {
		if (strncmp(gl->u->user, m, USERSIZ) == 0) {
			if (d == FROM_MAIL) {
				gl->u->from_cnt++;
				gl->u->from_byte += b;
			} else
				gl->u->to_cnt++;
		}
		return;
	}


	gl = (globuser *)malloc(sizeof(globuser));
	p = (user *)malloc(sizeof(user));
	strncpy(p->user, m, USERSIZ);
	if (d == FROM_MAIL) {
		p->from_cnt = 1;
		p->to_cnt = 0;
		p->from_byte = b;
	} else {
		p->from_cnt = 0;
		p->to_cnt = 1;
		p->from_byte = 0;
	}
	gl->u = p;
	gl->next = globusers[h];
	globusers[h] = gl;
	return;
}

void checkUserPtrInGeneral(user *p)
{
	int h;
	globuser *gl;

	gl = (globuser *)malloc(sizeof(globuser));
	gl->u = p;

	h = hash(p->user);
	gl->next = globusers[h];
	globusers[h] = gl;
	return;
}

void checkUser(char *dmn, char *m, int d, double byte)
{
	int tmpnval;
	int h, i;
	user *sym = NULL;

	tmpnval = -1;
	for (i = 0; i < DomainsTab.nval; i++) 
		if (strncmp(DomainsTab.alldomains[i].domain, dmn, DOMAINSIZ) == 0) {
			tmpnval = i;
			break;
		}

	/* If user's domain is not in our domains list,
	 * we only add it to general users tab */
	if (tmpnval == -1) {
		checkUserInGeneral(m, d, byte);
		return;
	}

	h = hash(m);
	for (sym = DomainsTab.alldomains[tmpnval].users[h]; sym != NULL; sym = sym->next) {
		if (strncmp(sym->user, m, USERSIZ) == 0) { 
			if (d == FROM_MAIL) {
				sym->from_cnt++;
				sym->from_byte += byte;
				DomainsTab.alldomains[tmpnval].from_cnt++;
				DomainsTab.alldomains[tmpnval].from_byte += byte;
			} else {
				sym->to_cnt++;
				DomainsTab.alldomains[tmpnval].to_cnt++;
			}
			return;
		}
	}
		
	sym = (user *)malloc(sizeof(user));
	strncpy(sym->user, m, USERSIZ);
	if (d == FROM_MAIL) {
		sym->from_cnt = 1;
		sym->from_byte = byte;
		sym->to_cnt = 0;
		DomainsTab.alldomains[tmpnval].from_cnt++;
		DomainsTab.alldomains[tmpnval].from_byte += byte;
	} else {
		sym->from_cnt = 0;
		sym->from_byte = 0;
		sym->to_cnt = 1;
		DomainsTab.alldomains[tmpnval].to_cnt++;
	}
	sym->next = DomainsTab.alldomains[tmpnval].users[h];
	DomainsTab.alldomains[tmpnval].users[h] = sym;
	checkUserPtrInGeneral(sym);
	return;
}

void freeSortDomainTab() 
{
	free(SortDomain.allusers);
	SortDomain.nval = 0;
	SortDomain.max = 0;
	SortDomain.allusers = 0x0;
}


int addUserToSortTab(user *usr)
{
	int *u;

	if (SortDomain.allusers == NULL) { /* first time */
		SortDomain.allusers = (int *)malloc(NVINIT * sizeof(int *));
		if (SortDomain.allusers == NULL)
			return -1;
		SortDomain.max = NVINIT;
		SortDomain.nval = 0;
	} else
	if (SortDomain.nval >= SortDomain.max) { /* growing....*/
		u = (int *)realloc(SortDomain.allusers,
				(NVGROW * SortDomain.max) * sizeof(int *));
		if (u == NULL)
			return -1;
		SortDomain.max *= NVGROW;
		SortDomain.allusers = u;
	}
	SortDomain.allusers[SortDomain.nval] = (int)usr;
	return SortDomain.nval++;
}


void sortDomainUsersFrom(char *domain) 
{
	int tmpnval, i, j, max, tmp;
	struct domain *domainptr;
	struct user *sym;
	struct user *tmp1, *tmp2;

	tmpnval = -1;
	for (i = 0; i < DomainsTab.nval; i++) 
		if (strncmp(DomainsTab.alldomains[i].domain, domain, DOMAINSIZ) == 0) {
			tmpnval = i;
			break;
		}
	domainptr = (struct domain *)&DomainsTab.alldomains[i];

	for (i = 0; i < MAXUSERS; i++) {
		for (sym = domainptr->users[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym);
	}

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if (tmp1->from_cnt > tmp2->from_cnt)
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}


void sortDomainUsersTo(char *domain) 
{
	int tmpnval, i, j, max, tmp;
	struct domain *domainptr;
	struct user *sym;
	struct user *tmp1, *tmp2;

	tmpnval = -1;
	for (i = 0; i < DomainsTab.nval; i++) 
		if (strcasecmp(DomainsTab.alldomains[i].domain, domain) == 0) {
			tmpnval = i;
			break;
		}
	domainptr = (struct domain *)&DomainsTab.alldomains[i];

	for (i = 0; i < MAXUSERS; i++) {
		for (sym = domainptr->users[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym);
	}

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if (tmp1->to_cnt > tmp2->to_cnt)
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}

void sortDomainUsersTotal(char *domain) 
{
	int tmpnval, i, j, max, tmp;
	struct domain *domainptr;
	struct user *sym;
	struct user *tmp1, *tmp2;

	tmpnval = -1;
	for (i = 0; i < DomainsTab.nval; i++) 
		if (strcasecmp(DomainsTab.alldomains[i].domain, domain) == 0) {
			tmpnval = i;
			break;
		}
	domainptr = (struct domain *)&DomainsTab.alldomains[i];

	for (i = 0; i < MAXUSERS; i++) {
		for (sym = domainptr->users[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym);
	}

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if ((tmp1->from_cnt + tmp1->to_cnt) > (tmp2->from_cnt + tmp2->to_cnt))
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}





void sortDomainUsersByte(char *domain) 
{
	int tmpnval, i, j, max, tmp;
	struct domain *domainptr;
	struct user *sym;
	struct user *tmp1, *tmp2;

	tmpnval = -1;
	for (i = 0; i < DomainsTab.nval; i++) 
		if (strncmp(DomainsTab.alldomains[i].domain, domain, DOMAINSIZ) == 0) {
			tmpnval = i;
			break;
		}
	domainptr = (struct domain *)&DomainsTab.alldomains[i];

	for (i = 0; i < MAXUSERS; i++) {
		for (sym = domainptr->users[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym);
	}

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if (tmp1->from_byte > tmp2->from_byte)
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}


void sortUsersFrom()
{
	int i, j, max;
	globuser *sym;
	int tmp;
	user *tmp1;
	user *tmp2;


	for (i = 0; i < MAXUSERS; i++) 
		for (sym = globusers[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym->u);

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if (tmp1->from_cnt > tmp2->from_cnt)
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}

void sortUsersTo()
{
	int i, j, max;
	globuser *sym;
	int tmp;
	user *tmp1;
	user *tmp2;


	for (i = 0; i < MAXUSERS; i++) 
		for (sym = globusers[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym->u);

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if (tmp1->to_cnt > tmp2->to_cnt)
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}


void sortUsersTotal()
{
	int i, j, max;
	globuser *sym;
	int tmp;
	user *tmp1;
	user *tmp2;


	for (i = 0; i < MAXUSERS; i++) 
		for (sym = globusers[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym->u);

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if ((tmp1->to_cnt + tmp1->from_cnt) > (tmp2->to_cnt + tmp2->from_cnt))
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}

void sortUsersByte()
{
	int i, j, max;
	globuser *sym;
	int tmp;
	user *tmp1;
	user *tmp2;


	for (i = 0; i < MAXUSERS; i++) 
		for (sym = globusers[i]; sym != NULL; sym = sym->next) 
			addUserToSortTab(sym->u);

	for (i = 0; i < SortDomain.nval; i++) {
		max = i;
		for (j = i+1; j <= SortDomain.nval - 1; j++) {
			tmp1 = (user *)(SortDomain.allusers[j]);
			tmp2 = (user *)(SortDomain.allusers[max]);
			if (tmp1 != NULL && tmp2 != NULL)
				if (tmp1->from_byte > tmp2->from_byte)
					max = j;
		}
		tmp = SortDomain.allusers[i];
		SortDomain.allusers[i] = SortDomain.allusers[max];
		SortDomain.allusers[max] = tmp;
	}
}

void sumTotalStats()
{
	globuser *s;
	int i;

	general.from_cnt = 0;
	general.to_cnt = 0;
	general.from_byte = 0;

	for (i = 0 ; i < MAXUSERS; i++)
		for (s = globusers[i]; s != NULL; s = s->next) {
			general.from_cnt += s->u->from_cnt;
			general.to_cnt += s->u->to_cnt;
			general.from_byte += s->u->from_byte;
		}
}


void sortDomainsFrom()
{
	domain tmp;
	int i, j, max;
	for (i = 0; i < DomainsTab.nval; i++) {
		max = i;
		for (j = i+1; j <= DomainsTab.nval - 1; j++) {
			if (DomainsTab.alldomains[j].from_cnt > DomainsTab.alldomains[max].from_cnt)
				max = j;
		}
		tmp = DomainsTab.alldomains[i];
		DomainsTab.alldomains[i] = DomainsTab.alldomains[max];
		DomainsTab.alldomains[max] = tmp;
	}
}

void sortDomainsTo()
{
	domain tmp;
	int i, j, max;

	for (i = 0; i < DomainsTab.nval; i++) {
		max = i;
		for (j = i+1; j <= DomainsTab.nval - 1; j++) {
			if (DomainsTab.alldomains[j].to_cnt > DomainsTab.alldomains[max].to_cnt)
				max = j;
		}
		tmp = DomainsTab.alldomains[i];
		DomainsTab.alldomains[i] = DomainsTab.alldomains[max];
		DomainsTab.alldomains[max] = tmp;
	}
}

void sortDomainsTotal()
{
	domain tmp;
	int i, j, max;
	for (i = 0; i < DomainsTab.nval; i++) {
		max = i;
		for (j = i+1; j <= DomainsTab.nval - 1; j++) {
			if ((DomainsTab.alldomains[j].from_cnt + DomainsTab.alldomains[j].to_cnt) > (DomainsTab.alldomains[max].from_cnt + DomainsTab.alldomains[max].to_cnt))
				max = j;
		}
		tmp = DomainsTab.alldomains[i];
		DomainsTab.alldomains[i] = DomainsTab.alldomains[max];
		DomainsTab.alldomains[max] = tmp;
	}
}

void sortDomainsByte()
{
	domain tmp;
	int i, j, max;

	for (i = 0; i < DomainsTab.nval; i++) {
		max = i;
		for (j = i+1; j <= DomainsTab.nval - 1; j++) {
			if (DomainsTab.alldomains[j].from_byte > DomainsTab.alldomains[max].from_byte)
				max = j;
		}
		tmp = DomainsTab.alldomains[i];
		DomainsTab.alldomains[i] = DomainsTab.alldomains[max];
		DomainsTab.alldomains[max] = tmp;
	}
}

void printDomains()
{
	int i;

	if (DomainsTab.alldomains[0].domain == NULL)
		return;

	for (i = 0; i < DomainsTab.nval; i++) 
		printf("Domain: %s, From: %d, To: %d, Byte: %ld\n", 
				DomainsTab.alldomains[i].domain,
				DomainsTab.alldomains[i].from_cnt,
				DomainsTab.alldomains[i].to_cnt,
				DomainsTab.alldomains[i].from_byte);
}

void printAllUsers()
{
	/*
	int i;

	if (UsersTab.allusers == NULL)
		return;

	for (i = 0; i < UsersTab.nval; i++)
		printf("User: %s, From: %d, To: %d, Byte: %ld\n",
				((user *)UsersTab.allusers[i])->user,
				((user *)UsersTab.allusers[i])->from_cnt,
				((user *)UsersTab.allusers[i])->to_cnt,
				((user *)UsersTab.allusers[i])->from_byte);
				*/
}

void printDomainUsers(char *domain)
{
	int tmpnval;
	int i;
	user *p;

	tmpnval = -1;
	for (i = 0; i < DomainsTab.nval; i++) 
		if (strncmp(DomainsTab.alldomains[i].domain, domain, DOMAINSIZ) == 0) {
			tmpnval = i;
			break;
		}

	if (tmpnval == -1)
		return;
	printf("Domain Index: %d\n", tmpnval);

	for (i = 0; i < MAXUSERS; i++) {
		for (p = DomainsTab.alldomains[tmpnval].users[i]; p != NULL; p = p->next) 
			printf("User: %s, From: %d, To: %d, Bytes: %ld\n", 
					p->user, p->from_cnt, p->to_cnt, p->from_byte);

	}
}

void printSortDomain()
{
	int i;

	for (i = 0; i < SortDomain.nval; i++)
		printf("User: %s, From: %d, To: %d, Byte: %ld\n",
				((user *)SortDomain.allusers[i])->user,
				((user *)SortDomain.allusers[i])->from_cnt,
				((user *)SortDomain.allusers[i])->to_cnt,
				((user *)SortDomain.allusers[i])->from_byte);
}
