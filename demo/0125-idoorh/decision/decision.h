#include "myconst.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

class Tanzaku;
class Cell;
class Step;
class Lane;

typedef struct{
    char det; //UorD
    double dist[STEP_NUM];
    double x[STEP_NUM];
    double y[STEP_NUM];
    double z[STEP_NUM];
}LS3D;

typedef struct{
    double a[BORDER_NUM_MAX];
    double b[BORDER_NUM_MAX];
    double p0x[TANZAKU_NUM_MAX];
    double p0y[TANZAKU_NUM_MAX];
    double wn[TANZAKU_NUM_MAX];//位置データ(tan_pos or tan_x)が1mのときの短冊の幅
}TANZAKU_FAC;

class Tanzaku
{
    public:
        int frame_arrival[TANZAKU_NUM_MAX];//何フレーム後にドアを開けるべきか
        int approach_cnt[TANZAKU_NUM_MAX]; //連続して観測すると増えていく

        int open_mode[TANZAKU_NUM_MAX]; //短冊ごとの判定結果

        bool isInSurveillanceArea(int tan_num, int index);
        bool isInInnerArea(int tan_num, int index);
        bool isInDetectionArea(int tan_num, int index);
        bool isCancel(Lane& lane, int tan_num);

        vector< deque<double> > x;
        vector< deque<double> > y;
        vector< deque<double> > v;
        vector< deque<double> > w;
        vector< deque<double> > scan_time;

        Tanzaku(){
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

class Cell
{
    public:
        int step_num[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップの数
        double sum_x[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのx座標値の合計
        double sum_y[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのy座標値の合計
        double sum_steptime[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのスキャン時刻の総和
};

typedef struct{
    int order;
}ORDER;

class Step
{
    public:
        deque<char> det;
        vector< deque<double> > dist;
        vector< deque<double> > x;
        vector< deque<double> > y;
        vector< deque<double> > z;

        void set_data(char& read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[]);
        bool isClose(int step_num);
        bool isStop(int step_num);
        
        Step(){
            det.resize(BUFFER_LENGTH);
            dist.resize(STEP_NUM);
            x.resize(STEP_NUM);
            y.resize(STEP_NUM);
            z.resize(STEP_NUM);
            for (int i = 0; i < STEP_NUM; i++)
            {
                dist[i].resize(BUFFER_LENGTH);
                x[i].resize(BUFFER_LENGTH);
                y[i].resize(BUFFER_LENGTH);
                z[i].resize(BUFFER_LENGTH);
            }
        }
};

class Lane
{
    private:
        int pending_cnt[LANE_NUM_MAX];
    public:
        bool isPending(int lane_num);
        void upd_pending_cnt(Tanzaku& tanzaku);

        Lane(){
            for (int i = 0; i < LANE_NUM_MAX; i++)
            {
                pending_cnt[i] = 0;
            }
        }
};

//-関数のプロトタイプ宣言-//
//mystd.cpp
double get_time(void);
//E.cpp
void upd_tan_approach_cnt(Tanzaku& tanzaku);
void cal_frame_arrival(Tanzaku& tanzaku);
//tan.cpp
void allocate_data_to_tanzaku(TANZAKU_FAC& fac, double steptime[], Step& sd, Cell& cell);
void cal_pos_group_near(Cell& cell, Tanzaku& tanzaku);
void cal_w(TANZAKU_FAC& fac, Tanzaku& tanzaku, deque<double>& sum_w);
void clear_buf(vector< deque<double> >& G_data_buf, int tan_approach_cnt[]);
void least_square(Tanzaku& tanzaku);
void judge_open_mode_tan(Tanzaku& tanzaku, deque<double>& sum_w);
double p_dist(double p0x, double p0y, double p1x, double p1y);
//log_ctr.cpp
void initialize_open_log();
void read_tan_fac(TANZAKU_FAC& fac);
void read_tan_wn(TANZAKU_FAC& fac);
void write_open_log(char data_det, double data_x[], double data_y[], double data_z[], int open_mode, int open_mode_tan[], vector< deque<double> >& tan_x_buf, vector< deque<double> >& v, int tan_approach_cnt[], double scantime);
//open.cpp
void judge_open_mode(Tanzaku& tan, Lane& lane, bool B_flag, int& open_mode);

//-グローバル変数-//
//ファイル読み込み用
extern double a[BORDER_NUM_MAX];
extern double b[BORDER_NUM_MAX];

//tan用
extern double p0x[TANZAKU_NUM_MAX];
extern double p0y[TANZAKU_NUM_MAX];

extern double tan_wn[TANZAKU_NUM_MAX]; //位置データが1mのときの短冊の幅
extern double w1[TANZAKU_NUM_MAX];
extern double tan_w[TANZAKU_NUM_MAX]; //短冊の幅

// extern double X_MAX; //センサの観測領域の限界（x軸）//frame_observeが変数のとき

//-タイムゾーン設定用-//
extern double steptime[STEP_NUM];

//-開閉判定用-//
extern int open_mode_door;

// edge
extern bool cancel_flag[TANZAKU_NUM_MAX];
