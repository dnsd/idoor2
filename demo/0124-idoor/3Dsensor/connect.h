#include <vector>
#include <deque>

using namespace std;

// センサスペック
#define STEP_NUM 2720
#define FREQ 0.05

// バッファ関連
const int BUFFER_LENGTH = 10;
const int CUR_INDEX = BUFFER_LENGTH-1;
const int PRE_INDEX = BUFFER_LENGTH-2;
const int PREPRE_INDEX = BUFFER_LENGTH-3;

// 監視エリア設定
const double AREA_START_Y = -1800.0;
const double AREA_END_Y = 1800.0;
const double AREA_START_X = 0.0;
const double AREA_END_X = 2600.0;
const double AREA_START_Z = -300.0;
const double AREA_END_Z = 2600.0;
const double SURVEILLANCE_R = 5000.0;

// 物体検知
const double MOVING_TH_MIN = 30; // mm
const double MOVING_TH_MAX = 200; // mm
const double BACKGROUND_DIFF_TH = 500; //mm
const int DETECTION_OBJECT_TH = 10; // しきい値以上のステップがエリア内にあるとき、物体が存在するとする。
const int DETECTION_MOVING_OBJECT_TH = 20; // しきい値以上のステップがエリア内にあるとき、動物体が存在するとする。
const double FREQ_GET_BACKGROUND = 10; // 秒

// ログ取得
const int LOGGING_TIME = 10000000;

typedef struct{
    char det; //UorD
    double dist[STEP_NUM];
    double x[STEP_NUM];
    double y[STEP_NUM];
    double z[STEP_NUM];
}LS3D;

typedef struct{
    bool hasObjects;
    bool hasMovingObjects;
}AREA_INFO;

class Step_buf
{
    public:
    	deque<char> det;
        vector< deque<double> > dist;
        vector< deque<double> > x;
        vector< deque<double> > y;
        vector< deque<double> > z;

        void bufsort();
        bool isInTheArea(int step_num);
        bool isMoving(int step_num);
        bool isStop(int step_num);
        void out_csv();
        void set_data(char read_data_det, double read_data_dist[], double read_data_x[], double read_data_y[], double read_data_z[]);

        Step_buf(){
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
        Step_buf(const Step_buf& origin){
            det = origin.det;
            dist = origin.dist;
            x = origin.x;
            y = origin.y;
            z = origin.z;
        }
};

// mystd.cpp
void CountDown(int count);
double get_time(void);
void sleep_const_freq(double cycle_start_time, double cycle_end_time, double freq);
string get_localtime();