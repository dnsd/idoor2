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

void judge_GHI(LS3D *OBJECT, int spotinfo_GHI[][44])
{
	int ha = -1*(SENSOR_HEIGHT - Ha); //センサーを原点としたときのHa
    int hw = -1*(SENSOR_HEIGHT - Hw); //センサーを原点としたときのHw

    //AGHIの判定
    int beaminfo[STEP_NUM]={0}; //ビームが何に当たっているか。0:当たってない、1:大人、2:子供、3:台車
    int STEP_NUM_adult[SPOT_NUM_X][SPOT_NUM_Y]={0}; //スポット内の点の数（大人に当たったもの）
    int STEP_NUM_child[SPOT_NUM_X][SPOT_NUM_Y]={0}; //スポット内の点の数（子供に当たったもの）
    int STEP_NUM_wagon[SPOT_NUM_X][SPOT_NUM_Y]={0}; //スポット内の点の数（台車に当たったもの）
    int STEP_NUM_floor[SPOT_NUM_X][SPOT_NUM_Y]={0}; //スポット内の点の数（床に当たったもの）

    for (int i = 0; i < SPOT_NUM_X; i++)
    {
        for (int j = 0; j < SPOT_NUM_Y; j++)
        {
            spotinfo_GHI[i][j] = 0;
        }
    }
    
    for (int k = 0; k < STEP_NUM; k++)
    {	
        for (int i = 0; i < SPOT_NUM_X; i++)
        {
            if(3000 - (SPOT_RES * i) >= OBJECT->x[k] && OBJECT->x[k] > 3000 - (SPOT_RES * (i+1)))
            {
                for (int j = 0; j < SPOT_NUM_Y; j++)
                {
                    if (-2200 + (SPOT_RES * j) <= OBJECT->y[k] && OBJECT->y[k] < -2200 + (SPOT_RES * (j+1)))
                    {
                        if (OBJECT->z[k] >= ha)
                        {
                            STEP_NUM_adult[i][j]++;
                            //break; //横のループを抜ける
                        }else if (ha > OBJECT->z[k] && OBJECT->z[k] >= hw)
                        {
                            STEP_NUM_child[i][j]++;
                            //break; //横のループを抜ける
                        }else if (hw >= OBJECT->z[k] && OBJECT->z[k] > -SENSOR_HEIGHT)
                        {
                            STEP_NUM_wagon[i][j]++;
                            //break; //横のループを抜ける
                        }else
                        {
                            STEP_NUM_floor[i][j]++;
                            //break; //横のループを抜ける
                        }      
                    }
                }
            }
        }
    }
    
    //スポット情報の出力
    // for (int i = 0; i < SPOT_NUM_X; i++)
    // {
    //     for (int j = 0; j < SPOT_NUM_Y; j++)
    //     {
    //         if (STEP_NUM_adult[i][j] > 0)
    //         {
    //             //spotinfo[i][j] = 24;
    //             spotinfo_GHI[i][j] = 24; //2進数に変換して使用する.動き情報はとりあえず000とする。
    //             //break;
    //         }else if (STEP_NUM_child[i][j] > 0)
    //         {
    //             //spotinfo[i][j] = 16;
    //             spotinfo_GHI[i][j] = 16; //2進数に変換して使用する.動き情報はとりあえず000とする。
    //             //break;
    //         }else if (STEP_NUM_wagon[i][j] > 0)
    //         {
    //             //spotinfo[i][j] = 8;
    //             spotinfo_GHI[i][j] = 8; //2進数に変換して使用する.動き情報はとりあえず000とする。
    //             //break;
    //         }else{
    //             //spotinfo[i][j] = 0;
    //             spotinfo_GHI[i][j] = 0; //2進数に変換して使用する.動き情報はとりあえず000とする。
    //         }
    //     }
    // }

    for (int i = 0; i < SPOT_NUM_X; i++)
    {
        for (int j = 0; j < SPOT_NUM_Y; j++)
        {
            if (STEP_NUM_adult[i][j] > 0 && STEP_NUM_adult[i][j] >= STEP_NUM_child[i][j] && STEP_NUM_adult[i][j] >= STEP_NUM_wagon[i][j])
            {
                //spotinfo[i][j] = 24;
                spotinfo_GHI[i][j] = 24; //2進数に変換して使用する.動き情報はとりあえず000とする。
                //break;
            }else if (STEP_NUM_child[i][j] > 0 && STEP_NUM_child[i][j] >= STEP_NUM_adult[i][j] && STEP_NUM_child[i][j] >= STEP_NUM_wagon[i][j])
            {
                //spotinfo[i][j] = 16;
                spotinfo_GHI[i][j] = 16; //2進数に変換して使用する.動き情報はとりあえず000とする。
                //break;
            }else if (STEP_NUM_wagon[i][j] > 0 && STEP_NUM_wagon[i][j] >= STEP_NUM_adult[i][j] && STEP_NUM_wagon[i][j] >= STEP_NUM_child[i][j])
            {
                //spotinfo[i][j] = 8;
                spotinfo_GHI[i][j] = 8; //2進数に変換して使用する.動き情報はとりあえず000とする。
                //break;
            }else{
                //spotinfo[i][j] = 0;
                spotinfo_GHI[i][j] = 0; //2進数に変換して使用する.動き情報はとりあえず000とする。
            }
        }
    }
}


