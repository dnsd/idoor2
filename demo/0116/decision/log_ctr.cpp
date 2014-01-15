// tanzaku_and_edge

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <ssm.hpp>
#include <cmath>
#include "send_spotinfo.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

void initialize_open_log()
{
	//ファイルオープン
	ofstream olog;
	olog.open("open_log");
	//ヘッダーの書き込み
	olog << TANZAKU_NUM_MAX << ",";

	//ファイルのクローズ
	olog << endl;
	olog.close();
}

// 1スキャンのログを複数行に分ける
void write_open_log(char data_det, double data_x[], double data_y[], double data_z[],
					int open_mode, int open_mode_tan[], vector< deque<double> >& G_x_buf, 
					vector< deque<double> >& v, int tan_approach_cnt[], double scantime)
{
	//ファイルオープン
	ofstream olog;
	olog.open("open_log", ios::app);
	olog.precision(16);

	//スキャン方向の書き込み(1)
	olog << data_det << "," << endl;

	//スキャンデータの書き込み(2)
	for (int i = 0; i < STEP_NUM; i++)
	{
		olog << data_x[i] << ",";
	}
	olog << endl;
	for (int i = 0; i < STEP_NUM; i++)
	{
		olog << data_y[i] << ",";
	}
	olog << endl;
	for (int i = 0; i < STEP_NUM; i++)
	{
		olog << data_z[i] << ",";
	}
	olog << endl;

	//開閉判定情報の書き込み（統合）(3)
	olog << open_mode << ",";

	//開閉判定情報の書き込み（短冊）(4)
	for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
	{
		olog << open_mode_tan[tan_num] << ",";
	}

	//短冊ごとのG_x_buf
	for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
	{
		olog << G_x_buf[tan_num][CUR_INDEX] << ",";
	}

	//短冊ごとのG_v_cur
	for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
	{
		olog << v[tan_num][CUR_INDEX] << ",";
	}

	//短冊ごとのapproach_cnt
	for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
	{
		olog << tan_approach_cnt[tan_num] << ",";
	}

	//スキャン時刻
	olog << scantime << ",";

	//書き込み終了
	olog << endl;
	olog.close();

}
