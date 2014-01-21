// タイミング実験用

//センサスペック
const int STEP_NUM = 2720;
const double SENSOR_HEIGHT = 2600; //mm
const double SENSOR_ANGLE = 37; //deg
const double FREQ = 0.05; //単位は秒

// ステップの分類
const double CLOSE_TH = 15.0; //10Hz //CLOSE_TH以上近づくものをbeam_closeとする。
const double STOP_TH = 100.0; // 10Hz //単位はmm

//人間
const double Ha = 1200.0; //これ以上高いものを大人とする
const double Hw = 300.0; //これより低いものを台車（ワゴン）とする //ワゴンと幅の「w」がかぶってる

//tan
const int TAN_CELL_NUM_MAX = 30;
const double TAN_CELL_RES = 100.0;

// バッファ関連
const int BUFFER_LENGTH = 6; //最小二乗法のバッファ範囲
const int CUR_INDEX = BUFFER_LENGTH - 1;
const int PRE_INDEX = BUFFER_LENGTH - 2;
const int PREPRE_INDEX = BUFFER_LENGTH - 3;


const double MAX_SPEED = 150.0; //フィルタを更新するorしないのしきい値（mm）//3.0m/s
const double MIN_SPEED = 15.0; //0.15m/s
const double V_MAX_TH = 3000.0; //3000mm/s以上で移動する人は観測しない

const int BORDER_NUM_MAX = 29; //短冊の境界線の本数
const int TANZAKU_NUM_MAX = 30; //短冊の個数

//開閉判定用
const int FRAME_OBSERVE_MIN = 4;

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
