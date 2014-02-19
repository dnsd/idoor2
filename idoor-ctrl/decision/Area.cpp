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

void Area::defineCuboid(double x1, double x2, double y1, double y2, double z1, double z2)
{
	sx1 = x1;
	sx2 = x2;
	sy1 = y1;
	sy2 = y2;
	sz1 = z1;
	sz2 = z2;
}

bool Area::hasObjects(Step& rd) // "r"ead"d"ata
{
	int step_num_cnt = 0;
	for (int i = 0; i < STEP_NUM; i++)
	{
		if (rd.dist[i][CUR_INDEX] != 0.0
                        && sx1 <= rd.x[i][CUR_INDEX] && rd.x[i][CUR_INDEX] <= sx2
			&& sy1 <= rd.y[i][CUR_INDEX] && rd.y[i][CUR_INDEX] <= sy2
			&& sz1 <= rd.z[i][CUR_INDEX] && rd.z[i][CUR_INDEX] <= sz2)
		{
			step_num_cnt++;
		}
	}

	if (step_num_cnt >= step_num_cnt_th)
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

    if (buf_num_cnt >= buf_num_cnt_th)
    {
        cout << "buf_num_cnt =" << buf_num_cnt << endl;
        return 4; // 高速全開
    }else{
        cout << "buf_num_cnt =" << buf_num_cnt << endl;
        return 0; // 開けない
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
