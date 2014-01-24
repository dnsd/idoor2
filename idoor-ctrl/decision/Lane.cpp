#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <ssm.hpp>
#include <cmath>
#include "decision.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

void Lane::set_on_the_lane_flag(Tanzaku& tan)
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
        if (tan.x[tan_num][CUR_INDEX] != 0.0)
        {
            int tmp = (int)tan.x[tan_num][CUR_INDEX] / 100;
            if(tmp <= PENDING_ZONE_WIDTH) tmp = PENDING_ZONE_WIDTH; //tmp-PENDING_ZONE_WIDTHがマイナスにならないようにする
            for (int i = tmp-PENDING_ZONE_WIDTH; i <= tmp+PENDING_ZONE_WIDTH; i++)
            {
                on_the_lane_flag[i] = true;
            }
        }
        // いっこ前なら位置データがあるとき
        if (tan.x[tan_num][CUR_INDEX] == 0.0 && tan.x[tan_num][PRE_INDEX] != 0.0)
        {
            int tmp = (int)tan.x[tan_num][PRE_INDEX] / 100;
            if(tmp <= PENDING_ZONE_WIDTH) tmp = PENDING_ZONE_WIDTH;//tmp-PENDING_ZONE_WIDTHがマイナスにならないようにする
            for (int i = tmp-PENDING_ZONE_WIDTH; i <= tmp+PENDING_ZONE_WIDTH; i++)
            {
                on_the_lane_flag[i] = true;
            }
        }
        // 位置データがないとき
        if (tan.x[tan_num][CUR_INDEX] == 0.0 && tan.x[tan_num][PRE_INDEX] == 0.0)
        {
            // なにもしない
        }
    }
}

bool Lane::hasObjects(int lane_num)
{
    if (on_the_lane_flag[lane_num] == true)
    {
        return true;
    }
    return false;
}

void Lane::set_entry_lane_flag(Tanzaku& tan)
{
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        int entry_lane_num = 0;

        for (int i=0; i<LANE_NUM_MAX; i++)
        {
            entry_lane_flag[i] = false;
        }

        if (tan.x[tan_num][CUR_INDEX] != 0.0 && tan.isInSurveillanceArea(tan_num, CUR_INDEX) == true)
        {
            if (tan.isInSurveillanceArea(tan_num, PREPRE_INDEX) == false)
            {
                entry_lane_num = (int)tan.x[tan_num][CUR_INDEX] / 100;
                entry_lane_flag[entry_lane_num] = true;
            }
        }   
    }
}

bool Lane::hasObjectsEntry(int lane_num)
{
    if (entry_lane_flag[lane_num] == true)
    {
        return true;
    }
    return false;
}

void Lane::set_pending_flag()
{
    // pending_flagをセットするか判断
    for (int lane_num = 0; lane_num < LANE_NUM_MAX; lane_num++)
    {
        if (hasObjectsEntry(lane_num) == true)
        {
            for (int i = lane_num - PENDING_ZONE_WIDTH; i <= lane_num + PENDING_ZONE_WIDTH; i++)
            {
                pending_flag[i] = true;
            }
        }
    }
    // pending_flagをクリアするか判断
    for (int lane_num = 0; lane_num < LANE_NUM_MAX; lane_num++)
    {
        // laneに位置データがあるとき：保留
        if (pending_flag[lane_num] == true && hasObjects(lane_num) == true)
        {
            // なにもしない
        }
        // laneに位置データがないとき：クリアする
        if (hasObjects(lane_num) == false)
        {
           pending_flag[lane_num] = false; 
        }
    }
}

bool Lane::isPending(int lane_num)
{
    if (pending_flag[lane_num] == true)
    {
        return true;
    }
    return false;
}
