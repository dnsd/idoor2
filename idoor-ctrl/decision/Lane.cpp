#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include "decision.h"

using namespace std;

bool Lane::isPending(int lane_num)
{
    if (pending_cnt[lane_num] > 0)
    {
        return true;
    }else{
        return false;
    }
}

void Lane::upd_pending_cnt(Tanzaku& tanzaku)
{
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        int target_lane_num = 0;
        // Detectionエリアに物体があるときはpending_cntに最大値をセットする
        if (tanzaku.isInDetectionArea(tan_num, CUR_INDEX) == true)
        {
            target_lane_num = (int)tanzaku.x[tan_num][CUR_INDEX] / LANE_W;
            if(target_lane_num <= PENDING_ZONE_WIDTH) target_lane_num = PENDING_ZONE_WIDTH; // tar_get_lane_num-PENDING_ZONE_WIDTHがマイナスにならないようにする
            for (int lane_num = target_lane_num - PENDING_ZONE_WIDTH; lane_num < target_lane_num + PENDING_ZONE_WIDTH; lane_num++)
            {
                pending_cnt[lane_num] = PENDING_CNT_MAX;
            }
        }
    }

    // upd_pending_cnt
    for (int i = 0; i < LANE_NUM_MAX; i++)
    {
        if (pending_cnt[i] > 0)
        {
            pending_cnt[i]--;
        }
    }
}