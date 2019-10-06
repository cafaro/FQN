/**
 * @file    Timing.c 
 * @author  MC 
 * @brief   Routines to monitor spent time in computation.
 *
 */

#include  "Timing.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

// simple timing routines for testing
// these use global variables, so they will not interleave well

long secs, usecs;
struct timeval tt;

void StartTheClock()
{
    gettimeofday(&tt,NULL);
    secs=tt.tv_sec;
    usecs=tt.tv_usec;
}

double StopTheClock()
{
    gettimeofday(&tt,NULL);
    secs=tt.tv_sec-secs;
    usecs=tt.tv_usec-usecs;
    return 1000*secs+((double)usecs/1000.0);
}
