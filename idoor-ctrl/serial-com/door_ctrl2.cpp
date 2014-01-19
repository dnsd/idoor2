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
#include "send_spotinfo.h"

//#include <opencv/cv.h>
//#include <opencv/highgui.h>


#define SERIAL_PORT "/dev/ttyUSB0"  /* シリアルインターフェースに対応するデバイスファイル */
#define BAUDRATE B500000           /* 通信速度の設定 */
//#define BAUDRATE B9600           /* 通信速度の設定 */
//#define BAUDRATE B115200           /* 通信速度の設定 */

using namespace std;
//using namespace cv;


const char *SENDTEXT1 = "\xff#TASO0DCE\n";   //高速全開
const char *SENDTEXT2 = "\xff#TASO1DCF\n";   //高速半開
const char *SENDTEXT3 = "\xff#TASO09C3\n";   //標準全開
const char *SENDTEXT4 = "\xff#TASO19C4\n";   //標準半開
const char *SENDTEXT5 = "\xff#TASO05BF\n";   //低速全開
const char *SENDTEXT6 = "\xff#TASO15C0\n";   //低速半開


SSMApi<ORDER_DATA> ORDER("ORDER", 20);

int main(int argc, char *argv[])
{
	int fd;                           /* ファイルディスクリプタ */

	double time1, time2;
	int num;


	//ssmのイニシャライズ（必ず忘れずに）
  	initSSM();	

	struct termios oldtio, newtio;    /* シリアル通信設定 */
    
	fd = open(SERIAL_PORT, O_RDWR);   /* デバイスをオープンする */

	cout << "ok"<<endl;

	ioctl(fd, TCGETS, &oldtio);       /* 現在のシリアルポートの設定を待避させる */
	newtio = oldtio;                  /* ポートの設定をコピー */
	newtio.c_lflag = 0;//ICANON;
	newtio.c_cflag = (BAUDRATE |/* CRTSCTS |*/ CS8 | PARENB | PARODD | CLOCAL | CREAD);
	newtio.c_oflag &= ~ONLCR;
	ioctl(fd, TCSETS, &newtio);       /* ポートの設定を有効にする */

	// CMD_sid = openSSM("COMAND", 0, 0);
	ORDER.open(SSM_READ);
	

	while(1)
	{

		cout << "start" <<endl;


		// readSSM(CMD_sid, &ORDER.data.order, &time1, -1);
		// ORDER.readNew();

		cout << ORDER.data.order <<endl;
		if (ORDER.readNew()){//if(OBJECTreadNew)
			if(ORDER.data.order == 1){	//高速全開

				write(fd, SENDTEXT1, 11);      /* デバイスへ255バイト書き込み */
			}
			else if(ORDER.data.order == 2){	//高速半開

				write(fd, SENDTEXT2, 11);      /* デバイスへ255バイト書き込み */
			}
			else if(ORDER.data.order == 3){	//標準全開

				write(fd, SENDTEXT3, 11);      /* デバイスへ255バイト書き込み */
			}
			else if(ORDER.data.order == 4){	//標準半開

				write(fd, SENDTEXT4, 11);      /* デバイスへ255バイト書き込み */
			}
			else if(ORDER.data.order == 5){	//低速全開

				write(fd, SENDTEXT5, 11);      /* デバイスへ255バイト書き込み */
			}
			else if(ORDER.data.order == 6){	//低速半開

				write(fd, SENDTEXT6, 11);      /* デバイスへ255バイト書き込み */
			}
		}
    		usleep(1000);	

	}
    
	ioctl(fd, TCSETS, &oldtio);       /* ポートの設定を元に戻す */
	close(fd);                        /* デバイスのクローズ */
	return 0;
}
