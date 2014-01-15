#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <ssm.hpp>
#include <cmath>
#include "connect.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

void Step_buf::bufsort()
{
    for (int i = 0; i < STEP_NUM; i++)
    {
        sort(dist[i].begin(), dist[i].end());
        sort(x[i].begin(), x[i].end());
        sort(y[i].begin(), y[i].end());
        sort(z[i].begin(), z[i].end());
    }
}

bool Step_buf::isInTheArea(int step_num)
{
    if (AREA_START_Y <= y[step_num][CUR_INDEX] && y[step_num][CUR_INDEX] <= AREA_END_Y
            && AREA_START_X <= x[step_num][CUR_INDEX] && x[step_num][CUR_INDEX] <= AREA_END_X
            && AREA_START_Z <= z[step_num][CUR_INDEX] && z[step_num][CUR_INDEX] <= AREA_END_Z)
    {
        return true;
    }else{
        return false;
    }
}

bool Step_buf::isMoving(int step_num)
{
    if ( 200 < fabs(dist[step_num][CUR_INDEX] - dist[step_num][PREPRE_INDEX])
            // && fabs(dist[step_num][CUR_INDEX] - dist[step_num][PREPRE_INDEX]) < MOVING_TH_MAX
            && dist[step_num][CUR_INDEX] != 0.0
            && dist[step_num][PRE_INDEX] != 0.0)
    {
        return true;
    }else{
        return false;
    }
}

bool Step_buf::isStop(int step_num)
{
    if (fabs(dist[step_num][CUR_INDEX] - dist[step_num][PRE_INDEX]) < MOVING_TH_MIN)
    {
        return true;
    }else{
        return false;
    }
}

void Step_buf::out_csv()
{
    ofstream ofs;
    ofs.open("buf_out.csv");
    ofs.precision(16);

    for(int j=0; j<CUR_INDEX; j++)
    {
        for(int i=0; i<STEP_NUM; i++)
        {
            ofs << dist[i][j] << ",";
        }
        ofs << endl;
    }

    ofs.close();
}

void Step_buf::set_data(char read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[])
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

