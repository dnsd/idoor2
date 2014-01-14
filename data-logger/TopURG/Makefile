CC      = gcc -o
INC_DIR = -I/usr/local/include
LIB_DIR = -L/usr/local/lib
OPT    = -lpthread -lssm -lm -lscip2awd


urg2ssm:urg2ssm.cpp LRF.h
	g++ -o $@ $^ $(LIB_DIR) $(LIB_DIR) $(OPT)

TopURG2gnu:TopURG2gnu.cpp
	g++ -o TopURG2gnu TopURG2gnu.cpp -lscip2awd -lm -lpthread -lssm

ssm2csv_for_TopURG:ssm2csv_for_TopURG.cpp
	g++ -o ssm2csv_for_TopURG ssm2csv_for_TopURG.cpp -lm -lssm

clean:
	rm ssm2plot urg2ssm3D urg2ssmR urg2ssmL urg2ssmB
