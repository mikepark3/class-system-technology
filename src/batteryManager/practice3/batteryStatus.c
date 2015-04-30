#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE_NAME "/dev/chr_dev" // 디바이스 파일

int main(int argc, char *argv[])
{
	int device, input = 1, output;

	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY);

	if (device >= 0)
	{
		printf("Device file open\n");
		while(input >=1 && input <=100)
		{
			scanf("%d", &input);
			write(device, &input, sizeof(input)); 
			printf("Write value is %d\n", input);

			read(device, &output, sizeof(output));
			printf("Read value is %d\n", output);
		}
	}
	else
	{
		printf("Device file open fail\n");
	}
	return 0;
}
