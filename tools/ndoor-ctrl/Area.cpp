#include <iostream>
// #include <iomanip>
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

void Area::defineCuboid(double x1, double x2, double y1, double y2, double z1, double z2)
{
	sx1 = x1;
	sx2 = x2;
	sy1 = y1;
	sy2 = y2;
	sz1 = z1;
	sz2 = z2;
}

// bool Area::hasObjects(Step& rd) // "r"ead"d"ata
// {
// 	int step_num_cnt = 0;
// 	for (int i = 0; i < STEP_NUM; i++)
// 	{
// 		if (rd.dist[i][CUR_INDEX] != 0.0
//                         && sx1 <= rd.x[i][CUR_INDEX] && rd.x[i][CUR_INDEX] <= sx2
// 			&& sy1 <= rd.y[i][CUR_INDEX] && rd.y[i][CUR_INDEX] <= sy2
// 			&& sz1 <= rd.z[i][CUR_INDEX] && rd.z[i][CUR_INDEX] <= sz2)
// 		{
// 			step_num_cnt++;
// 		}
// 	}

// 	if (step_num_cnt >= step_num_cnt_th)
// 	{
// 		return true;
// 	}else{
// 		return false;
// 	}
// }

bool Area::hasObjects(Step& rd) // "r"ead"d"ata
{
	int step_num_cnt = 0;
	double th_min[3] = {0.0, 0.0, 0.0};
	double th_max[3] = {0.0, 0.0, 0.0};

	if (rd.det = 'U')
	{
		for (int i = 0; i < 3; ++i)
		{
		}
	}
	for (int i = 0; i < STEP_NUM; ++i)
	{
		if ()
		{
			step_num_cnt++;
		}
	}
}

int Area::judgeOpen(Step& rd)
{
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

    if (buf_num_cnt >= buf_num_cnt_th)
    {
        cout << "buf_num_cnt =" << buf_num_cnt << endl;
        return 4; // 高速全開
    }else{
        cout << "buf_num_cnt =" << buf_num_cnt << endl;
        return 0; // 開けない
    }
}

void Area::setAreaTh(AABB aabb, BEAMANGLE angle){
	// AABBと直線の交差判定
	double sensor_pos[3] = {ORG_X, ORG_Y, ORG_Z}; // センサの取り付け位置

	// U
	for (int i = 0; i < STEP_NUM; ++i)
	{
		double beam_range[3]; // センサの観測
		calc_xyz(angle.ux, angle.uy, SENSOR_RANGE, beam_range[0], beam_range[1], beam_range[2]);

		double tmin = 0.0;
		double tmax = SENSOR_RANGE;
		double cp_min[3] = {0.0, 0.0, 0.0};
		double cp_max[3] = {0.0, 0.0, 0.0};

		for (int j = 0; j < 3; ++j)
		{
			float ood = 1.0 / beam_range[j];
			float t1 = (aabb.min[j] - sensor_pos[j]) * ood;
			float t2 = (aabb.max[j] - sensor_pos[j]) * ood;
			if (t1 > t2) swap(t1, t2);
			if (t1 > tmin) tmin = t1;
            if (t2 > tmax) tmax = t2;
            if (tmin > tmax) break;
		}
		for (int j = 0; j < 3; ++j)
		{
			cp_min[j] = sensor_pos[j] + beam_range[j] * tmin; // "c"ollision "p"oint
			cp_max[j] = sensor_pos[j] + beam_range[j] * tmax; // "c"ollision "p"oint
		}
		area_th_min_U[i][aabb.id] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_min[0], cp_min[1], cp_min[2]);
		area_th_max_U[i][aabb.id] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_max[0], cp_max[1], cp_max[2]);
	}

	// D
	for (int i = 0; i < STEP_NUM; ++i)
	{
		double beam_range[3]; // センサの観測
		calc_xyz(angle.dx, angle.dy, SENSOR_RANGE, beam_range[0], beam_range[1], beam_range[2]);

		double tmin = 0.0;
		double tmax = SENSOR_RANGE;
		double cp_min[3] = {0.0, 0.0, 0.0};
		double cp_max[3] = {0.0, 0.0, 0.0};

		for (int j = 0; j < 3; ++j)
		{
			float ood = 1.0 / beam_range[j];
			float t1 = (aabb.min[j] - sensor_pos[j]) * ood;
			float t2 = (aabb.max[j] - sensor_pos[j]) * ood;
			if (t1 > t2) swap(t1, t2);
			if (t1 > tmin) tmin = t1;
            if (t2 > tmax) tmax = t2;
            if (tmin > tmax) break;
		}
		for (int j = 0; j < 3; ++j)
		{
			cp_min[j] = sensor_pos[j] + beam_range[j] * tmin; // "c"ollision "p"oint
			cp_max[j] = sensor_pos[j] + beam_range[j] * tmax; // "c"ollision "p"oint
		}
		area_th_min_D[i][aabb.id] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_min[0], cp_min[1], cp_min[2]);
		area_th_max_D[i][aabb.id] = dist_2p3D(sensor_pos[0], sensor_pos[1], sensor_pos[2], cp_max[0], cp_max[1], cp_max[2]);
	}
}

void Area::set_step_num_cnt_th(int parameter)
{
	step_num_cnt_th = parameter;
}

void Area::set_buf_num_cnt_th(int parameter)
{
	buf_num_cnt_th = parameter;
}

void Area::set_buf_length_has_objects(int parameter)
{
	buf_length_has_objects = parameter;
	hasObjects_buf.resize(buf_length_has_objects);
}
