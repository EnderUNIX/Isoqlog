#ifndef DATA_H
#define DATA_H

enum {
	DOMAINSIZ = 128,
	USERSIZ = 256,
	MAXUSERS = 5000,
	MAXDOMAIN = 50,
	NVINIT = 1,
	NVGROW = 2,
	MULTIPLIER = 31,
	FROM_MAIL = 100,
	TO_MAIL = 101
};

enum {
	ERR_OVER_QUOTA = 0,
	ERR_NO_MAILBOX = 1,
	ERR_UNABLETO_CHDIR = 2,
	ERR_SMTPCONN_FAILED = 3
};

/* Common Errors	*/
int errors[10];

typedef struct domain {
	char domain[DOMAINSIZ];
	int from_cnt;
	int to_cnt;
	double from_byte;
	struct user *users[MAXUSERS];
} domain;
	
typedef struct user {
	char user[USERSIZ];
	int from_cnt;
	int to_cnt;
	double from_byte;
	struct user *next;
} user;

typedef struct globuser {
	user * u;
	struct globuser * next;
} globuser;


domain *domains[MAXDOMAIN];
globuser * globusers[MAXUSERS];

/* Total Stats - Mail Server Stats	*/
domain general;

int addDomain(char *);
int addUserToSortTab(user *);
void checkUser(char *, char *, int, double);
void checkUserInGeneral(char *, int, double);
void checkUserPtrInGeneral(user *);
void processIncomingMail(char *, char *, int, double);
void processOutgoingMail(char *, char *, int);
void freeSortDomainTab();

/* After all, calculate total sent, received mails and total 
 * sent mail size	*/
void sumTotalStats();

/* "Selection" Sorts */
void sortDomainUsersFrom(char *);
void sortDomainUsersTo(char *);
void sortDomainUsersByte(char *);
void sortDomainUsersTotal(char *);
void sortDomainsFrom();
void sortDomainsTo();
void sortDomainsByte();
void sortDomainsTotal();
void sortUsersFrom();
void sortUsersTo();
void sortUsersByte();
void sortUsersTotal();


/* Output */
void printDomains();
void printAllUsers();
void printDomainUsers(char *);
void printSortDomain();


/* hash function */
unsigned int hash(char *);


struct DomainsTab {
	int nval;
	int max;
	domain *alldomains;
} DomainsTab;

struct UsersTab {
	int nval;
	int max;
	int *allusers;
} UsersTab;



struct SortDomain {
	int nval;
	int max;
	int *allusers;
} SortDomain;
	
#endif
