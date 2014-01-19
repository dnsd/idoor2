//10Hz用 tanzaku_b

#include <iostream>
#include <unistd.h>
#include "LS3D.h"
#include "myconst.h"
#include <iomanip>
#include <GL/glut.h>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

#define pointsize 2

/*-----------------式の設定-------------------*/
#define TILTED_LINE_NUM 30
// #define TILTED_LINE_NUM 40

double a[TILTED_LINE_NUM];
double b[TILTED_LINE_NUM];
double pxa[TILTED_LINE_NUM]; //x軸上の点
double pya[TILTED_LINE_NUM]; //x軸上の点
double pxb[TILTED_LINE_NUM];
double pyb[TILTED_LINE_NUM];

#define r 5000
double theta = 0;

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

//スキャンデータの格納用変数
GLdouble vertex_U[STEP_NUM][3];
GLdouble vertex_D[STEP_NUM][3];

//-関数のプロトタイプ宣言-//
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int mx, int my);
void motion(int mx, int my);
void idle(void);
void initEnvironment(void);
void SetFac(double a[], double b[], double px0[], double py0[], double px1[], double py1[]);

//グリッド表示用//
FILE *fp;

int main(int argc, char **argv)
{
    SetFac(a, b, pxa, pya, pxb, pyb);

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
    glutIdleFunc(idle);

    // 環境初期化
    initEnvironment();

    // GLUTのメインループに処理を移す
    glutMainLoop();

    return 0;
}

void display(void)
{
    GLdouble vertex[SPOT_NUM_X][SPOT_NUM_Y][3];
    
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

    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, -SENSOR_HEIGHT, -SENSOR_HEIGHT);
    glVertex3d(SENSOR_HEIGHT, -SENSOR_HEIGHT, -SENSOR_HEIGHT);
    glVertex3d(SENSOR_HEIGHT, SENSOR_HEIGHT, -SENSOR_HEIGHT);
    glVertex3d(0.0, SENSOR_HEIGHT, -SENSOR_HEIGHT);
    glEnd();

    // glColor3d(0.0, 0.0, 0.0);
    // glBegin(GL_LINE_LOOP); // x軸
    // glVertex3d(0.0, 0.0, -SENSOR_HEIGHT);
    // glVertex3d(6000.0, -0.0, -SENSOR_HEIGHT);
    // glEnd();

    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP); //y軸
    glVertex3d(0.0, 4000.0, -SENSOR_HEIGHT);
    glVertex3d(0.0, -4000.0, -SENSOR_HEIGHT);
    glEnd();

    glBegin(GL_LINE_LOOP); //z軸
    glVertex3d(0.0, 0.0, 1000.0);
    glVertex3d(0.0, 0.0, -SENSOR_HEIGHT);
    glEnd();

    glPushMatrix();
    glColor3d(0.0, 0.0, 0.0); //色の設定
    glTranslated(0.0, 0.0, 0.0);//平行移動値の設定
    glutSolidSphere(100.0, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();

    for(int i=0; i<TILTED_LINE_NUM; i++)
    {
        glBegin(GL_LINE_LOOP);
        glVertex3d(pxa[i], pya[i], -SENSOR_HEIGHT);
        glVertex3d(pxb[i], pyb[i], -SENSOR_HEIGHT);
        glEnd();
    }
    
    //目印の赤い線
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(pxa[5], pya[5], -SENSOR_HEIGHT);
    glVertex3d(pxb[5], pyb[5], -SENSOR_HEIGHT);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3d(pxa[10], pya[10], -SENSOR_HEIGHT);
    glVertex3d(pxb[10], pyb[10], -SENSOR_HEIGHT);
    glEnd();

    //円周を線だけで表示
    glBegin( GL_LINE_LOOP );
    float cx, cy, cz; 
    glColor3f( 0.0, 0.0, 0.0 );//white
    for(int i=0;i<=180;i++){
    cx = 1000.0*sin(M_PI*(float)i/(float)180);
    cy = 1000.0*cos(M_PI*(float)i/(float)180);
    cz = -SENSOR_HEIGHT;
    glVertex3f( cx, cy, cz );
    }
    glEnd();

    glutSwapBuffers();
    usleep(1000);
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
        //  camera_yaw += 360.0;
        // else if ( camera_yaw > 360.0 )
        //  camera_yaw -= 360.0;

        // マウスの横移動に応じてZ軸を中心に回転
        camera_roll -= ( mx - last_mouse_x ) * 1.0;
        // if ( camera_roll < 0.0 )
        //  camera_roll += 360.0;
        // else if ( camera_roll > 360.0 )
        //  camera_roll -= 360.0;
        
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
    // 背景色を設定
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

void SetFac(double a[], double b[], double pxa[], double pya[], double pxb[], double pyb[])
{
    //x=ay+bで考える.

    //TANZAKU_NUM = 30
    // ofstream ofs;
    // ofs.open("tanzaku_fac");
    // for (int i = 0; i < TILTED_LINE_NUM; i++)
    // {
    //     theta = M_PI - (1.0 / 30.0 * M_PI) * i;
    //     pya[i] = 0.0;
    //     pxa[i] = 0.0;
    //     pxb[i] = r * sin(theta); // + b[i];
    //     pyb[i] = r * cos(theta);
    //     a[i] = (pxa[i] - pxb[i]) / (pya[i] - pyb[i]);
    //     ofs << a[i] << endl;
    //     ofs << b[i] << endl;
    // }
    // ofs.close();

    //y=ax+bで考える
    //TANZAKU_NUM = 40
    ofstream ofs;
    ofs.open("tanzaku_fac");
    for (int i = 0; i < 30; i++) //6°刻み
    // for (int i = 0; i < 40; i++) //4.5°刻み
    {
        theta = M_PI - (1.0 / 30.0 * M_PI) * i; //6°刻み
        // theta = M_PI - (1.0 / 40.0 * M_PI) * i; //4.5°刻み
        pya[i] = 0.0; //中心は原点
        pxa[i] = 0.0; //中心は原点
        // pya[i] = 0.0; //中心
        // pxa[i] = -500.0; //中心
        pxb[i] = r * sin(theta);
        pyb[i] = r * cos(theta);
        a[i] = (pya[i] - pyb[i]) / (pxa[i] - pxb[i]);
        ofs << a[i] << endl;
        ofs << b[i] << endl;
    }
    ofs.close();

    //TANZAKU_NUM = 29
    // ofstream ofs;
    // ofs.open("tanzaku_fac");
    // for (int i = 0; i < 15; i++)
    // {
    //     theta = M_PI - (1.0 / 30.0 * M_PI) * i;
    //     pya[i] = 0.0;
    //     pxa[i] = 0.0;
    //     pxb[i] = r * sin(theta);
    //     pyb[i] = r * cos(theta);
    //     a[i] = (pya[i] - pyb[i]) / (pxa[i] - pxb[i]);
    //     ofs << a[i] << endl;
    //     ofs << b[i] << endl;
    // }
    // for (int i = 15; i < TILTED_LINE_NUM; i++)
    // {
    //     theta = M_PI - (1.0 / 30.0 * M_PI) * (i+1); //x軸を飛ばすためにi+1
    //     pya[i] = 0.0;
    //     pxa[i] = 0.0;
    //     pxb[i] = r * sin(theta);
    //     pyb[i] = r * cos(theta);
    //     a[i] = (pya[i] - pyb[i]) / (pxa[i] - pxb[i]);
    //     ofs << a[i] << endl;
    //     ofs << b[i] << endl;
    // }
    // ofs.close();
}