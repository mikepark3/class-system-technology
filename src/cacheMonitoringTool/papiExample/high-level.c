#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <papi.h>

#define NUM_EVENTS 1 

/* Get actual CPU time in seconds */
float gettime() 
{
	return((float)PAPI_get_virt_usec()*1000000.0);
}

int main () 
{
	float t0, t1;
	int ret;
	//              papiStdEventDefs.h     fpapi.h         printf
	//PAPI_L1_DCM =    2147483648        -2147483648    -2147483648
	//PAPI_L1_DCH =    2147483710	     -2147483586    -2147483586
	//PAPI_L1_DCA =    2147483712        -2147483584    -2147483584
	//int events[NUM_EVENTS] = {2147483648, 2147483712};
	int events[NUM_EVENTS] = {PAPI_L1_DCA};
	//int events[NUM_EVENTS] = {PAPI_L1_DCM, PAPI_FP_OPS};
	long_long values[NUM_EVENTS];

	//printf("%d, %d\n", PAPI_L1_DCM, PAPI_FP_OPS);
	printf("%d, %d, %d\n", PAPI_L1_DCM, PAPI_L1_DCH, PAPI_L1_DCA);
	if((ret = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT)
	{
		fprintf(stderr, "Error: %s\n", PAPI_strerror(ret));
		exit(1);
	}

	if(PAPI_num_counters() < 2) {
		fprintf(stderr, "No hardware counters here, or PAPI not supported.\n");
		exit(1);
	}

	t0 = gettime();
	printf("t0 : %f\n", (float)t0);
	if((ret = PAPI_start_counters(events, NUM_EVENTS)) != PAPI_OK) {
		fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
		exit(1);
	}

	if((ret = PAPI_stop_counters(values, NUM_EVENTS)) != PAPI_OK) {
		fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
		exit(1);
	}
	t1 = gettime();
	printf("t1 : %f\n", (float)t1);

	printf("L1 data cache miss : %lld\n", values[0]);
	//printf("L2 data cache hit : %lld\n", (float)values[1]);
	printf("Total hardware flops : %lld\n",(float)values[1]);
}
