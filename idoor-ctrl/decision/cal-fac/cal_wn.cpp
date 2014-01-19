//trial
//gxベースで基準の幅を算出する。

#include <iostream>
#include "LS3D.h"
#include <iomanip>
#include <GL/glut.h>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

#define pointsize 2
#define beam_num 2720
#define SENSOR_HEIGHT 2550.0

/*-----------------式の設定-------------------*/
#define TILTED_LINE_NUM 30
#define TANZAKU_NUM 30

double a[TILTED_LINE_NUM];
double b[TILTED_LINE_NUM];
double pxa[TILTED_LINE_NUM]; //y=0のときのx
double pya[TILTED_LINE_NUM]; //0
double pxb[TILTED_LINE_NUM];
double pyb[TILTED_LINE_NUM];

const double G_xn = 1000.0;
double G_pn[TANZAKU_NUM]; //G_xが1mのときのG_pn
double wn[TANZAKU_NUM]; //G_xが1mのときの短冊の幅

/*-------------------プロトタイプ宣言----------------------*/
void display(void);
void reshape(int w, int h);
void MouseFunc(int button, int state, int x, int y);
void MouseMotion(int x, int y);

void cal_G_pn(double G_pn[]);
void SetFac(double G_pn[], double a[], double b[], double pxa[], double pya[], double pxb[], double pyb[]);
void set_wn(double G_pn[] ,double wn[]);
double p_dif(double Ax, double Ay, double Bx, double By);

/*----------------------構造体--------------------------*/
struct _MOUSE
{
    int Xstart, Ystart;
    bool flag;
    double weight;
};
_MOUSE MouseStatus={0,0,false,0.5};

struct _ObjectRotate
{
    double xAngle,yAngle;
};
_ObjectRotate Rotation={0,0};

/*SSM*/

void MY_INIT()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

void GLUT_INIT()
{
    glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE | GLUT_DEPTH);
}

void idle(void)
{
    glutPostRedisplay();
}

void resize( int w, int h )
{
    glViewport( 0, 0, w, h );
    /* 透視変換行列の設定 */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 2100.0, 310000.0);
    /* モデルビュー変換行列の設定 */
    glMatrixMode(GL_MODELVIEW);
	
}


int main (int argc, char **argv)
{

//-------------------------------------------
//SSMの初期化とオープン
//-------------------------------------------

/*初期化*/
/*SSMオープン*/
/*式の設定*/
    // cal_G_pn(G_pn);
    set_wn(G_pn, wn);
    SetFac(G_pn, a, b, pxa, pya, pxb, pyb);

    ofstream ofs;
    ofs.open("wn");
    for(int i=0; i<TANZAKU_NUM; i++)
    {
        // wn[i] = p_dif(pxb[i], pyb[i], pxb[i+1], pyb[i+1]);
        ofs << wn[i] << endl;
    }
    ofs.close();

//-------------------------------------------

    glutInit(&argc, argv);
    GLUT_INIT();
    glutCreateWindow("Viewer");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MouseMotion);
    glutIdleFunc(idle);
    MY_INIT();
    glutMainLoop();

/*SSMクローズ*/
    return 0;
}

void display(void)
{
    GLdouble vertex[beam_num][3];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(-10000.0, 10000.0, 4000.0, 0.0, 0.0, -1200.0, 0.0, 0.0, 1.0);

    glRotated(Rotation.xAngle,1,0,0);
    glRotated(Rotation.yAngle,0,1,0);


    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(0.0, -2400.0, -SENSOR_HEIGHT);
    glVertex3d(2400.0, -2400.0, -SENSOR_HEIGHT);
    glVertex3d(2400.0, 2400.0, -SENSOR_HEIGHT);
    glVertex3d(0.0, 2400.0, -SENSOR_HEIGHT);
    glEnd();

    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP); // x軸
    glVertex3d(0.0, 0.0, -SENSOR_HEIGHT);
    glVertex3d(6000.0, -0.0, -SENSOR_HEIGHT);
    glEnd();

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

    glFlush();
    glutSwapBuffers();

    usleep(1000);
}

void MouseFunc(int button,int state,int x,int y)
{
        if(button == GLUT_LEFT_BUTTON &&state == GLUT_DOWN){
                MouseStatus.Xstart = x;
                MouseStatus.Ystart = y;
                MouseStatus.flag = true;
        }
        else{
                MouseStatus.flag =false;
        }

}

void MouseMotion(int x,int y)
{
        if(MouseStatus.flag == false)return;

        static int xdir,ydir;

        xdir = x - MouseStatus.Xstart;
        ydir = y - MouseStatus.Ystart;

        Rotation.xAngle += (double)ydir * MouseStatus.weight;
        Rotation.yAngle += (double)xdir * MouseStatus.weight;

        MouseStatus.Xstart = x;
        MouseStatus.Ystart = y;

        glutPostRedisplay();
}

void cal_G_pn(double G_pn[])
{
    for (int i = 0; i < TANZAKU_NUM; i++)
    {
        //角度設定
        double theta = 0.0;
        theta = M_PI - ( ( 1.0 / 30.0 * M_PI) * i + (1.0 / 60.0 * M_PI) );
        G_pn[i] = G_xn / sin(theta);
    }
}

void SetFac(double G_pn[], double a[], double b[], double pxa[], double pya[], double pxb[], double pyb[])
{
    for (int i = 0; i < TILTED_LINE_NUM; i++)
    {
        //角度設定
        double theta = 0.0;

        theta = M_PI - (1.0 / 30.0 * M_PI) * i;
        pya[i] = 0.0;
        pxa[i] = 0.0;
        pxb[i] = G_pn[i] * sin(theta); // + b[i];
        pyb[i] = G_pn[i] * cos(theta);
        a[i] = (pxa[i] - pxb[i]) / (pya[i] - pyb[i]);
        // ofs << a[i] << endl;
        // ofs << b[i] << endl;
    }

    //ofs.close();
}

void set_wn(double G_pn[] ,double wn[])
{
    for (int i = 0; i < TANZAKU_NUM; i++)
    {
        double theta = 0.0;
        double pxb1, pxb2, pyb1, pyb2;

        //G_pnの計算
        theta = M_PI - ( ( 1.0 / 30.0 * M_PI) * i + (1.0 / 60.0 * M_PI) );
        G_pn[i] = G_xn / sin(theta);

        //pxb1,pyb1の計算
        theta = M_PI - (1.0 / 30.0 * M_PI) * i;
        pxb1 = G_pn[i] * sin(theta); // + b[i];
        pyb1 = G_pn[i] * cos(theta);

        //pxb2,pyb2の計算
        theta = M_PI - (1.0 / 30.0 * M_PI) * (i+1);
        pxb2 = G_pn[i] * sin(theta); // + b[i];
        pyb2 = G_pn[i] * cos(theta);

        //wnの計算
        wn[i] = p_dif(pxb1, pyb1, pxb2, pyb2);
    }
}

double p_dif(double Ax, double Ay, double Bx, double By)
{
    double dif2, dif;
    dif2 = (Bx - Ax) * (Bx - Ax) + (By - Ay) * (By - Ay);
    dif = sqrt(dif2);
    return dif;
}



