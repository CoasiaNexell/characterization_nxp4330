#include "fault.h"

#define UP_FREQ		1400000
#define UP_VOL		1350000
#define DOWN_FREQ	400000
#define DOWN_VOL	1075000

#define MARGIN_VOL_PREV	75000
#define MARGIN_VOL	12500

#define CPUFREQ_PATH	"/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed"
#define REGULATOR_PATH	"/sys/class/regulator/regulator"


#if (DEBUG)
#define DBG_MSG(msg...)	{ printf("[dvfs test]: " msg); }
#else
#define DBG_MSG(msg...)	do{} while(0);
#endif
#define CPU_TEST_SIZE 10 * KBYTE * KBYTE

pthread_t	dvfs_mthread;
int		dvfs_exit_thread;
int		select_dvfs = 0;

/*
 *	sys APIs
 */
int sys_read(const char *file, char *buffer, int buflen)
{
	int fd, ret;

	if (0 != access(file, F_OK)) {
		printf("cannot access file (%s).\n", file);
		return -errno;
	}

	fd = open(file, O_RDONLY);
	if (0 > fd) {
		printf("cannot open file (%s).\n", file);
		return -errno;
	}

	ret = read(fd, buffer, buflen);
	if (0 > ret) {
		printf("failed, read (file=%s, data=%s)\n", file, buffer);
		close(fd);
		return -errno;
	}

	close(fd);

	return ret;
}

int sys_write(const char *file, const char *buffer, int buflen)
{
	int fd;

	if (0 != access(file, F_OK)) {
		printf("cannot access file (%s).\n", file);
		return -errno;
	}

	fd = open(file, O_RDWR|O_SYNC);
	if (0 > fd) {
		printf("cannot open file (%s).\n", file);
		return -errno;
	}

	if (0 > write(fd, buffer, buflen)) {
		printf("failed, write (file=%s, data=%s)\n", file, buffer);
		close(fd);
		return -errno;
	}
	close(fd);

	return 0;
}

int set_cpu_speed(long khz)
{
	char data[128];
	sprintf(data, "%ld", khz);
	return sys_write(CPUFREQ_PATH, data, strlen(data));
}

int set_cpu_voltage(int id, long uV)
{
	char file[128];
	char data[32];
	unsigned int off_uV = uV/100;

	/* Cut off 100 micro volt. */
	off_uV = off_uV*100;

	sprintf(file, "%s.%d/%s", REGULATOR_PATH, id, "microvolts");
	sprintf(data, "%ld", uV);

	return sys_write(file, data, strlen(data));
}

static void *dvfs_test_thread(int id)
{
	int md_voltage = MARGIN_VOL;
	while (!dvfs_exit_thread) {
		if( select_dvfs == 0 )
		{
			set_cpu_voltage(1, UP_VOL - MARGIN_VOL);
			set_cpu_speed(1400000);
			usleep(120000);
			set_cpu_speed(800000);
			set_cpu_voltage(1, DOWN_VOL - MARGIN_VOL);
			usleep(10000);
		}
		else
		{
			set_cpu_voltage(1, UP_VOL - MARGIN_VOL_PREV - MARGIN_VOL);
			set_cpu_speed(1400000);
			usleep(100000);
			set_cpu_speed(800000);
			set_cpu_voltage(1, DOWN_VOL - MARGIN_VOL_PREV);
			usleep(100000);
		}
	}

}

int dvfs_test_run(void)
{
	int i = 0;

	dvfs_exit_thread = 0;
	if( pthread_create(&dvfs_mthread, NULL, dvfs_test_thread, i ) < 0 ) {
		return -1;
	}
	return 0;
}

int dvfs_stop(void)
{
	int32_t i;
	dvfs_exit_thread = 1;
	pthread_join(dvfs_mthread, NULL);

	set_cpu_speed(800000);
	set_cpu_voltage(1, DOWN_VOL);

	return SLT_RES_OK;
}

