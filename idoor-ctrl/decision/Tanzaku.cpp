#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <deque>
#include "decision.h"

using namespace std;

//いっことばしの考慮あり
void Tanzaku::calArrivalTime()
{
    double arrival_temp[TANZAKU_NUM_MAX];
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        if (x[tan_num][CUR_INDEX] != 0.0
                && v[tan_num][CUR_INDEX] > 0.0)
        {
            arrival_temp[tan_num] = x[tan_num][CUR_INDEX] / v[tan_num][CUR_INDEX];
            frame_arrival[tan_num] = arrival_temp[tan_num] / FREQ;
        }else if (x[tan_num][CUR_INDEX] == 0.0
                && x[tan_num][PRE_INDEX] != 0.0
                && v[tan_num][CUR_INDEX] > 0.0) 
        {
            arrival_temp[tan_num] = x[tan_num][PRE_INDEX] / v[tan_num][CUR_INDEX];
            frame_arrival[tan_num] = arrival_temp[tan_num] / FREQ;
        }else{
            frame_arrival[tan_num] = 100;
        }
    }
}

void Tanzaku::calObjectWidth(TANZAKU_FAC& fac)
{
    //短冊ごとの幅の算出
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        w[i].pop_front();
        w[i].push_back(0.0);
        if(x[i][CUR_INDEX] != 0.0)
        {
            w[i][CUR_INDEX] = fac.wn[i] * x[i][CUR_INDEX] / 1000.0;
        }
        if(x[i][CUR_INDEX] == 0.0 && x[i][PRE_INDEX] != 0.0)
        {
            w[i][CUR_INDEX] = fac.wn[i] * x[i][PRE_INDEX] / 1000.0; //位置データが無かったら一つ前のデータで補完   
        }
    }

    // 位置データが存在する短冊のうち端どうしの距離を幅とする方法
    int w_left = 0;
    int w_right = 0;
    double w_sum_cur = 0.0;
    for (int i = 0; i < TANZAKU_NUM_MAX; i++) //有効な短冊の範囲ぶん
    {
        if (approach_cnt[i] >= 2)
        {
            w_left = i;
            break;
        }
    }
    for (int i = TANZAKU_NUM_MAX; i > 0; i--) //有効な短冊の範囲ぶん
    {
        if (approach_cnt[i] >= 2)
        {
            w_right = i;
            break;
        }
    }
    if (w[w_left][CUR_INDEX] <= w[w_right][CUR_INDEX])
    {
        w_sum_cur = w[w_right][CUR_INDEX] * (w_right - w_left + 1);
    }else{
        w_sum_cur = w[w_left][CUR_INDEX] * (w_right - w_left + 1);
    }

    w_sum.pop_front();
    w_sum.push_back(w_sum_cur);

}

// 最小二乗法で速度を算出
void Tanzaku::calSpeed()
{
    double x[TANZAKU_NUM_MAX][BUFFER_LENGTH];
    double y[TANZAKU_NUM_MAX][BUFFER_LENGTH];

    //-xのわりあて-//
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        for (int j = 0; j < BUFFER_LENGTH; j++)
        {
            x[i][j] = FREQ * j + scan_time[i][j];
        }
    }

    //-yのわりあて-//
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        for(int j=0; j<BUFFER_LENGTH; j++)
        {
            y[i][j] = x[i][j];
        }
    }

    //-最小二乗法-// 
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        int data_num_cnt = 0;
        double sum_x = 0.0;
        double sum_y = 0.0;
        double sum_xy = 0.0;
        double sum_xx = 0.0;
        for (int j = 0; j <BUFFER_LENGTH; j++) //すべてのバッファを計算に使う
        {
            if (y[i][j] != 0.0)
            {
                sum_x += x[i][j];
                sum_y += y[i][j];
                sum_xx += x[i][j] * x[i][j];
                sum_xy += x[i][j] * y[i][j];
                data_num_cnt++;
            }
        }

        if (data_num_cnt >= 2 && (y[i][CUR_INDEX] != 0.0 || y[i][PRE_INDEX] != 0.0))
        {
            // 速度temp_vの算出
            double temp_v = 0.0;
            temp_v = (data_num_cnt * sum_xy - sum_x * sum_y) / (data_num_cnt * sum_xx - pow(sum_x, 2));
            // 速度方向を反転する
            // v[i] = temp_v * -1;
            // 算出された速度の評価：値が大きすぎる場合は無効にする。 
            if (fabs(temp_v) > V_MAX_TH)
            {
                v[i].pop_front();
                v[i].push_back(0.0);
            }else{
                // 速度方向を反転する
                v[i].pop_front();
                v[i].push_back(temp_v * -1);
            }
        }else{
            v[i].pop_front();
            v[i].push_back(0.0);
        }

    }
}

bool Tanzaku::isCancel(Lane& lane, int tan_num)
{
    // 物体がエリア内にいるとき（条件1）
    if (isInSurveillanceArea(tan_num, CUR_INDEX) == true)
    {
        // 位置データがあるとき
        if (x[tan_num][CUR_INDEX] != 0.0)
        {
            int tmp = (int)x[tan_num][CUR_INDEX] / 100;
            if (lane.isPending(tmp) == true)
            {
                return true;
            }
        }
        // いっこ前なら位置データがあるとき
        if (x[tan_num][CUR_INDEX] == 0.0 && x[tan_num][PRE_INDEX] != 0.0)
        {
            int tmp = (int)x[tan_num][PRE_INDEX] / 100;
            if (lane.isPending(tmp) == true)
            {
                return true;
            }
        }
        // 位置データがないとき
        if (x[tan_num][CUR_INDEX] == 0.0 && x[tan_num][PRE_INDEX] == 0.0)
        {
            return true;
        }
    }
    // 物体がエリア内にいないとき（条件1）
    if (isInSurveillanceArea(tan_num, CUR_INDEX) == false)
    {
        //cancel_flag[tan_num] = true;
        return true;
    }
    return false;
}

bool Tanzaku::isInDetectionArea(int tan_num, int index)
{
    // 位置データがDetectionAreaにあるときtrue
    // 位置データがSurveillanceエリアの中にあるか？
    if (x[tan_num][index] != 0.0 && isInSurveillanceArea(tan_num, index) == true)
    {
        // 位置データがDetectionエリアの境界の外にあるか？
        if (isInInnerArea(tan_num, index) == false)
        {
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool Tanzaku::isInInnerArea(int tan_num, int index)
{
    // 位置データが存在するときエリアの内外判定をする
    if (x[tan_num][index] != 0.0)
    {
        // 物体がエリアの中にあるとき真
        if (x[tan_num][index] <= AREA_D_END_X
                && AREA_D_START_Y <= y[tan_num][index]
                && y[tan_num][index] <= AREA_D_END_Y)
        {
            return true;
        }else{
            return false;
        }
    }
    // いっこ前にデータがあるときは保留
    if (x[tan_num][index] == 0.0 && x[tan_num][index-1] != 0.0)
    {
        // 物体がエリアの中にあればフラグをセット
        if (x[tan_num][index-1] <= AREA_D_END_X
                && AREA_D_START_Y <= y[tan_num][index-1]
                && y[tan_num][index-1] <= AREA_D_END_Y)
        {
            return true;
        }else{
            return false;
        }
    }
    // データが無いときはクリア
    if (x[tan_num][index] == 0.0 && x[tan_num][index-1] == 0.0)
    {
        return false;
    }
}

bool Tanzaku::isInSurveillanceArea(int tan_num, int index)
{
    // 位置データが存在するときエリアの内外判定をする
    if (x[tan_num][index] != 0.0)
    {
        // 物体がエリアの中にあるとき真
        if (x[tan_num][index] <= AREA_S_END_X
                && AREA_S_START_Y <= y[tan_num][index]
                && y[tan_num][index] <= AREA_S_END_Y)
        {
            return true;
        }else{
            return false;
        }
    }
    // いっこ前にデータがあるときは保留
    if (x[tan_num][index] == 0.0 && x[tan_num][index-1] != 0.0)
    {
        // 物体がエリアの中にあればフラグをセット
        if (x[tan_num][index-1] <= AREA_S_END_X
                && AREA_S_START_Y <= y[tan_num][index-1]
                && y[tan_num][index-1] <= AREA_S_END_Y)
        {
            return true;
        }else{
            return false;
        }
    }
    // データが無いときはクリア
    if (x[tan_num][index] == 0.0 && x[tan_num][index-1] == 0.0)
    {
        return false;
    }
}

// frame_observeを使用しない
void Tanzaku::judgeOpen()
{
    //-初期化-//
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        open_mode[i] = 0;
    }

    //-open_mode_tanの判定と出力-//
    if (w_sum[CUR_INDEX] <= DOOR_W_TH)
    {
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            //open_mode_tanの計算
            if (approach_cnt[i] >= FRAME_OBSERVE 
                    && approach_cnt != 0) //?
            {
                if (frame_arrival[i] <= (REQUIRED_FRAME_HIGH_HALF + MARGIN) )
                {
                    open_mode[i] = 2; //高速半開
                }else if ( (REQUIRED_FRAME_HIGH_HALF + MARGIN) < frame_arrival[i] 
                        && frame_arrival[i] <= (REQUIRED_FRAME_NORMAL_HALF + MARGIN) )
                {
                    open_mode[i] = 1; //低速半開
                }else{
                    open_mode[i] = 0; //開けない
                }
            }
        }  
    }else{
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            //open_mode_tanの計算
            if (approach_cnt[i] >= FRAME_OBSERVE 
                    && approach_cnt != 0)
            {
                if (frame_arrival[i] <= (REQUIRED_FRAME_HIGH_FULL + MARGIN) )
                {
                    open_mode[i] = 4; //高速全開
                }else if ( (REQUIRED_FRAME_HIGH_HALF + MARGIN) < frame_arrival[i] 
                        && frame_arrival[i] <= (REQUIRED_FRAME_NORMAL_FULL + MARGIN) )
                {
                    open_mode[i] = 3; //低速全開
                }else{
                    open_mode[i] = 0; //開けない
                }
            }
        } 
    }
    //open_mode_tanの判定と出力おわり//  
}

//スキャンの方向を考慮する方法（必ず同じ方向のものと比較する）
void Tanzaku::updApproachCnt()
{
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        if (x[i][CUR_INDEX] != 0.0
                && x[i][PREPRE_INDEX] != 0.0)
        {
            if ( (x[i][PREPRE_INDEX] - x[i][CUR_INDEX]) > (2 * MIN_SPEED)) //最低スピード以上ならインクリメント。 //最大スピードについても考えたほうがよい？
            {
                if (x[i][PRE_INDEX] != 0.0)
                {
                    approach_cnt[i]++;
                }
                if (x[i][PRE_INDEX] == 0.0) //PREが0のときは前回のぶんもインクリメント
                {
                    approach_cnt[i]++;
                    approach_cnt[i]++;
                }
            }
            if (x[i][PREPRE_INDEX] - x[i][CUR_INDEX] < (-2 * MIN_SPEED)) //遠ざかったときはクリア
            {
                approach_cnt[i] = 0;
            }
        }
        if (x[i][CUR_INDEX] == 0.0
            && x[i][PRE_INDEX] == 0.0) //物体がないときはクリア
        {
            approach_cnt[i] = 0;
        }
    }
}

int Tanzaku::vote(Lane& lane)
{
    int vote = 0;

    //-open_modeの判定と出力-//
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        if (isCancel(lane, tan_num) == false)
        {
            if (open_mode[tan_num] == 4)
            {
                return 4;
            }else if (open_mode[tan_num] == 2)
            {
                return 2;
            }else if (open_mode[tan_num] == 3)
            {
                vote = 3;
            }else if (open_mode[tan_num] == 1)
            {
                vote = 1;
            }
        }   
    }
    return vote;
}