// tanzaku_and_edge

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <ssm.hpp>
#include <cmath>
#include "send_spotinfo.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <iterator>

using namespace std;

// debug
void judge_open_mode(int open_mode_tan[], bool cancel_flag[], bool B_flag, int& open_mode)
{
	//-初期化-//
	open_mode = 0;

	//-open_modeの判定と出力-//
	if (MODE_E == true && MODE_B == false)
	{
		for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
		{
		    if (cancel_flag[tan_num] == false)
		    {
		    	if (open_mode_tan[tan_num] == 4)
			    {
			        open_mode = 4;
			        break;
			    }else if (open_mode_tan[tan_num] == 2)
			    {
			        open_mode = 2;
			        break;
			    }else if (open_mode_tan[tan_num] == 3)
			    {
			        open_mode = 3;
			    }else if (open_mode_tan[tan_num] == 1)
			    {
			        open_mode = 1;
			    }
			}	
		}    
	}

	if (MODE_E == true && MODE_B == true)
	{
		if (B_flag == true)
		{
			open_mode = 2;
		}else{
			for (int tan_num = 0; tan_num < TANZAKU_NUM_MAX; tan_num++)
			{
			    if (cancel_flag[tan_num] == false)
			    {
			    	if (open_mode_tan[tan_num] == 4)
				    {
				        open_mode = 4;
				        break;
				    }else if (open_mode_tan[tan_num] == 2)
				    {
				        open_mode = 2;
				        break;
				    }else if (open_mode_tan[tan_num] == 3)
				    {
				        open_mode = 3;
				    }else if (open_mode_tan[tan_num] == 1)
				    {
				        open_mode = 1;
				    }
				}	
			}
		}
	}
}

