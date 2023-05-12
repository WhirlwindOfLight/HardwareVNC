#include <iostream>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

bool TimeToTakePicture(double fps) {
    static std::chrono::steady_clock::time_point now, then;
    double elapsed, dnow, dthen;
#ifdef _WIN32
    SYSTEMTIME t;
    GetSystemTime(&t);
    dnow = t.wHour * 3600 + t.wMinute * 60 + t.wSecond + t.wMilliseconds / 1000.0;
#else
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    dnow = tv.tv_sec + (tv.tv_usec / 1000000.0);
#endif
    dthen = then.time_since_epoch().count() / 1e9;
    elapsed = dnow - dthen;
    if (elapsed > 1.0 / fps)
        then = now;
    return elapsed > 1.0 / fps;
}
