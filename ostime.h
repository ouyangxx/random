#ifndef __OSTIME_H__
#define __OSTIME_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#endif
    
typedef struct timeval Time_t;
    
typedef struct
{
    char toString[32];
    
}DateTime_t;
    
    
void _nsleep(unsigned long nsecs);
    
void _usleep(unsigned long usecs);
    
void _msleep(unsigned long msecs);
    
void _sleep(unsigned long secs);
    
    
void time_now(Time_t *t);
    
time_t time_valuesec(Time_t *t);
    
long time_valueusec(Time_t *t);
    
long time_usec_sub(Time_t *t1, Time_t *t2);
    
long time_sec_sub(Time_t *t1, Time_t *t2);


long CurrentTimeZone();
    
    
void CurrentDateTimeUtc(DateTime_t *dateTime);
    
time_t DateTimeUtc_valuesec(DateTime_t *dateTime);
    
long DateTimeUtc_usec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2);
    
long DateTimeUtc_sec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2);
    
void DateTimeUtc2Time_t(DateTime_t *dateTime, Time_t *t);
    
void Time_t2DateTimeUtc(Time_t *t, DateTime_t *dateTime);
    
    
void CurrentDateTimeLocal(DateTime_t *dateTime);
    
time_t DateTimeLocal_valuesec(DateTime_t *dateTime);
    
long DateTimeLocal_usec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2);
    
long DateTimeLocal_sec_sub(DateTime_t *dateTime1, DateTime_t *dateTime2);
    
void DateTimeLocal2Time_t(DateTime_t *dateTime, Time_t *t);
    
void Time_t2DateTimeLocal(Time_t *t, DateTime_t *dateTime);
    
    
void DateTimeUTC2Local(DateTime_t *dateTimeUtc, DateTime_t *dateTimeLocal);
    
void DateTimeLocal2UTC(DateTime_t *dateTimeLocal, DateTime_t *dateTimeUtc);
    
    
void NewDateTime(const char *strtime, DateTime_t *dateTime);

void NewTime_t(time_t secs, Time_t *t);
    
#ifdef __cplusplus
}
#endif

#endif
