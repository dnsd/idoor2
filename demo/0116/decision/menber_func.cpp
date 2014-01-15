#include <iostream>
#include <ssm.hpp>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "send_spotinfo.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

void step_data::set_data(char& read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[])
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

void step_data::set_close_flag()
{
    for (int i = 0; i < STEP_NUM; i++)
    {
        double dist_diff = 0.0;
        dist_diff = dist[i][PREPRE_INDEX] - dist[i][CUR_INDEX];
        if (dist_diff > CLOSE_TH)
        {
            close_flag[i][CUR_INDEX] = true;
        }else{
            close_flag[i][CUR_INDEX] = false;
        }
    }
}

void step_data::set_stop_flag()
{
    for (int i = 0; i < STEP_NUM; i++)
    {
        double dist_diff = 0.0;
        dist_diff = dist[i][PREPRE_INDEX] - dist[i][CUR_INDEX];
        if (fabs(dist_diff) < STOP_TH)
        {
            stop_flag[i][CUR_INDEX] = true;
        }else{
            stop_flag[i][CUR_INDEX] = false;
        }
    }
}