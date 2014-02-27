#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <ssm.hpp>
#include <cmath>
#include "decision.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

//-Area-//

bool Area::hasObjects(Step& rd) // "r"ead"d"ata
{
    // エリアに物体が存在するか？
    int step_cnt = 0;
    if (rd.det[CUR_INDEX] == 'U')
    {
        for (int i = 0; i < STEP_NUM; ++i)
        {
            if (rd.dist[i][CUR_INDEX] != 0.0
                    && area_th_min_U[i] <= rd.dist[i][CUR_INDEX]
                    && rd.dist[i][CUR_INDEX] <= area_th_max_U[i])
            {
                step_cnt++;
            }
        }
    }else if (rd.det[CUR_INDEX] == 'D')
    {
        for (int i = 0; i < STEP_NUM; ++i)
        {
            if (rd.dist[i][CUR_INDEX] != 0.0
                    && area_th_min_D[i] <= rd.dist[i][CUR_INDEX]
                    && rd.dist[i][CUR_INDEX] <= area_th_max_D[i])
            {
                step_cnt++;
            }
        }
    }
    cout << "step_cnt = " << step_cnt << endl;
    if (step_cnt >= step_cnt_th)
    {
        return true;
    }else{
        return false;
    }
}

int Area::judgeOpen(Step& rd)
{
    // ドアを開けるべきか？
    if(hasObjects(rd) == true)
    {
        hasObjects_buf.pop_front();
        hasObjects_buf.push_back(1);
    }else{
        hasObjects_buf.pop_front();
        hasObjects_buf.push_back(0);
    }

    int buf_num_cnt = 0;
    for (int i = 0; i < hasObjects_buf.size(); i++)
    {
        if (hasObjects_buf[i] == 1)
        {
            buf_num_cnt++;
        }
    }

    if (buf_num_cnt >= buf_cnt_th)
    {
        cout << "buf_num_cnt =" << buf_num_cnt << endl;
        return 4; // 高速全開
    }else{
        cout << "buf_num_cnt =" << buf_num_cnt << endl;
        return 0; // 開けない
    }
}

void Area::set_step_cnt_th(int parameter)
{
    step_cnt_th = parameter;
}

void Area::set_buf_cnt_th(int parameter)
{
    buf_cnt_th = parameter;
}

void Area::set_buf_length(int parameter)
{
    buf_length = parameter;
    hasObjects_buf.resize(buf_length);
}

//-AreaAABB-//
void AreaAABB::calAreaTh(BEAMANGLE angle){
    // AABBと直線の交差判定
    double sensor_pos[3] = {ORG_X, ORG_Y, ORG_Z}; // センサの取り付け位置

    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 136; ++j)
        {
            calc_xyz(angle.ux[i][j], angle.uy[i][j], SENSOR_RANGE, beam_range[136*i + j][0], beam_range[136*i + j][1], beam_range[136*i + j][2]);
            calc_xyz(angle.dx[i][j], angle.dy[i][j], SENSOR_RANGE, beam_range[136*i + j][0], beam_range[136*i + j][1], beam_range[136*i + j][2]);
        }
    }
    // U
    for (int i = 0; i < STEP_NUM; ++i)
    {
        double tmin = 0.0;
        double tmax = SENSOR_RANGE;
        double cp_min[3] = {0.0, 0.0, 0.0};
        double cp_max[3] = {0.0, 0.0, 0.0};

        for (int j = 0; j < 3; ++j)
        {
            float ood = 1.0 / beam_range[i][j];
            float t1 = (aabb_min[j] - sensor_pos[j]) * ood;
            float t2 = (aabb_max[j] - sensor_pos[j]) * ood;
            if (t1 > t2) swap(t1, t2);
            if (t1 > tmin) tmin = t1;
            if (t2 > tmax) tmax = t2;
            if (tmin > tmax) break;
        }
        for (int j = 0; j < 3; ++j)
        {
            cp_min[j] = sensor_pos[j] + beam_range[i][j] * tmin; // "c"ollision "p"oint
            cp_max[j] = sensor_pos[j] + beam_range[i][j] * tmax; // "c"ollision "p"oint
        }
        area_th_min_U[i] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_min[0], cp_min[1], cp_min[2]);
        area_th_max_U[i] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_max[0], cp_max[1], cp_max[2]);
    }

    // D
    for (int i = 0; i < STEP_NUM; ++i)
    {
        double tmin = 0.0;
        double tmax = SENSOR_RANGE;
        double cp_min[3] = {0.0, 0.0, 0.0};
        double cp_max[3] = {0.0, 0.0, 0.0};

        for (int j = 0; j < 3; ++j)
        {
            float ood = 1.0 / beam_range[i][j];
            float t1 = (aabb_min[j] - sensor_pos[j]) * ood;
            float t2 = (aabb_max[j] - sensor_pos[j]) * ood;
            if (t1 > t2) swap(t1, t2);
            if (t1 > tmin) tmin = t1;
            if (t2 > tmax) tmax = t2;
            if (tmin > tmax) break;
        }
        for (int j = 0; j < 3; ++j)
        {
            cp_min[j] = sensor_pos[j] + beam_range[i][j] * tmin; // "c"ollision "p"oint
            cp_max[j] = sensor_pos[j] + beam_range[i][j] * tmax; // "c"ollision "p"oint
        }
        area_th_min_D[i] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_min[0], cp_min[1], cp_min[2]);
        area_th_max_D[i] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_max[0], cp_max[1], cp_max[2]);
    }
}

void AreaAABB::defineAABB(double min0, double min1, double min2, double max0, double max1, double max2)
{
    aabb_min[0] = min0;
    aabb_min[1] = min1;
    aabb_min[2] = min2;
    aabb_max[0] = max0;
    aabb_max[1] = max1;
    aabb_max[2] = max2;
}
