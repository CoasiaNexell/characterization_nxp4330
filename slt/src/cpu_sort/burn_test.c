#include <zlib.h>
#include "fault.h"

#define DEVICE_PATH	"/mnt/mmc/"
#define DEBUG	0

#if (DEBUG)
#define DBG_MSG(msg...)	{ printf("[mmc test]: " msg); }
#else
#define DBG_MSG(msg...)	do{} while(0);
#endif
#define CPU_TEST_SIZE 10 * KBYTE * KBYTE

#define THREAD_NUM_BURN 4

pthread_t	burn_mthread[THREAD_NUM_BURN];
int		result[THREAD_NUM_BURN];
int		burn_exit_thread;

static void *burn_test_thread(int id)
{
	int ret = 0;

	ret = system("/usr/local/cpuburn & \n");
	if(ret)
		result[id]= SLT_RES_ERR;
}

int burn_test_run(void)
{
	int i = 0;
	burn_exit_thread = 0;
	for (i = 0; i < THREAD_NUM_BURN; i++) {
		if( pthread_create(&burn_mthread[i], NULL,
					burn_test_thread, i ) < 0 ) {
			printf(" burn test fail\n");
			return -1;
		}
	}
	return 0;
}

int burn_stop(void)
{
	int32_t i;
	system("pkill cpuburn");
	burn_exit_thread = 1;
	for( i = 0 ; i < THREAD_NUM_BURN; i++ )
	{
		pthread_join(burn_mthread[i], NULL);
	}
	return SLT_RES_OK;
}

int burn_status(void)
{
	int32_t i, count = 0;
	for( i = 0; i < THREAD_NUM_BURN; i++ )
	{
		if( result[i] < 0 )
			return SLT_RES_ERR;
		else if( SLT_RES_OK == result[i] )
			count ++;
	}

	if(count == THREAD_NUM_BURN) {
		return SLT_RES_OK;
	}
	else {
		printf("MEM ERR\n");
		system("pkill burntester\n");
		return SLT_RES_TESTING;
	}
}

