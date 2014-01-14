/* URGから得られたスキャンデータをSSMに書き込む                  */
/* Author Suehiro lab @ Department of Inteligent system  */
/* Refer to Atushi Watanabe                              */
/* Date 2010 05 10                                       */

/**
  @file libscip2awd_test.c
  @brief Test program for libscip2
  @author Atsushi WATANEBE <atusi_w@roboken.esys.tsukuba.ac.jp> <atusi_w@doramanjyu.com>
 */

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#include <scip2awd.h>
#include "LRF.h"
#include <ssm.h>
#include <math.h>

using namespace std;

//#define USE_GS


/** Flag */
int gShutoff;

/**
  @brief Ctrl+C trap
  @param aN not used
 */
void ctrlc( int aN )
{
  /* It is recommended to stop URG outputting data. */
  /* or cost much time to start communication next time. */
  gShutoff = 1;
  signal( SIGINT, NULL );
}

/*
   @brief Main function
   @param aArgc Number of Arguments 
   @param appArgv Arguments
   @return failed: 0, succeeded: 1 
*/
int main( int aArgc, char **appArgv )
{
  /* URG2SSM */
  LRF SCAN_DATA;
  initSSM();
  SSM_sid LRF_sid;
  double utime;
  LRF_sid=createSSM_time("Top-URG",1,sizeof(LRF),0.5,0.025);
  
  S2Port *port; // Device Prot
  S2Sdd_t buf; // Data recive dual buffer
  S2Scan_t *data; // Pointer to data buffer
  int j; // Loop valiant
  int ret; // Returned value
  struct timeval tm; // Local time

 /*
  if( aArgc != 2 ){
    fprintf( stderr, "USAGE: %s device\n", appArgv[0] );
    return 0;
  }
  */

  /* Start trapping ctrl+c signal */
  gShutoff = 0;
  signal( SIGINT, ctrlc );
  
  /* Open the port */
  if(aArgc<2)
    port = Scip2_Open( "/dev/ttyACM0", B115200 );
  if(aArgc == 2)
    port = Scip2_Open( appArgv[1], B115200 );
  if( port == 0 )
    {
      fprintf( stderr, "ERROR: Failed to open device.\n" );
      return 0;
    }
  printf("Port opened\n");

  /* Initialize buffer before getting scanned data */
  S2Sdd_Init( &buf );
  printf("Buffer initialized\n");

  /* Demand sending me scanned data */
  /* Data will be recived in another thread  */
  /* MS command */
  Scip2CMD_StartMS( port, 0, 1080, 1, 0, 0, &buf, SCIP2_ENC_3BYTE );

  //SSMのLRF_RANGE_DATA初期化
  for(j=0;j<1080;j++)
    {
      SCAN_DATA.dist[j]=0.0;
      SCAN_DATA.x[j]=0.0;
      SCAN_DATA.y[j]=0.0;
    }

  while( !gShutoff )
    {
      /* Start using scanned data */
      utime=gettimeSSM();
      ret = S2Sdd_Begin( &buf, &data );
      /* Returned value is 0 when buffer is having been used now */
      if( ret > 0 )
    	{
    	  /* You can start reciving next data after S2Sdd_Begin succeeded */
    	  gettimeofday( &tm, NULL );
    	  /* then you can analyze data in parallel with reciving next data */
    	  
    	  /* ---- analyze data ---- */
    	  gettimeofday( &tm, NULL );
    	  
    	  //SSMに書き込む
    	  for( j = 0; j < data->size; j++ )
    	    {
    	      // SCAN_DATA.dist[j] = (double)data->data[j]/1000.0; //[m]
            SCAN_DATA.dist[j] = (double)data->data[j];
    	      SCAN_DATA.x[j] = SCAN_DATA.dist[j]*cos((-135+0.25*j)*(M_PI/180));
    	      SCAN_DATA.y[j] = SCAN_DATA.dist[j]*sin((-135+0.25*j)*(M_PI/180));
    	    }
    	  writeSSM(LRF_sid, (char*)&SCAN_DATA, utime);
    	  //正面の距離データだけ表示
    	  // printf( "Front Dist [m] =  %lf\n", SCAN_DATA.dist[data->size/2]);
        cout << "Front Dist [mm] = " << SCAN_DATA.dist[data->size/2] << endl;
    	  usleep(90000); // 0.09秒
    	  /* Don't forget S2Sdd_End to unlock buffer */
    	  S2Sdd_End( &buf );
    	}
      else if( ret == -1 )
    	{
    	  fprintf( stderr, "ERROR: Fatal error occurred.\n" );
    	  Scip2_Close( port );
    	  printf("Port closed\n");
    	  /* Open the port */
    	  if(aArgc<2)
    	    port = Scip2_Open( "/dev/ttyACM0", B115200 );
    	  if(aArgc == 2)
    	    port = Scip2_Open( appArgv[1], B115200 );
    	  if( port == 0 )
    	    {
    	      fprintf( stderr, "ERROR: Failed to open device.\n" );
    	      return 0;
    	    }
    	  printf("Port opened\n");
    	  /* Initialize buffer before getting scanned data */
    	  S2Sdd_Init( &buf );
    	  printf("Buffer initialized\n");
    	  
    	  /* Demand sending me scanned data */
    	  /* Data will be recived in another thread  */
    	  /* MS command */
    	  Scip2CMD_StartMS( port, 0, 1080, 1, 0, 0, &buf, SCIP2_ENC_3BYTE );
    	  continue;
    	  //break;
    	}
      else
    	{
    	  usleep(100);
    	}
    }
  printf( "\nStopping\n" );
  
  
  ret = Scip2CMD_StopMS( port, &buf );
  if( ret == 0 )
    {
      fprintf( stderr, "ERROR: StopMS failed.\n" );
      return 0;
    }

  printf("Stopped\n");
  
  /* Destruct buffer */
  S2Sdd_Dest( &buf );
  printf("Buffer destructed\n");
  
  /* Close port */
  Scip2_Close( port );
  printf("Port closed\n");
  
  return 1;
}

