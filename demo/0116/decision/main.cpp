// tanzaku_and_edge

#include <iostream>
#include <ssm.hpp>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "send_spotinfo.h"

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
double a[BORDER_NUM_MAX];
double b[BORDER_NUM_MAX];
double px0[TANZAKU_NUM_MAX];
double py0[TANZAKU_NUM_MAX];

//-近づくビームの抽出用-//
double step_dist_dif[STEP_NUM];

//-B判定用-//
bool B_flag = false;
int stop_cnt = 0;

//-スポット情報-//
int spotinfo_B[30][44];
int spotinfo_CF[30][44];
int spotinfo_D[30][44];
int spotinfo_E[30][44];
int spotinfo_GHI[30][44];

deque<double> sum_w(BUFFER_LENGTH);
double tan_wn[TANZAKU_NUM_MAX]; //位置データ(tan_pos or tan_x)が1mのときの短冊の幅

//-タイムゾーン設定用-//
double steptime[STEP_NUM];

//-開閉判定用-//
int open_mode_door;

// edge
bool in_the_areaE_flag[TANZAKU_NUM_MAX] = {false};
int entry_lane_num[TANZAKU_NUM_MAX] = {0}; //trueのとき開信号無効、falseのとき開信号有効
bool on_the_lane_flag[LANE_NUM_MAX] = {false};
bool cancel_lane_flag[LANE_NUM_MAX] = {false};
bool cancel_flag[TANZAKU_NUM_MAX] = {false};

//-時間計測用-//
double time_1; //時間計測用
double time_2; //時間計測用
double time_3; //時間計測用

//-SSM-//
SSMApi<LS3D> SCAN_DATA("LS3D", 0);
SSMApi<LS3D> OBJECT("OBJECT", 1);
SSMApi<LS3D> BEAM_CLOSE("BEAM_CLOSE", 3);
SSMApi<ORDER> DORDER("ORDER", 30);

double temp_sum_x_in_cell[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX];

int main (int argc, char **argv)
{
	//-SSM-//
    initSSM();
    SCAN_DATA.open(SSM_READ);
    OBJECT.open(SSM_READ);
    BEAM_CLOSE.create(5.0, 1.0);
    DORDER.create(5.0, 1.0);

    //-クラス-//
    tanzaku_data tanzaku;
    step_data object;
    step_data step_close;

    //-ファイル読み込み-//
    //tan_facの読み込み //y=ax+bで考える
    fp = fopen("tanzaku_fac","r");
    for(int i=0; i<BORDER_NUM_MAX; i++)
    {
        fscanf(fp, "%lf", &a[i]);
        fscanf(fp, "%lf", &b[i]);

        // px0[i] = b[i]; //tanの線の切片が全部同じ
        // py0[i] = 0.0;
        px0[i] = 0.0;
        py0[i] = 0.0;
    }
    fclose(fp);

    //wnの読み込み（tan_xベース）
    fp = fopen("wn","r");
    for(int i=0; i<TANZAKU_NUM_MAX; i++)
    {
        fscanf(fp, "%lf", &tan_wn[i]);
    }
    fclose(fp);
    //-ファイル読み込みおわり-//

    //-初期設定-//
    // initialize_open_log();

    //ステップタイムの設定
    for (int i = 1; i <= STEP_NUM; i++)
    {
        steptime[i-1] = FREQ / STEP_NUM * i;
    }
    //-初期設定おわり-//

    ofs.open(FILENAME);
    ofs.precision(16);
    cout.precision(16);
	while(1) //データ取得ループ
	{
	    if (OBJECT.readNew())//if(OBJECTreadNew)
	    {
            SCAN_DATA.readNew(); //scantimeの読み込み用
	        time_1 = get_time();

            //-読み込みデータのセット-//
            object.set_data(OBJECT.data.det, OBJECT.data.dist, OBJECT.data.x, OBJECT.data.y, OBJECT.data.z);
            object.set_close_flag();
            object.set_stop_flag();

            //-share-//
            BEAM_CLOSE.data.det = object.det[CUR_INDEX];
            for(int i=0; i<STEP_NUM; i++) //ステップのループ
            {
                //前時刻との距離データの差
                step_dist_dif[i] = object.dist[i][PREPRE_INDEX] - object.dist[i][CUR_INDEX];

                BEAM_CLOSE.data.dist[i] = 0.0;
                BEAM_CLOSE.data.x[i] = 0.0;
                BEAM_CLOSE.data.y[i] = 0.0;
                BEAM_CLOSE.data.z[i] = 0.0;

                if(step_dist_dif[i] >= CLOSE_TH) //OBJECT.dataから近づいているステップを抽出
                {
                    BEAM_CLOSE.data.dist[i] = object.dist[i][CUR_INDEX];
                    BEAM_CLOSE.data.x[i] = object.x[i][CUR_INDEX];
                    BEAM_CLOSE.data.y[i] = object.y[i][CUR_INDEX];
                    BEAM_CLOSE.data.z[i] = object.z[i][CUR_INDEX];
                }

                // 短冊へのデータ割り当て(BEAM_CLOSEを利用)
                if (BEAM_CLOSE.data.dist[i] != 0.0) //BEAM_CLOSEが無いときにはループさせない
                {
                    for (int j = 0; j < TANZAKU_NUM_MAX; j++) //短冊のループ
                    {
                        if (BEAM_CLOSE.data.y[i] > a[j] * BEAM_CLOSE.data.x[i] + b[j] 
                            && BEAM_CLOSE.data.y[i] < a[j+1] * BEAM_CLOSE.data.x[i] + b[j+1])
                        {
                            for(int k=0; k<TAN_CELL_NUM_MAX; k++)
                            {
                                if (TAN_CELL_RES * k < BEAM_CLOSE.data.x[i] 
                                    && BEAM_CLOSE.data.x[i] <= TAN_CELL_RES * (k+1))
                                {
                                    tanzaku.step_num_in_cell[k][j]++;

                                    tanzaku.sum_x_in_cell[k][j] += BEAM_CLOSE.data.x[i];
                                    tanzaku.sum_y_in_cell[k][j] += BEAM_CLOSE.data.y[i];

                                    temp_sum_x_in_cell[k][j] += BEAM_CLOSE.data.x[i];

                                    tanzaku.sum_steptime_in_cell[k][j] += steptime[i];
                                    break;
                                }
                            }
                            break;
                        }
                    } //短冊のループおわり
                }

            } //ステップのループおわり

            BEAM_CLOSE.write();

            cal_G_data_group_near (tanzaku.step_num_in_cell, tanzaku.sum_x_in_cell, tanzaku.sum_y_in_cell, tanzaku.sum_steptime_in_cell, tanzaku.x, tanzaku.y, tanzaku.scan_time);

            for(int i=0; i<TAN_CELL_NUM_MAX; i++) //tan_cellとsum_pos_tan_cell_Uの初期化
            {
                for(int j=0; j<TANZAKU_NUM_MAX; j++)
                {
                    tanzaku.step_num_in_cell[i][j] = 0.0;
                    tanzaku.sum_x_in_cell[i][j] = 0.0;
                    tanzaku.sum_y_in_cell[i][j] = 0.0;
                    tanzaku.sum_steptime_in_cell[i][j] = 0.0;
                    temp_sum_x_in_cell[i][j] = 0.0;
                }
            }

            // clear_buf(tanzaku.x, tanzaku.approach_cnt); //すれちがい対策
            least_square(tanzaku.scan_time, tanzaku.x, tanzaku.v);


            //短冊データごとのの読み込み//

            upd_tan_approach_cnt(tanzaku.x, tanzaku.approach_cnt);//upd_tan_approach_cntの設定
            cal_frame_arrival(tanzaku.x, tanzaku.v, tanzaku.frame_arrival);//歩行者がドアに到達するまでのフレーム数
            cal_w(tan_wn, tanzaku.x, tanzaku.approach_cnt, tanzaku.w, sum_w); //幅の算出（tan_xベース）

            judge_open_mode_tan(tanzaku.approach_cnt, tanzaku.frame_arrival, sum_w, tanzaku.open_mode);
            set_in_the_areaE_flag(in_the_areaE_flag, tanzaku.x, tanzaku.y);
            set_entry_lane_num(tanzaku.x, tanzaku.y, in_the_areaE_flag, entry_lane_num);
            set_on_the_lane_flag(tanzaku.x, on_the_lane_flag);
            upd_cancel_lane_flag(tanzaku.x, entry_lane_num, cancel_lane_flag);
            upd_cancel_flag(tanzaku.x, in_the_areaE_flag, cancel_lane_flag, cancel_flag);

            // judge_B(object, stop_cnt, B_flag);

            judge_open_mode(tanzaku.open_mode, cancel_flag, B_flag, open_mode_door);
            //DORDER.data.order = open_mode_door;
            //DORDER.write();

            //-各種判定-//

            //-結果のファイル出力-//
            // ssmTimeT timeSSM = gettimeSSM();
            // ofs << timeSSM << ",";
            for(int i=0; i<TANZAKU_NUM_MAX; i++)
            {
                // ofs << tanzaku.v[i] << "," ;
                // ofs << tanzaku.scan_time[i][CUR_INDEX] << ",";
                // ofs << tanzaku.approach_cnt[i] << ",";
                // ofs << tanzaku.frame_arrival[i] << ",";
                // ofs << tanzaku.frame_observe[i] << ",";
                // ofs << tan_w[i] << ",";
                // ofs << (tanzaku.approach_cnt[i] - tanzaku.frame_observe[i]) << ",";
                // ofs << tanzaku.x[i][CUR_INDEX] << ",";
                // ofs << tanzaku.y[i][CUR_INDEX] << ",";
                // ofs << tanzaku.open_mode[i] << ",";
                // ofs << in_the_areaE_flag[i] << ",";
                // ofs << cancel_flag[i] << ",";
                // ofs << entry_lane_num[i] << ",";
            }

            // open_log出力
            // write_open_log(OBJECT.data.det, OBJECT.data.x, OBJECT.data.y, OBJECT.data.z, 
            //                 open_mode_door, tanzaku.open_mode, tanzaku.x, tanzaku.v, 
            //                 tanzaku.approach_cnt, SCAN_DATA.time);

	        time_2 = get_time();
            usleep(FREQ*1000000 - (time_2 - time_1)*1000000);
            time_3 = get_time();

            //-結果の標準出力-//
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
    BEAM_CLOSE.release();
    endSSM();
    return 0;
}

double get_time(void) //現在時刻を取得
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}
