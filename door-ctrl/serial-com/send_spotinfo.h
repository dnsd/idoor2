//10Hz用
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
    double pos[TANZAKU_NUM];
    double x[TANZAKU_NUM];
    double y[TANZAKU_NUM];
    double v[TANZAKU_NUM];
    double w[TANZAKU_NUM];
}TANZAKU_DATA;

typedef struct{
	int open_mode;
}ORDER_DATA;

typedef struct{
    int spotinfo[30][44];
}SPOTINFO;

//-関数のプロトタイプ宣言-//
double get_time(void);
void judge_B(LS3D *OBJECT, int spotinfo_B[][44]);
void judge_CF(LS3D *OBJECT, double beam_U_dist_pre[], double beam_U_dist_dif[], double beam_D_dist_pre[], double beam_D_dist_dif[], double background_U[], double background_D[], int spotinfo_CF[][44]);
void judge_D(int spotinfo_CF[][44], int spotinfo_D[][44], int *open_mode);
void judge_GHI(LS3D *OBJECT, int spotinfo_GHI[][44]);

double p_dif(double px0, double py0, double G_x, double G_y);
void least_square_steptime(int filter_range[], vector< deque<double> >& G_time_buf, vector< deque<double> >& G_pos_buf, double v[]);
void set_filter_range(int filter_range[], vector< deque<double> >& G_pos_buf);
void cal_w(double G_wn[], vector< deque<double> >& G_pos_buf, double G_w[]);
void cal_G_pos_group_near (int his[][TANZAKU_NUM], double sum_x_his[][TANZAKU_NUM], double sum_y_his[][TANZAKU_NUM], double G_x_group_near[], double G_y_group_near[], double G_pos_group_near[]);
void cal_G_data_group_near (int his[][TANZAKU_NUM], double sum_x_his[][TANZAKU_NUM], double sum_y_his[][TANZAKU_NUM], double sum_steptime_his[][TANZAKU_NUM], double G_x_group_near[], double G_y_group_near[], double G_pos_group_near[], double G_time_group_near[]);
void cal_G_data_group_near_test (int his[][TANZAKU_NUM], double sum_x_his[][TANZAKU_NUM], double sum_y_his[][TANZAKU_NUM], double sum_steptime_his[][TANZAKU_NUM], double G_x_group_near[], double G_y_group_near[], double G_pos_group_near[], double G_time_group_near[]);
void clear_buf(vector< deque<double> >& G_pos_buf, int frame_observe[], int approach[]);

void set_data(double TANZAKU_pos[], double TANZAKU_x[], double TANZAKU_y[], double TANZAKU_v[], double TANZAKU_w[], double pos[], double x[], double y[], double v[], double w[]);
void cal_frame_arrival(double pos[], double v[], int frame_arrival[]);
void judge_approach(vector< deque<double> >& G_pos_buf, int approach[]);
void set_frame_observe(vector< deque<double> >& G_pos_buf, int frame_observe[]); //グラデーション
void judge_cross(double G_x[], int cross_histgram[], int cross[]);
void judge_open_mode(int approach[], int frame_observe[], int frame_arrival[], double G_w[], int open_mode_tanzaku[], int *open_mode);
void set_buffer(int buffer_length, double buffer[][TANZAKU_NUM], double data[]);

//-グローバル変数-//
//ACF判定用
extern double beam_U_dist_cur[STEP_NUM];
extern double beam_U_dist_pre[STEP_NUM];
extern double beam_U_dist_dif[STEP_NUM];
extern double beam_D_dist_cur[STEP_NUM];
extern double beam_D_dist_pre[STEP_NUM];
extern double beam_D_dist_dif[STEP_NUM];

//B判定用//
extern int stop_cnt[spot_num_x][spot_num_y]; //静止判定の根拠になるカウンター

//-スポット情報-//
extern int spotinfo_CF[30][44];
extern int spotinfo_D[30][44];
extern int spotinfo_E[30][44];
extern int spotinfo_GHI[30][44];
extern int spotinfo_B[30][44];

//ファイル読み込み用
extern double background_U[STEP_NUM];
extern double background_D[STEP_NUM];
extern double a[TILTED_LINE_NUM];
extern double b[TILTED_LINE_NUM];

//tanzaku用
extern double px0[TANZAKU_NUM];
extern double py0[TANZAKU_NUM];

extern int his_U[his_num][TANZAKU_NUM];
extern int his_U_peak[TANZAKU_NUM]; //ピークの位置（何番目のグリッドか）
extern int his_D[his_num][TANZAKU_NUM];
extern int his_D_peak[TANZAKU_NUM]; //ピークの位置（何番目のグリッドか）

extern double sum_pos_his_U[his_num][TANZAKU_NUM]; //各グリッド中のスキャン点の位置データの和
extern double sum_x_his_U[his_num][TANZAKU_NUM];
extern double sum_y_his_U[his_num][TANZAKU_NUM];
extern double G_pos_U_cur[TANZAKU_NUM];
extern double G_x_U_cur[TANZAKU_NUM];
extern double G_y_U_cur[TANZAKU_NUM];
extern double sum_pos_his_D[his_num][TANZAKU_NUM]; //各グリッド中のスキャン点の位置データの和
extern double sum_x_his_D[his_num][TANZAKU_NUM];
extern double sum_y_his_D[his_num][TANZAKU_NUM];
extern double G_pos_D_cur[TANZAKU_NUM];
extern double G_x_D_cur[TANZAKU_NUM];
extern double G_y_D_cur[TANZAKU_NUM];
extern double G_x[TANZAKU_NUM];
extern double G_y[TANZAKU_NUM];

extern double pos_beam_U[STEP_NUM]; //位置データ（2次元）
extern double pos_beam_D[STEP_NUM]; //位置データ（2次元）

extern double G_buffer[G_BUFFER_LENGTH][TANZAKU_NUM];
extern double G_y_buffer[G_BUFFER_LENGTH][TANZAKU_NUM];
extern double G_x_buffer[G_BUFFER_LENGTH][TANZAKU_NUM];
extern double G_v_cur[TANZAKU_NUM];

extern double G_vx[TANZAKU_NUM];
extern double G_vy[TANZAKU_NUM];

extern int ls_filter_range[TANZAKU_NUM]; //フィルタの範囲

extern double v_buffer[G_BUFFER_LENGTH][TANZAKU_NUM]; //速度のバッファ

extern double G_wn[TANZAKU_NUM]; //位置データが1mのときの短冊の幅
extern double w1[TANZAKU_NUM];
extern double G_w[TANZAKU_NUM]; //短冊の幅

//-谷検出関連-//
extern double sum_pos_group_near[TANZAKU_NUM];
extern int sum_point_group_near[TANZAKU_NUM];
extern double G_pos_group_near[TANZAKU_NUM];

//-タイムゾーン設定用-//
extern double timezone_line[14]; //タイムゾーンの境界線

extern double steptime_U[STEP_NUM];
extern double steptime_D[STEP_NUM];
extern double sum_steptime_his_U[his_num][TANZAKU_NUM];
extern double sum_steptime_his_D[his_num][TANZAKU_NUM];
extern double G_time_U_cur[TANZAKU_NUM];
extern double G_time_D_cur[TANZAKU_NUM];
extern double G_time_buffer[G_BUFFER_LENGTH][TANZAKU_NUM];

extern double tl_a[14]; //時間帯の境界線の傾き
extern double tl_b[14]; //時間帯の境界線の切片

//-開閉判定用-//
extern double pos[TANZAKU_NUM];
extern double x[TANZAKU_NUM];
extern double y[TANZAKU_NUM];
extern double v[TANZAKU_NUM];
extern double w[TANZAKU_NUM];

extern double x_buffer[G_BUFFER_LENGTH][TANZAKU_NUM];
extern double y_buffer[G_BUFFER_LENGTH][TANZAKU_NUM];
extern double v_x[TANZAKU_NUM];
extern double v_y[TANZAKU_NUM];
extern int filter_range_x[TANZAKU_NUM];
extern int filter_range_y[TANZAKU_NUM];

extern int frame_arrival[TANZAKU_NUM]; //何フレーム後にドアを開けるべきか
extern int waiting_time[TANZAKU_NUM]; //ドアを開けるまでの時間

extern bool open[TANZAKU_NUM]; //ドアを開く命令
extern int approach[TANZAKU_NUM]; //連続して観測すると増えていく
extern int cross[LANE_NUM]; //連続して観測すると増えていく
extern int cross_histgram[LANE_NUM];
extern int frame_observe[TANZAKU_NUM];

extern int open_mode;
extern int open_mode_tanzaku[TANZAKU_NUM];