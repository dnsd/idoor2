//10Hz用

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define BUFFER_SIZE 60000


int main ()
{
    struct sockaddr_in dstAddr;
    int dstSock;
    char buf[BUFFER_SIZE];
    char *U_angle_line_buf[20];
    char *D_angle_line_buf[20];
    char str_buf[6];
    char x_str_buf[7];
    char y_str_buf[7];
    long U_x_beta[20][136];
    long U_y_beta[20][136];
    long D_x_beta[20][136];
    long D_y_beta[20][136];
    double U_angle_x[20][136];
    double U_angle_y[20][136];
    double D_angle_x[20][136];
    double D_angle_y[20][136];
    char c;
    char raw_buf[1650];
    char raw_U_buf[20][1650];
    char raw_D_buf[20][1650];
    int n;
    int numrcv;
    char *toSendText1 = "#TD2LISS\n";
    char *toSendText2 = "#TD2GET\n";
    char *toSendText3 = "#TD2STOP\n";

    FILE *fp; //ファイルポインタ
    FILE *fp2;
    FILE *fp3;

    int i,j,k; //ループ用
    char *tp, *tp2; //stotok関数用ポインタ

    double angle_convert(long angle); //プロトタイプ宣言

    //-ソケットの作成-//
    dstSock = socket(AF_INET, SOCK_STREAM, 0);

    //-接続先指定用構造体の準備-//
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(10940);
    dstAddr.sin_addr.s_addr = inet_addr("192.168.0.10");

    //-コネクト-//
    connect(dstSock, (struct sockaddr *)&dstAddr, sizeof(dstAddr));

    //-ファイルオープン-//
    fp = fopen("beta_data", "w");

    //----------------angleデータを取得----------------------//

    //-サーバにメッセージを送信-//
    memset(&dstAddr, 0, sizeof(dstAddr));
    send(dstSock, toSendText1, strlen(toSendText1)+1, 0);
    sleep(1);
    printf("sending...\n");

    //-サーバからデータを受信-//
    memset(raw_buf, 0, sizeof(raw_buf));
    memset(raw_U_buf, 0, sizeof(raw_U_buf));
    memset(raw_D_buf, 0, sizeof(raw_D_buf));

    for(i=0; i<1650; i++)
    {
        numrcv = recv(dstSock, &c, 1, 0);
        if(c=='\n')
        {
            raw_buf[i] = '\0';
            break;
        }
        raw_buf[i] = c;
    }

    if(raw_buf[12] == 'U')
    {
        for(i=0; i<1650; i++)
        {
            raw_U_buf[0][i] = raw_buf[i];
            if(raw_buf[i] == '\0') break;
        }
        raw_U_buf[0][1649] = '\0'; //debug
        for(j=1; j<20; j++)
        {
            for(i=0; i<1650; i++)
            {
                numrcv = recv(dstSock, &c, 1, 0);
                if(c=='\n')
                {
                    raw_U_buf[j][i] = '\0';
                    break;
                }
                raw_U_buf[j][i] = c;
            }
        }
        for(j=0; j<20; j++)
        {
            for(i=0; i<1650; i++)
            {
                numrcv = recv(dstSock, &c, 1, 0);
                if(c=='\n')
                {
                    raw_D_buf[j][i] = '\0';
                    break;
                }
                raw_D_buf[j][i] = c;
            }
        }
    }else if(raw_buf[12] == 'D'){
        for(i=0; i<1650; i++)
        {
            raw_D_buf[0][i] = raw_buf[i];
            if(raw_buf[i] == '\0') break;
        }
        for(j=1; j<20; j++)
        {
            for(i=0; i<1650; i++)
            {
                numrcv = recv(dstSock, &c, 1, 0);
                if(c=='\n')
                {
                    raw_D_buf[j][i] = '\0';
                    break;
                }
                raw_D_buf[j][i] = c;
            }
        }
        for(j=0; j<20; j++)
        {
            for(i=0; i<1650; i++)
            {
                numrcv = recv(dstSock, &c, 1, 0);
                if(c=='\n')
                {
                    raw_U_buf[j][i] = '\0';
                    break;
                }
                raw_U_buf[j][i] = c;
            }
        }
    }

    //-角度データを抽出-//
    for(i=0; i<20; i++)
    {
        char *tp;
        tp = strtok(&raw_U_buf[i][0], "#:");
        for(j=0; j<2; j++)
        {
            tp = strtok(NULL, "#:");
        }
        U_angle_line_buf[i] = tp;
    }

    for(i=0; i<20; i++)
    {
        for(j=0; j<136; j++)
        {
            x_str_buf[0] = *(U_angle_line_buf[i] + 12*j);
            x_str_buf[1] = *(U_angle_line_buf[i] + 12*j + 1);
            x_str_buf[2] = *(U_angle_line_buf[i] + 12*j + 2);
            x_str_buf[3] = *(U_angle_line_buf[i] + 12*j + 3);
            x_str_buf[4] = *(U_angle_line_buf[i] + 12*j + 4);
            x_str_buf[5] = *(U_angle_line_buf[i] + 12*j + 5);
            x_str_buf[6] = '\0';
            U_x_beta[i][j] = strtol(x_str_buf, NULL, 16);
            //U_angle_x[i][j] = angle_convert(U_x_beta[i][j]);

            y_str_buf[0] = *(U_angle_line_buf[i] + 12*j + 6);
            y_str_buf[1] = *(U_angle_line_buf[i] + 12*j + 7);
            y_str_buf[2] = *(U_angle_line_buf[i] + 12*j + 8);
            y_str_buf[3] = *(U_angle_line_buf[i] + 12*j + 9);
            y_str_buf[4] = *(U_angle_line_buf[i] + 12*j + 10);
            y_str_buf[5] = *(U_angle_line_buf[i] + 12*j + 11);
            y_str_buf[6] = '\0';
            U_y_beta[i][j] = strtol(y_str_buf, NULL, 16);
            //U_angle_y[i][j] = angle_convert(U_y_beta[i][j]);
        }
    }

    for(i=0; i<20; i++)
    {
        char *tp;
        tp = strtok(&raw_D_buf[i][0], "#:");
        for(j=0; j<2; j++)
        {
            tp = strtok(NULL, "#:");
        }
        D_angle_line_buf[i] = tp;
    }
    for(i=0; i<20; i++)
    {
        for(j=0; j<136; j++)
        {
            x_str_buf[0] = *(D_angle_line_buf[i] + 12*j);
            x_str_buf[1] = *(D_angle_line_buf[i] + 12*j + 1);
            x_str_buf[2] = *(D_angle_line_buf[i] + 12*j + 2);
            x_str_buf[3] = *(D_angle_line_buf[i] + 12*j + 3);
            x_str_buf[4] = *(D_angle_line_buf[i] + 12*j + 4);
            x_str_buf[5] = *(D_angle_line_buf[i] + 12*j + 5);
            x_str_buf[6] = '\0';
            D_x_beta[i][j] = strtol(x_str_buf, NULL, 16);
            //D_angle_x[i][j] = angle_convert(D_x_beta[i][j]);

            y_str_buf[0] = *(D_angle_line_buf[i] + 12*j + 6);
            y_str_buf[1] = *(D_angle_line_buf[i] + 12*j + 7);
            y_str_buf[2] = *(D_angle_line_buf[i] + 12*j + 8);
            y_str_buf[3] = *(D_angle_line_buf[i] + 12*j + 9);
            y_str_buf[4] = *(D_angle_line_buf[i] + 12*j + 10);
            y_str_buf[5] = *(D_angle_line_buf[i] + 12*j + 11);
            y_str_buf[6] = '\0';
            D_y_beta[i][j] = strtol(y_str_buf, NULL, 16);
            //D_angle_y[i][j] = angle_convert(D_y_beta[i][j]);
        }
    }

    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fprintf(fp, "%ld\n", U_x_beta[j][i]);
        }
    }
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fprintf(fp, "%ld\n", U_y_beta[j][i]);
        }
    }
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fprintf(fp, "%ld\n", D_x_beta[j][i]);
        }
    }
    for(j=0; j<20; j++)
    {
        for(i=0; i<136; i++)
        {
            fprintf(fp, "%ld\n", D_y_beta[j][i]);
        }
    }

    //-計測停止要求-//
    send(dstSock, toSendText3, strlen(toSendText3)+1, 0);

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
