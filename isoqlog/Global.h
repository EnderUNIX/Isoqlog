
#ifndef GLOBAL_H
#define GLOBAL_H
#include <time.h>

#define VERSION_STRING	"EnderUNIX Isoqlog 2.2-BETA"


enum {
        BUFSIZE = 1024,
        KEYSIZE = 64,
        VALSIZE = 256
};

time_t secs;
time_t today ;
struct tm *t2;





#endif
