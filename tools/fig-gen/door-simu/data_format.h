#include <GL/glut.h>
#include <vector>
#include <deque>

// #include "door_simu.h"

using namespace std;

// typedef struct {
// 	deque<char> det(BUFFER_LENGTH);

// 	vector< deque<GLdouble> > px(STEP_NUM_MAX, deque<GLdouble>(BUFFER_LENGTH));
// 	vector< deque<GLdouble> > py(STEP_NUM_MAX, deque<GLdouble>(BUFFER_LENGTH));
// 	vector< deque<GLdouble> > pz(STEP_NUM_MAX, deque<GLdouble>(BUFFER_LENGTH));

// 	deque<int> open_mode(BUFFER_LENGTH);
// 	vector< deque<int> > open_mode_tan(TANZAKU_NUM, deque<int>(BUFFER_LENGTH));
// 	vector< deque<double> > tan_x(TANZAKU_NUM, deque<double>(BUFFER_LENGTH));
// 	vector< deque<double> > tan_v(TANZAKU_NUM, deque<double>(BUFFER_LENGTH));
// 	vector< deque<int> > tan_approach_cnt(TANZAKU_NUM, deque<int>(BUFFER_LENGTH));
// 	vector< deque<int> > target_lane(LANE_NUM_MAX, deque<int>(BUFFER_LENGTH));
// 	vector< deque<int> > cancel_by_lane(TANZAKU_NUM, deque<int>(BUFFER_LENGTH));
// 	vector< deque<int> > laneR_cross_cnt(LANE_NUM_MAX, deque<int>(BUFFER_LENGTH));
// 	vector< deque<int> > laneL_cross_cnt(TANZAKU_NUM, deque<int>(BUFFER_LENGTH));
// }detection_log;

struct test{
	vector<int> vector_test;
};