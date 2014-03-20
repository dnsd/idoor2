#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <deque>
#include "decision.h"

using namespace std;

void Step::set_data(char& read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[])
{
    det.pop_front();
    for (int i = 0; i < STEP_NUM; i++)
    {
        dist[i].pop_front();
        x[i].pop_front();
        y[i].pop_front();
        z[i].pop_front();
    }

    det.push_back(read_data_det);
    for (int i = 0; i < STEP_NUM; i++)
    {
        dist[i].push_back(read_data_dist[i]);
        x[i].push_back(read_data_x[i]);
        y[i].push_back(read_data_y[i]);
        z[i].push_back(read_data_z[i]);
    }
}

void Step::init_steptime()
{
    for (int i = 1; i <= STEP_NUM; ++i)
    {
        steptime[i-1] = FREQ / STEP_NUM * i;
    }
}

bool Step::isClose(int step_num)
{
    double dist_diff = 0.0;
    dist_diff = dist[step_num][PREPRE_INDEX] - dist[step_num][CUR_INDEX];
    if (dist_diff > CLOSE_TH)
    {
        return true;
    }else{
        return false;
    }
}

bool Step::isStop(int step_num)
{
    double dist_diff = 0.0;
    dist_diff = dist[step_num][PREPRE_INDEX] - dist[step_num][CUR_INDEX];
    if (fabs(dist_diff) < STOP_TH)
    {
        return true;
    }else{
        return false;
    }
}
