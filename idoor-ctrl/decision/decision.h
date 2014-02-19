#include "myconst.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

class Area;
class Tanzaku;
struct Cell;
class Step;
class Lane;

class Area
{
    public:
        double sx1, sx2, sy1, sy2, sz1, sz2; // "s"etdata
        int step_num_cnt_th;
        int buf_num_cnt_th;
        int buf_length_has_objects;
        deque<int> hasObjects_buf;

        void defineCuboid(double x1, double x2, double y1, double y2, double z1, double z2);
        bool hasObjects(Step& readdata);
        int judgeOpen(Step& readdata);
        void set_step_num_cnt_th(int parameter);
        void set_buf_num_cnt_th(int parameter);
        void set_buf_length_has_objects(int parameter);
    Area(){
        sx1 = 0.0;
        sx2 = 1000.0;
        sy1 = -1000.0;
        sy2 = 1000.0;
        sz1 = 0.0;
        sz2 = 2000.0;
        step_num_cnt_th = AREA_C_STEP_NUM_TH; // エリアの中に一定数以上scanpointがあれば物体が存在するとみなす
        buf_num_cnt_th = BUF_NUM_HAS_OBJECTS; // バッファのうち物体が存在したフレームが一定数以上あればドアを開ける
        buf_length_has_objects = BUF_LENGTH_HAS_OBJECTS; //バッファの長さ
        hasObjects_buf.resize(buf_length_has_objects);
    }

};

struct Cell
{
        int step_num[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップの数
        double sum_x[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのx座標値の合計
        double sum_y[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのy座標値の合計
        double sum_steptime[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのスキャン時刻の総和
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

typedef struct{
    char det; //UorD
    double dist[STEP_NUM];
    double x[STEP_NUM];
    double y[STEP_NUM];
    double z[STEP_NUM];
}LS3D;

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
        int judgeOpen(Lane& lane);

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

typedef struct{
    double a[BORDER_NUM_MAX];
    double b[BORDER_NUM_MAX];
    double p0x[TANZAKU_NUM_MAX];
    double p0y[TANZAKU_NUM_MAX];
    double wn[TANZAKU_NUM_MAX];//位置データ(tan_pos or tan_x)が1mのときの短冊の幅
}TANZAKU_FAC;

//-関数のプロトタイプ宣言-//
//mystd.cpp
double get_time(void);
//tan.cpp
void allocate_data_to_tanzaku(TANZAKU_FAC& fac, double steptime[], Step& sd, Cell& cell);
void cal_frame_arrival(Tanzaku& tanzaku);
void cal_pos_group_near(Cell& cell, Tanzaku& tanzaku);
void cal_w(TANZAKU_FAC& fac, Tanzaku& tanzaku, deque<double>& sum_w);
void clear_buf(vector< deque<double> >& G_data_buf, int tan_approach_cnt[]);
void least_square(Tanzaku& tanzaku);
void judge_open_mode_tan(Tanzaku& tanzaku, deque<double>& sum_w);
double p_dist(double p0x, double p0y, double p1x, double p1y);
void upd_tan_approach_cnt(Tanzaku& tanzaku);
//log_ctr.cpp
void initialize_open_log();
void read_tan_fac(TANZAKU_FAC& fac);
void read_tan_wn(TANZAKU_FAC& fac);
void write_open_log(Step& sd, Tanzaku& tanzaku, int open_mode, double scantime);
//open.cpp
// void judge_open_mode(Tanzaku& tan, Lane& lane, bool B_flag, int& open_mode);
int judge_open_mode(int vote1, int vote2);

//-タイムゾーン設定用-//
extern double steptime[STEP_NUM];

//-開閉判定用-//
extern int open_mode_door;

// edge
extern bool cancel_flag[TANZAKU_NUM_MAX];
