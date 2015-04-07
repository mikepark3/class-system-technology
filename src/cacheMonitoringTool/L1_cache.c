/*****************************************************************************
*  This example code shows how to use most of PAPI's High level functions    * 
*  to start,count,read and stop on an event set. We use two preset events    *
*  here:                                                                     *
*     PAPI_TOT_INS: Total instructions executed in a period of time	     *
*     PAPI_TOT_CYC: Total cpu cycles in a period of time	             *
******************************************************************************/ 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <mysql/mysql.h>

#define DB_HOST "192.168.0.5"
#define DB_USER "root2"
#define DB_PASS "1234"
#define DB_NAME "my_report"

#define NUM_EVENTS 3 
#define THRESHOLD 10000
#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }

MYSQL* connection, conn;
int query_stat;

/* stupid codes to be monitored */ 
void computation_add()
{
   int tmp = 0;
   int i=0;

   for( i = 0; i < THRESHOLD; i++ )
   {
      tmp = tmp + i;
   }

}

/* insert values into table */
void insert_query(long long totInst, long long cacheMiss)
{
	char query[255];
	float hit =  (float)(1.0 - ((float)cacheMiss / (float)totInst));
	float miss = (float)((float)cacheMiss / (float)totInst);
	float reuse = (float)((float)(totInst - (float)cacheMiss) / (float)cacheMiss);

	/* print */
	printf("The total instructions executed for addition are %lld \n", totInst);
	printf("The L1 data cache misses are %lld \n", cacheMiss);
	printf("The L1 hit rate is %f \n", hit);
	printf("The L1 miss rate is %f \n", miss);
	printf("The L1 cache line reuse rate is %f \n", reuse);

	/* insert */
	sprintf(query, "insert into l1_cache_analysis(HitRate, MissRate, ReuseLine, Instruction) values ('%f', '%f', '%f', '%lld')", hit, miss, reuse, totInst);
        query_stat = mysql_query(connection, query);
        if(query_stat != 0)
        {
                fprintf(stderr, "Mysql query error : %s\n", mysql_error(&conn));
        }
}

int main()
{
   /*Declaring and initializing the event set with the presets*/
   int Events[3] = {PAPI_TOT_INS, PAPI_L1_DCM, PAPI_TOT_CYC};
   /*The length of the events array should be no longer than the 
     value returned by PAPI_num_counters.*/	
	
   /*declaring place holder for no of hardware counters */
   int num_hwcntrs = 0;
   int retval;
   char errstring[PAPI_MAX_STR_LEN];
   /*This is going to store our list of results*/
   long long values[NUM_EVENTS];


   /*Initialize and Connect to Database*/
   mysql_init(&conn);

   connection = NULL;
   connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
   if(connection == NULL)
   {
	fprintf(stderr, "Mysql connection error : %s\n", mysql_error(&conn));
	return 1;
   }

   /***************************************************************************
   *  This part initializes the library and compares the version number of the*
   * header file, to the version of the library, if these don't match then it *
   * is likely that PAPI won't work correctly.If there is an error, retval    *
   * keeps track of the version number.                                       *
   ***************************************************************************/

   if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
   {
      fprintf(stderr, "Error: %d %s\n",retval, errstring);
      exit(1);
   }


   /**************************************************************************
    * PAPI_num_counters returns the number of hardware counters the platform *
    * has or a negative number if there is an error                          *
    **************************************************************************/
   if ((num_hwcntrs = PAPI_num_counters()) < PAPI_OK)
   {
      printf("There are no counters available. \n");
      exit(1);
   }

   printf("There are %d counters in this system\n",num_hwcntrs);
		
   /**************************************************************************
    * PAPI_start_counters initializes the PAPI library (if necessary) and    *
    * starts counting the events named in the events array. This function    *
    * implicitly stops and initializes any counters running as a result of   *
    * a previous call to PAPI_start_counters.                                *
    **************************************************************************/
   if ( (retval = PAPI_start_counters(Events, NUM_EVENTS)) != PAPI_OK)
   {
      ERROR_RETURN(retval);
      exit(1);
   }


   /**********************************************************************
    * PAPI_read_counters reads the counter values into values array      *
    **********************************************************************/

   /* Your code goes here*/
   printf("\nWe try to do first additions\n");
   computation_add();
  
   if ( (retval=PAPI_read_counters(values, NUM_EVENTS)) != PAPI_OK)
   {
      ERROR_RETURN(retval);
      exit(1);
   }
   insert_query(values[0], values[1]);

   /************************************************************************
    * What PAPI_accum_counters does is it adds the running counter values  *
    * to what is in the values array. The hardware counters are reset and  *
    * left running after the call.                                         *
    ************************************************************************/
   while(1)
   {
	   sleep(10);
	   printf("\nNow we try to use PAPI_accum in while loop to accumulate values repeatedly\n"); 
	   /* Do some computation here */
	   computation_add();

	   if ( (retval=PAPI_accum_counters(values, NUM_EVENTS)) != PAPI_OK)
	   {
	      ERROR_RETURN(retval);
	      exit(1);
	   }
	   insert_query(values[0], values[1]);
   }


   /***********************************************************************
    * Stop counting events(this reads the counters as well as stops them  *
    ***********************************************************************/

   printf("\nNow we try to do last additions\n");
   computation_add();

   /******************* PAPI_stop_counters **********************************/
   if ((retval=PAPI_stop_counters(values, NUM_EVENTS)) != PAPI_OK)
   {
      ERROR_RETURN(retval);	
      exit(1);
   }
   insert_query(values[0], values[1]);

   mysql_close(connection);
   exit(0);	
}
