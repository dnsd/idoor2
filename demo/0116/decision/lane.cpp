// tanzaku_and_edge

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <ssm.hpp>
#include <cmath>
#include "send_spotinfo.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

void set_entry_lane_num(vector< deque<double> >& tan_x_buf, vector< deque<double> >& tan_y_buf, bool in_the_areaE_flag[],
                        int entry_lane_num[])
{
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        if (tan_x_buf[tan_num][CUR_INDEX] != 0.0 && in_the_areaE_flag[tan_num] == true)
        {
            if (AREA_E_END_X < tan_x_buf[tan_num][PREPRE_INDEX]
                || tan_y_buf[tan_num][PREPRE_INDEX] < AREA_E_START_Y
                || AREA_E_END_Y < tan_y_buf[tan_num][PREPRE_INDEX])
            {
                entry_lane_num[tan_num] = (int)tan_x_buf[tan_num][CUR_INDEX] / 100;
                if (entry_lane_num[tan_num] <= TARGET_LANE_RANGE) entry_lane_num[tan_num] = TARGET_LANE_RANGE;
            }else{
                entry_lane_num[tan_num] = 0;
            }
        }else{
            entry_lane_num[tan_num] = 0;
        }   
    }
}

void set_on_the_lane_flag(vector< deque<double> >& tan_x_buf, bool on_the_lane_flag[])
{
    // 初期化
    for (int lane_num = 0; lane_num < LANE_NUM_MAX; lane_num++)
    {
        on_the_lane_flag[lane_num] = false;
    }

    // on_the_lane_flagの更新
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        //位置データがあるとき
        if (tan_x_buf[tan_num][CUR_INDEX] != 0.0)
        {
            int tmp = (int)tan_x_buf[tan_num][CUR_INDEX] / 100;
            if(tmp <= TARGET_LANE_RANGE) tmp = TARGET_LANE_RANGE; //tmp-TARGET_LANE_RANGEがマイナスにならないようにする
            for (int i = tmp-TARGET_LANE_RANGE; i <= tmp+TARGET_LANE_RANGE; i++)
            {
                on_the_lane_flag[i] = true;
            }
        }
        // いっこ前なら位置データがあるとき
        if (tan_x_buf[tan_num][CUR_INDEX] == 0.0 && tan_x_buf[tan_num][PRE_INDEX] != 0.0)
        {
            int tmp = (int)tan_x_buf[tan_num][PRE_INDEX] / 100;
            if(tmp <= TARGET_LANE_RANGE) tmp = TARGET_LANE_RANGE;//tmp-TARGET_LANE_RANGEがマイナスにならないようにする
            for (int i = tmp-TARGET_LANE_RANGE; i <= tmp+TARGET_LANE_RANGE; i++)
            {
                on_the_lane_flag[i] = true;
            }
        }
        // 位置データがないとき
        if (tan_x_buf[tan_num][CUR_INDEX] == 0.0 && tan_x_buf[tan_num][PRE_INDEX] == 0.0)
        {
            // なにもしない
        }
    }
}

// entry_lane_flagをつかったもの
void upd_cancel_lane_flag(vector< deque<double> >& tan_x_buf, int entry_lane_num[], bool cancel_lane_flag[])
{
    // cancel_lane_flagをセットするか判断
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        // entry_flag_numが0以外のとき
        if (entry_lane_num[tan_num] != 0)
        {
            for (int i = entry_lane_num[tan_num]-TARGET_LANE_RANGE; i <= entry_lane_num[tan_num]+TARGET_LANE_RANGE; i++)
            {
                cancel_lane_flag[i] = true;
                // cout << i << ","; //debug
            }
        }
    }
    // cancel_lane_flagをクリアするか判断
    for (int lane_num = 0; lane_num < LANE_NUM_MAX; lane_num++)
    {
        // cancel_laneに位置データがあるとき：保留
        if (cancel_lane_flag[lane_num] == true && on_the_lane_flag[lane_num] == true)
        {
            // なにもしない
        }
        // cancel_laneに位置データがないとき：クリアする
        if (cancel_lane_flag[lane_num] == true && on_the_lane_flag[lane_num] == false)
        {
            cancel_lane_flag[lane_num] = false;
        }
    }
    // cout << endl; //debug
}

void upd_cancel_flag(vector< deque<double> >& tan_x_buf, bool in_the_areaE_flag[], bool cancel_lane_flag[],
                    bool cancel_flag[])
{
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        // 物体がエリア内にいるとき（条件1）
        if (in_the_areaE_flag[tan_num] == true)
        {
            // 位置データがあるとき
            if (tan_x_buf[tan_num][CUR_INDEX] != 0.0)
            {
                int tmp = (int)tan_x_buf[tan_num][CUR_INDEX] / 100;
                if (cancel_lane_flag[tmp] == true)
                {
                    cancel_flag[tan_num] = true;
                }else{
                    cancel_flag[tan_num] = false;
                }
            }
            // いっこ前なら位置データがあるとき
            if (tan_x_buf[tan_num][CUR_INDEX] == 0.0 && tan_x_buf[tan_num][PRE_INDEX] != 0.0)
            {
                int tmp = (int)tan_x_buf[tan_num][PRE_INDEX] / 100;
                if (cancel_lane_flag[tmp] == true)
                {
                    cancel_flag[tan_num] = true;
                }else{
                    cancel_flag[tan_num] = false;
                }
            }
            // 位置データがないとき
            if (tan_x_buf[tan_num][CUR_INDEX] == 0.0 && tan_x_buf[tan_num][PRE_INDEX] == 0.0)
            {
                cancel_flag[tan_num] = true;
            }
        }
        // 物体がエリア内にいないとき（条件1）
        if (in_the_areaE_flag[tan_num] == false)
        {
            cancel_flag[tan_num] = true;
        }
    }

}