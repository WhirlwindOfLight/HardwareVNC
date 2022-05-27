#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef LIBVNCSERVER_HAVE_GETTIMEOFDAY
/* if we have gettimeofday(), it is in this header */
#include <sys/time.h>
#endif
#if !defined LIBVNCSERVER_HAVE_GETTIMEOFDAY && defined WIN32
#include <fcntl.h>
#include <conio.h>
#include <sys/timeb.h>
static void gettimeofday(struct timeval* tv,char* dummy)
{
   SYSTEMTIME t;
   GetSystemTime(&t);
   tv->tv_sec=t.wHour*3600+t.wMinute*60+t.wSecond;
   tv->tv_usec=t.wMilliseconds*1000;
}
#endif

/* Throttles camera updates to supplied fps */
int TimeToTakePicture(double fps) {
    static struct timeval now={0,0}, then={0,0};
    double elapsed, dnow, dthen;
    gettimeofday(&now,NULL);
    dnow  = now.tv_sec  + (now.tv_usec /1000000.0);
    dthen = then.tv_sec + (then.tv_usec/1000000.0);
    elapsed = dnow - dthen;
    if (elapsed > 1.0 / fps)
        memcpy((char *)&then, (char *)&now, sizeof(struct timeval));
    return elapsed > 1.0 / fps;
}
