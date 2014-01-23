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
const int LANE_NUM_MAX = 26;

// エリアの定義
const double AREA_E_START_Y = -1600.0;
const double AREA_E_END_Y = 1000.0;
const double AREA_E_START_X = 0.0;
const double AREA_E_END_X = 2400.0;
const int FRAME_OBSERVE = 5;
const int PENDING_ZONE_WIDTH = 4; // 片側

// エリアBの定義
const double AREA_B_START_Y = -1000.0; //静止検出エリアの範囲の設定
const double AREA_B_END_Y = 1000.0; //静止検出エリアの範囲の設定
const double AREA_B_START_X = 0.0; //静止検出エリアの範囲の設定
const double AREA_B_END_X = 800.0; //静止検出エリアの範囲の設定
const int TH_NUM_OF_STEP_FOR_B = 30;
const int STOP_CNT_TH = 10; //このフレーム数の間静止物体があればドアを開ける。
