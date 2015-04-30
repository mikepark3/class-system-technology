#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE_NAME "/dev/chr_dev"

int main(int argc, char *argv[])
{
	int device, cur_level;

	device = open(DEVICE_FILE_NAME, O_RDONLY | O_NDELAY);

	if (device >= 0)
	{
		printf("Device file open\n");
		while(1)
		{
			read(device, &cur_level, sizeof(cur_level));
			// 여기서 현재 배터리 레벨을 출력하는데, gui로 출력하고, 계속해서 업데이트
			// ex) 아래 상태바가 한줄에 계속해서 출력되도록 프로그래밍
			// [============     ] 80%
			printf("Read value is %d\n", cur_level);
			sleep(1);
		}
	}
	else
	{
		printf("Device file open failed\n");
	}
	return 0;
}
