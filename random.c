#include <stdlib.h>
#include "random.h"
#include "ostime.h"

unsigned int random_uint()
{
    unsigned int ret, seed;
    unsigned int sec, usec;
    Time_t now;
    time_now(&now);
    sec = (unsigned int)time_valuesec(&now);
    usec = (unsigned int)time_valueusec(&now);
    seed = (((sec % 0xFFFF) << 16) | (usec % 0xFFFF));
    srand( seed );
    ret = rand();
    return ret;
}