#include "fault.h"

#define DEVICE_PATH	"/mnt/usb/"
#define USB_DEVICE	"/dev/sda1"
#define OTG_DEVICE	"/dev/sdb1"
#define DEBUG	0

#if (DEBUG)
#define DBG_MSG(msg...)	{ printf("[usb test]: " msg); }
#else
#define DBG_MSG(msg...)	do{} while(0);
#endif


pthread_t	usb_mthread[THREAD_NUM];
int		result[THREAD_NUM];
int		usb_exit_thread;

int mount_usb(void)
{
	int ret = 0;
	mkdir("/mnt/usb", 0755);
	ret = system("mount /dev/sda1 /mnt/usb");

	return ret;
}

int enable_host(int pwr)
{
	int cnt = 500;
	if(gpio_export(pwr))
	{
		printf("gpio open fail\n");
		return -1;
	}
	gpio_dir_out(pwr);
	gpio_set_value(pwr, 1);

	while(cnt--)
	{
		usleep(10000);
		if (!access(USB_DEVICE, F_OK))
			return 0;
	}
	return -1;

}

int enable_otg_host(void)
{
	int otg_loop_pwr = 19;
	int otg_host_test = 20;
	int cnt = 500;

	if(gpio_export(otg_loop_pwr))
	{
		printf("gpio open fail\n");
		return -1;
	}
	gpio_dir_out(otg_loop_pwr);
	gpio_set_value(otg_loop_pwr, 1);

	if(gpio_export(otg_host_test))
	{
		printf("gpio open fail2\n");
		return -1;
	}
	gpio_dir_out(otg_host_test);
	gpio_set_value(otg_host_test, 1);

	while(cnt--)
	{
		usleep(10000);
		if (!access(OTG_DEVICE, F_OK))
			return 0;
	}
	return -1;

}


static int usb_rw_test(unsigned char *w, unsigned char *r, int size, int index )
{
	int ret;
	int fd;
	char filepath[256];
	sprintf( filepath, "%s%s%d.dat",DEVICE_PATH, "rw_test", index );
	fd = open( filepath, O_CREAT|O_RDWR|O_SYNC );
	if(fd <0)
	{
		printf("open err (for write)\n ");
		return -1;
	}
	lseek(fd,512,0);
	ret = write(fd,w,size);
	close(fd);

	fd = open( filepath, O_CREAT|O_RDWR|O_SYNC );
	if( fd < 0 )
	{
		printf("open err (for read)\n");
		return -1;
	}
	lseek(fd,512,0);
	ret = read(fd,r,size);
	ret = memcmp(w,r,size);
	close(fd);
	return ret;
}

static void *usb_test_thread(int id)
{
	int size = TEST_SIZE;
	int i = 0, ret = -1 ;
	unsigned char *wbuf = NULL;
	unsigned char *rbuf = NULL;

	wbuf = malloc(TEST_SIZE);
	rbuf = malloc(TEST_SIZE);

	if(wbuf == NULL  || rbuf== NULL)
	{
		printf("Alloc buffer error\n");
		if(wbuf)
			free(wbuf);
		if(rbuf)
			free(rbuf);
		return (int *)-1;
	}

	srand(time(NULL));
	for(i=0;i<size;i++)
	{
		wbuf[i] = rand()%0xff;
#if (DEBUG)
		if(i% 16 ==0)
			printf("\n");
		printf("%2x ",wbuf[i] );
#endif
	}

	while (!usb_exit_thread)
	{
		memset(rbuf , 0, TEST_SIZE);      //      Clear Read Buffer
		ret = usb_rw_test(wbuf, rbuf, TEST_SIZE , id);
		if(ret != 0)
		{
			printf("usb test fail\n");
			result[id] = SLT_RES_ERR;
			goto out;
		}
	}
	printf("USB: end of thread (%d)\n", id);

	return 0;
out:
	free(wbuf);
	free(rbuf);
	return -1 ;
}

int usb_test_run(void)
{
	int i = 0, ret = 0;
	usb_exit_thread = 0;
	ret = mount_usb();
	if (ret) {
		printf("usb mount fail\n");
		return -1;
	}

	for (i = 0; i < THREAD_NUM; i++) {
		if( pthread_create(&usb_mthread[i], NULL,
					usb_test_thread, i ) < 0 ) {
			printf(" usb test fail\n");
			return -1;
		}
	}
	return 0;
}

int usb_stop(void)
{
	int32_t i;
	usb_exit_thread = 1;
	for( i = 0 ; i < THREAD_NUM; i++ )
	{
		pthread_join(usb_mthread[i], NULL);
	}
	return SLT_RES_OK;
}

int usb_status(void)
{
	int32_t i, count = 0;
	for( i = 0; i < THREAD_NUM; i++ )
	{
		if( result[i] < 0 )
			return SLT_RES_ERR;
		else if( SLT_RES_OK == result[i] )
			count ++;
	}

	if(count == THREAD_NUM) {
		return SLT_RES_OK;
	}
	else {
		printf("ERR\n");
		return SLT_RES_TESTING;
	}
}

