#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "ostime.h"

/**
 本地时间(locale time)
 格林威治时间（Greenwich Mean Time GMT）－ 原子时
 时间协调时间 （Universal Time Coordinated UTC）－比原子时更精确
 
 GPS 系统中有两种时间区分，一为UTC，另一为LT（地方时）两者的区别为时区不同，UTC就是0时区的时间
 可以认为格林威治时间就是时间协调时间（GMT=UTC），格林威治时间和UTC时间均用秒数来计算的。
 
 #include <time.h> 常用的时间函数：
 char *asctime(const struct tm* timeptr);＃将结构中的信息转换为真实世界的时间，以字符串的形式显示
 char *ctime(const time_t *timep);＃将timep转换为真是世界的时间，以字符串显示，它和asctime不同就在于传入的参数形式不一样
 double difftime(time_t time1, time_t time2);＃返回两个时间相差的秒数
 int gettimeofday(struct timeval *tv, struct timezone *tz);＃返回当前距离1970年的秒数和微妙数，后面的tz是时区，一般不用
 struct tm* gmtime(const time_t *timep);＃将time_t表示的时间转换为没有经过时区转换的UTC时间，是一个struct tm结构指针
 stuct tm* localtime(const time_t *timep);＃和gmtime类似，但是它是经过时区转换的时间。
 time_t mktime(struct tm* timeptr);＃将struct tm 结构的时间转换为从1970年至今的秒数
 time_t time(time_t *t);＃取得从1970年1月1日至今的秒数。
 
 
 
 * time_t: 表示从1970-01-01 00:00:00 至今的秒数
 *
 * localTime = UTC + 时区差
 * 时区差东为正，西为负，
 *    北京时区是东八区，领先UTC八小时，时区差记为 +0800, 北京时间 = UTC + (+0800)
 *    纽约时区是西五区，比UTC落后五小时，时区差记为－0500，纽约时间 = UTC + (－0500)
 *
 * 时区差计算方法：
        time_t now;
        time(&now);
        time_t local_secs = mktime(localtime(&now));
        time_t gm_secs = mktime(gmtime(&now));
        long timezone = difftime(local_secs, gm_secs); //表示local_secs-gm_secs
 * C语言内置宏timezone: 表示gm_secs－local_secs，是时区差的负值
 *
 * gmtime要比东八区的localtime小8小时
 * struct tm *gmtime(const time_t *timep); 此函数返回的时间日期未经时区转换，而是UTC 时间
 * struct tm *localtime(const time_t * timep); 此函数返回的时间日期已经转换成当地时区
 */



#ifdef WIN32
static int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year     = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday     = wtm.wDay;
    tm.tm_hour     = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm.tm_isdst    = -1;
    clock = mktime(&tm);
	#ifdef WIN32
	tp->tv_sec = clock & 0xffffffff;//time_t/__int64(64bit) -> long(32bit)
	#else
    tp->tv_sec = clock;
	#endif
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

void _nsleep(unsigned long nsecs)
{
    #ifdef WIN32
    __int64 timeElapsed;
    __int64 timeStart;
    __int64 timeFreq;
    
    QueryPerformanceFrequency((LARGE_INTEGER *)(&timeFreq));
	__int64 timeToWait = (__int64)((double)timeFreq * (double)nsecs / 1000000000.0f);
    QueryPerformanceCounter((LARGE_INTEGER *)(&timeStart));
    timeElapsed = timeStart;
    while ((timeElapsed - timeStart) < timeToWait)
    {
        QueryPerformanceCounter((LARGE_INTEGER *)(&timeElapsed));
    }
    #else
    struct timespec requested, remaining;
    
    requested.tv_sec  = 0;
    requested.tv_nsec = nsecs;
    
    while (nanosleep(&requested, &remaining) == -1)
        if (errno == EINTR)
            requested = remaining;
        else {
            
            break;
        }
    #endif
}

void _usleep(unsigned long usecs)
{
    #ifdef WIN32
	unsigned long nsecs = usecs * 1000;
    _nsleep(nsecs);
    #else
    struct timespec requested, remaining;
    
    if (usecs >= 1000 * 1000) {
        requested.tv_sec  = usecs / (1000 * 1000);
        requested.tv_nsec = (usecs % (1000 * 1000)) * 1000L;
    } else {
        requested.tv_sec  = 0;
        requested.tv_nsec = usecs * 1000L;
    }
    
    while (nanosleep(&requested, &remaining) == -1)
        if (errno == EINTR)
            requested = remaining;
        else {
            
            break;
        }
    #endif
}

void _msleep(unsigned long msecs)
{
    #ifdef WIN32
    Sleep(msecs);
    #else
    struct timespec requested, remaining;
    
    if (msecs >=  1000) {
        requested.tv_sec  = msecs / 1000;
        requested.tv_nsec = (msecs % 1000) * 1000 * 1000L;
    } else {
        requested.tv_sec  = 0;
        requested.tv_nsec = msecs * 1000 * 1000L;
    }
    
    while (nanosleep(&requested, &remaining) == -1)
        if (errno == EINTR)
            requested = remaining;
        else {
            
            break;
        }
    #endif
}

void _sleep(unsigned long secs)
{
    #ifdef WIN32
    Sleep(secs * 1000);
    #else
    sleep(secs);
    #endif
}

void time_now(Time_t *t)
{
    gettimeofday(t, NULL);
}

time_t time_valuesec(Time_t *t)
{
    return t->tv_sec;
}

long time_valueusec(Time_t *t)
{
    return t->tv_usec;
}

long time_usec_sub(Time_t *t1, Time_t *t2)
{
    return (t1->tv_sec  - t2->tv_sec) * 1000 * 1000 +
    (t1->tv_usec - t2->tv_usec);
}

long time_sec_sub(Time_t *t1, Time_t *t2)
{
    return (t1->tv_sec - t2->tv_sec) +
    (t1->tv_usec - t2->tv_usec) / (1000 * 1000);
}

long CurrentTimeZone()
{
    time_t now;
    time(&now);
    time_t local_secs = mktime(localtime(&now));
    time_t gm_secs = mktime(gmtime(&now));
    long timezone = (long)difftime(local_secs, gm_secs);
    return timezone;
}

void CurrentDateTimeUtc(DateTime_t *dateTime)
{
    Time_t now;
    time_now(&now);
	time_t tmNow = now.tv_sec;
	struct tm * gm = gmtime(&tmNow);
    int year = gm->tm_year + 1900;
    int month = gm->tm_mon + 1;
    int day = gm->tm_mday;
    int hour = gm->tm_hour;
    int minute = gm->tm_min;
    int second = gm->tm_sec;
    int millisecond = now.tv_usec/1000;
    
    memset(dateTime->toString, 0, sizeof(dateTime->toString));
    sprintf(dateTime->toString, "%04d-%02d-%02d %02d:%02d:%02d:%03d", year, month, day, hour, minute, second, millisecond);
}

time_t DateTimeUtc_valuesec(DateTime_t *dateTime)
{
    Time_t t;
    DateTimeUtc2Time_t(dateTime, &t);
    return time_valuesec(&t);
}

long DateTimeUtc_usec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2)
{
    Time_t t1, t2;
    DateTimeUtc2Time_t(dateTime1, &t1);
    DateTimeUtc2Time_t(dateTime2, &t2);
    return time_usec_sub(&t1, &t2);
}

long DateTimeUtc_sec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2)
{
    Time_t t1, t2;
    DateTimeUtc2Time_t(dateTime1, &t1);
    DateTimeUtc2Time_t(dateTime2, &t2);
    return time_sec_sub(&t1, &t2);
}

void DateTimeUtc2Time_t(DateTime_t *dateTime, Time_t *t)
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int millisecond = 0;
    sscanf(dateTime->toString, "%04d-%02d-%02d %02d:%02d:%02d:%03d", &year, &month, &day, &hour, &minute, &second, &millisecond);
    
    time_t clock;
    struct tm tm;
    tm.tm_year = year-1900;
    tm.tm_mon = month-1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min  = minute;
    tm.tm_sec  = second;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
	#ifdef WIN32
	t->tv_sec = clock & 0xffffffff + CurrentTimeZone();//time_t/__int64(64bit) -> long(32bit)
	#else
    t->tv_sec = clock + CurrentTimeZone();
	#endif
    t->tv_usec = millisecond * 1000;
}

void Time_t2DateTimeUtc(Time_t *t, DateTime_t *dateTime)
{
    struct tm * gm = gmtime(&t->tv_sec);
    int year = gm->tm_year + 1900;
    int month = gm->tm_mon + 1;
    int day = gm->tm_mday;
    int hour = gm->tm_hour;
    int minute = gm->tm_min;
    int second = gm->tm_sec;
    int millisecond = t->tv_usec/1000;
    
    memset(dateTime->toString, 0, sizeof(dateTime->toString));
    sprintf(dateTime->toString, "%04d-%02d-%02d %02d:%02d:%02d:%03d", year, month, day, hour, minute, second, millisecond);
}

void CurrentDateTimeLocal(DateTime_t *dateTime)
{
    Time_t now;
    time_now(&now);
    struct tm * gm = localtime(&now.tv_sec);
    int year = gm->tm_year + 1900;
    int month = gm->tm_mon + 1;
    int day = gm->tm_mday;
    int hour = gm->tm_hour;
    int minute = gm->tm_min;
    int second = gm->tm_sec;
    int millisecond = now.tv_usec/1000;
    
    memset(dateTime->toString, 0, sizeof(dateTime->toString));
    sprintf(dateTime->toString, "%04d-%02d-%02d %02d:%02d:%02d:%03d", year, month, day, hour, minute, second, millisecond);
}

time_t DateTimeLocal_valuesec(DateTime_t *dateTime)
{
    Time_t t;
    DateTimeLocal2Time_t(dateTime, &t);
    return time_valuesec(&t);
}

long DateTimeLocal_usec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2)
{
    Time_t t1, t2;
    DateTimeLocal2Time_t(dateTime1, &t1);
    DateTimeLocal2Time_t(dateTime2, &t2);
    return time_usec_sub(&t1, &t2);
}

long DateTimeLocal_sec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2)
{
    Time_t t1, t2;
    DateTimeLocal2Time_t(dateTime1, &t1);
    DateTimeLocal2Time_t(dateTime2, &t2);
    return time_sec_sub(&t1, &t2);
}

void DateTimeLocal2Time_t(DateTime_t *dateTime, Time_t *t)
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int millisecond = 0;
    sscanf(dateTime->toString, "%04d-%02d-%02d %02d:%02d:%02d:%03d", &year, &month, &day, &hour, &minute, &second, &millisecond);
    
    time_t clock;
    struct tm tm;
    tm.tm_year = year-1900;
    tm.tm_mon = month-1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min  = minute;
    tm.tm_sec  = second;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
	#ifdef WIN32
	t->tv_sec = clock & 0xffffffff;//time_t/__int64(64bit) -> long(32bit)
	#else
    t->tv_sec = clock;
	#endif
    t->tv_usec = millisecond * 1000;
}

void Time_t2DateTimeLocal(Time_t *t, DateTime_t *dateTime)
{
    struct tm * gm = localtime(&t->tv_sec);
    int year = gm->tm_year + 1900;
    int month = gm->tm_mon + 1;
    int day = gm->tm_mday;
    int hour = gm->tm_hour;
    int minute = gm->tm_min;
    int second = gm->tm_sec;
    int millisecond = t->tv_usec/1000;
    
    memset(dateTime->toString, 0, sizeof(dateTime->toString));
    sprintf(dateTime->toString, "%04d-%02d-%02d %02d:%02d:%02d:%03d", year, month, day, hour, minute, second, millisecond);
}

void DateTimeUTC2Local(DateTime_t *dateTimeUtc, DateTime_t *dateTimeLocal)
{
    Time_t t;
    DateTimeUtc2Time_t(dateTimeUtc, &t);
    Time_t2DateTimeLocal(&t, dateTimeLocal);
}

void DateTimeLocal2UTC(DateTime_t *dateTimeLocal, DateTime_t *dateTimeUtc)
{
    Time_t t;
    DateTimeLocal2Time_t(dateTimeLocal, &t);
    Time_t2DateTimeUtc(&t, dateTimeUtc);
}

void NewDateTime(const char *strtime, DateTime_t *dateTime)
{
    memset(dateTime->toString, 0, sizeof(dateTime->toString));
    strncpy(dateTime->toString, strtime, strlen(strtime));
    dateTime->toString[sizeof(dateTime->toString) - 1] = '\0';
}

void NewTime_t(time_t secs, Time_t *t)
{
	#ifdef WIN32
	t->tv_sec = secs & 0xffffffff;//time_t/__int64(64bit) -> long(32bit)
	#else
	t->tv_sec = secs;
	#endif
	t->tv_usec = 0;
}

#if 0
int main(void)
{
    Time_t t;
    time_now(&t);
    printf("utc sec: %ld\n", time_valuesec(&t));
    
    DateTime_t nowUtc, nowLocal;
    CurrentDateTimeUtc(&nowUtc);
    CurrentDateTimeLocal(&nowLocal);
    printf("nowUtc: %s, nowLocal: %s\n", nowUtc.toString, nowLocal.toString);
    
    Time_t t_utc, t_local;
    DateTimeUtc2Time_t(&nowUtc, &t_utc);
    DateTimeLocal2Time_t(&nowLocal, &t_local);
    printf("t_utc: %ld, t_local: %ld\n", t_utc.tv_sec, t_local.tv_sec);
    
    Time_t2DateTimeUtc(&t_utc, &nowUtc);
    Time_t2DateTimeLocal(&t_local, &nowLocal);
    printf("nowUtc: %s, nowLocal: %s\n", nowUtc.toString, nowLocal.toString);
    
    DateTimeUTC2Local(&nowUtc, &nowLocal);
    printf("nowUtc: %s, nowLocal: %s\n", nowUtc.toString, nowLocal.toString);
    
    DateTimeLocal2UTC(&nowLocal, &nowUtc);
    printf("nowUtc: %s, nowLocal: %s\n", nowUtc.toString, nowLocal.toString);
    
    DateTime_t startTime,finishTime;
    NewDateTime("2017-06-18 06:20:20", &startTime);
    NewDateTime("2017-06-18 22:20:20", &finishTime);
    time_t stime = DateTimeUtc_valuesec(&startTime);
    time_t ftime = DateTimeUtc_valuesec(&finishTime);
    int secsub = DateTimeUtc_sec_sub(&finishTime, &startTime);
    DateTime_t startTimeLocal, finishTimeLocal;
    DateTimeUTC2Local(&startTime, &startTimeLocal);
    DateTimeUTC2Local(&finishTime, &finishTimeLocal);
    printf("startTime: %ld, finishTime: %ld, secsub: %d, startTimeLocal: %s, finishTimeLocal: %s\n", stime, ftime, secsub, startTimeLocal.toString, finishTimeLocal.toString);
    
    return 0;
}
#endif
