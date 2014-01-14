CC      = gcc -o
INC_DIR = -I/usr/local/include
LIB_DIR = -L/usr/local/lib
OPT    = -lpthread -lssm -lm -lscip2awd


SimpleURG2ssm:SimpleURG2ssm.cpp LRF.h
	g++ -o $@ $^ $(LIB_DIR) $(LIB_DIR) $(OPT)

SimpleURG2gnu:SimpleURG2gnu.cpp
	g++ -o SimpleURG2gnu SimpleURG2gnu.cpp -lscip2awd -lm -lpthread -lssm

ssm2csv_for_SimpleURG:ssm2csv_for_SimpleURG.cpp
	g++ -o ssm2csv_for_SimpleURG ssm2csv_for_SimpleURG.cpp -lm -lssm

clean:
	rm ssm2plot urg2ssm3D urg2ssmR urg2ssmL urg2ssmB
