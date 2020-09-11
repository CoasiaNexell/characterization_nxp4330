#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sched.h>		/* schedule */
#include <sys/resource.h>
#include <linux/sched.h>	/* SCHED_NORMAL, SCHED_FIFO, SCHED_RR, SCHED_BATCH */
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>		/* stat */
#include <sys/vfs.h>		/* statfs */
#include <errno.h>		/* error */
#include <sys/time.h>		/* gettimeofday() */
#include <sys/times.h>		/* struct tms */
#include <time.h>		/* ETIMEDOUT */
#include <mntent.h>
#include <sys/mount.h>
#include <pthread.h>

#define RETRYCNT	200;
#define KBYTE		(1024)
#define MBYTE		(1024 * KBYTE)
#define GBYTE		(1024 * MBYTE)

#define DEBUG	0

#if (DEBUG)
#define DBG_MSG(msg...)	{ printf("[mmc test]: " msg); }
#else
#define DBG_MSG(msg...)	do{} while(0);
#endif


#define TEST_MMC 	1
#define TEST_USB 	1
#define TEST_CPU 	1
#define TEST_MEM 	1
#define TEST_BURN 	1
#define TEST_DVFS	1

#define	NUM_OF_CORE	4
#define TEST_NUM	3
#define THREAD_NUM	NUM_OF_CORE * TEST_NUM
#define TEST_SIZE	(128 * KBYTE)

typedef enum{
	SLT_RES_ERR     = -1,
	SLT_RES_OK      = 0,
	SLT_RES_TESTING = 1
} SLT_RESULT;


extern int mmc_test_run(void);
extern int mmc_status(void);
extern int mmc_stop(void);
extern int usb_test_run(void);
extern int usb_status(void);
extern int usb_stop(void);
extern int cpu_test_run(void);
extern int cpu_status(void);
extern int cpu_stop(void);
extern int mount_usb(void);

extern int mem_test_run(void);
extern int mem_status(void);
extern int mem_stop(void);

extern int dvfs_test_run(void);
extern int dvfs_stop(void);
extern int burn_stop(void);
extern int select_dvfs;

void print_usage(void)
{
	printf("usage: options\n"
		   " -h print this message\n"
	);
}

static uint64_t get_tick_count( void )
{
	uint64_t ret;
	struct timeval  tv;
	gettimeofday( &tv, NULL );
	ret = ((uint64_t)tv.tv_sec)*1000000 + tv.tv_usec;
	return ret;
}

int once = 0;
int cpuburn_started = 0;

int main(int argc, char **argv)
{
	int opt, ret = 0;
	uint64_t end, start;
	uint64_t testTime = 30 * 1000000;	/* 30 Sec */

	while(-1 != (opt = getopt(argc, argv, "hp:d:s:"))) {
		switch(opt) {
		 	 case 'h':   print_usage(); 	exit(0);
		}
	}


	select_dvfs = 1;
    cpuburn_started = 0;
	printf("TEST START \n");
	start = get_tick_count();

#if (TEST_MMC)
	ret = mmc_test_run();
	if (ret < 0) {
		printf("mmc test fail\n");
		ret = -1;
		goto out;
	}
#endif
#if (TEST_USB)
	ret = enable_host(17);
	if (ret < 0) {
		printf("usb test fail\n");
		ret = -1;
		goto out;
	}
	ret = usb_test_run();
	if (ret < 0) {
		printf("usb test fail\n");
		ret = -1;
		goto out;
	}
#endif
#if (TEST_CPU)
	ret = cpu_test_run();
	if (ret < 0) {
		printf("cpu test fail\n");
		ret = -1;
		goto out;
	}
#endif
#if (TEST_MEM)
	ret = mem_test_run();
	if (ret < 0) {
		printf("mem test fail\n");
		ret = -1;
		goto out;
	}
#endif

#if (TEST_DVFS)
	dvfs_test_run();
#endif

	while (1) {
#if (TEST_MMC)
		if( SLT_RES_ERR == mmc_status()) {
			printf("mmc test err \n");
			ret = -1;
			break;
		}
#endif
#if (TEST_USB)
		if( SLT_RES_ERR == usb_status()) {
			printf("usb test err \n");
			ret = -1;
			break;
		}
#endif
#if (TEST_CPU)
		if( SLT_RES_ERR == cpu_status()) {
			printf("cpu test err \n");
			ret = -1;
			break;
		}
#endif
#if (TEST_MEM)
		if( SLT_RES_ERR == mem_status()) {
			printf("mem test err \n");
			ret = -1;
			break;
		}
#endif
#if (TEST_BURN)
		if( cpuburn_started == 1)
		{
			if( SLT_RES_ERR == burn_status()) {
				printf("mem test err \n");
				ret = -1;
				break;
			}
		}
#endif

		end = get_tick_count();
		if( (end - start) > testTime )
			break;
		usleep( 1000000 );
		if( (end - start) > 20000000 )
		{
            if ( once == 0 )
            {
#if (TEST_BURN)
				ret = burn_test_run();
				if (ret < 0) {
					printf("burn test fail\n");
					ret = -1;
					goto out;
				}
                cpuburn_started = 1;
#endif
                select_dvfs = 0;
                once = 1;
            }
		}
	}

#if (TEST_MMC)
	mmc_stop();
#endif

#if (TEST_USB)
	usb_stop();
#endif
#if (TEST_CPU)
	cpu_stop();
#endif
#if (TEST_MEM)
	mem_stop();
#endif
#if (TEST_DVFS)
	dvfs_stop();
#endif
#if (TEST_BURN)
    burn_stop();
#endif

	if (!ret) {
		printf("\n\e[32m============================\e[0m\n");
		printf("\e[32m TEST OK \e[0m\n");
		printf("\e[32m============================\e[0m\n");
	} else {
		printf("\n\e[31m============================\e[0m\n");
		printf("\e[31m TEST FAIL \e[0m\n");
		printf("\e[31m============================\e[0m\n");
	}


out:
	return ret;
}
