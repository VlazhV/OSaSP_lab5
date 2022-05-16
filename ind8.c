#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>


typedef struct 
{
	char *buf_sep;
	size_t buf_sep_size;
	
	char *buf_full;
	size_t buf_full_size;
	
	FILE *f_out;
}thread_info;



void *thread_check(void *data)
{
	thread_info *info = (thread_info *)data;
	
	FILE *fout = info->f_out;
	
	for (size_t i = 0; i < info->buf_sep_size; ++i)
	{
		char curr_char = info->buf_sep[i];
		for (size_t j = 0; j < info->buf_full_size; ++j)
		{
			if (curr_char == info->buf_full[j])
			{
				size_t len = 1;
				size_t i_save = i, j_save = j;

				while (info->buf_sep[++i] == info->buf_full[++j])
					++len;
					
				fprintf(fout, "Macth [%u - %c]: file1 = %ld  ---- file2 = %ld ---- len = %ld \n", curr_char, curr_char, i_save, j_save, len);
				
				i = i_save;
				j = j_save;
			}
		}
		
	}
	pthread_exit(0);
}


size_t readfile(char *filename, char **buf)
{
	int res;
	
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror("error rf1: can't open file");
		return -1;
	}
	
	res = fseek(f, 0, SEEK_END);
	if (res == -1)
	{
		perror("error rf2: can't fseek");
		return -1;
	}
	
	size_t filesize = ftell(f);
	if (filesize == -1)
	{
		perror("error rf3: can't ftell");
		return -1;
	}
	
	rewind(f);
	
	*buf = (char*)calloc(filesize, 1);
	if (*buf == NULL)
	{
		perror("error rf4: can't alloc mem for buff");
		return -1;
	}
	
	for (size_t i = 0; i < filesize; ++i)
		(*buf)[i] = getc(f);
		
	res = fclose(f);
	if (res != 0)
	{
		perror("error rf5 : can't close file");
		return -1;
	}
	
	return filesize;
}

int convertN (char *strN)
{
	char *endptr;
	
	int N = strtol(strN, &endptr, 10);
	
	if (errno == ERANGE)
	{
		perror("error cn1: cannot convert");
		return -1;
	}
	
	
	if (endptr[0] != '\0')
	{		
		fprintf(stderr, "error cn2: cannot convert(N must be full correct)\n");
		return -1;	
	}
	else if (endptr - strN == 0)
	{
		fprintf(stderr, "error cn2: cannot convert(N must be full correct)\n");
		return -1;
	}
	
	
	if (N <= 0)
	{
		fprintf(stderr, "error cn3: max count of processes-children must be a positive value\n");
		return -1;
	}
	
	return N;
}




int main(int argc, char *argv[])
{
	
	if (argc != 5)
	{
		fprintf(stderr, "./ind8.exe file1 file2 N fileout");
		return -1;
	}
	
	int nThreads = convertN(argv[3]);
	if (nThreads == -1)
		return -1;
		
	FILE *f_out = fopen(argv[4], "w");
	if (f_out == NULL)
	{
		perror ("error : can't open fileout");
		return -1;
	}
	
	char *buf1, *buf2;
	
	size_t buf1size = readfile(argv[1], &buf1);
	if (buf1size == -1)
		return -1;
		
		
	size_t buf2size = readfile(argv[2], &buf2);
	if (buf2size == -1)
		return -1;
		
	size_t blocksize = buf1size / nThreads;
	
	char *sepfile = buf1;
	size_t sepfilesize = buf1size;
	
	char *fullfile = buf2;
	size_t fullfilesize = buf2size;
	
	
	if (blocksize == 0)
	{
		if (buf1size >= buf2size)
		{
			puts("number of threads less than size of any file\nnumber of threads will be size of file1");
			printf("N = %ld", buf1size);
			blocksize = 1;
		}
		else
		{
			blocksize = buf2size / nThreads;
			if (blocksize == 0)
			{
				puts("number of threads less than size of any file\nnumber of threads will be size of file2");
				printf("N = %ld", buf2size);
				blocksize = 1;
				sepfile = buf2;
				sepfilesize = buf2size;
				
				fullfile = buf1;
				fullfilesize = buf1size;
			}
		}
	}
			
	int res;	

	pthread_t pt_arr[nThreads];
	
	for (int i = 0; i < nThreads; ++i)
	{
		pthread_t pt;
		
		
		
		
		thread_info *info = (thread_info*)calloc(1, sizeof(thread_info));
		if (info == NULL)
		{
			perror("error : can't alloc mem");
			return -1;
		}
		
		info->buf_sep = sepfile + i*blocksize;
		info->buf_sep_size = blocksize;
		info->buf_full = fullfile;
		info->buf_full_size = fullfilesize;
		info->f_out = f_out;
		
		res = pthread_create(&pt, NULL, &thread_check, info);
		if (res != 0)
		{
			fprintf(stderr, "error : thread %d can't create\n", i);
			perror(" ");
		}
		else
		{
			printf("thread #%d created\n", i);
			pt_arr[i] = pt;
		}
	}
	
	

	void *retval;
	for (int i = 0; i < nThreads; ++i)
	{
		res = pthread_join(pt_arr[i], &retval);
		
		if (res != 0)
		{
			fprintf(stderr, "error: can't join thread %d", i);
			perror(" ");
		}
		else if ( retval != NULL)
			fprintf(stderr, "thread %d terminated UNsuccessfully\n", i);
		else
			printf("thread #%d terminated\n", i);
		
	}
	
	
	free(buf1);
	free(buf2);
	
	res = fclose(f_out);
	if (res != 0)
	{
		perror("error : can't close fileout");
		return -1;
	}
	return 0;
}