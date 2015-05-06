#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE_NAME "/dev/chr_dev_psu"

static inline void loadBar(int x, int n, int w)
{
	float ratio;
	int i, c;

	// Calculuate the ratio of complete-to-incomplete.
	ratio = x/(float)n;
	c     = ratio * w;
	 
	// Show the percentage complete.
	printf("%3d%% [", (int)(ratio*100) );

	// Show the load bar.
	for (i=0; i<c; i++)
		printf("=");
	 
	for (i=c; i<w; i++)
		printf(" ");
	 
	// ANSI Control codes to go back to the
	// previous line and clear it.
	printf("]\r");
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	int device_fd, cur_level;
	device_fd = open(DEVICE_FILE_NAME, O_RDONLY | O_NDELAY);
	
	if (device_fd >= 0)
	{
		//printf("Device file open\n");
		while(1)
		{

			read(device_fd, &cur_level, sizeof(cur_level));
			//printf("Read value is %d\n", cur_level);
			loadBar(cur_level, 100, 100);
			//sleep(1);
		}
	}
	else
	{
		printf("Device file open failed\n");
	}
	return 0;
}
