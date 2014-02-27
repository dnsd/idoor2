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
	int step_cnt = 0;
	if (rd.det == 'U')
	{
		for (int i = 0; i < STEP_NUM; ++i)
		{
			if (rd.dist[i] != 0.0
				&& area_th_min_U[i] <= rd.dist[i]
				&& rd.dist[i] <= area_th_max_U)
			{
				step_cnt++;
			}
		}
	}else if (rd.det == 'D')
	{
		for (int i = 0; i < STEP_NUM; ++i)
		{
			if (rd.dist[i] != 0.0
				&& area_th_min_D[i] <= rd.dist[i]
				&& rd.dist[i] <= area_th_max_D)
			{
				step_cnt++;
			}
		}
	}

	if (step_num_cnt >= step_cnt_th)
	{
		return true;
	}else{
		return false;
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
void AreaAABB::calAreaTh()
{
	
}

void AreaAABB::defineAABB(double min0, double min1, double min2, double max0, double max1, double max2)
{
	min[0] = min0;
    min[1] = min1;
    min[2] = min2;
    max[0] = max0;
    max[1] = max1;
    max[2] = max2;
}