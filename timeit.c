


#ifndef ADSTIMEIT_C
#define ADSTIMEIT_C

#include <time.h>

clock_t ADS_timeit_start = 0;

void set_time_checkpoint()
{
	ADS_timeit_start = clock();
}

long get_time_sec()
{
	clock_t diff = clock() - ADS_timeit_start;
	int64_t msec = diff * 1000 / CLOCKS_PER_SEC;
	return msec/1000;
}

long get_time_msec()
{
	clock_t diff = clock() - ADS_timeit_start;
	int64_t msec = diff * 1000 / CLOCKS_PER_SEC;
	return msec;
}

void print_time_chekpoint()
{
	printf("diffsec = %ld sec %ld millisec ",get_time_sec(),get_time_msec()%1000);
}


#endif

