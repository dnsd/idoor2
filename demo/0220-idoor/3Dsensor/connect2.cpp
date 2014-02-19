// 5Hz用

#include <iostream>
#include <stdio.h>	/* printf()、fprintf()に必要 */
#include <sys/socket.h>	/* socket()、connect()、send()、recv()に必要 */
#include <arpa/inet.h>	/* sockaddr_in、inet_addr()に必要 */
#include <stdlib.h>	/* atoi()に必要 */
#include <string.h>	/* memset()に必要 */
#include <unistd.h>	/* close()に必要 */
#include <math.h>
#include <fstream> //ofstreamに使用

using namespace std;

#define BUFFER_SIZE 60000

#define STEP_NUM 2880 //ステップ数（片道）
#define STEP_NUM_PER_LINE 144//ステップ数（1ラインあたり）

ofstream ofs;

int main(int argc, char *argv[])
{
	int sock;			/* ソケットディスクリプタ */
	struct sockaddr_in sensorAddr;/* センサーのアドレス */
	unsigned short sensorPort;	/* センサーのポート番号 */
	const char *toSendText1 = "#TD2LISS\n";/* センサーに送信する文字列 */
	const char *toSendText2 = "#TD2STOP\n";/* センサーに送信する文字列 */
	const char *toSendText3 = "#TD2GET\n";/* センサーに送信する文字列 */
	// char echoBuffer[RCVBUFSIZE];	/* エコー文字列用のバッファ */
	unsigned int SendTextLen;	/* エコーする文字列のサイズ */
	int bytesRcvd, totalBytesRcvd;	/* 一回のrecv()で読み取られるバイト数と全バイト数 */

	char angle_data_buf[40][1746]; //angleデータのバッファ

	/* TCPによる信頼性の高いストリームソケットを作成 */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		printf("socket() failed");

	/* サーバのアドレス構造体を作成 */
	sensorAddr.sin_family = AF_INET;			/* インターネットアドレスファミリ */
	sensorAddr.sin_addr.s_addr = inet_addr("192.168.0.10");	/* センサーのIPアドレス */
	sensorAddr.sin_port = htons(10940);		/* センサーのポート番号 */

	/* センサーへの接続を確立 */
	if (connect(sock, (struct sockaddr *) &sensorAddr, sizeof(sensorAddr)) < 0)
		printf("connect() failed");

	// //-----angleデータを取得-----//
	// //-サーバにメッセージを送信-//
	// memset(&sensorAddr, 0, sizeof(sensorAddr));		/* 構造体にゼロを埋める */
	// SendTextLen = strlen(toSendText1)+1;	/* 入力データの長さを調べる */
	// if (send(sock, toSendText1, SendTextLen, 0) != SendTextLen)
	// {
	// 	printf("send() sent a different number of bytes than expected");
	// }
	// printf("sending...\n");
	// usleep(1000000); //1秒待機

	// //-サーバからデータを受信-//
	// int LissStringLen = (12 * STEP_NUM_PER_LINE) + 18; //angleデータの長さ
	// for (int i = 0; i < 40; i++)
	// {
	// 	for (int j = 0; j < LissStringLen; j++)
	// 	{
	// 		char temp_c;
	// 		recv(sock, &temp_c, 1, 0);
	// 		if (temp_c == '\n')
	// 		{
	// 			angle_data_buf[i][j] = '\0';
	// 			break;
	// 		}
	// 		angle_data_buf[i][j] = temp_c;
	// 	}
	// }

	// for (int i = 0; i < 40; ++i)
	// {
	// 	cout << angle_data_buf[i][12] << endl;
	// }

	//----計測停止要求----//
	//-サーバにメッセージを送信-//
	memset(&sensorAddr, 0, sizeof(sensorAddr));		/* 構造体にゼロを埋める */
	SendTextLen = strlen(toSendText2);	/* 入力データの長さを調べる */
	if (send(sock, toSendText2, SendTextLen, 0) != SendTextLen)
	{
		printf("send() sent a different number of bytes than expected");
	}
	printf("sending...\n");
	usleep(1000000); //1秒待機

	//-サーバからデータを受信-//
	char msg_buf[12];
	for (int i = 0; i < 12; i++)
	{
		char temp_c;
		recv(sock, &temp_c, 1, 0);
		if (temp_c == '\n')
		{
			msg_buf[i] = '\0';
			break;
		}
		msg_buf[i] = temp_c;
	}

	for (int i = 0; i < 12; i++)
	{
		cout << msg_buf[i];
	}
	cout << endl;

	// ----センサデータ取得----//
	// -サーバにメッセージを送信-//
	memset(&sensorAddr, 0, sizeof(sensorAddr));		/* 構造体にゼロを埋める */
	SendTextLen = strlen(toSendText3)+1;	/* 入力データの長さを調べる */
	if (send(sock, toSendText3, SendTextLen, 0) != SendTextLen)
	{
		printf("send() sent a different number of bytes than expected");
	}
	printf("send\n");
	usleep(1000000); //1秒待機

	// -サーバからデータを受信-//
	ofs.open("debug.txt");

	char header_buf[26];
	char scandata_buf[20][1177];
	char footer_buf[33];
	for (int i = 0; i < 26; i++)
	{
		char temp_c;
		recv(sock, &temp_c, 1, 0);
		if (temp_c == '\n')
		{
			header_buf[i] = '\0';
			break;
		}
		header_buf[i] = temp_c;
		ofs << header_buf[i];
	}
	ofs << endl;

	cout << "!" << endl;

	for (int i = 0; i < 20; i++)
	{
		cout << "!" << endl;
		for (int j = 0; j < 1177; j++)
		{
			char temp_c;
			recv(sock, &temp_c, 1, 0);
			if (temp_c == '\n')
			{
				scandata_buf[i][j] = '\0';
				break;
			}
			scandata_buf[i][j] = temp_c;
			ofs << scandata_buf[i][j];
		}
		ofs << endl;
	}	

	cout << "!" << endl;

	for (int i = 0; i < 33; i++)
	{
		char temp_c;
		recv(sock, &temp_c, 1, 0);
		if (temp_c == '\n')
		{
			footer_buf[i] = '\0';
			break;
		}
		footer_buf[i] = temp_c;
	}

	for (int i = 0; i < 20; i++)
	{
		for (int j = 8; j < 11; j++)
		{
			cout << scandata_buf[i][j];
		}
		cout << endl;
	}
	ofs.close();

	//----計測停止要求----//
	//-サーバにメッセージを送信-//
	memset(&sensorAddr, 0, sizeof(sensorAddr));		/* 構造体にゼロを埋める */
	SendTextLen = strlen(toSendText2);	/* 入力データの長さを調べる */
	if (send(sock, toSendText2, SendTextLen, 0) != SendTextLen)
	{
		printf("send() sent a different number of bytes than expected");
	}
	printf("sending...\n");
	usleep(1000000); //1秒待機

	//-サーバからデータを受信-//
	for (int i = 0; i < 12; i++)
	{
		char temp_c;
		recv(sock, &temp_c, 1, 0);
		if (temp_c == '\n')
		{
			msg_buf[i] = '\0';
			break;
		}
		msg_buf[i] = temp_c;
	}

	for (int i = 0; i < 12; i++)
	{
		cout << msg_buf[i];
	}
	cout << endl;

	close(sock);
	exit(0);
}

double angle_convert(long angle)
{
	double alpha, beta;
    beta = (double)angle;
    alpha = ((2*beta)/(pow(2, 24)-1)) - 1;
    return alpha;
}