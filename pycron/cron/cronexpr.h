#ifndef _CRONEXPR_H_
#define _CRONEXPR_H_

#include "stdafx.h"
#include "rbtree.h"


typedef struct  cronexpr_target {
    lpr_rbtree_t* sets[7];
    int lastdayOfWeek;
    int lastdayOfMonth;
    char* expr;
}
cronexpr_t;
cronexpr_t* cronexpr_create(const char* expr);
void cronexpr_destroy(cronexpr_t* cron);
time_t cronexpr_after(cronexpr_t* cron, time_t afterTime);
time_t cronexpr_before(cronexpr_t* cron, time_t beforeTime);


#define MALLOC_FUN(f) static void * f(size_t size) {return malloc(size);}
#define FREE_FUN(f) static void f(void *p) { free(p);}

#if defined(_WIN32)
#define lpr_string_icmp stricmp
#define lpr_string_nprintf _snprintf
#else
#define lpr_string_icmp strcasecmp
#define lpr_string_nprintf snprintf
#endif


#if defined(_WIN32)
#define lpr_localtime(a, b) localtime_s(b, a)
#else
#define lpr_localtime(a, b) localtime_r(a,b)
#endif



#endif /*_CRONEXPR_H_*/

