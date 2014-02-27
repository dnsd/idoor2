#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include "decision.h"

double beta2angle(long angle)
{
    // センサから取得した角度データを使える形に加工する
    double alpha, beta;
    beta = (double)angle;
    alpha = ((2*beta)/(pow(2, 24)-1)) - 1; // 北陽独自の変換式
    return alpha;
}

void calc_xyz(double angle_x, double angle_y, long dist, double& x, double& y, double& z)
{
    // ビームの距離値と角度からスキャン点の座標を算出
    double tx, ty, tz;
    tx = dist * cos(angle_x) * cos(angle_y);
    ty = dist * sin(angle_x) * cos(angle_y);
    tz = dist * sin(angle_y);
    x = tz * sin(ALPHA) + tx * cos(ALPHA) + ORG_X;
    y = ty + ORG_Y;
    z = tz * cos(ALPHA) - tx * sin(ALPHA) + ORG_Z;
}

void read_beta_data(BEAMANGLE& angle)
{
    // ビームの角度データの読み込み
	FILE *fp;
	fp = fopen("beta_data", "r");

	long beta_ux[20][136];
    long beta_uy[20][136];
    long beta_dx[20][136];
    long beta_dy[20][136];

	for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta_ux[j][i]);
            angle.ux[j][i] = beta2angle(beta_ux[j][i]);
        }
    }
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta_uy[j][i]);
            angle.uy[j][i] = beta2angle(beta_uy[j][i]);
        }
    }
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta_dx[j][i]);
            angle.dx[j][i] = beta2angle(beta_dx[j][i]);
        }
    }
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta_dy[j][i]);
            angle.dy[j][i] = beta2angle(beta_dy[j][i]);
        }
    }
}
