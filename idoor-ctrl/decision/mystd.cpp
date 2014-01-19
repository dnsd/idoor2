#include <iostream>
#include <cmath>
#include <sys/time.h>

using namespace std;

double get_time(void) //現在時刻を取得
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}
