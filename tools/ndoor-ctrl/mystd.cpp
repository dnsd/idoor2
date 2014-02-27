#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace std;

void CountDown(int count);
double dist_2p(double p1_x, double p1_y, double p2_x, double p2_y);
string get_localtime();
double get_time(void);
string IntToString(int number);
void sleep_const_freq(double cycle_start_time, double cycle_end_time, double freq);

void CountDown(int count)
{
    for (; count < 0; count--) {
        cout << count << endl;
        usleep(1000000);
    }
}

double dist_2p(double p1_x, double p1_y, double p2_x, double p2_y)
{
    double dist = 0.0;

    dist = sqrt( (p2_x - p1_x)*(p2_x - p1_x) + (p2_y - p1_y)*(p2_y - p1_y) );

    return dist;
}

double dist_2p3D(double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z)
{
    double dist = 0.0;

    dist = sqrt( (p2_x - p1_x)*(p2_x - p1_x) + (p2_y - p1_y)*(p2_y - p1_y) + (p2_z - p1_z)*(p2_z - p1_z) );

    return dist;
}

string get_localtime()
{
    time_t timer;
    struct tm *t_st;
    string jst;

    time(&timer);

    t_st = localtime(&timer);

    jst += IntToString(t_st->tm_year+1900);
    jst += IntToString(t_st->tm_mon+1);
    jst += IntToString(t_st->tm_mday);
    jst += IntToString(t_st->tm_hour);
    jst += IntToString(t_st->tm_min);
    jst += IntToString(t_st->tm_sec);

    return jst;
}

double get_time(void) 
{
    //-現在時刻を取得-//
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}

string IntToString(int number)
{
    stringstream ss;
    ss << number;
    return ss.str();
}

void sleep_const_freq(double cycle_start_time, double cycle_end_time, double freq)
{
    //-周期を合わせるために待機-//
    //freqの単位は「秒」
    if ((cycle_end_time - cycle_start_time) < freq)
    {
        cout << "wait = " << (cycle_end_time - cycle_start_time) << endl;
        usleep(freq * 1000000 - (cycle_end_time - cycle_start_time) * 1000000);
    }
}
