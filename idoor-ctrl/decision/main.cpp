#include <iostream>
#include <ssm.hpp>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "decision.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

#define FILENAME "debug.csv"

//-ファイル読み込み用-//
FILE *fp; //ファイルポインタ
ofstream ofs;

//-短冊の形状データ-//
double tan_fac_a[BORDER_NUM_MAX];
double tan_fac_b[BORDER_NUM_MAX];
double tan_fac_p0x[TANZAKU_NUM_MAX];
double tan_fac_p0y[TANZAKU_NUM_MAX];
double tan_wn[TANZAKU_NUM_MAX]; //位置データ(tan_pos or tan_x)が1mのときの短冊の幅

//-B判定用-//
bool B_flag = false;
int stop_cnt = 0;

deque<double> sum_w(BUFFER_LENGTH);

//-タイムゾーン設定用-//
double steptime[STEP_NUM];

//-開閉判定用-//
int open_mode_door;

// edge
// bool cancel_flag[TANZAKU_NUM_MAX] = {false};

//-時間計測用-//
double time_1; //時間計測用
double time_2; //時間計測用
double time_3; //時間計測用

//-SSM-//
SSMApi<LS3D> SCAN_DATA("LS3D", 0);
SSMApi<LS3D> OBJECT("OBJECT", 1);
SSMApi<ORDER> DORDER("ORDER", 30);

int main (int argc, char **argv)
{
    //-SSM-//
    initSSM();
    SCAN_DATA.open(SSM_READ);
    OBJECT.open(SSM_READ);
    DORDER.create(5.0, 1.0);

    //-クラス-//
    Tanzaku tanzaku;
    Cell cell;
    Step ped;
    Lane lane;

    //-初期設定-//
    initialize_open_log();
    read_tan_fac(tan_fac_a, tan_fac_b, tan_fac_p0x, tan_fac_p0y);
    read_tan_wn(tan_wn);

    // ステップタイムの設定
    for (int i = 1; i <= STEP_NUM; i++)
    {
        steptime[i-1] = FREQ / STEP_NUM * i;
    }

    // 出力の設定
    ofs.open(FILENAME);
    ofs.precision(16);
    cout.precision(16);
    //-初期設定おわり-//

    while(1) //データ取得ループ
    {
        if (OBJECT.readNew())//if(OBJECTreadNew)
        {
            SCAN_DATA.readNew(); //scantimeの読み込み用
            time_1 = get_time();

            //-読み込みデータのセット-//
            ped.set_data(OBJECT.data.det, OBJECT.data.dist, OBJECT.data.x, OBJECT.data.y, OBJECT.data.z);

            // 短冊を使った位置・速度データの計算
            allocate_data_to_tanzaku(tan_fac_a, tan_fac_b, steptime, ped, cell);
            cal_pos_group_near(cell.step_num, cell.sum_x, cell.sum_y, cell.sum_steptime, tanzaku.x, tanzaku.y, tanzaku.scan_time);
            least_square(tanzaku.scan_time, tanzaku.x, tanzaku.v);

            // 短冊を使った開き判定
            upd_tan_approach_cnt(tanzaku.x, tanzaku.approach_cnt);//upd_tan_approach_cntの設定
            cal_frame_arrival(tanzaku.x, tanzaku.v, tanzaku.frame_arrival);//歩行者がドアに到達するまでのフレーム数
            cal_w(tan_wn, tanzaku.x, tanzaku.approach_cnt, tanzaku.w, sum_w); //幅の算出（tan_xベース）
            judge_open_mode_tan(tanzaku.approach_cnt, tanzaku.frame_arrival, sum_w, tanzaku.open_mode);

            // レーンを使った素通りのキャンセル
            lane.set_on_the_lane_flag(tanzaku);
            lane.set_entry_lane_flag(tanzaku);
            lane.set_pending_flag();

            // judge_B(ped, stop_cnt, B_flag);
            
            // 最終的な開き命令
            judge_open_mode(tanzaku, lane, B_flag, open_mode_door);
            DORDER.data.order = open_mode_door;

            // SSM書き込み
            DORDER.write();

            //-結果のファイル出力-//
            for(int i=0; i<TANZAKU_NUM_MAX; i++)
            {
                // ofs << tanzaku.v[i] << "," ;
            }

            for (int i = 0; i < LANE_NUM_MAX; i++)
            {
                // ofs << cancel_lane_flag[i] << ",";
            }

            // ofs << endl;

            // open_log出力
            write_open_log(OBJECT.data.det, OBJECT.data.x, OBJECT.data.y, OBJECT.data.z, 
                     open_mode_door, tanzaku.open_mode, tanzaku.x, tanzaku.v, 
                     tanzaku.approach_cnt, SCAN_DATA.time);

            time_2 = get_time();
            usleep(FREQ*1000000 - (time_2 - time_1)*1000000);
            time_3 = get_time();

            //-結果の標準出力-//
            for (int i = 0; i < TANZAKU_NUM_MAX; i++)
            {
                // cout << tanzaku.v[i][CUR_INDEX];
                // cout << tanzaku.frame_arrival[i] << ",";
                // cout << tanzaku.open_mode[i] << ",";
            }
            // cout << endl;
            // cout << "time = " << time_3 - time_1 << endl;
            cout << "open_mode_door = " << open_mode_door << endl; //開き判定のコンソール出力
            // cout << SCAN_DATA.time << endl;
            // cout << endl;
        }else{  //if(OBJECTreadNew)
            usleep(1000); //CPU使用率100％対策
        } 
    }
    //-SSMクローズ-//
    SCAN_DATA.close();
    OBJECT.close();
    endSSM();
    return 0;
}
