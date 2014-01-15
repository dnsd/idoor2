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

void judge_CF(LS3D *OBJECT, double beam_U_dist_pre[], double beam_U_dist_dif[], double beam_D_dist_pre[], double beam_D_dist_dif[], double background_U[], double background_D[], int spotinfo_CF[][44])
{
	int step_num_come[SPOT_NUM_X][SPOT_NUM_Y] = {0}; //スポット内の点の数（近づいているもの）
    int step_num_back[SPOT_NUM_X][SPOT_NUM_Y] = {0}; //スポット内の点の数（遠ざかっているもの）
    int step_num_stop[SPOT_NUM_X][SPOT_NUM_Y] = {0}; //動きがないもの

    for (int i = 0; i < SPOT_NUM_X; i++) //初期化
    {
        for (int j = 0; j < SPOT_NUM_Y; j++)
        {
            spotinfo_CF[i][j] = 0;
        }
    }

    if (OBJECT->det == 'U') //U
    {
        for (int k = 0; k < STEP_NUM; k++) //ビームの数だけループ
        {
            if (beam_U_dist_pre[k] == 0)
            {
                beam_U_dist_dif[k] = background_U[k] - OBJECT->dist[k];
            }else{
                beam_U_dist_dif[k] = beam_U_dist_pre[k] - OBJECT->dist[k];
            }
            for (int i = 0; i < SPOT_NUM_X; i++) //縦のスポットのループ
            {
                if(3000 - (SPOT_RES * i) >= OBJECT->x[k] && OBJECT->x[k] > 3000 - (SPOT_RES * (i+1)))
                {
                    for (int j = 0; j < SPOT_NUM_Y; j++)
                    {
                        if (-2200 + (SPOT_RES * j) <= OBJECT->y[k] && OBJECT->y[k] < -2200 + (SPOT_RES * (j+1)))
                        {
                            //if (beam_U_dist_dif[k] > CLOSE_TH)
                            if(beam_U_dist_dif[k] > CLOSE_TH)
                            {
                                step_num_come[i][j]++;
                            }
                            if(beam_U_dist_dif[k] < (-1*CLOSE_TH))
                            {
                                step_num_back[i][j]++;
                            }
                            // else{
                            // step_num_stop[i][j]++;
                            // }
                        }
                    }
                }
            } //縦のスポットのループ
            beam_U_dist_pre[k] = OBJECT->dist[k]; //dist_preの設定
        } //ビームの数だけループ
        
        // for (int k = 0; k < STEP_NUM; k++)
        // {
        //     //if(OBJECT->dist[k] != 0) ofs << OBJECT->dist[k] << ",";
        //     if(OBJECT->dist[k] != 0) ofs << beam_U_dist_dif[k] << ",";
        // }
        // ofs << endl;
        
    } //U

    
    if (OBJECT->det == 'D') //D
    {
        for (int k = 0; k < STEP_NUM; k++) //ビームの数だけループ
        {
            if (beam_D_dist_pre[k] == 0)
            {
                beam_D_dist_dif[k] = background_D[k] - OBJECT->dist[k];
            }else{
                beam_D_dist_dif[k] = beam_D_dist_pre[k] - OBJECT->dist[k];
            }
            for (int i = 0; i < SPOT_NUM_X; i++) //縦のスポットのループ
            {
                if(3000 - (SPOT_RES * i) >= OBJECT->x[k] && OBJECT->x[k] > 3000 - (SPOT_RES * (i+1)))
                {
                    for (int j = 0; j < SPOT_NUM_Y; j++)
                    {
                        if (-2200 + (SPOT_RES * j) <= OBJECT->y[k] && OBJECT->y[k] < -2200 + (SPOT_RES * (j+1)))
                        {
                            //if (beam_D_dist_dif[k] > CLOSE_TH)
                            if(beam_D_dist_dif[k] > CLOSE_TH)
                            {
                                step_num_come[i][j]++;
                            }
                            if(beam_D_dist_dif[k] < (-1*CLOSE_TH))
                            {
                                step_num_back[i][j]++;
                            }
                            // else{
                            // step_num_stop[i][j]++;
                            // }
                        }
                    }
                }
            } //縦のスポットのループ
            beam_D_dist_pre[k] = OBJECT->dist[k]; //dist_preの設定
        } //ビームの数だけループ
    } //D//横のスポットのループ
    

    //スポット情報の出力
    for (int i = 0; i < SPOT_NUM_X; i++)
    {
        for (int j = 0; j < SPOT_NUM_Y; j++)
        {
            if(step_num_back[i][j] >= CF_TH && step_num_back[i][j] > step_num_come[i][j]) //離れているスポット
            // if(step_num_back[i][j] >= CF_TH && step_num_come[i][j] != 0) //離れているスポット
            {
                spotinfo_CF[i][j] = 5;
            }
            if (step_num_come[i][j] >= CF_TH && step_num_come[i][j] >= step_num_back[i][j])
            {
                spotinfo_CF[i][j] = 2;
            }
            // else{
            // //spotinfo[i][j] = 0;
            // ACF->spotinfo[i][j] = 0;
            // }
            
        }
    }
}