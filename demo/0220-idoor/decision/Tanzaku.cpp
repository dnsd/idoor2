#include <iostream>
#include <iomanip>
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

void allocate_data_to_tanzaku(TANZAKU_FAC& fac, double steptime[], Step& sd, Cell& cell)
{
    // cell関連のデータの初期化
    for(int i=0; i<TAN_CELL_NUM_MAX; i++) 
    {
        for(int j=0; j<TANZAKU_NUM_MAX; j++)
        {
            cell.step_num[i][j] = 0.0;
            cell.sum_x[i][j] = 0.0;
            cell.sum_y[i][j] = 0.0;
            cell.sum_steptime[i][j] = 0.0;
        }
    }

    // 短冊へのデータの割り当て
    for(int i=0; i<STEP_NUM; i++)
    {
        if (sd.dist[i][CUR_INDEX] != 0.0 && sd.isClose(i) == true) //c_beamを抽出
        {
            for (int j = 0; j < TANZAKU_NUM_MAX; j++) //短冊のループ
            {
                if (sd.y[i][CUR_INDEX] > fac.a[j] * sd.x[i][CUR_INDEX] + fac.b[j] 
                        && sd.y[i][CUR_INDEX] < fac.a[j+1] * sd.x[i][CUR_INDEX] + fac.b[j+1])
                {
                    for(int k=0; k<TAN_CELL_NUM_MAX; k++)
                    {
                        if (TAN_CELL_RES * k < sd.x[i][CUR_INDEX] 
                                && sd.x[i][CUR_INDEX] <= TAN_CELL_RES * (k+1))
                        {
                            cell.step_num[k][j]++;

                            cell.sum_x[k][j] += sd.x[i][CUR_INDEX];
                            cell.sum_y[k][j] += sd.y[i][CUR_INDEX];

                            cell.sum_steptime[k][j] += steptime[i];
                            break;
                        }
                    }
                    break;
                }
            } //短冊のループおわり
        }
    } //ステップのループおわり
}

//いっことばしの考慮あり
void cal_frame_arrival(Tanzaku& tanzaku)
{
    double arrival_temp[TANZAKU_NUM_MAX];
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        if (tanzaku.x[tan_num][CUR_INDEX] != 0.0
                && tanzaku.v[tan_num][CUR_INDEX] > 0.0)
        {
            arrival_temp[tan_num] = tanzaku.x[tan_num][CUR_INDEX] / tanzaku.v[tan_num][CUR_INDEX];
            tanzaku.frame_arrival[tan_num] = arrival_temp[tan_num] / FREQ;
        }else if (tanzaku.x[tan_num][CUR_INDEX] == 0.0
                && tanzaku.x[tan_num][PRE_INDEX] != 0.0
                && tanzaku.v[tan_num][CUR_INDEX] > 0.0) 
        {
            arrival_temp[tan_num] = tanzaku.x[tan_num][PRE_INDEX] / tanzaku.v[tan_num][CUR_INDEX];
            tanzaku.frame_arrival[tan_num] = arrival_temp[tan_num] / FREQ;
        }else{
            tanzaku.frame_arrival[tan_num] = 100;
        }
    }
}

// グループ全体の重心を位置データとする
void cal_pos_group_near(Cell& cell, Tanzaku& tanzaku)
{
    int tan_num;

    double tan_x_group_near[TANZAKU_NUM_MAX] = {0};
    double tan_y_group_near[TANZAKU_NUM_MAX] = {0};
    double tan_scantime_group_near[TANZAKU_NUM_MAX] = {0};

    for (tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        int ground_cnt = 0; //谷を見つけるためのカウンタ

        int tan_cell_num = 0; //ループのカウンター
        int step_num_in_groupA = 0; //グループAに含まれるステップの数

        double sum_x_in_groupA = 0.0;
        double sum_y_in_groupA = 0.0;
        double sum_steptime_in_groupA= 0.0;

        //-グルーピングして、グループごとのピークを検出-//
        //0じゃないヒストグラムまで進む
        for (tan_cell_num = 0; tan_cell_num < TAN_CELL_NUM_MAX; tan_cell_num++)
        {
            if(cell.step_num[tan_cell_num][tan_num] > 0) break;
        }

        //-groupAに座標データを足し合わせていく-//
        for (; tan_cell_num < TAN_CELL_NUM_MAX; tan_cell_num++)
        {
            if (cell.step_num[tan_cell_num] == 0)
            {
                // 谷間を見つける
                // 2回連続でstep_num_in_cellが空であればグループが終了したとみなす
                ground_cnt++;
            }else{
                step_num_in_groupA += cell.step_num[tan_cell_num][tan_num];
                sum_x_in_groupA += cell.sum_x[tan_cell_num][tan_num];
                sum_y_in_groupA += cell.sum_y[tan_cell_num][tan_num];
                sum_steptime_in_groupA += cell.sum_steptime[tan_cell_num][tan_num];
                ground_cnt = 0;
            }
            if (ground_cnt >= 2) break;
        }

        //-グループの重心を求める-//
        if (step_num_in_groupA > 0)
        {
            tan_x_group_near[tan_num] = sum_x_in_groupA / step_num_in_groupA;
            tan_y_group_near[tan_num] = sum_y_in_groupA / step_num_in_groupA;
            tan_scantime_group_near[tan_num] = sum_steptime_in_groupA / step_num_in_groupA;
        }else{
            tan_x_group_near[tan_num] = 0.0;
            tan_y_group_near[tan_num] = 0.0; 
            tan_scantime_group_near[tan_num] = 0.0;
        }
    } //tanループ

    // バッファに格納
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        tanzaku.x[i].pop_front();
        tanzaku.y[i].pop_front();
        tanzaku.scan_time[i].pop_front();
        tanzaku.x[i].push_back(tan_x_group_near[i]);
        tanzaku.y[i].push_back(tan_y_group_near[i]);
        tanzaku.scan_time[i].push_back(tan_scantime_group_near[i]);
    }
}

void cal_w(TANZAKU_FAC& fac, Tanzaku& tanzaku, deque<double>& sum_w)
{
    //短冊ごとの幅の算出
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        tanzaku.w[i].pop_front();
        tanzaku.w[i].push_back(0.0);
        if(tanzaku.x[i][CUR_INDEX] != 0.0)
        {
            tanzaku.w[i][CUR_INDEX] = fac.wn[i] * tanzaku.x[i][CUR_INDEX] / 1000.0;
        }
        if(tanzaku.x[i][CUR_INDEX] == 0.0 && tanzaku.x[i][PRE_INDEX] != 0.0)
        {
            tanzaku.w[i][CUR_INDEX] = fac.wn[i] * tanzaku.x[i][PRE_INDEX] / 1000.0; //位置データが無かったら一つ前のデータで補完   
        }
    }

    // 位置データが存在する短冊のうち端どうしの距離を幅とする方法
    int w_left = 0;
    int w_right = 0;
    double sum_w_cur = 0.0;
    for (int i = 0; i < TANZAKU_NUM_MAX; i++) //有効な短冊の範囲ぶん
    {
        if (tanzaku.approach_cnt[i] >= 2)
        {
            w_left = i;
            break;
        }
    }
    for (int i = TANZAKU_NUM_MAX; i > 0; i--) //有効な短冊の範囲ぶん
    {
        if (tanzaku.approach_cnt[i] >= 2)
        {
            w_right = i;
            break;
        }
    }
    if (tanzaku.w[w_left][CUR_INDEX] <= tanzaku.w[w_right][CUR_INDEX])
    {
        sum_w_cur = tanzaku.w[w_right][CUR_INDEX] * (w_right - w_left + 1);
    }else{
        sum_w_cur = tanzaku.w[w_left][CUR_INDEX] * (w_right - w_left + 1);
    }

    sum_w.pop_front();
    sum_w.push_back(sum_w_cur);

}

// 速度変化が大きすぎるときにデータを無視する
void clear_buf(vector< deque<double> >& G_data_buf)
{
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        if (fabs(G_data_buf[i][PRE_INDEX] - G_data_buf[i][CUR_INDEX]) > MAX_SPEED 
                && G_data_buf[i][CUR_INDEX] != 0.0 
                && G_data_buf[i][PRE_INDEX] != 0.0)
        {
            for (int j = 0; j < BUFFER_LENGTH; j++)
            {
                G_data_buf[i][j] = 0.0;
            }
        }
        if (fabs(G_data_buf[i][PREPRE_INDEX] - G_data_buf[i][CUR_INDEX]) > (2 * MAX_SPEED) 
                && G_data_buf[i][CUR_INDEX] != 0.0
                && G_data_buf[i][PRE_INDEX] == 0.0 
                && G_data_buf[i][PREPRE_INDEX] != 0.0)
        {
            for (int j = 0; j < BUFFER_LENGTH; j++)
            {
                G_data_buf[i][j] = 0.0;
            }
        }
    }
}

// frame_observeを使用しない
void judge_open_mode_tan(Tanzaku& tanzaku, deque<double>& sum_w)
{
    //-初期化-//
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        tanzaku.open_mode[i] = 0;
    }

    //-open_mode_tanの判定と出力-//
    if (sum_w[CUR_INDEX] <= DOOR_W_TH)
    {
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            //open_mode_tanの計算
            if (tanzaku.approach_cnt[i] >= FRAME_OBSERVE 
                    && tanzaku.approach_cnt != 0) //?
            {
                if (tanzaku.frame_arrival[i] <= (REQUIRED_FRAME_HIGH_HALF + MARGIN) )
                {
                    tanzaku.open_mode[i] = 2; //高速半開
                }else if ( (REQUIRED_FRAME_HIGH_HALF + MARGIN) < tanzaku.frame_arrival[i] 
                        && tanzaku.frame_arrival[i] <= (REQUIRED_FRAME_NORMAL_HALF + MARGIN) )
                {
                    tanzaku.open_mode[i] = 1; //低速半開
                }else{
                    tanzaku.open_mode[i] = 0; //開けない
                }
            }
        }  
    }else{
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            //open_mode_tanの計算
            if (tanzaku.approach_cnt[i] >= FRAME_OBSERVE 
                    && tanzaku.approach_cnt != 0)
            {
                if (tanzaku.frame_arrival[i] <= (REQUIRED_FRAME_HIGH_FULL + MARGIN) )
                {
                    tanzaku.open_mode[i] = 4; //高速全開
                }else if ( (REQUIRED_FRAME_HIGH_HALF + MARGIN) < tanzaku.frame_arrival[i] 
                        && tanzaku.frame_arrival[i] <= (REQUIRED_FRAME_NORMAL_FULL + MARGIN) )
                {
                    tanzaku.open_mode[i] = 3; //低速全開
                }else{
                    tanzaku.open_mode[i] = 0; //開けない
                }
            }
        } 
    }
    //open_mode_tanの判定と出力おわり//  
}

void least_square(Tanzaku& tanzaku)
{
    double x[TANZAKU_NUM_MAX][BUFFER_LENGTH];
    double y[TANZAKU_NUM_MAX][BUFFER_LENGTH];

    //-xのわりあて-//
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        for (int j = 0; j < BUFFER_LENGTH; j++)
        {
            x[i][j] = FREQ * j + tanzaku.scan_time[i][j];
        }
    }

    //-yのわりあて-//
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        for(int j=0; j<BUFFER_LENGTH; j++)
        {
            y[i][j] = tanzaku.x[i][j];
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
                tanzaku.v[i].pop_front();
                tanzaku.v[i].push_back(0.0);
            }else{
                // 速度方向を反転する
                tanzaku.v[i].pop_front();
                tanzaku.v[i].push_back(temp_v * -1);
            }
        }else{
            tanzaku.v[i].pop_front();
            tanzaku.v[i].push_back(0.0);
        }

    }
}

// 2点間の距離を求める
double p_dist(double p0x, double p0y, double p1x, double p1y)
{
    double dist2, dist;
    dist2 = (p1x - p0x) * (p1x - p0x) + (p1y - p0y) * (p1y - p0y);
    dist = sqrt(dist2);
    return dist;
}

//スキャンの方向を考慮する方法（必ず同じ方向のものと比較する）
void upd_tan_approach_cnt(Tanzaku& tanzaku)
{
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        if (tanzaku.x[i][CUR_INDEX] != 0.0
                && tanzaku.x[i][PREPRE_INDEX] != 0.0)
        {
            if ( (tanzaku.x[i][PREPRE_INDEX] - tanzaku.x[i][CUR_INDEX]) > (2 * MIN_SPEED)) //最低スピード以上ならインクリメント。 //最大スピードについても考えたほうがよい？
            {
                if (tanzaku.x[i][PRE_INDEX] != 0.0)
                {
                    tanzaku.approach_cnt[i]++;
                }
                if (tanzaku.x[i][PRE_INDEX] == 0.0) //PREが0のときは前回のぶんもインクリメント
                {
                    tanzaku.approach_cnt[i]++;
                    tanzaku.approach_cnt[i]++;
                }
            }
            if (tanzaku.x[i][PREPRE_INDEX] - tanzaku.x[i][CUR_INDEX] < (-2 * MIN_SPEED)) //遠ざかったときはクリア
            {
                tanzaku.approach_cnt[i] = 0;
            }
        }
        if (tanzaku.x[i][CUR_INDEX] == 0.0
                && tanzaku.x[i][PRE_INDEX] == 0.0) //物体がないときはクリア
        {
            tanzaku.approach_cnt[i] = 0;
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

int Tanzaku::judgeOpen(Lane& lane)
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

