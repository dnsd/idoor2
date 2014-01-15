// tanzaku_and_edge

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <ssm.hpp>
#include <cmath>
#include "send_spotinfo.h"

using namespace std;

void judge_B(step_data& data, int& stop_cnt, bool& B_flag)
{
  int num_of_step_in_areaB = 0;
  for (int i = 0; i < STEP_NUM; i++)
  {
    if (data.dist[i][CUR_INDEX] != 0.0 && data.stop_flag[i][CUR_INDEX] == true)
    {
      if (AREA_B_START_Y <= data.y[i][CUR_INDEX] && data.y[i][CUR_INDEX] <= AREA_B_END_Y)
      {
        if (AREA_B_START_X <= data.x[i][CUR_INDEX] && data.x[i][CUR_INDEX] <= AREA_B_END_X)
        {
          num_of_step_in_areaB++;
        }
      }
    }
  }

  if (num_of_step_in_areaB >= TH_NUM_OF_STEP_FOR_B)
  {
    stop_cnt++;
  }else{
    stop_cnt = 0;
  }

  if (stop_cnt >= STOP_CNT_TH)
  {
    B_flag = true;
  }else{
    B_flag = false;
  }
}
