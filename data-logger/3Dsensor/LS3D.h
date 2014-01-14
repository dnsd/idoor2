#define STEP_NUM 2720

typedef struct{
    char det; //UorD
    double dist[STEP_NUM];
    double x[STEP_NUM];
    double y[STEP_NUM];
    double z[STEP_NUM];
}LS3D;

typedef struct{
    int status; //0のときclose,1の時open
}STATUS;

typedef struct{
    char det; //UorD
    double dist[STEP_NUM]; //[beam_num]
}BEAM;
