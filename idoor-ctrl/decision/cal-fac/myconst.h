//10Hz trial_only_lane

//モード設定（デバッグ用）
const bool MODE_B = false;
const bool MODE_D = false;
const bool MODE_E = true;


//センサスペック
const int STEP_NUM = 2720;
const double SENSOR_HEIGHT = 2540; //mm
const double SENSOR_ANGLE = 37; //deg
const double FREQ = 0.05; //単位は秒


//スポットの寸法
const int SPOT_RES = 100; //ひとつのスポットの辺の長さ
const int SPOT_NUM_Y = 44; //スポットの数（よこ）
const int SPOT_NUM_X = 30; //スポットの数（たて）

//B判定用
const int STOP_CNT_TH = 8; //このフレーム数以上物体があったらBとする。
const int B_SPOT_NUM_TH = 5; //これ以上B判定のスポットがあるときドア開

//CF判定用
// const DIST_DIF_TH 50.0 //5Hz
const double DIST_DIF_TH = 15.0; //10Hz //DIST_DIF_TH以上近づくものをbeam_closeとする。
const int CF_TH = 1; //スポット内にしきい値以上comeまたはbackの点があればC,Fを出力する

//D判定用
const int D_TH = 5; //Cの数がこれ以上だったらD判定を出す（ドアを開ける）

//人間
const double Ha = 1200.0; //これ以上高いものを大人とする
const double Hw = 300.0; //これより低いものを台車（ワゴン）とする //ワゴンと幅の「w」がかぶってる

//tanzaku
const int TAN_CELL_NUM_MAX = 30;
const double TAN_CELL_RES = 100.0;

const int G_BUFFER_LENGTH = 6; //最小二乗法のバッファ範囲
// const int FILTER_RANGE_MAX = 6;

const int CUR_INDEX = G_BUFFER_LENGTH - 1;
const int PRE_INDEX = G_BUFFER_LENGTH - 2;
const int PREPRE_INDEX = G_BUFFER_LENGTH - 3;


// const MAX_SPEED 500 //フィルタを更新するorしないのしきい値（mm）//5Hz
// const double MAX_SPEED = 150.0; //フィルタを更新するorしないのしきい値（mm）//10Hzのとき3m/s
const double MAX_SPEED = 150.0; //フィルタを更新するorしないのしきい値（mm）//3.0m/s
const double MIN_SPEED = 15.0; //0.15m/s
const double V_MAX_TH = 3000.0; //3000mm/s以上で移動する人は観測しない

const int TANZAKU_NUM_MAX = 30; //短冊の個数

const int BORDER_NUM_MAX = 39;
// const int TANZAKU_NUM_MAX = 40;

//開閉判定用
const int FRAME_OBSERVE_MIN = 4;
const int FRAME_OBSERVE = 10; //0.5秒

//-ドア-//
// const double DOOR_W_MIN = 200.0; //これ以下だったら開けない
// const double door_v_slow = 1000.0; //単位はmm/sec、両開き
// const double door_v_high = 1200.0; //単位はmm/sec、両開き
// const double DOOR_W_HALF = 650.0; //単位はmm
// const double DOOR_W_FULL = 1080.0; //暫定
// const int REQUIRED_FRAME_SLOW_HALF = 13; //ゆっくりドアを開くときの所要フレーム数（半開き） //10Hz
// const int REQUIRED_FRAME_SLOW_FULL = 21;
// const int REQUIRED_FRAME_HIGH_HALF = 11; //素早くドアを開くときの所要フレーム数 （半開き）//10Hz
// const int REQUIRED_FRAME_HIGH_FULL = 18;

const double DOOR_W_MIN = 200.0; //これ以下だったら開けない
const double door_v_slow = 700.0; //単位はmm/sec、両開き
const double door_v_high = 1000.0; //単位はmm/sec、両開き
const double DOOR_W_HALF = 700.0; //単位はmm
const double DOOR_W_FULL = 1080.0; //暫定
const int REQUIRED_FRAME_SLOW_HALF = 20; //ゆっくりドアを開くときの所要フレーム数（半開き） //10Hz
const int REQUIRED_FRAME_SLOW_FULL = 31;
const int REQUIRED_FRAME_HIGH_HALF = 14; //素早くドアを開くときの所要フレーム数 （半開き）//10Hz
const int REQUIRED_FRAME_HIGH_FULL = 22;

const double DOOR_W_TH = 800.0; //一人or複数人の判定用

// const int MARGIN = 0;
// const int MARGIN = 6;
// const int MARGIN = 12;
const int MARGIN = 20;

const int FRAME_TH_OPEN_MODE = 60; //frame_arrivalがこれ以上の短冊はopen_modeの判定に使わない

//lane
const double LANE_W = 100.0;
const int LANE_NUM_MAX = 30;
const double LANE_CELL_RES = 100.0;
const double LANE_LENGTH = 2600;
const int LANE_CELL_NUM_MAX = 52;
const int LANEL_CELL_NUM_START = 30; //30→0 //+4
const int LANER_CELL_NUM_START = 22; //22→52 //-4
const int TARGET_LANE_RANGE = 2;
const int DECISION_TIME_OF_PASSING = 10; //このフレーム以上レーンの上を進んでいたら素通り