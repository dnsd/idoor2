#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

int main(int argc, char *argv[])
{
    int fd;	// ファイルディスクリプタ

    //ポート名を直接入力している。コマンドライン引数か,
    //端末上での入力でコンパイルのし直しの必要なく、ポートを開けるように変更してください。    
    char port_name[] = "/dev/ttyUSB0"; 
    
    if ((fd = open(port_name, O_RDWR | O_NOCTTY)) == -1) {
        perror(port_name);
        return -1;
    }

    struct termios oldtio;	// ポート設定退避用 
    
    // 現在のシリアルポートの設定を退避する
    tcgetattr(fd, &oldtio);
    
    struct termios newtio;  // 新しいポート設定格納用
    
    //termios構造体に値を設定し、通信設定を行っている
    //main文の内部にだらだらと初期化の記述をすると汚いので、
    //初期化用の関数を作ってみてください。
    
    // 構造体をクリア
	bzero(&newtio, sizeof(newtio));
    
    // 端末の設定 (制御モード)
	/*
	  baudrate_value : ボーレートの設定
	  CS8            : 8n1 (8 ビット，ノンパリティ，ストップビット 1)
	  CLOCAL         : ローカル接続，モデム制御なし
	  CREAD          : 受信文字(receiving characters)を有効にする．
	*/
	newtio.c_cflag = B38400 | CS8 | CREAD | CLOCAL;

	// 端末の設定 (入力モード)
	// 特に何も設定しない
	newtio.c_iflag = 0;

	// 端末の設定 (出力モード)
	// 特に何も設定しない = Raw モード (出力データに何も手を加えない)で出力
	newtio.c_oflag = 0;

	// 端末の設定 (ローカルモード)
	// 特に何も設定しない = 非カノニカルモード
	newtio.c_lflag = 0;	
	
	// タイムアウト設定とread関数の設定
	newtio.c_cc[VTIME] = 0;	// キャラクタ間タイマは未使用
	newtio.c_cc[VMIN]  = 1;	// 1文字で読み込みは満足される

	// 受信したが、読んでないデータをフラッシュ
	tcflush(fd, TCIFLUSH);

	// ポートを newtio の設定にする
	tcsetattr(fd, TCSANOW, &newtio);
	
	char c; //マイコンに送信する文字の変数を宣言
	
	c = 'z'; //シングルクオートで囲むとその内部の文字の文字コード
		
	write(fd, &c, 1);	// 1文字出力
			
	// 受信の実験用(x,y,zを送ると何が返ってくるか？)
	if (c == 'x' || c == 'y' || c == 'z'){
		read(fd, &c, 1);	// 1文字入力
		std::cout << "Received:" << c << std::endl;
	}
	
	
	return 0;
}

