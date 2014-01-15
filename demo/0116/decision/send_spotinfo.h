// tanzaku_and_edge

#include "myconst.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

typedef struct{
    char det; //UorD
    double dist[2720];
    double x[2720];
    double y[2720];
    double z[2720];
}LS3D;

typedef struct{
    char order;
}ORDER;

class tanzaku_data
{
public:
    int step_num_in_cell[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップの数
    double sum_x_in_cell[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのx座標値の合計
    double sum_y_in_cell[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのy座標値の合計
    double sum_steptime_in_cell[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのスキャン時刻の総和

    int frame_arrival[TANZAKU_NUM_MAX];//何フレーム後にドアを開けるべきか
    int approach_cnt[TANZAKU_NUM_MAX]; //連続して観測すると増えていく

    int open_mode[TANZAKU_NUM_MAX]; //短冊ごとの判定結果

    vector< deque<double> > x;
    vector< deque<double> > y;
    vector< deque<double> > v;
    vector< deque<double> > w;
    vector< deque<double> > scan_time;

    tanzaku_data(){
        x.resize(TANZAKU_NUM_MAX);
        y.resize(TANZAKU_NUM_MAX);
        v.resize(TANZAKU_NUM_MAX);
        w.resize(TANZAKU_NUM_MAX);
        scan_time.resize(TANZAKU_NUM_MAX);
        for (int i = 0; i < TANZAKU_NUM_MAX; i++)
        {
            x[i].resize(BUFFER_LENGTH);
            y[i].resize(BUFFER_LENGTH);
            v[i].resize(BUFFER_LENGTH);
            w[i].resize(BUFFER_LENGTH);
            scan_time[i].resize(BUFFER_LENGTH);
        }
    }
};

class step_data
{
public:
    deque<char> det;
    vector< deque<double> > dist;
    vector< deque<double> > x;
    vector< deque<double> > y;
    vector< deque<double> > z;

    vector < deque<bool> > close_flag;
    vector < deque<bool> > stop_flag;

    void set_data(char& read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[]);
    void set_close_flag();
    void set_stop_flag();

    step_data(){
        det.resize(BUFFER_LENGTH);
        dist.resize(STEP_NUM);
        x.resize(STEP_NUM);
        y.resize(STEP_NUM);
        z.resize(STEP_NUM);
        close_flag.resize(STEP_NUM);
        stop_flag.resize(STEP_NUM);
        for (int i = 0; i < STEP_NUM; i++)
        {
            dist[i].resize(BUFFER_LENGTH);
            x[i].resize(BUFFER_LENGTH);
            y[i].resize(BUFFER_LENGTH);
            z[i].resize(BUFFER_LENGTH);
            close_flag[i].resize(BUFFER_LENGTH);
            stop_flag[i].resize(BUFFER_LENGTH);
        }
    }
};

//-関数のプロトタイプ宣言-//
//main.cpp
double get_time(void);
//B.cpp
void judge_B(step_data& data, int& stop_cnt, bool& B_flag);
//CF.cpp
void judge_CF(LS3D *OBJECT, double beam_U_dist_pre[], double beam_U_dist_dif[], double beam_D_dist_pre[], double beam_D_dist_dif[], double background_U[], double background_D[], int spotinfo_CF[][44]);
//D.cpp
void judge_D(int spotinfo_CF[][44], int spotinfo_D[][44], int *open_mode);
//GHI.cpp
void judge_GHI(LS3D *OBJECT, int spotinfo_GHI[][44]);
//E.cpp
void upd_tan_approach_cnt(vector< deque<double> >& G_data_buf, int tan_approach_cnt[]);
void cal_frame_arrival(vector< deque<double> >& G_data_buf, vector< deque<double> >& v, int frame_arrival[]);
//tan.cpp
double p_dif(double px0, double py0, double tan_x, double tan_y);
void cal_G_data_group_near (int tan_cell[][TANZAKU_NUM_MAX], double sum_x_tan_cell[][TANZAKU_NUM_MAX], double sum_y_tan_cell[][TANZAKU_NUM_MAX], double sum_steptime_tan_cell[][TANZAKU_NUM_MAX], vector< deque<double> >& tan_x_buf, vector< deque<double> >& tan_y_buf, vector< deque<double> >& tan_time_buf);
void set_in_the_areaE_flag(bool in_the_areaE_flag[], vector< deque<double> >& tan_x_buf, vector< deque<double> >& tan_y_buf);
void least_square(vector< deque<double> >& tan_time_buf, vector< deque<double> >& G_data_buf, vector< deque<double> >& v);
void cal_w(double tan_wn[], vector< deque<double> >& G_data_buf, int tan_approach_cnt[], vector< deque<double> >& tan_w, deque<double>& sum_w);
void clear_buf(vector< deque<double> >& G_data_buf, int tan_approach_cnt[]);
void judge_open_mode_tan(int tan_approach_cnt[], int frame_arrival[], deque<double>& sum_w, int open_mode_tan[]);
//log_ctr.cpp
void initialize_open_log();
void write_open_log(char data_det, double data_x[], double data_y[], double data_z[], int open_mode, int open_mode_tan[], vector< deque<double> >& tan_x_buf, vector< deque<double> >& v, int tan_approach_cnt[], double scantime);
//lane.cpp
void set_entry_lane_num(vector< deque<double> >& tan_x_buf, vector< deque<double> >& tan_y_buf, bool in_the_areaE_flag[], int entry_lane_num[]);
void set_on_the_lane_flag(vector< deque<double> >& tan_x_buf, bool on_the_lane_flag[]);
void upd_cancel_lane_flag(vector< deque<double> >& tan_x_buf, int entry_lane_num[], bool cancel_lane_flag[]);
void upd_cancel_flag(vector< deque<double> >& tan_x_buf, bool in_the_areaE_flag[], bool cancel_lane_flag[], bool cancel_flag[]);
//open_mode.cpp
void judge_open_mode(int open_mode_tan[], bool cancel_flag[], bool B_flag, int& open_mode);

//-グローバル変数-//
//ACF判定用
extern double step_dist_dif[STEP_NUM];

//B判定用//
extern bool B_flag;
extern int stop_cnt;

//-スポット情報-//
extern int spotinfo_CF[30][44];
extern int spotinfo_D[30][44];
extern int spotinfo_E[30][44];
extern int spotinfo_GHI[30][44];
extern int spotinfo_B[30][44];

//ファイル読み込み用
extern double a[BORDER_NUM_MAX];
extern double b[BORDER_NUM_MAX];

//tan用
extern double px0[TANZAKU_NUM_MAX];
extern double py0[TANZAKU_NUM_MAX];

extern double tan_wn[TANZAKU_NUM_MAX]; //位置データが1mのときの短冊の幅
extern double w1[TANZAKU_NUM_MAX];
extern double tan_w[TANZAKU_NUM_MAX]; //短冊の幅

extern double X_MAX; //センサの観測領域の限界（x軸）//frame_observeが変数のとき

//-タイムゾーン設定用-//
extern double steptime[STEP_NUM];

//-開閉判定用-//
extern int open_mode_door;

// edge
extern bool in_the_areaE_flag[TANZAKU_NUM_MAX];
extern int entry_lane_num[TANZAKU_NUM_MAX]; //trueのとき開信号無効、falseのとき開信号有効
extern bool on_the_lane_flag[LANE_NUM_MAX];
extern bool cancel_lane_flag[LANE_NUM_MAX];
extern bool cancel_flag[TANZAKU_NUM_MAX];
