//10Hz

//モード設定（デバッグ用）
const bool MODE_B = false;
const bool MODE_D = false;
const bool MODE_E = true;


//センサスペック
const int STEP_NUM = 2720;
const double SENSOR_HEIGHT = 2650;
const double FREQ = 0.05; //単位は秒

//スポットの寸法
const int spot_res = 100; //ひとつのスポットの辺の長さ
const int spot_num_y = 44; //スポットの数（よこ）
const int spot_num_x = 30; //スポットの数（たて）

//B判定用
const int stop_th = 10; //このフレーム数以上物体があったらBとする。

//CF判定用
// const DIST_DIF_TH 50.0 //5Hz
const double DIST_DIF_TH = 50.0; //10Hz
const int CF_TH = 1; //スポット内にしきい値以上comeまたはbackの点があればC,Fを出力する

//D判定用
const int D_TH = 5; //Cの数がこれ以上だったらD判定を出す（ドアを開ける）

//人間
const double Ha = 1200.0; //これ以上高いものを大人とする
const double Hw = 300.0; //これより低いものを台車（ワゴン）とする //ワゴンと幅の「w」がかぶってる

//tanzaku
const int his_num = 30;
const double his_res = 100.0;
// const int his_num = 15;
// const double his_res = 200.0;

const int G_BUFFER_LENGTH = 6; //最小二乗法のバッファ範囲
const int FILTER_RANGE_MAX = 6;
// const FILTER_TH 500 //フィルタを更新するorしないのしきい値（mm）//5Hz
const double FILTER_TH = 150.0; //フィルタを更新するorしないのしきい値（mm）//10Hzのとき3m/s
const double V_MAX_TH = 3000.0; //3000mm/s以上で移動する人は観測しない

const int TILTED_LINE_NUM = 29;
const int TANZAKU_NUM = 29;

const int TIMEZONE_NUM = 26; //タイムゾーンの数

//開閉判定用
const int LANE_NUM = 30; //横のレーンの数
const double LANE_RES = 100.0; //横のレーンの幅

//-ドア-//
const double DOOR_W_MIN = 200.0; //これ以下だったら開けない
const double door_v_slow = 800.0; //単位はmm/sec、両開き
const double door_v_high = 1000.0; //単位はmm/sec、両開き
const double DOOR_W_HALF = 600.0; //単位はmm
const double DOOR_W_FULL = 1200.0; //暫定

// const REQUIRED_FRAME_SLOW_HALF 7 //5Hz
// const REQUIRED_FRAME_SLOW_FULL 14 //5Hz
// const REQUIRED_FRAME_HIGH_HALF 6 //5Hz
// const REQUIRED_FRAME_HIGH_FULL 12 //5Hz

// const int REQUIRED_FRAME_SLOW_HALF = 14; //ゆっくりドアを開くときの所要フレーム数（半開き） //10Hz
const int REQUIRED_FRAME_SLOW_HALF = 18; //ゆっくりドアを開くときの所要フレーム数（半開き） //10Hz
const int REQUIRED_FRAME_SLOW_FULL = 28;
const int REQUIRED_FRAME_HIGH_HALF = 12; //素早くドアを開くときの所要フレーム数 （半開き）//10Hz
const int REQUIRED_FRAME_HIGH_FULL = 24;

// const frame_observe 4 //観測フレーム数
// const int FRAME_OBSERVE_APPROACH = 6; //観測フレーム数
// const int FRAME_OBSERVE_CROSS = 15; //観測フレーム数
// const frame_observe_3 20 //観測フレーム数

const double X_TH_FRAME_OBSERVE = 1200.0; //短冊が初めて物体を検出したときのG_xがこれ以下のときはframe_observeを大きくする（単位はmm）

const int FRAME_TH_OPEN_MODE = 60; //frame_arrivalがこれ以上の短冊はopen_modeの判定に使わない
