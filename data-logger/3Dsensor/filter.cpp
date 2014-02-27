#include <iostream>
#include <cmath>
#include <fstream>
#include <sys/time.h>
#include <ssm.hpp>
#include "connect.h"

using namespace std;

//-関数-//
void cal_background_diff (Step_buf& raw_data, Step_buf& ped, LS3D& background);
void get_background (Step_buf& raw_data, LS3D& background );
bool initialize(Step_buf& raw_data_U, Step_buf& raw_data_D, LS3D& background_U, LS3D& background_D);
bool judge_MovingObjects_exist(Step_buf& ped_data);
bool judge_Objects_exist(Step_buf& ped_data);
void apply_area_filter(Step_buf& ped_data);

//-SSM-//
SSMApi<LS3D> SCAN_DATA("LS3D", 0);
SSMApi<LS3D> OBJECT("OBJECT", 1);
SSMApi<AREA_INFO> AREA("AREA_INFO", 21);

int main (int argc, char *argv[])
{
    //-SSM-//
    initSSM();
    OBJECT.create(5.0,1.0);
    SCAN_DATA.open(SSM_READ);
    AREA.create(5.0, 1.0);

    //-クラスと構造体-//
    Step_buf raw_data_U; //生データ
    Step_buf raw_data_D; //生データ
    Step_buf ped_data; //加工後のデータ
    LS3D background_U; //環境データ
    LS3D background_D; //環境データ

    //-時間の制御用-//
    double time_MovingObjects_are_detected = get_time(); // 動体が検出された時の時刻
    double time_MovingObjects_gets_out = get_time(); // 動体がいなくなったときの時刻
    double time_get_background = 0.0;

    //-初期化-//
    initialize(raw_data_U, raw_data_D, background_U, background_D);
    time_get_background = get_time();
    // raw_data.out_csv(); // デバッグ用

    //-ループ開始-//
    while(1)
    {
        if (SCAN_DATA.readNew())
        {
            double loop_start_time = get_time();

            if (SCAN_DATA.data.det == 'U')
            {
                //-読み込みデータのセット-//
                raw_data_U.set_data(SCAN_DATA.data.det, SCAN_DATA.data.dist, SCAN_DATA.data.x, SCAN_DATA.data.y, SCAN_DATA.data.z);
                //-差分データの計算-//
                cal_background_diff (raw_data_U, ped_data, background_U);
                //-エリアフィルタの適用-//
                apply_area_filter(ped_data);
                //-差分データをSSM構造体にセット-//
                OBJECT.data.det = ped_data.det[CUR_INDEX];
                for (int i=0; i<STEP_NUM; i++)
                {
                    OBJECT.data.dist[i] = ped_data.dist[i][CUR_INDEX];
                    OBJECT.data.x[i] = ped_data.x[i][CUR_INDEX];
                    OBJECT.data.y[i] = ped_data.y[i][CUR_INDEX];
                    OBJECT.data.z[i] = ped_data.z[i][CUR_INDEX];
                }
            }
            if (SCAN_DATA.data.det == 'D')
            {
                //-読み込みデータのセット-//
                raw_data_D.set_data(SCAN_DATA.data.det, SCAN_DATA.data.dist, SCAN_DATA.data.x, SCAN_DATA.data.y, SCAN_DATA.data.z);
                //-差分データの計算-//
                cal_background_diff (raw_data_D, ped_data, background_D);
                //-エリアフィルタの適用-//
                apply_area_filter(ped_data);
                //-差分データをSSM構造体にセット-//
                OBJECT.data.det = ped_data.det[CUR_INDEX];
                for (int i=0; i<STEP_NUM; i++)
                {
                    OBJECT.data.dist[i] = ped_data.dist[i][CUR_INDEX];
                    OBJECT.data.x[i] = ped_data.x[i][CUR_INDEX];
                    OBJECT.data.y[i] = ped_data.y[i][CUR_INDEX];
                    OBJECT.data.z[i] = ped_data.z[i][CUR_INDEX];
                }
            }

            //-静止物体が存在するかの判定-//
            // AREA.data.hasObjects = judge_Objects_exist(ped_data);
            //-動体が存在するかの判定-//
            AREA.data.hasMovingObjects = judge_MovingObjects_exist(ped_data);

            if ( AREA.data.hasMovingObjects == true)
            {
                // 動物体を検出したときの時刻を取得
                time_MovingObjects_are_detected = get_time();
            }else{
                // 動物体が存在しないときの時刻を取得
                time_MovingObjects_gets_out = get_time();
                // 最後に物体を検出してからの時間を算出
                double timelength_noMovingObjects = time_MovingObjects_gets_out - time_MovingObjects_are_detected;
                cout << "timelength_noMovingObjects = " << timelength_noMovingObjects << endl;
                // 動物体が長い間存在していなかったら、環境データを更新する。
                if (timelength_noMovingObjects > 3
                        && (get_time() - time_get_background) > 180)
                {
                    get_background (raw_data_U, background_U);
                    get_background (raw_data_D, background_D);
                    time_get_background = get_time();
                    time_MovingObjects_are_detected = get_time();
                    time_MovingObjects_gets_out = get_time();
                }
            }

            //-SSMの書き込み-//
            OBJECT.write();
            AREA.write();
            double loop_end_time = get_time();
            sleep_const_freq(loop_start_time, loop_end_time, FREQ);
        }else{
            usleep(10000); // CPU使用率100%対策
        }
    }

    //-SSMクローズ-//
    OBJECT.release();
    SCAN_DATA.close();
    AREA.release();
    endSSM();
    
    return 0;
}

void cal_background_diff (Step_buf& raw_data, Step_buf& ped, LS3D& background)
{
    ped.det.pop_front();
    ped.det.push_back(raw_data.det[CUR_INDEX]);
    for (int i=0; i<STEP_NUM; i++)
    {
        if (BACKGROUND_DIFF_TH < fabs(raw_data.dist[i][CUR_INDEX] - background.dist[i])
                && raw_data.dist[i][CUR_INDEX] != 0.0
                && raw_data.dist[i][CUR_INDEX] <= SURVEILLANCE_R
                && background.dist[i] != 0.0)
        {
            ped.dist[i].pop_front();
            ped.x[i].pop_front();
            ped.y[i].pop_front();
            ped.z[i].pop_front();
            ped.dist[i].push_back(raw_data.dist[i][CUR_INDEX]);
            ped.x[i].push_back(raw_data.x[i][CUR_INDEX]);
            ped.y[i].push_back(raw_data.y[i][CUR_INDEX]);
            ped.z[i].push_back(raw_data.z[i][CUR_INDEX]);
        }else{
            ped.dist[i].pop_front();
            ped.x[i].pop_front();
            ped.y[i].pop_front();
            ped.z[i].pop_front();
            ped.dist[i].push_back(0.0);
            ped.x[i].push_back(0.0);
            ped.y[i].push_back(0.0);
            ped.z[i].push_back(0.0);
        }
    }
}

void get_background (Step_buf& raw_data, LS3D& background )
{
    Step_buf copy_data(raw_data);
    copy_data.bufsort();

    background.det = copy_data.det[CUR_INDEX];
    for (int i=0; i<STEP_NUM; i++)
    {
        int mid = copy_data.dist[i].size() / 2;
        background.dist[i] = copy_data.dist[i][mid];
        background.x[i] = copy_data.x[i][mid];
        background.y[i] = copy_data.y[i][mid];
        background.z[i] = copy_data.z[i][mid];
    }

    cout << "-----環境データを取得-----" << endl;
}

bool initialize(Step_buf& raw_data_U, Step_buf& raw_data_D, LS3D& background_U, LS3D& background_D)
{
    cout << "初期化開始" << endl;

    int scan_cnt_U = 0;
    int scan_cnt_D = 0;
    double loop_start_time = 0.0;
    double loop_end_time = 0.0;

    while(scan_cnt_U < BUFFER_LENGTH && scan_cnt_D < BUFFER_LENGTH)
    {
        if (SCAN_DATA.readNew())
        {
            loop_start_time = get_time();
            
            //-読み込みデータのセット-//
            if(SCAN_DATA.data.det == 'U')
            {
                raw_data_U.set_data(SCAN_DATA.data.det, SCAN_DATA.data.dist, SCAN_DATA.data.x, SCAN_DATA.data.y, SCAN_DATA.data.z);
                scan_cnt_U++;
            }
            if(SCAN_DATA.data.det == 'D')
            {
                raw_data_D.set_data(SCAN_DATA.data.det, SCAN_DATA.data.dist, SCAN_DATA.data.x, SCAN_DATA.data.y, SCAN_DATA.data.z);
                scan_cnt_D++;
            }

            loop_end_time = get_time();
            sleep_const_freq(loop_start_time, loop_end_time, FREQ);
        }else{
            usleep(1000);
        }
    }

    get_background (raw_data_U, background_U);
    get_background (raw_data_D, background_D);

    cout << "初期化完了" << endl;
}

bool judge_MovingObjects_exist(Step_buf& ped_data)
{
    int moving_step_cnt = 0;
    // 動いているステップの数を数える
    for(int i=0; i<STEP_NUM; i++)
    {
        if(ped_data.isMoving(i) == true) moving_step_cnt++;
    }
    // 動いているステップの数が多いときに、動物体があるとする
    if (moving_step_cnt >= DETECTION_MOVING_OBJECT_TH)
    {
        cout << "動物体がある" << endl;
        return true;
    }else{
        cout << "動物体がない" << endl;
        return false;
    }
}

bool judge_Objects_exist(Step_buf& ped_data)
{
    int step_cnt = 0;
    for(int i=0; i<STEP_NUM; i++)
    {
        if(ped_data.dist[i][CUR_INDEX] != 0.0)
        {
            step_cnt++;
        }
    }

    if(step_cnt > 3)
    {
        return true;
    }else{
        return false;
    }
}

void apply_area_filter(Step_buf& ped_data)
{
    for (int i = 0; i < STEP_NUM; i++)
    {
        if (ped_data.isInTheArea(i) == false)
        {
            ped_data.dist[i][CUR_INDEX] = 0.0;
            ped_data.x[i][CUR_INDEX] = 0.0;
            ped_data.y[i][CUR_INDEX] = 0.0;
            ped_data.z[i][CUR_INDEX] = 0.0;
        }
    }
}
