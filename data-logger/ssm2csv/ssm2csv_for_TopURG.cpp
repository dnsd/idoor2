#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#include "LRF.h"
#include <ssm.h>
#include <ssm.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

double utime;

//-SSM-//
SSMApi<LRF> SCAN_DATA("Top-URG", 0);

int main( int aArgc, char **appArgv )
{
	// SSM
	initSSM();
	SCAN_DATA.open(SSM_READ);

	// ファイル操作
	ofstream ofs;
	ofs.open("TopURG_DATA.csv");
	cout.precision(16);
	ofs.precision(16);

	while(1)
	{
		if (SCAN_DATA.readNew())
		{
			ofs << SCAN_DATA.time << "," << SCAN_DATA.data.dist[540] << endl;
		}
		usleep(10000); //0.01秒
	}

	SCAN_DATA.close();
	endSSM();
	return 0;
}