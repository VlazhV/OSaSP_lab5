#include <time.h>
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