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

// バッファ関連
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