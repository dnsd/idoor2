#include <iostream>
#include <ssm.hpp>
#include <cmath>
#include <fstream>
#include <sys/time.h>
#include <vector>
#include <deque>
#include "decision.h"

using namespace std;

#define FILENAME "debug.csv"

//-ファイル読み込み用-//
FILE *fp; //ファイルポインタ
ofstream ofs;

//-短冊の形状データ-//
TANZAKU_FAC fac;

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

    //-クラスのインスタンス-//
    Tanzaku tanzaku;
    Cell cell;
    Step ped;
    Lane lane;

    //-初期設定-//
    log_ctr::initialize_open_log();
    log_ctr::read_tan_fac(fac);
    log_ctr::read_tan_wn(fac);

    // 出力の設定
    ofs.open(FILENAME);
    ofs.precision(16);
    cout.precision(16);
    //-初期設定おわり-//

    while(1) //データ取得ループ
    {
        if (OBJECT.readNew())//if(OBJECTreadNew)
        {
            double time_loop_start = get_time();
            SCAN_DATA.readNew(); //scantimeの読み込み用

            //-読み込みデータのセット-//
            ped.set_data(OBJECT.data.det, OBJECT.data.dist, OBJECT.data.x, OBJECT.data.y, OBJECT.data.z);

            //-短冊を使った位置・速度データの計算-//
            tanzaku.allocateStep(fac, ped, cell);
            tanzaku.calPos(cell, tanzaku);
            tanzaku.calSpeed();

            //-短冊を使った開き判定-//
            tanzaku.updApproachCnt();
            tanzaku.calArrivalTime(); //歩行者がドアに到達するまでのフレーム数
            tanzaku.calObjectWidth(fac); //幅の算出（tan_xベース）
            tanzaku.judgeOpen();

            //-レーンを使った素通りのキャンセル-//
            lane.upd_pending_cnt(tanzaku);
            
            //-最終的な開き命令-//
            DORDER.data.order = open::judge_open_mode(tanzaku.vote(lane));

            //-SSM書き込み-//
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
            // log_ctr::write_open_log(ped, tanzaku, DORDER.data.order, SCAN_DATA.time);

            //-周期の調整-//
            double time_loop_end = get_time();
            usleep(FREQ*1000000 - (time_loop_end - time_loop_start)*1000000);

            //-結果の標準出力-//
            for (int i = 0; i < TANZAKU_NUM_MAX; i++)
            {
                // cout << tanzaku.open_mode[i] << ",";
            }
            // cout << endl;
            cout << " order = " << DORDER.data.order << endl; //開き判定のコンソール出力
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
