#include "myconst.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

struct AABB;
struct LS3D;
struct VEC3D;

class Area;
class Tanzaku;
class Cell;
class Step;
class Lane;

struct AABB{
    double min[3];
    double max[3];
};

struct LS3D{
    char det; //UorD
    double dist[STEP_NUM];
    double x[STEP_NUM];
    double y[STEP_NUM];
    double z[STEP_NUM];
};

struct VEC3D{
    double x;
    double y;
    double z;
};

class Area
{
    public:
        bool hasObjects(Step& readdata);
        int judgeOpen(Step& readdata);

        void set_step_cnt_th(int parameter); // これ以上スキャン点があれば物体が存在するとみなす
        void set_buf_length(int parameter); // バッファの長さ
        void set_buf_cnt_th(int parameter); // 物体が存在するフレーム数のしきい値
        
    private:
        int step_cnt_th;
        int buf_cnt_th;
        int buf_length;
        deque<int> hasObjects_buf;
        vector<float> area_th_min_U; // エリアの境界をビームの距離値で表現
        vector<float> area_th_max_U; // エリアの境界をビームの距離値で表現
        vector<float> area_th_min_D; // エリアの境界をビームの距離値で表現
        vector<float> area_th_max_D; // エリアの境界をビームの距離値で表現

    Area(){
        step_cnt_th = 20; // エリアの中に一定数以上scanpointがあれば物体が存在するとみなす
        buf_cnt_th = 10; // バッファのうち物体が存在したフレームが一定数以上あればドアを開ける
        buf_length = 15; //バッファの長さ
        hasObjects_buf.resize(buf_length);
        area_th_min_U.resize(STEP_NUM);
        area_th_max_U.resize(STEP_NUM);
        area_th_min_D.resize(STEP_NUM);
        area_th_max_D.resize(STEP_NUM);
    }
    Area(int para1, int para2, int para3){
        step_cnt_th = para1; // エリアの中に一定数以上scanpointがあれば物体が存在するとみなす
        buf_cnt_th = para2; // バッファのうち物体が存在したフレームが一定数以上あればドアを開ける
        buf_length = para3; //バッファの長さ
        hasObjects_buf.resize(buf_length);
        area_th_min_U.resize(STEP_NUM);
        area_th_max_U.resize(STEP_NUM);
        area_th_min_D.resize(STEP_NUM);
        area_th_max_D.resize(STEP_NUM);
    }

};

class AreaAABB : public Area
{
    public:
        void calAreaTh();
        void defineAABB(double min0, double min1, double min2, double max0, double max1, double max2);
    private:
        double min[3]; // AABBの定義
        double max[3]; // AABBの定義
        double sensor_pos[3]; // センサーの位置
    AreaAABB(){
        min[0] = DEFAULT_AABB_MIN0;
        min[1] = DEFAULT_AABB_MIN1;
        min[2] = DEFAULT_AABB_MIN2;
        max[0] = DEFAULT_AABB_MAX0;
        max[1] = DEFAULT_AABB_MAX1;
        max[2] = DEFAULT_AABB_MAX2;

        sensor_pos[0] = ORG_X;
        sensor_pos[1] = ORG_Y;
        sensor_pos[2] = ORG_Z;
    }
    AreaAABB(double min0, double min1, double min2, double max0, double max1, double max2){
        min[0] = min0;
        min[1] = min1;
        min[2] = min2;
        max[0] = max0;
        max[1] = max1;
        max[2] = max2;

        sensor_pos[0] = ORG_X;
        sensor_pos[1] = ORG_Y;
        sensor_pos[2] = ORG_Z;
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

