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

// 2点間の距離を求める
double p_dist(double p0x, double p0y, double p1x, double p1y)
{
    double dist2, dist;
    dist2 = (p1x - p0x) * (p1x - p0x) + (p1y - p0y) * (p1y - p0y);
    dist = sqrt(dist2);
    return dist;
}

// 短冊へのデータの割り当て
void allocate_data_to_tanzaku(double tan_fac_a[], double tan_fac_b[], double steptime[], Step& sd, Cell& cell)
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
                if (sd.y[i][CUR_INDEX] > tan_fac_a[j] * sd.x[i][CUR_INDEX] + tan_fac_b[j] 
                        && sd.y[i][CUR_INDEX] < tan_fac_a[j+1] * sd.x[i][CUR_INDEX] + tan_fac_b[j+1])
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

// グループ全体の重心を位置データとする
void cal_pos_group_near(int step_num_in_cell[][TANZAKU_NUM_MAX], double sum_x_in_cell[][TANZAKU_NUM_MAX], double sum_y_in_cell[][TANZAKU_NUM_MAX], double sum_steptime_in_cell[][TANZAKU_NUM_MAX],
        vector< deque<double> >& tan_x_buf, vector< deque<double> >& tan_y_buf, vector< deque<double> >& tan_scantime_buf)
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
            if(step_num_in_cell[tan_cell_num][tan_num] > 0) break;
        }

        //-groupAに座標データを足し合わせていく-//
        for (; tan_cell_num < TAN_CELL_NUM_MAX; tan_cell_num++)
        {
            if (step_num_in_cell[tan_cell_num] == 0)
            {
                // 谷間を見つける
                // 2回連続でstep_num_in_cellが空であればグループが終了したとみなす
                ground_cnt++;
            }else{
                step_num_in_groupA += step_num_in_cell[tan_cell_num][tan_num];
                sum_x_in_groupA += sum_x_in_cell[tan_cell_num][tan_num];
                sum_y_in_groupA += sum_y_in_cell[tan_cell_num][tan_num];
                sum_steptime_in_groupA += sum_steptime_in_cell[tan_cell_num][tan_num];
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
        tan_x_buf[i].pop_front();
        tan_y_buf[i].pop_front();
        tan_scantime_buf[i].pop_front();
        tan_x_buf[i].push_back(tan_x_group_near[i]);
        tan_y_buf[i].push_back(tan_y_group_near[i]);
        tan_scantime_buf[i].push_back(tan_scantime_group_near[i]);
    }
}

void least_square(vector< deque<double> >& G_time_buf, vector< deque<double> >& G_data_buf, vector< deque<double> >& v)
{
    double x[TANZAKU_NUM_MAX][BUFFER_LENGTH];
    double y[TANZAKU_NUM_MAX][BUFFER_LENGTH];

    //-xのわりあて-//
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        for (int j = 0; j < BUFFER_LENGTH; j++)
        {
            x[i][j] = FREQ * j + G_time_buf[i][j];
        }
    }

    //-yのわりあて-//
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        for(int j=0; j<BUFFER_LENGTH; j++)
        {
            y[i][j] = G_data_buf[i][j];
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

void cal_w(double tan_wn[], vector< deque<double> >& G_data_buf,
        int tan_approach_cnt[],  
        vector< deque<double> >& tan_w, deque<double>& sum_w)
{
    //短冊ごとの幅の算出
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        // tan_w[i] = 0.0;
        tan_w[i].pop_front();
        tan_w[i].push_back(0.0);
        if(G_data_buf[i][CUR_INDEX] != 0.0)
        {
            tan_w[i][CUR_INDEX] = tan_wn[i] * G_data_buf[i][CUR_INDEX] / 1000.0;
        }
        if(G_data_buf[i][CUR_INDEX] == 0.0 && G_data_buf[i][PRE_INDEX] != 0.0)
        {
            tan_w[i][CUR_INDEX] = tan_wn[i] * G_data_buf[i][PRE_INDEX] / 1000.0; //位置データが無かったら一つ前のデータで補完   
        }
    }

    // 位置データが存在する短冊のうち端どうしの距離を幅とする方法
    int w_left = 0;
    int w_right = 0;
    double sum_w_cur = 0.0;
    for (int i = 0; i < TANZAKU_NUM_MAX; i++) //有効な短冊の範囲ぶん
    {
        if (tan_approach_cnt[i] >= 2)
        {
            w_left = i;
            break;
        }
    }
    for (int i = TANZAKU_NUM_MAX; i > 0; i--) //有効な短冊の範囲ぶん
    {
        if (tan_approach_cnt[i] >= 2)
        {
            w_right = i;
            break;
        }
    }
    if (tan_w[w_left][CUR_INDEX] <= tan_w[w_right][CUR_INDEX])
    {
        sum_w_cur = tan_w[w_right][CUR_INDEX] * (w_right - w_left + 1);
    }else{
        sum_w_cur = tan_w[w_left][CUR_INDEX] * (w_right - w_left + 1);
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

//いっことばしの考慮あり
void cal_frame_arrival(vector< deque<double> >& G_data_buf, vector< deque<double> >& v, int frame_arrival[])
{
    double arrival_temp[TANZAKU_NUM_MAX];
    for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
    {
        if (G_data_buf[tan_num][CUR_INDEX] != 0.0
                && v[tan_num][CUR_INDEX] > 0.0)
        {
            arrival_temp[tan_num] = G_data_buf[tan_num][CUR_INDEX] / v[tan_num][CUR_INDEX];
            frame_arrival[tan_num] = arrival_temp[tan_num] / FREQ;
        }else if (G_data_buf[tan_num][CUR_INDEX] == 0.0
                && G_data_buf[tan_num][PRE_INDEX] != 0.0
                && v[tan_num][CUR_INDEX] > 0.0) 
        {
            arrival_temp[tan_num] = G_data_buf[tan_num][PRE_INDEX] / v[tan_num][CUR_INDEX];
            frame_arrival[tan_num] = arrival_temp[tan_num] / FREQ;
        }else{
            frame_arrival[tan_num] = 100;
        }
    }
}

// frame_observeを使用しない
void judge_open_mode_tan(int tan_approach_cnt[], int frame_arrival[], deque<double>& sum_w, int open_mode_tan[])
{
    //-初期化-//
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        open_mode_tan[i] = 0;
    }

    //-open_mode_tanの判定と出力-//
    if (sum_w[CUR_INDEX] <= DOOR_W_TH)
    {
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            //open_mode_tanの計算
            if (tan_approach_cnt[i] >= FRAME_OBSERVE 
                    && tan_approach_cnt != 0) //?
            {
                if (frame_arrival[i] <= (REQUIRED_FRAME_HIGH_HALF + MARGIN) )
                {
                    open_mode_tan[i] = 2; //高速半開
                }else if ( (REQUIRED_FRAME_HIGH_HALF + MARGIN) < frame_arrival[i] 
                        && frame_arrival[i] <= (REQUIRED_FRAME_NORMAL_HALF + MARGIN) )
                {
                    open_mode_tan[i] = 1; //低速半開
                }else{
                    open_mode_tan[i] = 0; //開けない
                }
            }
        }  
    }else{
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            //open_mode_tanの計算
            if (tan_approach_cnt[i] >= FRAME_OBSERVE 
                    && tan_approach_cnt != 0)
            {
                if (frame_arrival[i] <= (REQUIRED_FRAME_HIGH_FULL + MARGIN) )
                {
                    open_mode_tan[i] = 4; //高速全開
                }else if ( (REQUIRED_FRAME_HIGH_HALF + MARGIN) < frame_arrival[i] 
                        && frame_arrival[i] <= (REQUIRED_FRAME_NORMAL_FULL + MARGIN) )
                {
                    open_mode_tan[i] = 3; //低速全開
                }else{
                    open_mode_tan[i] = 0; //開けない
                }
            }
        } 
    }
    //open_mode_tanの判定と出力おわり//  
}

//スキャンの方向を考慮する方法（必ず同じ方向のものと比較する）
void upd_tan_approach_cnt(vector< deque<double> >& G_data_buf, int tan_approach_cnt[])
{
    for (int i = 0; i < TANZAKU_NUM_MAX; i++)
    {
        if (G_data_buf[i][CUR_INDEX] != 0.0
                && G_data_buf[i][PREPRE_INDEX] != 0.0)
        {
            if ( (G_data_buf[i][PREPRE_INDEX] - G_data_buf[i][CUR_INDEX]) > (2 * MIN_SPEED)) //最低スピード以上ならインクリメント。 //最大スピードについても考えたほうがよい？
            {
                if (G_data_buf[i][PRE_INDEX] != 0.0)
                {
                    tan_approach_cnt[i]++;
                }
                if (G_data_buf[i][PRE_INDEX] == 0.0) //PREが0のときは前回のぶんもインクリメント
                {
                    tan_approach_cnt[i]++;
                    tan_approach_cnt[i]++;
                }
            }
            if (G_data_buf[i][PREPRE_INDEX] - G_data_buf[i][CUR_INDEX] < (-2 * MIN_SPEED)) //遠ざかったときはクリア
            {
                tan_approach_cnt[i] = 0;
            }
        }
        if (G_data_buf[i][CUR_INDEX] == 0.0
                && G_data_buf[i][PRE_INDEX] == 0.0) //物体がないときはクリア
        {
            tan_approach_cnt[i] = 0;
        }
    }
}

bool Tanzaku::isInDetectionArea(int tan_num, int index)
{
    // 位置データが存在するときエリアの内外判定をする
    if (x[tan_num][index] != 0.0)
    {
        // 物体がエリアの中にあるとき真
        if (x[tan_num][index] <= AREA_E_END_X
                && AREA_E_START_Y <= y[tan_num][index]
                && y[tan_num][index] <= AREA_E_END_Y)
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
        if (x[tan_num][index-1] <= AREA_E_END_X
                && AREA_E_START_Y <= y[tan_num][index-1]
                && y[tan_num][index-1] <= AREA_E_END_Y)
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

bool Tanzaku::isCancel(Lane& lane, int tan_num)
{
    // 物体がエリア内にいるとき（条件1）
    if (isInDetectionArea(tan_num, CUR_INDEX) == true)
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
    if (isInDetectionArea(tan_num, CUR_INDEX) == false)
    {
        //cancel_flag[tan_num] = true;
        return true;
    }
    return false;
}
