#include <vector>
#include <deque>

using namespace std;

// センサスペック
const double FREQ = 0.05;
const int STEP_NUM = 1081;

// バッファ関連
const int BUFFER_LENGTH = 10;
const int CUR_INDEX = BUFFER_LENGTH-1;
const int PRE_INDEX = BUFFER_LENGTH-2;
const int PREPRE_INDEX = BUFFER_LENGTH-3;

// 監視エリア設定
const double AREA_START_Y = -1000.0;
const double AREA_END_Y = 1000.0;
const double AREA_START_X = 1000.0;
const double AREA_END_X = 2000.0;
const double SURVEILLANCE_R = 3000.0;

// 物体検知
const int MOVING_TH_MIN = 30; // mm
const int MOVING_TH_MAX = 200; // mm
const int BACKGROUND_DIFF_TH = 500; //mm
const int DETECTION_OBJECT_TH = 10; // しきい値以上のステップがエリア内にあるとき、物体が存在するとする。
const int DETECTION_MOVING_OBJECT_TH = 20; // しきい値以上のステップがエリア内にあるとき、動物体が存在するとする。
const double FREQ_GET_BACKGROUND = 10; // 秒

// ログ取得
const int LOGGING_TIME = 10000000;

class Step_buf;

typedef struct{
    double dist[STEP_NUM];
    double x[STEP_NUM];
    double y[STEP_NUM];
}LRF;

typedef struct{
    int order;
}ORDER;

typedef struct{
    bool hasObjects;
    bool hasMovingObjects;
}AREA_INFO;

typedef struct{
    bool door_status;
}DOOR_STATUS;

class Step_buf
{
    public:
        vector< deque<double> > dist;
        vector< deque<double> > x;
        vector< deque<double> > y;

        void bufsort();
        bool isInTheArea(int step_num);
        bool isMoving(int step_num);
        bool isStop(int step_num);
        void out_csv();
        void set_data(double read_data_dist[], double read_data_x[], double read_data_y[]);

        Step_buf(){
            dist.resize(STEP_NUM);
            x.resize(STEP_NUM);
            y.resize(STEP_NUM);
            for (int i = 0; i < STEP_NUM; i++)
            {
                dist[i].resize(BUFFER_LENGTH);
                x[i].resize(BUFFER_LENGTH);
                y[i].resize(BUFFER_LENGTH);
            }
        }
        Step_buf(const Step_buf& origin){
            dist = origin.dist;
            x = origin.x;
            y = origin.y;
        }
};

// mystd.cpp
void CountDown(int count);
double get_time(void);
void sleep_const_freq(double cycle_start_time, double cycle_end_time, double freq);
string get_localtime();
