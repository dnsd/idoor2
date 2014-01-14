#include <iostream>
#include <cmath>
#include <ssm.hpp>
#include <cstdio>
#include "LRF.h"

#define GP_PATH "/usr/bin/gnuplot"

FILE *gnu_p;

void GNU_Init(void)
{
	// Gnuplotのオープン
	gnu_p = popen(GP_PATH, "w");
	fprintf(gnu_p, "set xrange [-5000:5000]\n");
	fprintf(gnu_p, "set xlabel \"x[mm]\"\n");
	fprintf(gnu_p, "set yrange [-5000:5000]\n");
	fprintf(gnu_p, "set ylabel \"y[mm]\"\n");
	fprintf(gnu_p, "set grid\n");
	fprintf(gnu_p, "set size square\n");
	fprintf(gnu_p, "unset key\n");
}

void GNU_Plot(LRF *urg_data)
{
	int i;

	fprintf(gnu_p, "plot \"-\"\n");	// バッファ出力開始

	// LRFの二次元座標データをバッファに出力
	for (i = 0; i < 1081; i++)
	{
		fprintf(gnu_p, "%lf %lf\n", urg_data->x[i], urg_data->y[i]);
	}

	// fprintf(gnu_p, "%lf %lf\n", urg_data->x[0], urg_data->y[0]);
	// fprintf(gnu_p, "%lf %lf\n", urg_data->x[1060], urg_data->y[1060]);

	fprintf(gnu_p, "e\n");	// バッファ出力終了
	fflush(gnu_p);			// Gnuplotに描画させる
}

void GNU_Close(void)
{	
	fclose(gnu_p);
}

int main(int aArgc, char **appArgv)
{
	initSSM();	//SSMイニシャライズ

	LRF urg_data;	// LRFの座標データ

	GNU_Init();	// Gnuplotの初期化
	
	SSM_sid URG_sid; //SSM管理id
	double time; //SSM書き込み時間

	URG_sid = openSSM("Top-URG",0,0);	//SSMを開く（”名前”，番号，時間）

	while(1)
	{
		if(readSSM(URG_sid, (char*)&urg_data, &time,-1)){	//読みにいく（if文はエラー処理）
			GNU_Plot(&urg_data);	// Gnuplotに描画
			// usleep(200000); //0.2秒待機
			usleep(100000); //0.1秒待機
		}
	}
	
	GNU_Close();	// Gnuplotのクローズ
    return 0;
}