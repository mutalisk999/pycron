#include "cronexpr.h"

#define SECOND  0
#define MINUTE  1
#define HOUR  2
#define DAY_OF_MONTH  3
#define MONTH  4
#define DAY_OF_WEEK  5
#define YEAR  6

static const int ALL_SPEC_INT  = 99;
static const int NO_SPEC_INT  = 98;

#define EXPR_DELIM  ' '
#define EXPR_DELIM_COMMA  ','

#define MAX_TOKEN_NUM 32



typedef struct value_set {
    int pos;
    int value;
}
value_set_t;


static int intcmp(int* i1, int* i2) {
    return *i1 - *i2;
}

static int *intdup(int i) {
    int *r = (int *)malloc(sizeof(int));
    *r = i;
    return r;
}



static int split(char* str, char delim, char* result[], int size) {
    int i;
    char *t, *p;
    for(i=0, t=p=str; i<size; ) {
        if(*p == delim || *p == '\0') {
            result[i++] = t;
            if(*p != '\0') {
                *p++ = '\0';
                t = p;
                continue;
            } else {
                break;
            }
        }
        p++;
    }
    return i;
}


static int addToSet(cronexpr_t* cron, int val, int end, int incr, int type) {
    int i, startAt, stopAt;
    lpr_rbtree_t* vSet = cron->sets[type];

    if(type == SECOND || type == MINUTE) {
        if((val < 0 || val > 59 || end > 59) && (val != ALL_SPEC_INT)) {
            fprintf(stderr,"%s:秒数和分钟数必须在0和59之间 : %d", cron->expr, val);
            return -1;
        }
    } else if (type == HOUR) {
        if((val < 0 || val >23 || end > 23)&& (val != ALL_SPEC_INT)) {
            fprintf(stderr,"%s:小时数必须在0和23之间 : %d", cron->expr,  val);
            return -1;
        }
    } else if (type == DAY_OF_MONTH) {
        if((val < 1 || val > 31 || end > 31) && (val != ALL_SPEC_INT) && (val != NO_SPEC_INT)) {
            fprintf(stderr,"%s:天数必须在1和31之间 : %d", cron->expr,  val);
            return -1;
        }        
    } else if (type == MONTH) {
        if((val < 0 || val > 11 || end >11) && (val != ALL_SPEC_INT)) {
            fprintf(stderr,"%s:月份必须在0和11之间 : %d", cron->expr,  val);
            return -1;
        }
    } else if (type == DAY_OF_WEEK) {
        if((val < 0 || val > 6 || end > 6) && (val != ALL_SPEC_INT) && (val != NO_SPEC_INT)) {
            fprintf(stderr,"%s:星期数必须在0和6之间 : %d", cron->expr,  val);
            return -1;
        }
    } else if (type == YEAR) {  
        if((val != ALL_SPEC_INT) && (val != NO_SPEC_INT))
            val -= 1900;
    }

    if ((incr == 0 || incr == -1) && val != ALL_SPEC_INT) {
        if (val != -1)
            lpr_rbtree_insert(vSet, intdup(val), (void*)1, 1);
        else
            lpr_rbtree_insert(vSet, intdup(NO_SPEC_INT), (void*)1, 1);
        return 0;
    }

    startAt = val;
    stopAt = end;

    if (val == ALL_SPEC_INT && incr <= 0) {
        incr = 1;
        lpr_rbtree_insert(vSet, intdup(ALL_SPEC_INT), (void*)1, 1);
    }

    if (type == SECOND || type == MINUTE) {
        if (stopAt == -1)
            stopAt = 59;
        if (startAt == -1 || startAt == ALL_SPEC_INT)
            startAt = 0;
    } else if (type == HOUR) {
        if (stopAt == -1)
            stopAt = 23;
        if (startAt == -1 || startAt == ALL_SPEC_INT)
            startAt = 0;
    } else if (type == DAY_OF_MONTH) {
        if (stopAt == -1)
            stopAt = 31;
        if (startAt == -1 || startAt == ALL_SPEC_INT)
            startAt = 1;
    } else if (type == MONTH) {
        if (stopAt == -1)
            stopAt = 11;
        if (startAt == -1 || startAt == ALL_SPEC_INT)
            startAt = 0;
    } else if (type == DAY_OF_WEEK) {
        if (stopAt == -1)
            stopAt = 6;
        if (startAt == -1 || startAt == ALL_SPEC_INT)
            startAt = 0;
    } else if (type == YEAR) {
        if (stopAt == -1)
            stopAt = 135;
        if (startAt == -1 || startAt == ALL_SPEC_INT)
            startAt = 70;
    }
    /*fprintf(stdout, "startAt = %d stopAt = %d\n", startAt, stopAt);*/
    for (i = startAt; i <= stopAt; i += incr)
        lpr_rbtree_insert(vSet, intdup(i), (void*)1, 1);

    return 0;
}


static int getMonthNumber(const char* month) {
    if(lpr_string_icmp(month, "JAN") == 0)
        return 0;
    if(lpr_string_icmp(month, "FEB") == 0)
        return 1;
    if(lpr_string_icmp(month, "MAR") == 0)
        return 2;
    if(lpr_string_icmp(month, "APR") == 0)
        return 3;
    if(lpr_string_icmp(month, "MAY") == 0)
        return 4;
    if(lpr_string_icmp(month, "JUN") == 0)
        return 5;
    if(lpr_string_icmp(month, "JUL") == 0)
        return 6;
    if(lpr_string_icmp(month, "AUG") == 0)
        return 7;
    if(lpr_string_icmp(month, "SEP") == 0)
        return 8;
    if(lpr_string_icmp(month, "OCT") == 0)
        return 9;
    if(lpr_string_icmp(month, "NOV") == 0)
        return 10;
    if(lpr_string_icmp(month, "DEC") == 0)
        return 11;
    return -1;
}

static int getWeekNumber(const char* week) {
    if(lpr_string_icmp(week, "SUN") == 0)
        return 0;
    if(lpr_string_icmp(week, "MON") == 0)
        return 1;
    if(lpr_string_icmp(week, "TUE") == 0)
        return 2;
    if(lpr_string_icmp(week, "WED") == 0)
        return 3;
    if(lpr_string_icmp(week, "THU") == 0)
        return 4;
    if(lpr_string_icmp(week, "FRI") == 0)
        return 5;
    if(lpr_string_icmp(week, "SAT") == 0)
        return 6;
    return -1;
}

static int getNumericValue(const char* value ,int offset) {
    char temp[11];
	offset;
    strncpy(temp, value, 10);
    temp[10] = '\0';
    return atoi(temp);
}

static value_set_t getValueSet(const char* s, int i) {
    char c = s[i];
    value_set_t vs;

    vs.pos = i;

    while( c >= '0' && c <= '9') {
        vs.pos ++;
        if(vs.pos >= (int)strlen(s))
            break;
        c = s[vs.pos];
    }
    if( vs.pos > i) {
        char temp[11];
        strncpy(temp, s+i, vs.pos);
        temp[vs.pos] = '\0';
        vs.value = atoi(temp);
    }
    return vs;
}

static int checkNext(cronexpr_t* cron, int pos, const char* s, int val, int type) {
    int end = -1;
    int i = pos;
    int slen = (int)strlen(s);
    char c;

    if (i >= slen) {
        return addToSet(cron, val, end, -1, type);
    }

    c = s[pos];

    /*if (c == 'L') {
        if (type == DAY_OF_WEEK) lastdayOfWeek = true;
        else
            throw new ParseException("'L' option is not valid here. (pos="
                    + i + ")", i);
        TreeSet set = getSet(type);
        set.add(new Integer(val));
        i++;
        return i;
    }

    if (c == 'W') {
        if(type == DAY_OF_MONTH) nearestWeekday = true;
        else
            throw new ParseException("'W' option is not valid here. (pos="
                    + i + ")", i);
        TreeSet set = getSet(type);
        set.add(new Integer(val));
        i++;
        return i;
    }*/

    if (c == '-') {
        value_set_t value;
        
        i++;        
        value = getValueSet(s,i);
        i = value.pos;
        end = value.value;

        if (i < slen && ((c = s[i]) == '/')) {
            int incr;
            i++;
            
            value = getValueSet(s, i);
            i = value.pos;
            incr = value.value;

            return addToSet(cron, val, end, incr, type);
        } else {
            return addToSet(cron, val, end, 1, type);
        }
    }

    if (c == '/') {
        int incr;
        value_set_t value;
        
        i++;
        value = getValueSet(s, i);
        incr = value.value;
        i = value.pos;
        
        if(incr != -1) {
            return addToSet(cron, val, end, incr, type);
        } else {
            fprintf(stderr,"%s:/后面应该为数字", cron->expr);
            return -1;
        }
    }

    return addToSet(cron, val, end, 0, type);
}


static int storeExpressionVals(cronexpr_t* cron, const char* value, int type) {

    int incr = 0;
    int i = 0;
    int valueLen = (int)strlen(value);
    char c = value[i];

    /*月份或周*/
    if(c >= 'A' && c <= 'Z' && strcmp(value, "L") && strcmp(value,"LW")) {
        char sub[4];
        int sval = -1;
        int eval = -1;
        
        strncpy(sub, value, 3);
        sub[3] = '\0';

        if(type == MONTH) {
            sval = getMonthNumber(sub);
            if(sval < 0) {
                fprintf(stderr,"%s:月份的值不正确:%s", cron->expr, sub);
                return -1;
            }
            if(valueLen > 3) {
                c = value[3];
                if(c == '-') {
                    i += 4;
                    strncpy(sub, value+i, 3);
                    sub[3] = '\0';
                    eval = getMonthNumber(sub);
                    if(eval < 0) {
                        fprintf(stderr,"%s:月份的值不正确:%s",cron->expr, sub);
                        return -1;
                    }
                }
            }
        } else if(type == DAY_OF_WEEK) {
            sval = getWeekNumber(sub);
            if(sval < 0) {
                fprintf(stderr,"%s:星期的值不正确:%s",cron->expr, sub);
                return -1;
            }
            if(valueLen > 3) {
                c = value[3];
                if(c == '-') {
                    i += 4;
                    strncpy(sub, value + 1, 3);
                    sub[3] = '\0';

                    eval = getWeekNumber(sub);
                    if(eval < 0) {
                        fprintf(stderr,"%s:星期的值不正确:%s",cron->expr, sub);
                        return -1;
                    }
                } else if (c == 'L') {
                    cron->lastdayOfWeek = 1;
                    i ++ ;
                }
            }
        } else {
            fprintf(stderr,"%s:该位置的值不正确:%s",cron->expr, sub);
            return -1;
        }
        if(eval != -1)
            incr = 1;
        return addToSet(cron, sval, eval, incr, type);
    }

    if(c == '?') {
        if(type != DAY_OF_WEEK && type != DAY_OF_MONTH) {
            fprintf(stderr,"%s:?只能用于DAY_OF_WEEK或DAY_OF_MONTH", cron->expr);
            return -1;
        }
        return addToSet(cron, NO_SPEC_INT, -1, 0, type);
    }

    if(c == '*' || c== '/') {
        if(c == '*' && (i + 1) >= valueLen) {
            return addToSet(cron, ALL_SPEC_INT, -1, 0, type);
        } else if(c == '/' && (i + 1 >= valueLen || value[i+1] == ' ' || value[i+1] == '\t')) {
            fprintf(stderr,"%s:/后面只能跟着数字 : %s", cron->expr, value);
            return -1;
        } else if(c == '*')
            i ++;
        c = value[i];
        if(c == '/') {
            i ++ ;
            if(i >= valueLen) {
                fprintf(stderr,"%s:字符串非正常结束 : %s", cron->expr,value);
                return -1;
            }
            incr = getNumericValue(value, i);
            i ++;
            if(incr >= 10)
                i ++;
            if(incr > 59 && (type == SECOND || type == MINUTE)) {
                fprintf(stderr,"%s:增量大于59 : %d", cron->expr,incr);
                return -1;
            } else if(incr > 23 && type == HOUR) {
                fprintf(stderr,"%s:增量大于23 : %d",cron->expr, incr);
                return -1;
            } else if(incr > 31 && type == DAY_OF_MONTH) {
                fprintf(stderr,"%s:增量大于31 : %d",cron->expr, incr);
                return -1;
            } else if(incr > 7 && type == DAY_OF_WEEK) {
                fprintf(stderr,"%s:增量大于7 : %d",cron->expr, incr);
                return -1;
            } else if(incr > 12 && type == MONTH) {
                fprintf(stderr,"%s:增量大于12 : %d", cron->expr, incr);
                return -1;
            }
        } else
            incr = 1;
        return addToSet(cron, ALL_SPEC_INT, -1, incr, type);

    } else if (c == 'L') {
        i ++;
        if(type == DAY_OF_MONTH)
            cron->lastdayOfMonth = 1;
        if(type == DAY_OF_WEEK)
            return addToSet(cron, 6, 6, 0, type);
    } else if (c >= '0' && c <= '9') {
        int n = i;
        char sub[11];
        
        n ++;
        if(n < valueLen) {
            c = value[n];
            while(c >= '0' && c <= '9') {
                n ++ ;
                if(n >= valueLen)
                    break;
                c = value[n];
            }
        }
        
        strncpy(sub, value + i, n);
        sub[n] = '\0';

        return checkNext(cron, n, value, atoi(sub), type);
    } else {
        fprintf(stderr,"%s:无法预期的字符%s", cron->expr,value);
        return -1;
    }

    return 0;
}


static int build(cronexpr_t* cron, const char* expr) {
    char buffer[1024];
    char *exprsTokens[MAX_TOKEN_NUM] = {NULL};
    int exprCurrent = SECOND;
    int exprsTokensCount, i, j;

    strcpy(buffer, expr);
    exprsTokensCount = split(buffer, EXPR_DELIM, exprsTokens, MAX_TOKEN_NUM);


    for(i=0; i<exprsTokensCount; i++ ) {
        char* valuesTokens[16];
        int valuesTokensCount;

        valuesTokensCount = split(exprsTokens[i], EXPR_DELIM_COMMA, valuesTokens, 16);

        for(j=0; j<valuesTokensCount; j++) {
            if(storeExpressionVals(cron,valuesTokens[j], exprCurrent))
                return -1;
        }
        exprCurrent ++ ;
    }

if(exprCurrent <= DAY_OF_WEEK) {}

    if(exprCurrent <= YEAR) {
        if(storeExpressionVals(cron,"*",YEAR)) {
            return -1;
        }
    }

    return 0;
}


void cronexpr_destroy(cronexpr_t* cron) {
    int i;
    
    if(cron == NULL)
    	return;
    
    free(cron->expr);
    
    for(i=0; i<7; i++) {
        lpr_rbtree_destroy(cron->sets[i]);
    }
    free(cron);
}

MALLOC_FUN(cronexpr_create_malloc)
FREE_FUN(cronexpr_create_free)

cronexpr_t *cronexpr_create(const char* expr) {
    int i;
    cronexpr_t *cron = (cronexpr_t *)malloc(sizeof(cronexpr_t));
    memset(cron, 0x0, sizeof(cronexpr_t));
    cron->expr = strdup(expr);
    for(i=0; i<7; i++) {
        cron->sets[i] = lpr_rbtree_create(cronexpr_create_malloc, (lpr_cmp_func_t)intcmp, (lpr_free_func_t)cronexpr_create_free, NULL);
    }
    if(build(cron, expr) || cronexpr_after(cron, 0) == 0) {
        cronexpr_destroy(cron);
        return NULL;
    }
    return cron;
}
/**
 * 判断一个年数是不是闰年 
 * @param year 年数
 * @return bool 如果为闰年返回true,否则返回false
 */
static int isLeapYear(int tm_year) {
    tm_year += 1900;
    if(tm_year % 4 == 0) {
        if(tm_year % 100 == 0 ) {
            if(tm_year % 400 == 0) {
                return 1;
            }
            return 0;
        }
        return 1;
    }
    return 0;
}
/**
 * 得到一个月的最后一天
 * @param year 年数
 * @param month 月份
 * @return int 该月的最后一天
 */
static int getLastDayOfMonth(int tm_year,int tm_month) {
    int febDay = 28;
    
    switch(tm_month + 1) {
    case 2:
        if(isLeapYear(tm_year)) {
            febDay = 29;
        }
        return febDay;
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
        break;
    default:
        return 31;
    }
}
/*
static void print_entry (void* key, void* value) {
    fprintf(stdout, "%d",*(int*)key);
}

static void print_tm(struct tm* tm) {
    fprintf(stdout, "%d-%d-%d, %d:%d:%d",
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
}
*/
static struct tm* inc_seconds(struct tm* tm, int seconds) {
    time_t t = mktime(tm);
    t += seconds;
    lpr_localtime(&t, tm);
    return tm;
}

static struct tm* inc_month(struct tm* tm) {
    int lastMonthDay;
    if(tm->tm_mon >= 11)  {
        tm->tm_year ++;
        tm->tm_mon = 0;
    } else {
        tm->tm_mon ++;
    }
    lastMonthDay = getLastDayOfMonth(tm->tm_year+1900, tm->tm_mon+1);
    if(tm->tm_mday > lastMonthDay) {
        tm->tm_mday = lastMonthDay;
    }
    return tm;
}

time_t cronexpr_before(cronexpr_t* cron, time_t beforeTime) {
	static int stage[] = {1, 60, 60*60, 60*60*24, 60*60*24*31, 60*60*24*31*12};
	time_t begin = 0, atime;
	int i;
	
	for(i=0; i<sizeof(stage)/sizeof(int); i++) {
		atime = beforeTime - stage[i];
		begin = cronexpr_after(cron, atime);
		if(begin < beforeTime)
			break;
	}
	
	if(begin >= beforeTime) {
		fprintf(stderr,"%s: Getting time before (%d) is failed.", cron->expr, beforeTime);
        return 0;  
	}
	
	for (; ;) {
		atime = cronexpr_after(cron, begin);
		if(atime >= beforeTime)
			break;
		begin = atime;
	} 
	
	assert(begin < beforeTime);
	return begin;
}

time_t cronexpr_after(cronexpr_t* cron, time_t afterTime) {
    lpr_rbnode_t* iter;
    int gotOne = 0;
    int tmp = 0;
    struct tm ltm;
    
    afterTime ++;
    lpr_localtime(&afterTime, &ltm);

    while(!gotOne) {
        /* get second
         look first position in set element> sec*/
        iter = lpr_rbtree_lower_bound(cron->sets[SECOND], &ltm.tm_sec);
        if(iter != lpr_rbtree_last()) {
            ltm.tm_sec = *(int*)iter->key;
        } else {
            ltm.tm_sec = *(int*)lpr_rbtree_first(cron->sets[SECOND])->key;
            inc_seconds(&ltm, 60); /*1min*/
        }

        /* get minute */
        iter = lpr_rbtree_lower_bound(cron->sets[MINUTE], &ltm.tm_min);
        if(iter != lpr_rbtree_last()) {
            tmp = ltm.tm_min;
            ltm.tm_min = *(int*)iter->key;
        } else {
            ltm.tm_min = *(int*)lpr_rbtree_first(cron->sets[MINUTE])->key;
            inc_seconds(&ltm, 60 * 60); /*1 hour*/
        }
        if(ltm.tm_min != tmp) {
            ltm.tm_sec = 0;
            continue;
        }
        tmp = -1;

        /* get hour */
        iter = lpr_rbtree_lower_bound(cron->sets[HOUR], &ltm.tm_hour);
        if(iter != lpr_rbtree_last()) {
            tmp = ltm.tm_hour;
            ltm.tm_hour = *(int*)iter->key;
        } else {
            ltm.tm_hour = *(int*)lpr_rbtree_first(cron->sets[HOUR])->key;
            inc_seconds(&ltm, 60 * 60 * 24); /*1 day*/
        }
        if(ltm.tm_hour != tmp) {
            ltm.tm_sec = 0;
            ltm.tm_min = 0;
            continue;
        }
        tmp = -1;

        
        /* get day */
        {
            int dayOfMSpec = (lpr_rbtree_search(cron->sets[DAY_OF_MONTH], &NO_SPEC_INT) == NULL);
            int dayOfWSpec = (lpr_rbtree_search(cron->sets[DAY_OF_WEEK], &NO_SPEC_INT) == NULL);
            if (dayOfMSpec && !dayOfWSpec) { /* get day by day of month rule */
                int tmon = ltm.tm_mon;
                iter = lpr_rbtree_lower_bound(cron->sets[DAY_OF_MONTH], &ltm.tm_mday);
                if (cron->lastdayOfMonth) {
                    tmp = ltm.tm_mday;
                    ltm.tm_mday = getLastDayOfMonth(ltm.tm_year, ltm.tm_mon);
                } else if (iter != lpr_rbtree_last()) {
                    tmp = ltm.tm_mday;
                    ltm.tm_mday = *(int*)iter->key;
                } else {
                    ltm.tm_mday = *(int*)lpr_rbtree_first(cron->sets[DAY_OF_MONTH])->key;
                    inc_month(&ltm);
                }
    
                if (ltm.tm_mday != tmp || ltm.tm_mon != tmon) {/*if month changes,lastday may change*/
                    ltm.tm_sec = 0;
                    ltm.tm_min = 0;
                    ltm.tm_hour = 0;
                    continue;
                }
            } else if (dayOfWSpec && !dayOfMSpec) { /* get day by day of week rule */
                int dow = 0;                        /* desired */
                int daysToAdd = 0;
                
                time_t ctime = mktime(&ltm);
                lpr_localtime(&ctime, &ltm);
                
                if(cron->lastdayOfWeek) {
                    dow = *(int*)lpr_rbtree_first(cron->sets[DAY_OF_WEEK])->key; /*desired*/
                } else {
                    iter = lpr_rbtree_lower_bound(cron->sets[DAY_OF_WEEK], &ltm.tm_wday);
                    if(iter != lpr_rbtree_last()) {
                        dow = *(int*)iter->key;
                    }
                }
                daysToAdd = (dow >= ltm.tm_wday) ? dow-ltm.tm_wday : dow + 7 -ltm.tm_wday;
                if(daysToAdd > 0) {                   
                    ltm.tm_sec = 0;
                    ltm.tm_min = 0;
                    ltm.tm_hour = 0;
                    inc_seconds(&ltm, 60 * 60 * 24 * daysToAdd);
                    continue;
                }
            } else { /* dayOfWSpec && !dayOfMSpec */
                fprintf(stderr,"%s:Support for specifying both a day-of-week AND a day-of-month parameter is not implemented.", cron->expr);
                return 0;                
            }
        }
        tmp = -1;

        /* test for expressions that never generate a valid fire date,
         but keep looping...*/
        if (ltm.tm_year > 135)
            return 0;

        /* get month */
        iter = lpr_rbtree_lower_bound(cron->sets[MONTH], &ltm.tm_mon);
        if(iter != lpr_rbtree_last()) {
            tmp = ltm.tm_mon;
            ltm.tm_mon = *(int*)iter->key;
        } else {
            ltm.tm_mon = *(int*)lpr_rbtree_first(cron->sets[MONTH])->key;
            ltm.tm_year ++;
        }
        if(ltm.tm_mon != tmp) {
            ltm.tm_sec = 0;
            ltm.tm_min = 0;
            ltm.tm_hour = 0;
            ltm.tm_mday = 0;
            continue;
        }
        tmp = -1;

        /* get year */
        iter = lpr_rbtree_lower_bound(cron->sets[YEAR], &ltm.tm_year);
        if(iter != lpr_rbtree_last()) {
            tmp = ltm.tm_year;
            ltm.tm_year = *(int*)iter->key;
        } else {
            return 0;
        }
        if(ltm.tm_year != tmp) {
            ltm.tm_sec = 0;
            ltm.tm_min = 0;
            ltm.tm_hour = 0;
            ltm.tm_mday = 0;
            ltm.tm_mon = 0;
            continue;
        }

        gotOne = 1;
    }
    return mktime(&ltm);
}

/* python wrapper end */

/* test interface next_cronexpr_time and last_cronexpr_time */
/*
int main()
{
	time_t base = time(0);
	time_t next = next_cronexpr_time("0 * * * * ?", base);
	time_t last = last_cronexpr_time("0 * * * * ?", base);
	struct tm* tm = localtime(&next);
	fprintf(stdout, "%d-%d-%d, %d:%d:%d\n",
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec); 
	tm = localtime(&last);
	fprintf(stdout, "%d-%d-%d, %d:%d:%d\n",
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
    getchar();
   	return 0;
}
*/

/*
int main() {
    cronexpr_t * cron = cronexpr_create("0 0 * ? SEP SUN");
    if(cron) {
        int i;
        time_t base = cronexpr_after(cron, time(0)), begin, atime;//0 + 60*60*15
        
        for(i=0; i<100; i++) {
            struct tm* tm;
            atime = base;
            base = cronexpr_after(cron, base);  
            begin = cronexpr_before(cron, base);
            assert(begin == atime);
            
            tm = localtime(&begin);
            fprintf(stdout, "%d-%d-%d, %d:%d:%d",
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);            
                 
            
            tm = localtime(&base);
            fprintf(stdout, " - %d-%d-%d, %d:%d:%d\n",
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);                   
        }
        cronexpr_destroy(cron);
    }
    return 0;
}
*/

