#include <iostream>
#include <stdio.h>	/* printf()、fprintf()に必要 */
#include <sys/socket.h>	/* socket()、connect()、send()、recv()に必要 */
#include <arpa/inet.h>	/* sockaddr_in、inet_addr()に必要 */
#include <stdlib.h>	/* atoi()に必要 */
#include <string.h>	/* memset()に必要 */
#include <unistd.h>	/* close()に必要 */
#include <math.h>
#include <fstream> //ofstreamに使用
#include <ssm.hpp>
#include "LS3D.h"

using namespace std;

#define BUFFER_SIZE 60000
#define STEP_NUM_PER_LINE 144//ステップ数（1ラインあたり）

#define ORG_X 600.0
#define ORG_Y 0.0
#define ORG_Z 2600.0
#define ALPHA 70.0 / 180.0 * M_PI //取り付け角(10Hz)

//-SSM-//
SSMApi<LS3D> SCAN_DATA("LS3D", 0);

typedef struct{
	long U_x[20][136];
	long U_y[20][136];
	long D_x[20][136];
	long D_y[20][136];
}ANGLE;

double angle_convert(long angle);
void calc_xyz(double angle_x, double angle_y, double dist, double *x, double *y, double *z);
void clear_scandata(LS3D& scandata);
void read_angle_data(ANGLE& angle);

int main(int argc, char *argv[])
{
	struct sockaddr_in dstAddr;
    int dstSock;
    char c;
    char raw_start_buf[26];
    char raw_line_buf[20][1113];
    char raw_end_buf[33];
    char *line_buf[22];
    char str_buf[5];
    int n;
    int numrcv;
    const char *toSendText1 = "#TD2GET\n";

    ANGLE angle;

    //-SSMの初期設定-//
    initSSM();
    SCAN_DATA.create(5.0,1.0);

    // //-取得データバッファの初期化-//
    // clear_scandata(SCAN_DATA);

    //-角度データの読み込み-//
    read_angle_data(angle);

    //-通信設定-//
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

    //-データ取得ループ-//
    while(1){ //距離データ取得ループ

        for(int i=0; i<26; i++)
        {
            numrcv = recv(dstSock, &c, 1, 0);
            if(c=='\n')
            {
                raw_start_buf[i] = '\0';
                break;
            }
            raw_start_buf[i] = c;
            SCAN_DATA.data.det = raw_start_buf[19]; //ループの外だとなぜか文字が消える
        }

        for(int j=0; j<20; j++)
        {
            for(int i=0; i<1177; i++)
            {
                numrcv = recv(dstSock, &c, 1, 0);
                if(c=='\n'){
                    raw_line_buf[j][i] = '\0';
                    break;
                }
                raw_line_buf[j][i] = c;
            }
        }

        for(int i=0; i<33; i++)
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
        for(int i=0; i<20; i++)
        {
            char *tp;
            tp = strtok(&raw_line_buf[i][0], "#:");
            for(int j=0; j<3; j++)
            {
                tp = strtok(NULL, "#:");
            }
            line_buf[i] = tp;
        }


        for(int i=0; i<20; i++)
        {
            if(line_buf[i] != NULL)
            {
                for(int j=0; j<136; j++)
                {
                    str_buf[0] = *(line_buf[i] + 8*j);
                    str_buf[1] = *(line_buf[i] + 8*j + 1);
                    str_buf[2] = *(line_buf[i] + 8*j + 2);
                    str_buf[3] = *(line_buf[i] + 8*j + 3);
                    str_buf[4] = '\0';

                    SCAN_DATA.data.dist[i*136 + j] = strtol(str_buf, NULL, 16);
                }
            }
        }

        //-xyz座標値に変換-//
        if(SCAN_DATA.data.det == 'U')
        {
            for(int i=0; i<20; i++)
            {
                for(int j=0; j<136; j++)
                {
                    calc_xyz(angle.U_x[i][j], angle.U_y[i][j], SCAN_DATA.data.dist[i*136 + j], &SCAN_DATA.data.x[i*136 + j], &SCAN_DATA.data.y[i*136 + j], &SCAN_DATA.data.z[i * 136 + j]);
                }
            }
        }else if(SCAN_DATA.data.det == 'D'){
            for(int i=0; i<20; i++)
            {
                for(int j=0; j<136; j++)
                {
                    calc_xyz(angle.D_x[i][j], angle.D_y[i][j], SCAN_DATA.data.dist[i*136 + j], &SCAN_DATA.data.x[i*136 + j], &SCAN_DATA.data.y[i*136 + j], &SCAN_DATA.data.z[i * 136 + j]);
                }
            }
        }

        //-SSM書き込み-//

        printf("%c\n", SCAN_DATA.data.det);
        SCAN_DATA.write();

        usleep(10000);
    } //距離データ取得ループ

}

double angle_convert(long angle)
{
	double alpha, beta;
    beta = (double)angle;
    alpha = ((2*beta)/(pow(2, 24)-1)) - 1;
    return alpha;
}

void calc_xyz(double angle_x, double angle_y, double dist, double *x, double *y, double *z)
{
    double tx, ty, tz;
    tx = dist * cos(angle_x) * cos(angle_y);
    ty = dist * sin(angle_x) * cos(angle_y);
    tz = dist * sin(angle_y);
    *x = tz * sin(ALPHA) + tx * cos(ALPHA) + ORG_X;
    *y = ty + ORG_Y;
    *z = tz * cos(ALPHA) - tx * sin(ALPHA) + ORG_Z;
}

// void clear_scandata(LS3D& scandata)
// {
// 	for (int i = 0; i < STEP_NUM; i++)
// 	{
// 		scandata.det = 'U';
//         scandata.dist[i] = 0.0;
//         scandata.x[i] = 0.0;
//         scandata.y[i] = 0.0;
//         scandata.z[i] = 0.0;
// 	}
// }

void read_angle_data(ANGLE& angle)
{
	ANGLE beta;
	FILE *fp;
	fp = fopen("beta_data", "r");
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta.U_x[j][i]);
            angle.U_x[j][i] = angle_convert(beta.U_x[j][i]);
        }
    }
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta.U_y[j][i]);
            angle.U_y[j][i] = angle_convert(beta.U_y[j][i]);
        }
    }
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta.D_x[j][i]);
            angle.D_x[j][i] = angle_convert(beta.D_x[j][i]);
        }
    }
    for(int j=0; j<20; j++)
    {
        for(int i=0; i<136; i++)
        {
            fscanf(fp, "%ld", &beta.D_y[j][i]);
            angle.D_y[j][i] = angle_convert(beta.D_y[j][i]);
        }
    }
}