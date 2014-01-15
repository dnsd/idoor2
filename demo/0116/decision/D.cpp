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

void judge_D(int spotinfo_CF[][44], int spotinfo_D[][44], int *open_mode)
{
  if (MODE_D == true)
  {
    int C_cnt=0;

    for (int i = 0; i < SPOT_NUM_X; i++) //初期化とCのカウント
    {
        for (int j = 0; j < SPOT_NUM_Y; j++)
        {
            spotinfo_D[i][j] = 0;
            if (spotinfo_CF[i][j] == 2)
            {
              C_cnt++;
            }
        }
    }

    if (C_cnt >= D_TH) //Cの数がしきい値以上ならドアを開いてD判定を出す。
    {
      *open_mode = 4; //高速全開
      for (int i = 0; i < SPOT_NUM_X; i++)
      {
        for (int j = 0; j < SPOT_NUM_Y; j++)
        {
          if (spotinfo_CF[i][j] == 2)
          {
            spotinfo_D[i][j] == 3;
          }
        }
      }
    }

  }
}