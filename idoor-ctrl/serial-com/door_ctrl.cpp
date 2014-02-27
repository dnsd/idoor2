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

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyACM0"
#define _POSIX_SOURCE 1 /* POSIX 準拠のソース */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

SSMApi<ORDER> DORDER("ORDER", 30);

using namespace std;

int main()
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    char buf2[255];

    cout.precision(16);

    // ssmのイニシャライズ
    initSSM();
    DORDER.open(SSM_READ);

    fd = open(MODEMDEVICE, O_WRONLY | O_NOCTTY ); 
    if (fd <0) {perror(MODEMDEVICE); exit(-1); }

    tcgetattr(fd,&oldtio); /* 現在のポート設定を待避 */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* キャラクタ間タイマは未使用 */
    newtio.c_cc[VMIN]     = 5;   /* 5文字受け取るまでブロックする */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    buf[0] = 'o';
    buf2[0] = 'c';
    while(1)
    {
        // DORDER.readNew();
        // cout << DORDER.data.order << endl;
        // buf[0] = 'o';
        // buf2[0] = 'c';
        // if(DORDER.data.order >= 1)
        // {
        //     write(fd, buf, 1);
        // }else{
        //     write(fd, buf2, 1);
        // }

        if (DORDER.readNew() == true)
        {
            cout << get_time() << endl;
            cout << DORDER.data.order << endl;
            if(DORDER.data.order >= 1)
            {
                write(fd, buf, 1);
            }else{
                write(fd, buf2, 1);
            }
        }

        usleep(1000);
    }
    

    tcsetattr(fd,TCSANOW,&oldtio);//設定をもとに戻す
    close(fd);
}
