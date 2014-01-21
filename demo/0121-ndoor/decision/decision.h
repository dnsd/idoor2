#include "myconst.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

class Area;
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
        step_num_cnt_th = 20; // エリアの中に一定数以上scanpointがあれば物体が存在するとみなす
        buf_num_cnt_th = 10; // バッファのうち物体が存在したフレームが一定数以上あればドアを開ける
        buf_length_has_objects = 15; //バッファの長さ
        hasObjects_buf.resize(buf_length_has_objects);
    }

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

//-関数のプロトタイプ宣言-//
//mystd.cpp
double get_time(void);
//log_ctr.cpp
void initialize_open_log();
void write_open_log(char data_det, double data_x[], double data_y[], double data_z[], int open_mode, int open_mode_tan[], vector< deque<double> >& tan_x_buf, vector< deque<double> >& v, int tan_approach_cnt[], double scantime);
//open.cpp
int judge_open_mode(int vote1);


//-開閉判定用-//
extern int open_mode_door;

