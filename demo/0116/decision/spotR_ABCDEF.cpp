// tanzaku_and_edge

#include <iostream>
#include "send_spotinfo.h"
#include <ssm.hpp>
#include <iomanip>
#include <GL/glut.h>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>

#define pointsize 2

#define sensor_height 2400
#define beam_num 2720
#define freq 0.1 //単位は秒

#define spot_res 100 //ひとつのスポットの辺の長さ
#define spot_num_y 44 //スポットの数（よこ）
#define spot_num_x 30 //スポットの数（たて）

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

//スキャンデータの格納用変数
GLdouble vertex_U[beam_num][3];
GLdouble vertex_D[beam_num][3];

//-関数のプロトタイプ宣言-//
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int mx, int my);
void motion(int mx, int my);
void idle(void);
void initEnvironment(void);

/*グリッド表示用*/
FILE *fp;

//-SSM-//
SSMApi<SPOTINFO> B("B", 8);
SSMApi<SPOTINFO> CF("CF", 9);
SSMApi<SPOTINFO> D("D", 10);
SSMApi<SPOTINFO> E("E", 11);

int main(int argc, char **argv)
{
    //SSM
    initSSM();
    B.open(SSM_READ);
    CF.open(SSM_READ);
    D.open(SSM_READ);
    E.open(SSM_READ);

    //GLUTの初期化
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(320, 320);
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

    //-SSMクローズ-//
    B.close();
    CF.close();
    D.close();
    E.close();
    endSSM();
    return 0;
}

void display(void)
{
    GLdouble vertex[spot_num_x][spot_num_y][3];

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
    glTranslatef( -1000.0, 0.0, sensor_height);

    if (CF.readNew())
    {
        B.readNew();
        D.readNew();
        E.readNew();

        //1メートルのエリア
        glColor3d(0.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(0.0, -1000.0, -sensor_height);
        glVertex3d(1000.0, -1000.0, -sensor_height);
        glVertex3d(1000.0, 1000.0, -sensor_height);
        glVertex3d(0.0, 1000.0, -sensor_height);
        glEnd();

        //監視領域(スポット)
        glColor3d(0.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(0.0, -2200.0, -sensor_height);
        glVertex3d(3000.0, -2200.0, -sensor_height);
        glVertex3d(3000.0, 2200.0, -sensor_height);
        glVertex3d(0.0, 2200.0, -sensor_height);
        glEnd();

        glColor3d(0.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(0.0, 0.0, -sensor_height);
        glVertex3d(6000.0, -0.0, -sensor_height);
        glEnd();

        glColor3d(0.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(0.0, 4000.0, -sensor_height);
        glVertex3d(0.0, -4000.0, -sensor_height);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3d(0.0, 0.0, 1000.0);
        glVertex3d(0.0, 0.0, -sensor_height);
        glEnd();

        //センサ
        glPushMatrix();
        glColor3d(0.0, 0.0, 0.0); //色の設定
        glTranslated(0.0, 0.0, 0.0);//平行移動値の設定
        glutSolidSphere(100.0, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
        glPopMatrix();

        //スポット情報の描画
        for (int i = 0; i < spot_num_x; i++)
        {
            for (int j = 0; j < spot_num_y; j++)
            {
                vertex[i][j][0] = 3000 - (spot_res * i);
                vertex[i][j][1] = -2200 + (spot_res * j);
                vertex[i][j][2] = -sensor_height;
                if(CF.data.spotinfo[i][j] == 2 && D.data.spotinfo[i][j] != 3 && E.data.spotinfo[i][j] != 4 && B.data.spotinfo[i][j] != 1) //近づいてるスポット
                {
                    glPushMatrix();
                    glColor3f(1.0, 1.0, 0.0);
                    glTranslated(vertex[i][j][0]+50, vertex[i][j][1]+50, vertex[i][j][2]);
                    glutSolidCube(100.0);
                    glPopMatrix();
                }
                if (CF.data.spotinfo[i][j] == 5 && B.data.spotinfo[i][j] != 1) //遠ざかったスポット
                {
                    glPushMatrix();
                    glColor3f(0.0, 0.5, 1.0);
                    glTranslated(vertex[i][j][0]+50, vertex[i][j][1]+50, vertex[i][j][2]);
                    glutSolidCube(100.0);
                    glPopMatrix();
                }
                if (B.data.spotinfo[i][j] == 1) //静止物体があるスポット
                {
                    glPushMatrix();
                    glColor3f(0.0, 0.0, 0.0);
                    glTranslated(vertex[i][j][0]+50, vertex[i][j][1]+50, vertex[i][j][2]);
                    glutSolidCube(100.0);
                    glPopMatrix();
                }
                if (D.data.spotinfo[i][j] == 3 && E.data.spotinfo[i][j] != 4) //D:ドアに近づいてくるスポット（D）
                {
                    glPushMatrix();
                    glColor3f(1.0, 0.0, 0.0);
                    glTranslated(vertex[i][j][0]+50, vertex[i][j][1]+50, vertex[i][j][2]);
                    glutSolidCube(100.0);
                    glPopMatrix();
                }
                if (E.data.spotinfo[i][j] == 4) //E:ドアをくぐる人がいるスポット
                {
                    glPushMatrix();
                    glColor3f(0.0, 1.0, 0.0);
                    glTranslated(vertex[i][j][0]+50, vertex[i][j][1]+50, vertex[i][j][2]);
                    glutSolidCube(100.0);
                    glPopMatrix();
                }
            }
        }
        glutSwapBuffers();
    } //if(OBJECT.readNew)
    sleepSSM(0.005);
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
    // // 光源を作成する
    // float  light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
    // float  light0_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    // float  light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // float  light0_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    // glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
    // glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
    // glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );
    // glLightfv( GL_LIGHT0, GL_AMBIENT, light0_ambient );
    // glEnable( GL_LIGHT0 );

    // // 光源計算を有効にする
    // glEnable( GL_LIGHTING );

    // // 物体の色情報を有効にする
    // glEnable( GL_COLOR_MATERIAL );

    // // Ｚテストを有効にする
    // glEnable( GL_DEPTH_TEST );

    // // 背面除去を有効にする
    // glCullFace( GL_BACK );
    // glEnable( GL_CULL_FACE );

    // 背景色を設定
    glClearColor(1.0, 1.0, 1.0, 0.0);
}