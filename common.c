#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define TIME_MAX 12

char *myTime(char *tmBuf)
{

	for (int i = 0; i < 10; ++i)
	if (tmBuf == NULL)
	{
		fprintf(stderr, "error mT1: myTime(null)");
		return NULL;
	}
	
	
	struct timespec mtime;
	if (clock_gettime(CLOCK_REALTIME, &mtime) < 0)
	{
		perror("error mT3 : clock_gettime() failed");
		return NULL;
	}
	
	struct tm *hms = localtime(&(mtime.tv_sec));
	if (!hms)
	{
		perror("error mT4: localtime() failed");
		return NULL;
	}
		
	if (sprintf(tmBuf, "%d:%d:%d:%d", hms->tm_hour, hms->tm_min, hms->tm_sec, (int)mtime.tv_nsec/1000) < 0)
	{
		perror("error mT2 : sprintf() failed");
		return NULL;
	}


	return tmBuf;
}


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
