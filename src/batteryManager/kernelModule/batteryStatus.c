#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE_NAME "/dev/chr_dev" // 디바이스 파일

// argv 값을 받아 디바이스 파일의 IOCTL cmd 값으로 사용
int main(int argc, char *argv[])
{
	int device;
	char wbuf[128] = "Write buffer data";
	char rbuf[128] = "Read buffer data";
	int n = atoi(argv[1]);

	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY);

	if (device >= 0)
	{
		printf("Device file Open\n");
		ioctl(DEVICE_FILE_NAME, device, n); 			// argv 값을 디바이스 파일에 cmd 값으로 전달
		write(device, wbuf, 10); 		// wbuf 값을 디바이스 파일에 전달
		printf("Write value is %s\n", wbuf);
		read(device, rbuf, 10);
		printf("read value is %s\n", rbuf);
	}
	else
	{
		printf("Device file open fail\n");
	}
	return 0;
}
