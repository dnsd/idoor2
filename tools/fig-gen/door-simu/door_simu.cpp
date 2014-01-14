// for_tanzkau_and_edge

#include <iostream>
#include <iomanip>
#include <GL/glut.h>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <deque>
#include <iterator>
#include "door_simu.h"

using namespace std;

//視点操作用
float camera_pitch = 60.0; //X軸中心の回転角
float camera_yaw = 0.0; //Y軸中心の回転角
float camera_roll = 45.0; //Z軸中心の回転角
float camera_distance = 10000.0; //中心からのカメラ距離

//マウスドラッグ用
int  drag_mouse_r = 0;  // 右ボタンをドラッグ中かどうかのフラグ（0:非ドラッグ中,1:ドラッグ中）
int  drag_mouse_l = 0;  // 左ボタンをドラッグ中かどうかのフラグ（0:非ドラッグ中,1:ドラッグ中）
int  last_mouse_x;      // 最後に記録されたマウスカーソルのＸ座標
int  last_mouse_y;      // 最後に記録されたマウスカーソルのＹ座標

// 再生用
bool play_flag = false;

// ドアのアニメーション用
deque<GLdouble> door_pos(BUFFER_LENGTH);
int door_status = 0; //0：閉まっている。　1：開いている途中。　2：待ち時間。　3：閉じている途中。
int door_moving_cnt = 0; 

/*-----------------式の設定-------------------*/
#define TILTED_LINE_NUM 30
double a[TILTED_LINE_NUM];
double b[TILTED_LINE_NUM];
double pxa[TILTED_LINE_NUM]; //x軸上の点
double pya[TILTED_LINE_NUM]; //x軸上の点
double pxb[TILTED_LINE_NUM];
double pyb[TILTED_LINE_NUM];
#define r 5000
double theta = 0;

//-関数のプロトタイプ宣言-//
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int mx, int my);
void motion(int mx, int my);
void idle(void);
void initEnvironment(void);
void read();
void key(unsigned char key , int x , int y);
void specialkey(int key, int x, int y);
void SetFac(double a[], double b[], double pxa[], double pya[], double pxb[], double pyb[]);
void timer_scan(int value);
void glDrawArrowd(float x1, float y1,float x2, float y2,float length_arrow,float width_arrow,float scale);
void door_ctr();
void console_output();
void read_new_data(detection_log& read_data, ifstream& ifs, char char_scan[], GLdouble vertex[][3], int& cur_tid);

//スキャンデータの格納用変数
GLdouble vertex[STEP_NUM_MAX][3];

//ログファイル読み込み用
char char_scan[60000]; //一行分
string string_scan;
ifstream ifs;

//読み取ったデータ
detection_log read_data;

int cur_tid = (BUFFER_LENGTH - 1);

int main(int argc, char **argv)
{
    SetFac(a, b, pxa, pya, pxb, pyb);

	//ファイル読み込み準備
	ifs.open(argv[1]);
	ifs.precision(16);
	cout.precision(16);
	ifs.getline(char_scan, sizeof(char_scan));

	//GLUTの初期化
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Viewer");

	// コールバック関数の登録
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	// glutIdleFunc(idle);
	glutKeyboardFunc(key);
	glutSpecialFunc(specialkey);
	glutTimerFunc(100 , timer_scan , 0);

	// 環境初期化
	initEnvironment();

	// GLUTのメインループに処理を移す
	glutMainLoop();

	return 0;
}

void display(void)
{
	//-描画の準備-//
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//変換行列
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -camera_distance);
	glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
	glRotatef(-camera_yaw, 0.0, 1.0, 0.0);
	glRotatef(-camera_roll, 0.0, 0.0, 1.0);

	// 変換行列を設定（物体のモデル座標系→カメラ座標系）
	//（物体が (0.0, 1.0, 0.0) の位置にあり、静止しているとする）
	glTranslatef( -1000.0, 0.0, SENSOR_HEIGHT);

	//-スキャン点の描画-//
    glLineWidth(1.0); 
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(3);
    glBegin(GL_POINTS);
    for(int j=0; j<STEP_NUM_MAX; j++)
    {
    	if (read_data.det[cur_tid-2] == 'U')
    	{
    		glColor3d(1.0, 0.0, 0.0);
        	if(read_data.px[j][cur_tid-2] > 0) glVertex3d(read_data.px[j][cur_tid-2], read_data.py[j][cur_tid-2], read_data.pz[j][cur_tid-2]);
    	}else if(read_data.det[cur_tid-2] == 'D'){
    		glColor3d(1.0, 0.0, 0.0);
        	if(read_data.px[j][cur_tid-2] > 0) glVertex3d(read_data.px[j][cur_tid-2], read_data.py[j][cur_tid-2], read_data.pz[j][cur_tid-2]);
    	}
    }
    for(int j=0; j<STEP_NUM_MAX; j++)
    {
        if (read_data.det[cur_tid-1] == 'U')
    	{
    		glColor3d(1.0, 0.0, 0.0);
        	if(read_data.px[j][cur_tid-1] > 0) glVertex3d(read_data.px[j][cur_tid-1], read_data.py[j][cur_tid-1], read_data.pz[j][cur_tid-1]);
    	}else if(read_data.det[cur_tid-1] == 'D'){
    		glColor3d(1.0, 0.0, 0.0);
        	if(read_data.px[j][cur_tid-1] > 0) glVertex3d(read_data.px[j][cur_tid-1], read_data.py[j][cur_tid-1], read_data.pz[j][cur_tid-1]);
    	}
    }
    glEnd();

	//-座標軸＆各種線の描画-//
	// 監視領域
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, -1700.0, -SENSOR_HEIGHT);
    glVertex3d(2200.0, -1700.0, -SENSOR_HEIGHT);
    glVertex3d(2200.0, 1700.0, -SENSOR_HEIGHT);
    glVertex3d(0.0, 1700.0, -SENSOR_HEIGHT);
    glEnd();

    // x軸
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, 0.0, -SENSOR_HEIGHT);
    glVertex3d(6000.0, -0.0, -SENSOR_HEIGHT);
    glEnd();

     //y軸
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, 4000.0, -SENSOR_HEIGHT);
    glVertex3d(0.0, -4000.0, -SENSOR_HEIGHT);
    glEnd();

    //z軸
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, 0.0, 1000.0);
    glVertex3d(0.0, 0.0, -SENSOR_HEIGHT);
    glEnd();

    //-物体の描画-//
    // センサ
    glPushMatrix();
    glColor3d(0.0, 0.0, 0.0); //色の設定
    glTranslated(0.0, 0.0, 0.0);//平行移動値の設定
    glutSolidSphere(100.0, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();

    //ドア（左側）
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, -door_pos[cur_tid], -SENSOR_HEIGHT);
    glVertex3d(0.0, -door_pos[cur_tid]-DOOR_WIDTH, -SENSOR_HEIGHT);
    glVertex3d(0.0, -door_pos[cur_tid]-DOOR_WIDTH, -SENSOR_HEIGHT+DOOR_HEIGHT);
    glVertex3d(0.0, -door_pos[cur_tid], -SENSOR_HEIGHT+DOOR_HEIGHT);
    glEnd();
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, -door_pos[cur_tid]-100, -SENSOR_HEIGHT+100.0);
    glVertex3d(0.0, -door_pos[cur_tid]-DOOR_WIDTH+100.0, -SENSOR_HEIGHT+100.0);
    glVertex3d(0.0, -door_pos[cur_tid]-DOOR_WIDTH+100.0, -SENSOR_HEIGHT+DOOR_HEIGHT-100.0);
    glVertex3d(0.0, -door_pos[cur_tid]-100, -SENSOR_HEIGHT+DOOR_HEIGHT-100.0);
    glEnd();
    // ドア（右側）
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, door_pos[cur_tid], -SENSOR_HEIGHT);
    glVertex3d(0.0, door_pos[cur_tid]+DOOR_WIDTH, -SENSOR_HEIGHT);
    glVertex3d(0.0, door_pos[cur_tid]+DOOR_WIDTH, -SENSOR_HEIGHT+DOOR_HEIGHT);
    glVertex3d(0.0, door_pos[cur_tid], -SENSOR_HEIGHT+DOOR_HEIGHT);
    glEnd();
    glColor3d(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, door_pos[cur_tid]+100, -SENSOR_HEIGHT+100);
    glVertex3d(0.0, door_pos[cur_tid]+DOOR_WIDTH-100, -SENSOR_HEIGHT+100);
    glVertex3d(0.0, door_pos[cur_tid]+DOOR_WIDTH-100, -SENSOR_HEIGHT+DOOR_HEIGHT-100);
    glVertex3d(0.0, door_pos[cur_tid]+100, -SENSOR_HEIGHT+DOOR_HEIGHT-100);
    glEnd();

    // 目盛
    // glPointSize(3);
	// glBegin(GL_POINTS);
	// 	glVertex3d(1000.0, -414.2, -SENSOR_HEIGHT);
	// 	glVertex3d(1000.0, -1000.0, -SENSOR_HEIGHT);
	// glEnd();

    // ex1
	// glDrawArrowd(-1700.0, 1700.0, 0.0, 0.0, 1500.0, 100.0, 1);
	// glDrawArrowd(-2200.0, 911.2, 0.0, 0.0, 1500.0, 100.0, 1);
	// glDrawArrowd(-2400.0, 0.0, 0.0, 0.0, 1500.0, 100.0, 1);

    // ex2
	// glDrawArrowd(500.0, 0.0, 500.0, 1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(1000.0, 0.0, 1000.0, 1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(1500.0, 0.0, 1500.0, 1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(2000.0, 0.0, 2000.0, 1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(500.0, 0.0, 500.0, -1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(1000.0, 0.0, 1000.0, -1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(1500.0, 0.0, 1500.0, -1000.0, 1500.0, 100.0, 1);
	// glDrawArrowd(2000.0, 0.0, 2000.0, -1000.0, 1500.0, 100.0, 1);

	// glDrawArrowd(1500.0, -1700.0, 500.0, 1000.0, 3700.0, 100.0, 1);
	// glDrawArrowd(1500.0, -1700.0, 1000.0, 1000.0, 3500.0, 100.0, 1);
	// glDrawArrowd(1500.0, -1700.0, 2000.0, 1000.0, 3500.0, 100.0, 1);

    // ex3
 // glColor3f( 0, 0, 1 );
 //    glDrawArrowd(1700.0, 1700.0, 0.0, 0.0, 1500.0, 100.0, 1);
 //    glColor3f( 0, 1, 0 );
	// glDrawArrowd(2200.0, 911.2, 0.0, 0.0, 1500.0, 100.0, 1);
	// glColor3f( 1, 0, 0 );
	// glDrawArrowd(2400.0, 100.0, 0.0, 100.0, 1500.0, 100.0, 1);

	// glColor3f( 0, 0, 1 );
	// glDrawArrowd(1700.0, -1700.0, 0.0, 0.0, 1500.0, 100.0, 1);
 //    glColor3f( 0, 1, 0 );
	// glDrawArrowd(2200.0, -911.2, 0.0, 0.0, 1500.0, 100.0, 1);
	// glColor3f( 1, 0, 0 );
	// glDrawArrowd(2400.0, -100.0, 0.0, -100.0, 1500.0, 100.0, 1);
  
	// glColor3f( 0, 0, 1 );
	// glDrawArrowd(1700.0, -1700.0, 0.0, 0.0, 1500.0, 100.0, 1);
	// glDrawArrowd(1500.0, -1900.0, 0.0, 0.0, 1500.0, 100.0, 1);
	// glColor3f( 0, 1, 0 );
	// glDrawArrowd(2200.0, -900.2, 0.0, 0.0, 1500.0, 100.0, 1);
	// glDrawArrowd(2100.0, -1100.2, 0.0, 0.0, 1500.0, 100.0, 1);
	// glColor3f( 1, 0, 0 );
	// glDrawArrowd(2400.0, 100.0, 0.0, 100.0, 1500.0, 100.0, 1);
	// glDrawArrowd(2400.0, -100.0, 0.0, -100.0, 1500.0, 100.0, 1);

	// pending
	// glColor3f( 0, 0, 1 );
	// glLineWidth(3.0);
 //    glBegin(GL_LINE_LOOP);
 //    glVertex3d(13*100.0, -1700.0, -SENSOR_HEIGHT);
 //    glVertex3d(13*100.0, 1700.0, -SENSOR_HEIGHT);
 //    glEnd();
	// for (int i = 14; i <= 19; i++)
 //    {
 //    	glColor3f( 0, 0, 0 );
 //        glLineWidth(1.0);
 //        glBegin(GL_LINE_LOOP);
 //        glVertex3d(i*100.0, -1700.0, -SENSOR_HEIGHT);
 //        glVertex3d(i*100.0, 1700.0, -SENSOR_HEIGHT);
 //        glEnd();
 //    }    
 //    glColor3f( 0, 0, 1 );
 //    glLineWidth(3.0);
 //    glBegin(GL_LINE_LOOP);
 //    glVertex3d(20*100.0, -1700.0, -SENSOR_HEIGHT);
 //    glVertex3d(20*100.0, 1700.0, -SENSOR_HEIGHT);
 //    glEnd();

    // 短冊
    glColor3d( 0, 0, 0 );
    for(int i=0; i<TILTED_LINE_NUM; i++)
    {
        glLineWidth(1.0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(pxa[i], pya[i], -SENSOR_HEIGHT);
        glVertex3d(pxb[i], pyb[i], -SENSOR_HEIGHT);
        glEnd();
    }
    glColor3d( 0, 0, 1 );
    for(int i=8; i<=9; i++)
    {
        glLineWidth(3.0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(pxa[i], pya[i], -SENSOR_HEIGHT);
        glVertex3d(pxb[i], pyb[i], -SENSOR_HEIGHT);
        glEnd();
    }

	cout << "camera_pitch = " << camera_pitch << endl;
    cout << "camera_roll = " << camera_roll << endl;
    cout << "camera_yaw = " << camera_yaw << endl; 
    cout << "camera_distance = " << camera_distance << endl;
    cout << endl;

    glutSwapBuffers();
}

void reshape(int w, int h)
{
	// ウィンドウ内の描画を行う範囲を設定（ウィンドウ全体に描画するように設定）
	glViewport(0, 0, w, h);

	// カメラ座標系→スクリーン座標系への変換行列を設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w/h, 100.0, 50000.0); //あやしい
}

void mouse(int button, int state, int mx, int my)
{
	// 左ボタンが押されたらドラッグ開始のフラグを設定
	if ( ( button == GLUT_LEFT_BUTTON ) && ( state == GLUT_DOWN ) )
		drag_mouse_l = 1;
	// 左ボタンが離されたらドラッグ終了のフラグを設定
	else if ( ( button == GLUT_LEFT_BUTTON ) && ( state == GLUT_UP ) )
		drag_mouse_l = 0;

	// 右ボタンが押されたらドラッグ開始のフラグを設定
	if ( ( button == GLUT_RIGHT_BUTTON ) && ( state == GLUT_DOWN ) )
		drag_mouse_r = 1;
	// 右ボタンが離されたらドラッグ終了のフラグを設定
	else if ( ( button == GLUT_RIGHT_BUTTON ) && ( state == GLUT_UP ) )
		drag_mouse_r = 0;

	// 現在のマウス座標を記録
	last_mouse_x = mx;
	last_mouse_y = my;
}

void motion(int mx, int my)
{
	// 左ボタンのドラッグ中であれば、マウスの移動量に応じて視点を移動する
	if ( drag_mouse_l == 1 )
	{
		// マウスの縦移動に応じて距離を移動
		// camera_distance += ( my - last_mouse_y ) * 0.2;
		camera_distance += ( my - last_mouse_y ) * 20.0;
		if ( camera_distance < 500.0 )
			camera_distance = 500.0;
	}
	
	// 右ボタンのドラッグ中であれば、マウスの移動量に応じて視点を回転する
	if ( drag_mouse_r == 1 )
	{
		// マウスの横移動に応じてＹ軸を中心に回転
		// camera_yaw -= ( mx - last_mouse_x ) * 1.0;
		// if ( camera_yaw < 0.0 )
		// 	camera_yaw += 360.0;
		// else if ( camera_yaw > 360.0 )
		// 	camera_yaw -= 360.0;

		// マウスの横移動に応じてZ軸を中心に回転
		camera_roll -= ( mx - last_mouse_x ) * 1.0;
		// if ( camera_roll < 0.0 )
		// 	camera_roll += 360.0;
		// else if ( camera_roll > 360.0 )
		// 	camera_roll -= 360.0;
		
		// マウスの縦移動に応じてＸ軸を中心に回転
		camera_pitch -= ( my - last_mouse_y ) * 1.0;
		if ( camera_pitch < 0.0 )
			camera_pitch = 0.0;
		else if ( camera_pitch > 90.0 )
			camera_pitch = 90.0;
	}

	// 今回のマウス座標を記録
	last_mouse_x = mx;
	last_mouse_y = my;

	// 再描画の指示を出す（この後で再描画のコールバック関数が呼ばれる）
	glutPostRedisplay();
}

void idle(void)
{
    glutPostRedisplay();
}

void  initEnvironment( void )
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

void read()
{
	//cur_tid操作
	if (cur_tid == (BUFFER_LENGTH - 1))
	{
		char *tp;
		int xyz_index = 0;
		char dir = 'U';
		// 新規読み込み
		// 角度の行の読み込み
		// getline(ifs, string_scan);
		// char_scan = string_scan.c_str();
		ifs.getline(char_scan, sizeof(char_scan));
		tp = strtok(char_scan, ","); //一度目のstrtokには分解対象を指定
		dir = *tp;

		// cout << dir << endl;

		// ビームの行の読み込み
		for (xyz_index = 0; xyz_index <= 2; xyz_index++)
		{
			int step_num = 0;
			ifs.getline(char_scan, sizeof(char_scan));
			tp = strtok(char_scan, ","); //一度目のstrtokには分解対象を指定
			vertex[step_num][xyz_index] = atof(tp);
			step_num++;

			while(tp = strtok(NULL, ","))
			{
				if (step_num < STEP_NUM_MAX)
				{
					vertex[step_num][xyz_index] = atof(tp);
					step_num++;
				}
			}
		}

		cur_tid = (BUFFER_LENGTH - 1);
		read_data.det.pop_front();
		read_data.det.push_back(dir);
		for (int step_num = 0; step_num < STEP_NUM_MAX; step_num++)
		{
			read_data.px[step_num].pop_front();
			read_data.py[step_num].pop_front();
			read_data.pz[step_num].pop_front();
			read_data.px[step_num].push_back(vertex[step_num][0]);
			read_data.py[step_num].push_back(vertex[step_num][1]);
			read_data.pz[step_num].push_back(vertex[step_num][2]);
		}

		//-結果の行の読み込み-//
		// int tan_num = 0;
		ifs.getline(char_scan, sizeof(char_scan));
		
		// open_mode読み込み
		tp = strtok(char_scan, ",");
		read_data.open_mode.pop_front();
		read_data.open_mode.push_back(atoi(tp));

		// cout << "!" << endl;
		
		// open_mode_tanの読み込み
		for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
		{
			tp = strtok(NULL, ",");
			read_data.open_mode_tan[tan_num].pop_front();
			read_data.open_mode_tan[tan_num].push_back(atoi(tp));
		}
		for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
		{
			tp = strtok(NULL, ",");
			read_data.tan_x[tan_num].pop_front();
			read_data.tan_x[tan_num].push_back(atof(tp));
		}
		for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
		{
			tp = strtok(NULL, ",");
			read_data.tan_v[tan_num].pop_front();
			read_data.tan_v[tan_num].push_back(atof(tp));
		}

		//tan_approach_cntの読み込み
		for (int tan_num = 0; tan_num < TANZAKU_NUM; tan_num++)
		{
			tp = strtok(NULL, ",");
			read_data.tan_approach_cnt[tan_num].pop_front();
			read_data.tan_approach_cnt[tan_num].push_back(atof(tp));
		}

		//スキャン時刻の読み込み
		tp = strtok(NULL, ",");
		read_data.scantime.pop_front();
		read_data.scantime.push_back(atof(tp));

	}else if ((BUFFER_LENGTH - 1) > cur_tid && cur_tid > 0){
		cur_tid++;
	}else if (cur_tid <= 0){
		cur_tid = 0;
	}
}

void key(unsigned char key , int x , int y)
{
	if (key == 'p')
	{
		// 1倍再生
		cout << "key = " << key << endl;
		play_flag = true;
	}
	if (key == 's')
	{
		// 一時停止
		cout << "key = " << key << endl;
		play_flag = false;
	}
}

// 巻き戻しを考慮
void specialkey(int key, int x, int y)
{
	if (key == GLUT_KEY_RIGHT)
	{
		read();
		door_ctr();
	}

	if (key == GLUT_KEY_LEFT)
	{
		if (cur_tid > 0)
		{
			cur_tid--;
		}else if(cur_tid <= 0){
			cur_tid = 0;
		}
	}

	console_output();

	// 再描画の指示を出す（この後で再描画のコールバック関数が呼ばれる）
	glutPostRedisplay();
}

void timer_scan(int value)
{
	if (play_flag == true)
	{
		read();
		door_ctr();
		console_output();
	}
	glutPostRedisplay();
	glutTimerFunc(SCAN_CYCLE , timer_scan , 0);
}

void SetFac(double a[], double b[], double pxa[], double pya[], double pxb[], double pyb[])
{
    //y=ax+bで考える
    ofstream ofs;
    ofs.open("tanzaku_fac");
    for (int i = 0; i < 30; i++) //6°刻み
    {
        theta = M_PI - (1.0 / 30.0 * M_PI) * i; //6°刻み
        pya[i] = 0.0; //中心は原点
        pxa[i] = 0.0; //中心は原点
        pxb[i] = r * sin(theta);
        pyb[i] = r * cos(theta);
        a[i] = (pya[i] - pyb[i]) / (pxa[i] - pxb[i]);
        ofs << a[i] << endl;
        ofs << b[i] << endl;
    }
    ofs.close();
}

void glDrawArrowd(float x1, float y1,float x2, float y2,float length_arrow,float width_arrow,float scale)
{
 double deg;
 float length,width;
 double sn,cs;
 float ratio;
 
 length = scale*length_arrow;
 width = scale*width_arrow;
 deg = atan2(y2-y1,x2-x1);
 sn = sin(deg);
 cs = cos(deg);
 //矢印の傘の部分と棒の部分の長さの比率は1:2とする。
 //普通に計算するとintの型変換で0になるのでratio変数へ格納
 ratio = (float)2/(float)3;
 
 glBegin(GL_QUADS);
  glVertex3d(x1+width/2*sn, y1-width/2*cs,-SENSOR_HEIGHT);
  glVertex3d(x1+width/2*sn+ratio*length*cs, y1-width/2*cs+ratio*length*sn, -SENSOR_HEIGHT);
  glVertex3d(x1-width/2*sn+ratio*length*cs, y1+width/2*cs+ratio*length*sn, -SENSOR_HEIGHT);
  glVertex3d(x1-width/2*sn, y1+width/2*cs, -SENSOR_HEIGHT);
 glEnd();
 // glBegin(GL_QUADS);
 //  glVertex3d(-1*(x1+width/2*sn), -1*(y1-width/2*cs),-SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1+width/2*sn+ratio*length*cs), -1*(y1-width/2*cs+ratio*length*sn), -SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1-width/2*sn+ratio*length*cs), -1*(y1+width/2*cs+ratio*length*sn), -SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1-width/2*sn), -1*(y1+width/2*cs), -SENSOR_HEIGHT);
 // glEnd();
  
 glBegin(GL_TRIANGLES);
  glVertex3d(x1+ratio*length*cs,y1+ratio*length*sn, -SENSOR_HEIGHT);
  glVertex3d(x1+width/2*sn+ratio*length*cs+width*sn,y1-width/2*cs+ratio*length*sn-width*cs, -SENSOR_HEIGHT);
  glVertex3d(x1+length*cs,y1+length*sn, -SENSOR_HEIGHT);
 glEnd();
 glBegin(GL_TRIANGLES);
  glVertex3d(x1+length*cs,y1+length*sn, -SENSOR_HEIGHT);
  glVertex3d(x1-width/2*sn+ratio*length*cs-width*sn,y1+width/2*cs+ratio*length*sn+width*cs, -SENSOR_HEIGHT);
  glVertex3d(x1+ratio*length*cs,y1+ratio*length*sn, -SENSOR_HEIGHT);
 glEnd();
 // glBegin(GL_TRIANGLES);
 //  glVertex3d(-1*(x1+ratio*length*cs), -1*(y1+ratio*length*sn), -SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1+width/2*sn+ratio*length*cs+width*sn) ,-1*(y1-width/2*cs+ratio*length*sn-width*cs), -SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1+length*cs),-1*(y1+length*sn), -SENSOR_HEIGHT);
 // glEnd();
 // glBegin(GL_TRIANGLES);
 //  glVertex3d(-1*(x1+length*cs), -1*(y1+length*sn), -SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1-width/2*sn+ratio*length*cs-width*sn), -1*(y1+width/2*cs+ratio*length*sn+width*cs), -SENSOR_HEIGHT);
 //  glVertex3d(-1*(x1+ratio*length*cs), -1*(y1+ratio*length*sn), -SENSOR_HEIGHT);
 // glEnd();

 glFlush();
}

void door_ctr()
{
	// cur_tid操作
	if (cur_tid == (BUFFER_LENGTH - 1))
	{
		// ドアが閉まっているとき
		if (door_status == 0
			&& read_data.open_mode[cur_tid] >= 1)
		{
			door_status = 1;
			door_moving_cnt++;
			// door_pos[cur_tid] = (GATE_WIDTH / TIME_REQUIRED);
			door_pos.pop_front();
			door_pos.push_back((GATE_WIDTH / TIME_REQUIRED));
		}
		// ドアが開いている途中
		if (door_status == 1)
		{
			door_moving_cnt++;
			// door_pos[cur_tid] = (door_pos[cur_tid-1] + (GATE_WIDTH / (double)TIME_REQUIRED));
			door_pos.pop_front();
			door_pos.push_back((door_pos[cur_tid-1] + (GATE_WIDTH / (double)TIME_REQUIRED)));
			if (door_moving_cnt == TIME_REQUIRED)
			{
				door_status = 2;
			}
		}
		// 待ち時間
		if (door_status == 2)
		{
			door_moving_cnt++;
			door_pos.pop_front();
			door_pos.push_back(GATE_WIDTH);
			if (door_moving_cnt == (TIME_REQUIRED + CLOSE_DELAY))
			{
				door_status = 3;
			}
		}
		// ドアを閉じる途中
		if (door_status == 3)
		{
			door_moving_cnt++;
			door_pos.pop_front();
			door_pos.push_back((door_pos[cur_tid-1] - (GATE_WIDTH / (double)TIME_REQUIRED)));
			if (door_moving_cnt == (TIME_REQUIRED + CLOSE_DELAY + TIME_REQUIRED))
			{
				door_status = 0;
				door_moving_cnt = 0;
			}
		}
	}
	cout << "door_status = " << door_status << endl;
	cout << "door_moving_cnt = " << door_moving_cnt << endl;
	cout << "door_pos[cur_tid] = " << door_pos[cur_tid] << endl;
}

void console_output()
{
	// 結果のコンソール出力
	cout << "scantime = " << read_data.scantime[cur_tid] << ",  ";
	cout << "dir = " << read_data.det[cur_tid] << ",  ";
	cout << "open_mode = " << read_data.open_mode[cur_tid] << ",  ";

	cout << "basis tan_x =";
	for (int i = 0; i < TANZAKU_NUM; i++)
	{
		if (read_data.open_mode_tan[i][cur_tid] != 0)
		{
			cout << (int)read_data.tan_x[i][cur_tid] << ", ";
		}
	}
	// cout << endl;

	cout << "basis tan_v =";
	for (int i = 0; i < TANZAKU_NUM; i++)
	{
		if (read_data.open_mode_tan[i][cur_tid] != 0)
		{
			cout << (int)read_data.tan_v[i][cur_tid] << ", ";
		}
	}
	cout << endl;

	// cout << "      object      = ";
	// for (int i = 0; i < TANZAKU_NUM; i++)
	// {
	// 	if (tan_x[i][cur_tid] != 0.0 || tan_x[i][cur_tid-1])
	// 	{
	// 		cout << "*" << ", ";
	// 	}else{
	// 		cout << "0" << ", ";
	// 	}
	// }
	// cout << endl;

	cout << " tan_approach_cnt = ";
	for (int i = 0; i < TANZAKU_NUM; i++)
	{
		cout << read_data.tan_approach_cnt[i][cur_tid] << ", ";
	}
	cout << endl;

	cout << "open_mode_tan = ";
	for (int i = 0; i < TANZAKU_NUM; i++)
	{
		cout << (int)read_data.open_mode_tan[i][cur_tid] << ", ";
	}
	cout << endl;

	cout << "tan_x = ";
	for (int i = 0; i < TANZAKU_NUM; i++)
	{
		cout << (int)read_data.tan_x[i][cur_tid] << ", ";
	}
	cout << endl;
	// cout << "tan_v = ";
	// for (int i = 0; i < TANZAKU_NUM; i++)
	// {
	// 	cout << (int)tan_v[i][cur_tid] << ", ";
	// }
	// cout << endl;
	
	cout << endl;
}