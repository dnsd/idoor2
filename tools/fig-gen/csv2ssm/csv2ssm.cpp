#include <iostream>
#include <ssm.hpp>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "LS3D.h"

using namespace std;

//-センサ情報-//
const double freq = 0.05;
const double ORG_X = 0.0;
const double ORG_Y = 0.0;
const double ORG_Z = 0.0;
const double ALPHA = 70.0 / 180.0 * M_PI; //取り付け角

//-プロトタイプ宣言-//
double angle_convert(long angle);
void calc_xyz(double angle_x, double angle_y, long dist, double *x, double *y, double *z);
double get_time(void);

//-SSM-//
SSMApi<LS3D> SCAN_DATA("LS3D", 0);

int main(int argc, char **argv)
{
	//SSM
	initSSM();
	SCAN_DATA.create(5.0, 1.0);

	//-----角度データ読み込み-----//
	FILE *fp; //ファイルポインタ

    //-角度用-//
    long U_x_beta[2720];
    long U_y_beta[2720];
    long D_x_beta[2720];
    long D_y_beta[2720];
    double U_angle_x[2720];
    double U_angle_y[2720];
    double D_angle_x[2720];
    double D_angle_y[2720];

	fp = fopen("beta_data", "r");
	for (int i = 0; i < 2720; i++)
    {
    	fscanf(fp, "%ld", &U_x_beta[i]);
        U_angle_x[i] = angle_convert(U_x_beta[i]);
    }
    for (int i = 0; i < 2720; i++)
    {
    	fscanf(fp, "%ld", &U_y_beta[i]);
        U_angle_y[i] = angle_convert(U_y_beta[i]);
    }
    for (int i = 0; i < 2720; i++)
    {
    	fscanf(fp, "%ld", &D_x_beta[i]);
        D_angle_x[i] = angle_convert(D_x_beta[i]);
    }
    for (int i = 0; i < 2720; i++)
    {
    	fscanf(fp, "%ld", &D_y_beta[i]);
        D_angle_y[i] = angle_convert(D_y_beta[i]);
    }

	//-----csvファイル読み込み&コンバート-----//
	string str; //?
	char line[6000]; //ラインデータ
	int count = 0; //?

	//-時間計測用-//
    double time_1; //時間計測用
    double time_2; //時間計測用
    double time_3; //時間計測用

	ifstream ifs;
	ifs.open(argv[1]);

	cout << argv[1] << endl;

	ifs.getline(line, sizeof(line)); //一行読み捨て
	while(ifs.getline(line, sizeof(line)))
	{
		time_1 = get_time();

		char *tp;
		int index = 0; //何回目のstrか?
		char dir = 'U';
		int line_num = 0;
		int step_num = 0;
		int step_data[136];

		tp = strtok(line, ","); //一度目のstrtokには分解対象を指定
		// cout << index << ":" << atoi(tp) << endl;
		index++;

		// cout << *tp << "///";
		// puts(tp);
		// cout << "///";
		// for (int i = 0; i < 50; i++)
		// {
		// 	cout << line[i];
		// }
		// cout << endl;

		while(tp = strtok(NULL, ","))
		{

			if (index == 3)
			{
				dir = *tp;
				// puts(tp);
			} else if (index == 4)
			{
				line_num = atoi(tp); //line_numの更新
				// puts(tp);
			} else if (index == (step_num * 5 + 6))
			{

				step_data[step_num] = atoi(tp);
				SCAN_DATA.data.dist[(136*line_num)+step_num] = step_data[step_num];
				step_num++;

			}
			index++;
			// cout << index << ":" << tp << endl;
		}

		if (line_num == 19) //1スキャンが終わったら
		{

			//-xyz座標値に変換-//
			if (dir == 'U')
			{
				SCAN_DATA.data.det = 'U';
				for (int i = 0; i < 2720; i++)
				{
					calc_xyz(U_angle_x[i], U_angle_y[i], SCAN_DATA.data.dist[i], &SCAN_DATA.data.x[i], &SCAN_DATA.data.y[i], &SCAN_DATA.data.z[i]);
				}

			}else{
				SCAN_DATA.data.det = 'D';
				for (int i = 0; i < 2720; i++)
				{
					calc_xyz(D_angle_x[i], D_angle_y[i], SCAN_DATA.data.dist[i], &SCAN_DATA.data.x[i], &SCAN_DATA.data.y[i], &SCAN_DATA.data.z[i]);
				}
			}

			//-書き込み予定時刻まで待機-//
			time_2 = get_time();
			usleep(freq*1000000 - (time_2 - time_1)*1000000);
			time_3 = get_time();
			cout << "time = " << time_3 - time_1 << endl;

			//-SSMへの書き込み-//
			SCAN_DATA.write();
		}

		// count++;
		// cout << count << endl;
	}

	SCAN_DATA.release();
	endSSM();

	cout << "complete!" << endl;

	return 0;
}

double angle_convert(long angle)
{
    double alpha, beta;
    beta = (double)angle;
    alpha = ((2*beta)/(pow(2, 24)-1)) - 1;
    return alpha;
}

void calc_xyz(double angle_x, double angle_y, long dist, double *x, double *y, double *z)
{
    double tx, ty, tz;
    tx = dist * cos(angle_x) * cos(angle_y);
    ty = dist * sin(angle_x) * cos(angle_y);
    tz = dist * sin(angle_y);
    *x = tz * sin(ALPHA) + tx * cos(ALPHA) + ORG_X;
    *y = ty + ORG_Y;
    *z = tz * cos(ALPHA) - tx * sin(ALPHA) + ORG_Z;
}

double get_time(void) //現在時刻を取得
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}