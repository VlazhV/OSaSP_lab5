#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "list.c"



#define FORMAT_LEN 62
#define MIN_WORD_LEN 3

typedef struct 
{
	char *buf_sep;
	size_t buf_sep_size;
	int sep_block_num;
	
	char *buf_full;
	size_t buf_full_size;
	
//	FILE *f_out;
}thread_info;



void *thread_check(void *data)
{
	thread_info *info = (thread_info *)data;
	
	node *head = list_ini();
	if (head == NULL)
		pthread_exit(NULL); //ERROR
	
	
	int curr_char;
	int res;
	node *resp;
	
	
	
	for (size_t i = 0; i < info->buf_sep_size; ++i)
	{
		curr_char = info->buf_sep[i];
		
		if (!isalnum(curr_char))
			continue;
		
		
		for (size_t j = 0; j < info->buf_full_size; ++j)
		{
			if (curr_char == info->buf_full[j])
			{
				res = list_check(head, i + info->buf_sep_size * info->sep_block_num, j);
				if (res == -1)
					pthread_exit(NULL);
					
				if (res == 0)
				{
					size_t i_save = i;
					size_t j_save = j;
					size_t len = 1;
					
					while (i < info->buf_sep_size && j < info->buf_full_size 
							&& 
							info->buf_sep[++i] == info->buf_full[++j] 
							&& 
							isalnum(info->buf_sep[i])
							)
						++len;
						
					if (len >= MIN_WORD_LEN)
					{
						resp = list_add_tail(head, info->buf_sep[i_save], i_save + info->buf_sep_size * info->sep_block_num, j_save, len);
	//					printf("#%d list_add\n", info->sep_block_num);
						if (resp == NULL)
							pthread_exit(NULL);
					}
					
					i = i_save;
					j = j_save;
				}
				else
				{
		//			printf("res = %d\n", res);
					j += res - 1;
					continue;
				}
			}
		}
	}
	//puts("EXIT");
	pthread_exit(head);
}


size_t readfile(char *filename, char **buf)
{
	int res;
	
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		fprintf(stderr, "%s : ", filename);
		perror("error rf1: can't open file");
		return -1;
	}
	
	res = fseek(f, 0, SEEK_END);
	if (res == -1)
	{
		fprintf(stderr, "%s : ", filename);
		perror("error rf2: can't fseek");
		return -1;
	}
	
	size_t filesize = ftell(f);
	if (filesize == -1)
	{
		fprintf(stderr, "%s : ", filename);
		perror("error rf3: can't ftell");
		return -1;
	}
	
	rewind(f);
	
	*buf = (char*)calloc(filesize, 1);
	if (*buf == NULL)
	{
		fprintf(stderr, "%s : ", filename);
		perror("error rf4: can't alloc mem for buff");
		return -1;
	}

	int c;
	for (size_t i = 0; i < filesize; ++i)
	{
		c = getc(f);
		if (c != EOF)
			(*buf)[i] = c;
		else
			return -1;
	}
		
	res = fclose(f);
	if (res != 0)
	{
		fprintf(stderr, "%s : ", filename);
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
		fprintf(stderr, "error cn3: max count of thread-children must be a positive value\n");
		return -1;
	}
	
	return N;
}




int main(int argc, char *argv[])
{
	
	if (argc != 5)
	{
		fprintf(stderr, "./ind8.exe file1 file2 N fileout\n");
		return -1;
	}
	
	int nThreads = convertN(argv[3]);
	if (nThreads == -1)
		return -1;
		
	
	
	char *buf1, *buf2;
	
	size_t buf1size = readfile(argv[1], &buf1);
	if (buf1size == -1)
		return -1;
		
		
	size_t buf2size = readfile(argv[2], &buf2);
	if (buf2size == -1)
		return -1;
		
	
		
	size_t blocksize = buf1size / nThreads;
	
	char *sepfile = buf1;
	char *sepfilename = argv[1];
	size_t sepfilesize = buf1size;
	
	char *fullfile = buf2;
	char *fullfilename = argv[2];
	size_t fullfilesize = buf2size;
	
	
	if (blocksize == 0)
	{
		if (buf1size >= buf2size)
		{
			puts("number of threads less than size of any file\nnumber of threads will be size of file1");
			printf("N = %ld", buf1size);
			nThreads = buf1size;
			blocksize = 1;
		}
		else
		{
			blocksize = buf2size / nThreads;
			if (blocksize == 0)
			{
				puts("number of threads less than size of any file\nnumber of threads will be size of file2");
				printf("N = %ld", buf2size);
				nThreads = buf2size;
				
				blocksize = 1;
				sepfile = buf2;
				sepfilesize = buf2size;
				sepfilename = argv[2];
				
				fullfile = buf1;
				fullfilesize = buf1size;
				fullfilename = argv[1];
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
		info->sep_block_num = i;
//		info->f_out = f_out;
		
		res = pthread_create(&pt, NULL, &thread_check, info);
		if (res != 0)
		{
			fprintf(stderr, "error : thread %d can't create\n", i);
			perror(" ");
		}
		else
		{
			printf("#%d %lu created\n", i, pt);
			pt_arr[i] = pt;
		}
	}
	
	node *head = NULL;
	node *next_head = NULL;
	void *retval;
	
	
	for (int i = 0; i < nThreads; ++i)
	{
		res = pthread_join(pt_arr[i], &retval);
		
		if (res != 0)
		{
			fprintf(stderr, "error: can't join thread %d", i);
			perror(" ");
		}
		else if ( retval == NULL)
			fprintf(stderr, "#%d %lu terminated UNsuccessfully\n", i, pt_arr[i]);
		else
		{
			printf ("#%d %lu terminated successfully\n", i, pt_arr[i]);
			if (head == NULL)			
				head =(node*)retval;
			else
			{
				next_head = (node*)retval;
				list_concat(head, next_head);
			}
				
		}
		
	}
	
//	puts(res_str);


	FILE *f_out = fopen(argv[4], "w");
	if (f_out == NULL)
	{
		perror ("error : can't open fileout");
		return -1;
	}
	
	
	list_print(f_out, head, sepfilename, fullfilename);
	
	free(buf1);
	free(buf2);
//		
	res = fclose(f_out);
	if (res != 0)
	{
		perror("error : can't close fileout");
		return -1;
	}
	return 0;
}


//thread_info *info = (thread_info *)data;
	
//	FILE *fout = info->f_out;
	
//	
//	int added = 0;
//	int rows = 1;
//	
//	char *res = (char *)calloc(FORMAT_LEN, 1);
//	if (res == NULL)
//	{
//		fprintf(stderr, "%lu : calloc() failed", pthread_self());
//		perror(" ");
//		pthread_exit(NULL);
//	}
//	
//	
//	char *add = (char *)calloc(FORMAT_LEN, 1);
//	if (add == NULL)
//	{
//		fprintf(stderr, "%lu : calloc() failed", pthread_self());
//		perror(" ");
//		pthread_exit(NULL);
//	}
//	
//	
//	for (size_t i = 0; i < info->buf_sep_size; ++i)
//	{
//		char curr_char = info->buf_sep[i];
//		for (size_t j = 0; j < info->buf_full_size; ++j)
//		{
//			if (curr_char == info->buf_full[j])
//			{
//				size_t len = 1;
//				size_t i_save = i, j_save = j;
//
//				while (info->buf_sep[++i] == info->buf_full[++j] && i < info->buf_sep_size && j < info->buf_full_size)
//					++len;
//					
//				sprintf(add, FORMAT, curr_char, curr_char, i_save + info->sep_block_num * info->buf_sep_size, j_save, len );
//				res = strcat(res, add);
//
//				if (++added == rows)
//				{
//					rows *= 2;
//					res = realloc(res, rows * FORMAT_LEN);
//					if  (res == NULL)
//					{
//						fprintf(stderr, "%lu : realloc() failed", pthread_self());
//						perror(" ");
//						pthread_exit((int *) -1);
//					}
//				}
//				
//				i = i_save;
//				j = j_save;
//			}
//		}
//		
//	}
//	
//	free(add);
//
//	pthread_exit((char *)res);