#include <cstdio>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ssm.hpp>
#include "surveillance.h"

#define SERIAL_PORT "/dev/ttyUSB0"  /* シリアルインターフェースに対応するデバイスファイル */
// #define BAUDRATE B500000           /* 通信速度の設定 */
#define BAUDRATE B9600           /* 通信速度の設定 */
// #define BAUDRATE B115200           /* 通信速度の設定 */

using namespace std;

SSMApi<ORDER> DORDER("ORDER", 30);

int main(int argc, char *argv[])
{
    int fd;                           /* ファイルディスクリプタ */
    const char* serial_port = argv[1];

    // ssmのイニシャライズ
    initSSM();
    DORDER.create(5.0, 1.0);
    
    // シリアル通信の設定
    struct termios oldtio, newtio;    /* シリアル通信設定 */
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);   /* デバイスをオープンする */

    tcgetattr(fd, &oldtio); // シリアルポートの設定を退避

    bzero(&newtio, sizeof(newtio));

    newtio.c_lflag = 0;//ICANON;
    newtio.c_cflag = (BAUDRATE | CS8 | CREAD | CLOCAL);
    newtio.c_iflag = 0;
    newtio.c_oflag = 0;

    newtio.c_oflag &= ~ONLCR;
    
    // タイムアウト設定とread関数の設定
    newtio.c_cc[VTIME] = 0; // キャラクタ間タイマは未使用
    newtio.c_cc[VMIN] = 1; // 一文字で読み込みは満足される

    // 受信したけど読み込んでないデータはフラッシュ
    tcflush(fd, TCIFLUSH);

    // ポートをnewtioの設定にする
    tcsetattr(fd, TCSANOW, &newtio);

    // ioctl(fd, TCSETS, &newtio);       /* ポートの設定を有効にする */
    const char* buf = "o";

    // データ送信ループ
    while(1)
    {
        if (DORDER.readNew()){//if(OBJECTreadNew)
            cout << DORDER.data.order <<endl;

            if(DORDER.data.order == 1){  //高速全開

                write(fd, buf, 1);      /* デバイスへ255バイト書き込み */
            }
            else if(DORDER.data.order == 2){ //高速半開

                write(fd, buf, 1);      /* デバイスへ255バイト書き込み */
            }
            else if(DORDER.data.order == 3){ //標準全開

                write(fd, buf, 1);      /* デバイスへ255バイト書き込み */
            }
            else if(DORDER.data.order == 4){ //標準半開

                write(fd, buf, 1);      /* デバイスへ255バイト書き込み */
            }
            else if(DORDER.data.order == 5){ //低速全開

                write(fd, buf, 1);      /* デバイスへ255バイト書き込み */
            }
            else if(DORDER.data.order == 0){ //低速半開
                // write(fd, buf, 1);      /* デバイスへ255バイト書き込み */
            }
        }
            usleep(1000);
    }

    return 0;
}
