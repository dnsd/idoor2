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

//-B判定用-//
bool B_flag = false;
int stop_cnt = 0;

//-開閉判定用-//

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
    Step ped;
    Area area1;

    //-初期設定-//
    area1.defineCuboid(AREA_START_X, AREA_END_X, AREA_START_Y, AREA_END_Y, AREA_START_Z, AREA_END_Z);
    area1.set_step_num_cnt_th(STEP_NUM_CNT_TH);
    area1.set_buf_num_cnt_th(BUF_NUM_CNT_TH);
    area1.set_buf_length_has_objects(BUF_LENGTH_HAS_OBJECTS);

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

            // 開き命令
            DORDER.data.order = judge_open_mode(area1.judgeOpen(ped));

            // SSM書き込み
            DORDER.write();

            //-結果のファイル出力-//


            time_2 = get_time();
            usleep(FREQ*1000000 - (time_2 - time_1)*1000000);
            time_3 = get_time();

            //-結果の標準出力-//
            cout << "open_mode_door = " << DORDER.data.order << endl; //開き判定のコンソール出力
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
