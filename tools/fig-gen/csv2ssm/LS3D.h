typedef struct{
    char det; //UorD
    double dist[2720];
    double x[2720];
    double y[2720];
    double z[2720];
}LS3D;

typedef struct{
    int status; //0のときclose,1の時open
}STATUS;

typedef struct{
    char det; //UorD
    double dist[2720]; //[beam_num]
}BEAM;

