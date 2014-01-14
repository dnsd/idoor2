// for_only_lane

#include <iostream>
#include <iomanip>
#include <GL/glut.h>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <deque>
#include <iterator>
#include "door_simu.h"

using namespace std;

void read_new_data(detection_log& read_data, ifstream& ifs, char scan[], GLdouble vertex[][3], int &cur_tid)
{
	char *tp;
	int xyz_index = 0;
	char dir = 'U';

	// 新規読み込み
	// 角度の行の読み込み
	ifs.getline(scan, sizeof(scan));
	tp = strtok(scan, ","); //一度目のstrtokには分解対象を指定
	dir = *tp;

	// ビームの行の読み込み
	for (xyz_index = 0; xyz_index <= 2; xyz_index++)
	{
		int step_num = 0;
		ifs.getline(scan, sizeof(scan));
		tp = strtok(scan, ","); //一度目のstrtokには分解対象を指定
		vertex[step_num][xyz_index] = atof(tp);
		step_num++;

		while(tp = strtok(NULL, ","))
		{
			if (step_num < STEP_NUM_MAX)
			{
				vertex[step_num][xyz_index] = atof(tp);
				step_num++;
			}
		}
	}

	cur_tid = (BUFFER_LENGTH - 1);
	read_data.det.pop_front();
	read_data.det.push_back(dir);
	for (int step_num = 0; step_num < STEP_NUM_MAX; step_num++)
	{
		read_data.px[step_num].pop_front();
		read_data.py[step_num].pop_front();
		read_data.pz[step_num].pop_front();
		read_data.px[step_num].push_back(vertex[step_num][0]);
		read_data.py[step_num].push_back(vertex[step_num][1]);
		read_data.pz[step_num].push_back(vertex[step_num][2]);
	}

	//-結果の行の読み込み-//
	// int tan_num = 0;
	ifs.getline(scan, sizeof(scan));
	
	// open_mode読み込み
	tp = strtok(scan, ",");
	read_data.open_mode.pop_front();
	read_data.open_mode.push_back(atoi(tp));

	// cout << "!" << endl;
	
	// open_mode_tanの読み込み
	for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
	{
		tp = strtok(NULL, ",");
		read_data.open_mode_tan[tan_num].pop_front();
		read_data.open_mode_tan[tan_num].push_back(atoi(tp));
	}
	for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
	{
		tp = strtok(NULL, ",");
		read_data.tan_x[tan_num].pop_front();
		read_data.tan_x[tan_num].push_back(atof(tp));
	}
	for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
	{
		tp = strtok(NULL, ",");
		read_data.tan_v[tan_num].pop_front();
		read_data.tan_v[tan_num].push_back(atof(tp));
	}

	//tan_approach_cntの読み込み
	for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
	{
		tp = strtok(NULL, ",");
		read_data.tan_approach_cnt[tan_num].pop_front();
		read_data.tan_approach_cnt[tan_num].push_back(atof(tp));
	}

	//cancel_by_lineの読み込み
	for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
	{
		tp = strtok(NULL, ",");
		read_data.cancel_by_lane[tan_num].pop_front();
		read_data.cancel_by_lane[tan_num].push_back(atof(tp));
	}

	//target_laneの読み込み
	for (int tan_num = 0; tan_num < LANE_NUM_MAX; tan_num++)
	{
		tp = strtok(NULL, ",");
		read_data.target_lane[tan_num].pop_front();
		read_data.target_lane[tan_num].push_back(atof(tp));
	}

	//laneL_cross_cnt
	for (int lane_num = 0; lane_num < LANE_NUM_MAX; lane_num++)
	{
		tp = strtok(NULL, ",");
		read_data.laneL_cross_cnt[lane_num].pop_front();
		read_data.laneL_cross_cnt[lane_num].push_back(atof(tp));
	}

	//laneR_cross_cnt
	for (int lane_num = 0; lane_num < LANE_NUM_MAX; lane_num++)
	{
		tp = strtok(NULL, ",");
		read_data.laneR_cross_cnt[lane_num].pop_front();
		read_data.laneR_cross_cnt[lane_num].push_back(atof(tp));
	}
}