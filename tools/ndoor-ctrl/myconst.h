// タイミング実験用

//センサスペック
const int STEP_NUM = 2720;
const double SENSOR_HEIGHT = 2600; //mm
const double SENSOR_ANGLE = 37; //deg
const double FREQ = 0.05; //単位は秒

// バッファ関連
const int CUR_INDEX = BUFFER_LENGTH - 1;
const int PRE_INDEX = BUFFER_LENGTH - 2;
const int PREPRE_INDEX = BUFFER_LENGTH - 3;

//-ドア-//
const double DOOR_V_NORMAL = 1000.0; //単位はmm/sec、両開き
const double DOOR_V_HIGH = 1800.0; //単位はmm/sec、両開き
const double DOOR_W_HALF = 800.0; //単位はmm
const double DOOR_W_FULL = 1800.0; //暫定

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