#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "mytime.c"


void *thread_fun(void* _)
{
	char *tmBuf = (char*)calloc(TIME_MAX, 1);
	myTime(tmBuf);
	
	printf("pid = %d, tid = %d\t\t\t%s\n", getpid(), gettid(), tmBuf != NULL ? tmBuf : "time error");
		
	//sleep(10);
	pthread_exit(0);
}

int main()
{
	char *tmBuf = (char*)calloc(TIME_MAX, 1);
	myTime(tmBuf);
	
	printf("pid = %d, ppid = %d, tid = %d\t%s\n", getpid(), getppid(), gettid(), tmBuf != NULL ? tmBuf : "time error");
	
	pthread_t pt1, pt2;

	
	pthread_create(&pt1, NULL, &thread_fun, NULL);
	pthread_create(&pt2, NULL, &thread_fun, NULL);	
	pthread_join(pt1, NULL);
	pthread_join(pt2, NULL);
	
	return 0;
}
