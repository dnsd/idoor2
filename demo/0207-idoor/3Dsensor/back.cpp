//10Hz用 tanzaku_b

#include <iostream>
#include "LS3D.h"
#include <ssm.hpp>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <time.h>
#include <sys/time.h>

using namespace std;

#define beam_num 2720
#define freq 0.05 //単位は秒

#define diff_min 100 //測定値が背景よりdiff_min以上小さければobjectとする。
#define diff_max 6000

LS3D background_U;
LS3D background_D;
LS3D buf_U[10];
LS3D buf_D[10];
double dist_diff[2720];

//-時間計測用-//
double time_1; //時間計測用
double time_2; //時間計測用
double time_3; //時間計測用

void get_background(LS3D *background_U, LS3D *background_D); //プロトタイプ宣言
double get_time(void); //プロトタイプ宣言

//-SSM-//
SSMApi<LS3D> SCAN_DATA("LS3D", 0);
SSMApi<LS3D> OBJECT("OBJECT", 1);

int main (int argc, char *argv[])
{
    //-SSM-//
    initSSM();
    OBJECT.create(5.0,1.0);
    SCAN_DATA.open(SSM_READ);
    
    get_background(&background_U, &background_D);

    // ofstream ofs;
    // ofs.open("background");
    // for (int i = 0; i < beam_num; i++)
    // {
    //     ofs << background_U.dist[i] << endl;
    // }
    // for (int i = 0; i < beam_num; i++)
    // {
    //     ofs << background_D.dist[i] << endl;
    // }
    // ofs.close();



    cout << "get background" << endl;

    while(1) //データ取得ループ
    {
        if(SCAN_DATA.readNew()) //readNew
        {
            time_1 = get_time();
            if(SCAN_DATA.data.det == 'U')
            {
                OBJECT.data.det = 'U';
                for(int i=0; i<beam_num; i++)
                {
                    dist_diff[i] = fabs(SCAN_DATA.data.dist[i] - background_U.dist[i]);
                    if(dist_diff[i] <= diff_min)
                    {
                        OBJECT.data.dist[i] = 0.0;
                        OBJECT.data.x[i] = 0.0;
                        OBJECT.data.y[i] = 0.0;
                        OBJECT.data.z[i] = 0.0;
                    }else{//(dist_diff[i] > diff_min /*&& dist_diff[i] < diff_max*/){
                        OBJECT.data.dist[i] = SCAN_DATA.data.dist[i];
                        OBJECT.data.x[i] = SCAN_DATA.data.x[i];
                        OBJECT.data.y[i] = SCAN_DATA.data.y[i];
                        OBJECT.data.z[i] = SCAN_DATA.data.z[i];
                    }
                }
            }
            if(SCAN_DATA.data.det == 'D')
            {
                OBJECT.data.det = 'D';
                for(int i=0; i<beam_num; i++)
                {
                    dist_diff[i] = fabs(SCAN_DATA.data.dist[i] - background_D.dist[i]);
                    if(dist_diff[i] <= diff_min )
                    {
                        OBJECT.data.dist[i] = 0.0;
                        OBJECT.data.x[i] = 0.0;
                        OBJECT.data.y[i] = 0.0;
                        OBJECT.data.z[i] = 0.0;
                    }else{//(dist_diff[i] > diff_min /*&& dist_diff[i] < diff_max*/){
                        OBJECT.data.dist[i] = SCAN_DATA.data.dist[i];
                        OBJECT.data.x[i] = SCAN_DATA.data.x[i];
                        OBJECT.data.y[i] = SCAN_DATA.data.y[i];
                        OBJECT.data.z[i] = SCAN_DATA.data.z[i];
                    }
                }
            }
            OBJECT.write();

            time_2 = get_time();
            usleep(freq*1000000 - (time_2 - time_1)*1000000);
            time_3 = get_time();
            cout << "time = " << time_3 - time_1 << endl;

        }else{ //readNew
            usleep(1000); //CPU使用率100％対策
        }
    } //データ取得ループ
    //-SSMクローズ-//
    OBJECT.release();
    SCAN_DATA.close();
    endSSM();
    
    return 0;
}

void get_background(LS3D *background_U, LS3D *background_D)
{
    int cnt_U=0;
    int cnt_D=0;
    
    while(cnt_U < 10 && cnt_D <10)
    {
        if(SCAN_DATA.readNew())
        {
            if(SCAN_DATA.data.det == 'U')
            {
                for(int i=0; i<beam_num; i++)
                {
                    buf_U[cnt_U].dist[i] = SCAN_DATA.data.dist[i];
                    buf_U[cnt_U].det = SCAN_DATA.data.det;
                    buf_U[cnt_U].x[i] = SCAN_DATA.data.x[i];
                    buf_U[cnt_U].y[i] = SCAN_DATA.data.y[i];
                    buf_U[cnt_U].z[i] = SCAN_DATA.data.z[i];
                }
                cnt_U++;
            }
            if(SCAN_DATA.data.det == 'D')
            {
                for(int i=0; i<beam_num; i++)
                {
                    buf_D[cnt_D].dist[i] = SCAN_DATA.data.dist[i];
                    buf_D[cnt_D].det = SCAN_DATA.data.det;
                    buf_D[cnt_D].x[i] = SCAN_DATA.data.x[i];
                    buf_D[cnt_D].y[i] = SCAN_DATA.data.y[i];
                    buf_D[cnt_D].z[i] = SCAN_DATA.data.z[i];
                }
                cnt_D++;
            }

        }
    }

    //-バブルソート-//
    for(int k=0; k<beam_num; k++) //スキャンラインの数だけループ
    {
        double w;
        for(int i=0;i<(10-1);i++){ // 一番小さいデータを配列の右端から詰めていく
            for(int j=0;j<(10-1)-i;j++){// 詰めた分だけ比較する配列要素は減る
                if(buf_U[j].dist[k] < buf_U[j+1].dist[k]){
                    w = buf_U[j].dist[k];
                    buf_U[j].dist[k] = buf_U[j+1].dist[k];
                    buf_U[j+1].dist[k] = w;
                }
            }
        }
    }
    for(int k=0; k<beam_num; k++) //スキャンラインの数だけループ
    {
        double w;
        for(int i=0;i<(10-1);i++){ // 一番小さいデータを配列の右端から詰めていく
            for(int j=0;j<(10-1)-i;j++){// 詰めた分だけ比較する配列要素は減る
                if(buf_D[j].dist[k] < buf_D[j+1].dist[k]){
                    w = buf_D[j].dist[k];
                    buf_D[j].dist[k] = buf_D[j+1].dist[k];
                    buf_D[j+1].dist[k] = w;
                }
            }
        }
    }

    for(int i=0; i<beam_num; i++)
    {
        background_U->dist[i] = buf_U[5].dist[i];
        background_U->x[i] = buf_U[5].x[i];
        background_U->y[i] = buf_U[5].y[i];
        background_U->z[i] = buf_U[5].z[i];

        background_D->dist[i] = buf_D[5].dist[i];
        background_D->x[i] = buf_D[5].x[i];
        background_D->y[i] = buf_D[5].y[i];
        background_D->z[i] = buf_D[5].z[i];
    }

        background_U->det = 'U';
        background_D->det = 'D';
}

double get_time(void) //現在時刻を取得
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}
