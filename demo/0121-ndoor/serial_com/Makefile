INC_DIR = -I/usr/local/include
LIB_DIR = -L/usr/local/lib
OPT    = -lpthread -lssm -lm -lscip2awd

urg2ssm:urg2ssm.cpp
	g++ -o $@ $^ $(LIB_DIR) $(LIB_DIR) $(OPT)
ped_detection:ped_detection.cpp
	g++ -o ped_detection ped_detection.cpp Step.cpp mystd.cpp -lssm -lm
ctrl_ssm-logger:ctrl_ssm-logger.cpp
	g++ -o ctrl_ssm-logger ctrl_ssm-logger.cpp Step.cpp mystd.cpp -lssm -lm -lpthread
logger_test:logger_test.cpp
	g++ -o logger_test logger_test.cpp mystd.cpp -lssm -lm
ctrl_camera:ctrl_camera.cpp
	g++ -o ctrl_camera ctrl_camera.cpp mystd.cpp -I/usr/local/include/opencv2 -I/usr/local/include/opencv -L/usr/local/lib -lopencv_highgui -lopencv_core -lssm -lm
TopURG2gnu:TopURG2gnu.cpp
	g++ -o TopURG2gnu TopURG2gnu.cpp -lscip2awd -lm -lpthread -lssm
door_ctrl:door_ctrl.cpp
	g++ -o door_ctrl door_ctrl.cpp mystd.cpp -lm -lssm
serial_test:serial_test.cpp
	g++ -o serial_test serial_test.cpp mystd.cpp -lm -lssm
get_door_data:get_door_data.cpp
	g++ -o get_door_data get_door_data.cpp mystd.cpp -lm -lssm
