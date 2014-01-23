//10Hz用

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <ssm.h>
#include <math.h>
#include <signal.h>
#include "LS3D.h"

#define BUFFER_SIZE 23599
#define freq 0.05

#define ORG_X 250.0
#define ORG_Y 0.0
#define ORG_Z 2520.0
// #define ALPHA (1.0 / 2.0 * M_PI) - 1.0 / 9.0 * M_PI //取り付け角(5Hz)
#define ALPHA 61.0 / 180.0 * M_PI //取り付け角(10Hz)

int main ()
{
    struct sockaddr_in dstAddr;
    int dstSock;
    char c;
    char raw_start_buf[26];
    char raw_line_buf[20][1113];
    char raw_end_buf[33];
    char *line_buf[22];
    char str_buf[5];
    char det;
    long dist[20][137];
    int n;
    int numrcv;
    char *toSendText1 = "#TD2GET\n";
    char *toSendText2 = "#TD2STOP\n";

    FILE *fp; //ファイルポインタ

    int i,j,k;

    //-角度用-//
    long U_x_beta[20][136];
    long U_y_beta[20][136];
    long D_x_beta[20][136];
    long D_y_beta[20][136];
    double U_angle_x[20][136];
    double U_angle_y[20][136];
    double D_angle_x[20][136];
    double D_angle_y[20][136];

    //-時間計測用-//
    double time_1; //時間計測用
    double time_2; //時間計測用
    double time_3; //時間計測用

    //-座標値-//
    double x;

    double angle_convert(long angle); //プロトタイプ宣言
    void calc_xyz(double angle_x, double angle_y, long dist, double *x, double *y, double *z); //プロトタイプ宣言
    double get_time(void); //プロトタイプ宣言

    //-SSM-//
    LS3D SCAN_DATA;
    initSSM();
    SSM_sid LS3D_sid;

    LS3D_sid = createSSM_time("LS3D", 0, sizeof(LS3D), 1.0, 0.025);

    //-SCAN_DATAの初期化-//
    // for(i=0; i<2880; i++)
    for(i=0; i<STEP_NUM; i++)
    {
        SCAN_DATA.det = 'U';
        SCAN_DATA.dist[i] = 0.0;
        SCAN_DATA.x[i] = 0.0;
        SCAN_DATA.y[i] = 0.0;
        SCAN_DATA.z[i] = 0.0;
    }

    //-----------角度データ読み込み-----------//
    //-ファイルオープン-//
    fp = fopen("beta_data", "r");
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &U_x_beta[j][i]);
            U_angle_x[j][i] = angle_convert(U_x_beta[j][i]);
        }
    }
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &U_y_beta[j][i]);
            U_angle_y[j][i] = angle_convert(U_y_beta[j][i]);
        }
    }
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &D_x_beta[j][i]);
            D_angle_x[j][i] = angle_convert(D_x_beta[j][i]);
        }
    }
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &D_y_beta[j][i]);
            D_angle_y[j][i] = angle_convert(D_y_beta[j][i]);
        }
    }

    //-----------距離データ取得ループ-----------//
    //-ソケットの作成-//
    dstSock = socket(AF_INET, SOCK_STREAM, 0);

    //-接続先指定用構造体の準備-//
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(10940);
    dstAddr.sin_addr.s_addr = inet_addr("192.168.0.10");

    //-コネクト-//
    connect(dstSock, (struct sockaddr *)&dstAddr, sizeof(dstAddr));

    //-サーバにメッセージを送信-//
    memset(&dstAddr, 0, sizeof(dstAddr));
    send(dstSock, toSendText1, strlen(toSendText1)+1, 0);
    sleep(1);
    printf("sending...\n");

    //-サーバからデータを受信-//
    memset(raw_start_buf, 0, sizeof(raw_start_buf));
    memset(raw_line_buf, 0, sizeof(raw_line_buf));
    memset(raw_end_buf, 0, sizeof(raw_end_buf));

    while(1){ //距離データ取得ループ
        //time_1 = get_time();

        for(i=0; i<26; i++)
        {
            numrcv = recv(dstSock, &c, 1, 0);
            if(c=='\n')
            {
                raw_start_buf[i] = '\0';
                break;
            }
            raw_start_buf[i] = c;
            det = raw_start_buf[19]; //ループの外だとなぜか文字が消える
        }

        for(j=0; j<20; j++)
        {
            for(i=0; i<1177; i++)
            {
                numrcv = recv(dstSock, &c, 1, 0);
                if(c=='\n'){
                    raw_line_buf[j][i] = '\0';
                    break;
                }
                raw_line_buf[j][i] = c;
            }
        }

        for(i=0; i<33; i++)
        {
            numrcv = recv(dstSock, &c, 1, 0);
            if(c=='\n')
            {
                raw_end_buf[i] = '\0';
                break;
            }
            raw_end_buf[i] = c;
        }

        //-距離データを抽出-//
        for(i=0; i<20; i++)
        {
            char *tp;
            tp = strtok(&raw_line_buf[i][0], "#:");
            for(j=0; j<3; j++)
            {
                tp = strtok(NULL, "#:");
            }
            line_buf[i] = tp;
        }


        for(i=0; i<20; i++)
        {
            if(line_buf[i] != NULL)
            {
                for(j=0; j<136; j++)
                {
                    str_buf[0] = *(line_buf[i] + 8*j);
                    str_buf[1] = *(line_buf[i] + 8*j + 1);
                    str_buf[2] = *(line_buf[i] + 8*j + 2);
                    str_buf[3] = *(line_buf[i] + 8*j + 3);
                    str_buf[4] = '\0';

                    dist[i][j] = strtol(str_buf, NULL, 16);
                }
            }
        }

        //-xyz座標値に変換-//
        if(det == 'U')
        {
            SCAN_DATA.det = 'U';
            for(i=0; i<20; i++)
            {
                for(j=0; j<136; j++)
                {
                    calc_xyz(U_angle_x[i][j], U_angle_y[i][j], dist[i][j], &SCAN_DATA.x[i*136 + j], &SCAN_DATA.y[i*136 + j], &SCAN_DATA.z[i * 136 + j]);
                }
            }
        }else if(det == 'D'){
            SCAN_DATA.det = 'D';
            for(i=0; i<20; i++)
            {
                for(j=0; j<136; j++)
                {
                    calc_xyz(D_angle_x[i][j], D_angle_y[i][j], dist[i][j], &SCAN_DATA.x[i*136 + j], &SCAN_DATA.y[i*136 + j], &SCAN_DATA.z[i * 136 + j]);
                }
            }
        }

        //-SSM書き込み-//
        for(i=0; i<20; i++)
        {
            for(j=0; j<136; j++)
            {
                SCAN_DATA.dist[136*i + j] = dist[i][j];
            }
        }

        printf("%c\n", SCAN_DATA.det);

        writeSSM(LS3D_sid, (char*)&SCAN_DATA, gettimeSSM());

        usleep(1000);

        //time_2 = get_time();
        //usleep(freq*1000000 - (time_2 - time_1)*1000000);
        //time_3 = get_time();

    } //距離データ取得ループ

    //-計測停止要求-//
    send(dstSock, toSendText2, strlen(toSendText2)+1, 0);

    //-socketの終了-//
    close(dstSock);

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
