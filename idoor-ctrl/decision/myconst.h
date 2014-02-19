// タイミング実験用
//センサスペック
const int STEP_NUM = 2720;
const double SENSOR_HEIGHT = 2600; //mm
const double SENSOR_ANGLE = 37; //deg
const double FREQ = 0.05; //単位は秒

// ステップの分類
const double CLOSE_TH = 15.0; //10Hz //CLOSE_TH以上近づくものをbeam_closeとする。
const double STOP_TH = 100.0; // 10Hz //単位はmm

//tan
const int TAN_CELL_NUM_MAX = 30;
const double TAN_CELL_RES = 100.0;
const int TANZAKU_NUM_MAX = 30; //短冊の個数
const int FRAME_OBSERVE = 5;

// バッファ関連
const int BUFFER_LENGTH = 6; //最小二乗法のバッファ範囲
const int CUR_INDEX = BUFFER_LENGTH - 1;
const int PRE_INDEX = BUFFER_LENGTH - 2;
const int PREPRE_INDEX = BUFFER_LENGTH - 3;


const double MAX_SPEED = 150.0; //フィルタを更新するorしないのしきい値（mm）//3.0m/s
const double MIN_SPEED = 15.0; //0.15m/s
const double V_MAX_TH = 3000.0; //3000mm/s以上で移動する人は観測しない

const int BORDER_NUM_MAX = 29; //短冊の境界線の本数

//-ドア-//
// const double DOOR_V_NORMAL = 1000.0; //単位はmm/sec、両開き
// const double DOOR_V_HIGH = 1800.0; //単位はmm/sec、両開き
// const double DOOR_W_HALF = 800.0; //単位はmm
// const double DOOR_W_FULL = 1800.0; //暫定
const int REQUIRED_FRAME_NORMAL_HALF = 16; //ゆっくりドアを開くときの所要フレーム数（半開き）
const int REQUIRED_FRAME_NORMAL_FULL = 36;
const int REQUIRED_FRAME_HIGH_HALF = 14; //素早くドアを開くときの所要フレーム数 （半開き）
const int REQUIRED_FRAME_HIGH_FULL = 34;
const int MARGIN = 10; //何フレーム前までにドアが開いていてほしいか。

const double DOOR_W_MIN = 200.0; //これ以下だったら開けない
const double DOOR_W_TH = 800.0; //一人or複数人の判定用

//lane
const double LANE_W = 100.0;
const int LANE_NUM_MAX = 30;
const int PENDING_CNT_MAX = 20; // pending zoneが消えるまでの時間（フレーム数）
const int PENDING_ZONE_WIDTH = 4; // 片側

// エリアS("S"urveilance)の定義
const double AREA_S_START_Y = -1600.0;
const double AREA_S_END_Y = 1600.0;
const double AREA_S_START_X = 0.0;
const double AREA_S_END_X = 2600.0;

// エリアD("D"eteciton)エリアの定義
const double AREA_D_MARGIN = 300.0;
const double AREA_D_START_Y = AREA_S_START_Y + AREA_D_MARGIN;
const double AREA_D_END_Y = AREA_S_END_Y - AREA_D_MARGIN;
const double AREA_D_START_X = 0.0;
const double AREA_D_END_X = AREA_S_END_X - AREA_D_MARGIN;

// エリアC("C"onventional)の定義
const double AREA_C_START_Y = -1000.0; //検出エリアの範囲の設定
const double AREA_C_END_Y = 1000.0; //検出エリアの範囲の設定
const double AREA_C_START_X = 0.0; //検出エリアの範囲の設定
const double AREA_C_END_X = 1000.0; //検出エリアの範囲の設定
const double AREA_C_START_Z = 0.0; //検出エリアの範囲の設定
const double AREA_C_END_Z = 2000.0; //検出エリアの範囲の設定
const int AREA_C_STEP_NUM_TH = 20; // エリアの中に一定数以上scanpointがあれば物体が存在するとみなす
const int BUF_LENGTH_HAS_OBJECTS = 10; //バッファの長さ
const int BUF_NUM_HAS_OBJECTS = 7; // バッファのうち物体が存在したフレームが一定数以上あればドアを開ける