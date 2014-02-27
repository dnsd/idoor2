// タイミング実験用

//センサスペック
const int STEP_NUM = 2720;
const double ORG_X = 250.0;
const double ORG_Y = 0.0;
const double ORG_Z = 2520.0;
const double ALPHA = 61.0 / 180.0 * M_PI; //取り付け角(10Hz)
const double SENSOR_RANGE = 7000; //センサの観測範囲
const double SENSOR_ANGLE = 37; //deg
const double FREQ = 0.05; //単位は秒

// ステップの分類
const double CLOSE_TH = 15.0; //10Hz //CLOSE_TH以上近づくものをbeam_closeとする。
const double STOP_TH = 100.0; // 10Hz //単位はmm

// バッファ関連
const int BUFFER_LENGTH = 6;
const int CUR_INDEX = BUFFER_LENGTH - 1;
const int PRE_INDEX = BUFFER_LENGTH - 2;
const int PREPRE_INDEX = BUFFER_LENGTH - 3;

// 物体検出エリアの定義
const double AREA_START_Y = -1000.0; //検出エリアの範囲の設定
const double AREA_END_Y = 1000.0; //検出エリアの範囲の設定
const double AREA_START_X = 0.0; //検出エリアの範囲の設定
const double AREA_END_X = 800.0; //検出エリアの範囲の設定
const double AREA_START_Z = 0.0; //検出エリアの範囲の設定
const double AREA_END_Z = 2000.0; //検出エリアの範囲の設定
const int STEP_NUM_CNT_TH = 30;
const int BUF_NUM_CNT_TH = 10; //このフレーム数の間物体があればドアを開ける。
const int BUF_LENGTH_HAS_OBJECTS = 15;

// 物体の設定
const int OBJECTNUM = 5;

// Area
const int DEFAULT_STEP_CNT_TH = 20;
const int DEFAULT_BUF_CNT_TH = 10;
const int DEFAULT_BUF_LENGTH = 15;

// AABB
const double DEFAULT_AABB_MIN0 = 0.0;
const double DEFAULT_AABB_MIN1 = -1000.0;
const double DEFAULT_AABB_MIN2 = 0.0;
const double DEFAULT_AABB_MAX0 = 1000.0;
const double DEFAULT_AABB_MAX1 = 1000.0;
const double DEFAULT_AABB_MAX2 = 3000.0;

