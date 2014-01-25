main.o:main.cpp
	g++ -c main.cpp
CF.o:CF.cpp
	g++ -c CF.cpp
GHI.o:GHI.cpp
	g++ -c GHI.cpp
send_spotinfo:main.cpp CF.cpp GHI.cpp
	g++ -g -o send_spotinfo main.cpp CF.cpp GHI.cpp B.cpp D.cpp tanzaku.cpp log_ctr.cpp lane.cpp open.cpp -lssm -lm
spotR_CF:spotR_CF.cpp
	g++ -o spotR_CF spotR_CF.cpp -lglut -lGLU -lGL -lssm
spotR_BCF:spotR_BCF.cpp
	g++ -o spotR_BCF spotR_BCF.cpp -lglut -lGLU -lGL -lssm
spotR_CDF:spotR_CDF.cpp
	g++ -o spotR_CDF spotR_CDF.cpp -lglut -lGLU -lGL -lssm
spotR_D:spotR_D.cpp
	g++ -o spotR_D spotR_D.cpp -lglut -lGLU -lGL -lssm
spotR_E:spotR_E.cpp
	g++ -o spotR_E spotR_E.cpp -lglut -lGLU -lGL -lssm
spotR_GHI:spotR_GHI.cpp
	g++ -o spotR_GHI spotR_GHI.cpp -lglut -lGLU -lGL -lssm
spotR_ABCDEF:spotR_ABCDEF.cpp
	g++ -o spotR_ABCDEF spotR_ABCDEF.cpp -lglut -lGLU -lGL -lssm
cal_tanzaku_fac:cal_tanzaku_fac.cpp
	g++ -o cal_tanzaku_fac cal_tanzaku_fac.cpp -lglut -lGLU -lGL -lm
cal_w1:cal_w1.cpp
	g++ -o cal_w1 cal_w1.cpp -lglut -lGLU -lGL -lm
cal_wn:cal_wn.cpp
	g++ -o cal_wn cal_wn.cpp -lglut -lGLU -lGL -lm
back:back.cpp
	g++ -o back back.cpp -lssm -lm
objectR:objectR.cpp
	g++ -o objectR objectR.cpp -lglut -lGLU -lGL -lssm
simu:simu.cpp
	g++ -o simu simu.cpp -lm -lssm
cal_timezone_info:cal_timezone_info.cpp
	g++ -o cal_timezone_info cal_timezone_info.cpp -lglut -lGLU -lGL -lm -lssm
