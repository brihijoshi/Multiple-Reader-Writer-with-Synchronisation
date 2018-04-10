/*
 * CSE231: Operating Systems
 * Assignment 3, Part 1: Multiple reader writer with synchronization
 * Brihi Joshi (2016142)
 * Taejas Gupta (2016204)
 * April 10, 2018
 *
 * References:
 * Shared memory segments: http://docs.hfbk.net/beej.us/bgipc/output/print/bgipc_A4.pdf
 */



#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_SIZE 1024



struct element
{
	int val;
	pthread_mutex_t r_lock;
	pthread_mutex_t w_lock;
	int num_curr_readers;
	pthread_mutex_t r_num_lock;
};



// Practically an array, not a queue.
struct shared_queue
{
	struct element elem[5];
	pthread_mutex_t w_lock_queue;
};

struct shared_queue *sq;



struct reader_args
{
	int r_no;
	int r_index;
	int r_arrival_time;
}



struct writer_args
{
	int w_no;
	int w_index;
	int w_val;
	int w_arrival_time;
};



void *read_element(void *r_args)
{
	struct reader_args args = r_args;

	sleep(args.r_arrival_time);

	printf("Reader %d is attempting to read element %d.\n", args.r_no, args.r_index);
	if(pthread_mutex_trylock(&sq->elem[args.r_no].r_lock) == 0)
	{
		pthread_mutex_lock(&sq->elem[args.r_no].r_num_lock);
		sq->elem[args.r_no].num_curr_readers++;
		pthread_nutex_unlock(&sq->elem[args.r_no].r_num_lock);

		sleep(5);

		printf("Value read by reader %d is %d.\n", args.r_no, sq->elem[args.r_no].val);

		
	}
}



int main()
{
	int i;
	key_t key;
	int shmid;
	pthread_t pthread;
	int num_readers, num_writers;
	//int read_index[1024], write_index[1024], write_value[1024];
	struct reader_args r_args[1024];
	struct writer_args w_args[1024];
	
	// Making the key for the shared memory segment.
	if((key = ftok("sync.c", 'R')) == -1)
	{
		perror("ftok");
		exit(1);
	}

	// Creating a new shared memory segment or connecting to the existing segment.
	if((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1)
	{
		perror("shmget");
		exit(1);
	}

	// Attaching to the shared memory segment.
	sq = shmat(shmid, (void *)0, 0);
	if(data == (char *)(-1))
	{
		perror("shmat");
		exit(1);
	}

	// Initializing the shared queue.
	pthread_mutex_init(&sq.w_lock_queue, NULL);
	for(i = 0; i < 5; i++)
	{
		sq->elem[i].val = 0;
		pthread_mutex_init(&sq->elem[i].r_lock, NULL);
		pthread_mutex_init(&sq->elem[i].w_lock, NULL);
		sq->elem[i].num_curr_readers = 0;
		pthread_mutex_init(&sq->elem[i].r_num_lock, NULL);
	}

	printf("Enter the number of readers: ");
	scanf("%d", &num_readers);
	i = 0;
	while(i < num_readers)
	{
		r_args[i].r_no = i;
		printf("Enter the index of the element to be read by reader %d: ", i + 1);
		scanf("%d", &r_args.r_index[i]);
		if(r_args.r_index[i] < 1 || r_args.r_index[i] > 5)
			printf("The index must be between 1 and 5, try again.\n");
		else
		{
			printf("Enter the arrival time of the reader: ");
			scanf("%d", &r_args.r)
			i++;
		}
	}
	printf("Enter the number of writers: ");
	scanf("%d", &num_writers);
	i = 0;
	while(i < num_writers)
	{
		w_args[i].w_no = i;
		printf("Enter the index of the element to be written to by writer %d: ", i + 1);
		scanf("%d", &w_args[i].w_index);
		if(w_args[i].w_index < 1 || w_args[i].w_index > 5)
			printf("The index must be between 1 and 5, try again.\n");
		else
		{
			printf("Enter the value of the element to be written by writer %d: ", i + 1);
			scanf("%d", %w_args[i].w_val);
			i++;
		}
	}

	// Creating and executing the threads.
	for(i = 0; i < num_readers; i++)
		pthread_create(&pthread, NULL, read_element, (void *)&r_args[i]);
	for(i = 0; i < num_writers; i++)
		pthread_create(&pthread, NULL, write_element, (void *)&w_args[i]);

	return 0;
}