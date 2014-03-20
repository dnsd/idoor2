#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>
#include "myconst.h"

using namespace std;

// ほんとは名前空間をつかうべき？
struct Cell;
struct TANZAKU_FAC;
// class Area;
class Tanzaku;
class Step;
class Lane;

struct Cell
{
        int step_num[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップの数
        double sum_x[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのx座標値の合計
        double sum_y[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのy座標値の合計
        double sum_steptime[TAN_CELL_NUM_MAX][TANZAKU_NUM_MAX]; //セル内にあるステップのスキャン時刻の総和
};

struct TANZAKU_FAC{
    double a[BORDER_NUM_MAX];
    double b[BORDER_NUM_MAX];
    double p0x[TANZAKU_NUM_MAX];
    double p0y[TANZAKU_NUM_MAX];
    double wn[TANZAKU_NUM_MAX];//位置データ(tan_pos or tan_x)が1mのときの短冊の幅
};

// class Area
// {
//     public:
//         double sx1, sx2, sy1, sy2, sz1, sz2; // "s"etdata
//         int step_num_cnt_th;
//         int buf_num_cnt_th;
//         int buf_length_has_objects;
//         deque<int> hasObjects_buf;

//         void defineCuboid(double x1, double x2, double y1, double y2, double z1, double z2);
//         bool hasObjects(Step& readdata);
//         int judgeOpen(Step& readdata);
//         void set_step_num_cnt_th(int parameter);
//         void set_buf_num_cnt_th(int parameter);
//         void set_buf_length_has_objects(int parameter);
//     Area(){
//         sx1 = 0.0;
//         sx2 = 1000.0;
//         sy1 = -1000.0;
//         sy2 = 1000.0;
//         sz1 = 0.0;
//         sz2 = 2000.0;
//         step_num_cnt_th = AREA_C_STEP_NUM_TH; // エリアの中に一定数以上scanpointがあれば物体が存在するとみなす
//         buf_num_cnt_th = BUF_NUM_HAS_OBJECTS; // バッファのうち物体が存在したフレームが一定数以上あればドアを開ける
//         buf_length_has_objects = BUF_LENGTH_HAS_OBJECTS; //バッファの長さ
//         hasObjects_buf.resize(buf_length_has_objects);
//     }

// };

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
        vector<double> steptime;

        void set_data(char& read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[]);
        void init_steptime();
        bool isClose(int step_num);
        bool isStop(int step_num);
        
        Step(){
            // 配列のサイズを変更
            det.resize(BUFFER_LENGTH);
            dist.resize(STEP_NUM);
            x.resize(STEP_NUM);
            y.resize(STEP_NUM);
            z.resize(STEP_NUM);
            steptime.resize(STEP_NUM);
            for (int i = 0; i < STEP_NUM; i++)
            {
                dist[i].resize(BUFFER_LENGTH);
                x[i].resize(BUFFER_LENGTH);
                y[i].resize(BUFFER_LENGTH);
                z[i].resize(BUFFER_LENGTH);
            }

            // 初期化
            init_steptime();
        }
};

class Tanzaku
{
    public:
        int frame_arrival[TANZAKU_NUM_MAX];//何フレーム後にドアを開けるべきか
        int approach_cnt[TANZAKU_NUM_MAX]; //連続して観測すると増えていく

        int open_mode[TANZAKU_NUM_MAX]; //短冊ごとの判定結果

        void calSpeed();
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

        // ビームを短冊に割り当てる（インライン関数）
        void allocateStep(TANZAKU_FAC& fac, Step& sd, Cell& cell)
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

                                    cell.sum_steptime[k][j] += sd.steptime[i];
                                    break;
                                }
                            }
                            break;
                        }
                    } //短冊のループおわり
                }
            } //ステップのループおわり
        }

        // グループ全体の重心を位置データとする(インライン関数)
        void calPos(Cell& cell, Tanzaku& tanzaku)
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
};

//-関数のプロトタイプ宣言-//
//mystd.cpp
double get_time(void);
//tan.cpp
void cal_frame_arrival(Tanzaku& tanzaku);
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
int judge_open_mode(int vote1);
int judge_open_mode(int vote1, int vote2);
int judge_open_mode(int vote1, int vote2, int vote3);
int judge_open_mode(int vote1, int vote2, int vote3, int vote4);

//-タイムゾーン設定用-//
extern double steptime[STEP_NUM];

// edge
extern bool cancel_flag[TANZAKU_NUM_MAX];
