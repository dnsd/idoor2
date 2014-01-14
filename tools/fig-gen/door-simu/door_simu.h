#include <GL/glut.h>
#include <vector>
#include <deque>

using namespace std;

#define POINTSIZE 2
#define BUFFER_LENGTH 60

const double SENSOR_HEIGHT = 3000.0;
const float SCAN_POINT_SIZE = 2;
const int STEP_NUM_MAX = 2720;

const int TANZAKU_NUM = 30;
const int LANE_NUM_MAX = 30;

// ドアスペック
const double DOOR_HEIGHT = 2000.0;
const double DOOR_WIDTH = 1000.0;
const double GATE_WIDTH = 360.0; //片側
const int TIME_REQUIRED = 16; //ドアを開けるのに必要なフレーム数
const int CLOSE_DELAY = 30; //ドアを閉め始めるまでの待ち時間。

// アニメーション用
const int SCAN_CYCLE = 50; //ミリ秒

struct detection_log
{
public:
	deque<char> det;

	vector< deque<GLdouble> > px;
	vector< deque<GLdouble> > py;
	vector< deque<GLdouble> > pz;

	deque<int> open_mode;
	vector< deque<int> > open_mode_tan;
	vector< deque<double> > tan_x;
	vector< deque<double> > tan_v;
	vector< deque<int> > tan_approach_cnt;
	vector< deque<int> > cancel_by_lane;
	vector< deque<int> > target_lane;
	vector< deque<int> > laneL_cross_cnt;
	vector< deque<int> > laneR_cross_cnt;

	deque<double> scantime;

	detection_log(){
		//スキャン方向の書き込み(1)
		det.resize(BUFFER_LENGTH);

		//スキャンデータの書き込み(2)
		px.resize(STEP_NUM_MAX);
		for (int i = 0; i < STEP_NUM_MAX; i++) px[i].resize(BUFFER_LENGTH);
		py.resize(STEP_NUM_MAX);
		for (int i = 0; i < STEP_NUM_MAX; i++) py[i].resize(BUFFER_LENGTH);
		pz.resize(STEP_NUM_MAX);
		for (int i = 0; i < STEP_NUM_MAX; i++) pz[i].resize(BUFFER_LENGTH);

		//開閉判定情報の書き込み（統合）(3)
		open_mode.resize(BUFFER_LENGTH);
		//開閉判定情報の書き込み（短冊）(4)
		open_mode_tan.resize(TANZAKU_NUM);
		for (int i = 0; i < TANZAKU_NUM; ++i) open_mode_tan[i].resize(BUFFER_LENGTH);
		//短冊ごとのG_x_buf
		tan_x.resize(TANZAKU_NUM);
		for (int i = 0; i < TANZAKU_NUM; ++i) tan_x[i].resize(BUFFER_LENGTH);
		//短冊ごとのG_v_cur
		tan_v.resize(TANZAKU_NUM);
		for (int i = 0; i < TANZAKU_NUM; ++i) tan_v[i].resize(BUFFER_LENGTH);
		//短冊ごとのapproach_cnt
		tan_approach_cnt.resize(TANZAKU_NUM);
		for (int i = 0; i < TANZAKU_NUM; ++i) tan_approach_cnt[i].resize(BUFFER_LENGTH);
		//スキャン時刻
		scantime.resize(BUFFER_LENGTH);

		// cancel_by_lane.resize(TANZAKU_NUM);
		// for (int i = 0; i < TANZAKU_NUM; ++i) cancel_by_lane[i].resize(BUFFER_LENGTH);
		// target_lane.resize(LANE_NUM_MAX);
		// for (int i = 0; i < LANE_NUM_MAX; ++i) target_lane[i].resize(BUFFER_LENGTH);
		// laneL_cross_cnt.resize(LANE_NUM_MAX);
		// for (int i = 0; i < LANE_NUM_MAX; ++i) laneL_cross_cnt[i].resize(BUFFER_LENGTH);
		// laneR_cross_cnt.resize(LANE_NUM_MAX);
		// for (int i = 0; i < LANE_NUM_MAX; ++i) laneR_cross_cnt[i].resize(BUFFER_LENGTH);
	}
};
