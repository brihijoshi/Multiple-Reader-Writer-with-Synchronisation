/*
 * CSE231: Operating Systems
 * Assignment 3, Part 1: Multiple reader writer with synchronization
 * Brihi Joshi (2016142)
 * Taejas Gupta (2016204)
 * April 12, 2018
 */



#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_SIZE 1024
#define MAX_PTHREADS 1024



int sq[MAX_SIZE];
int F, R;
int r_num[MAX_SIZE], w_num[MAX_SIZE];
sem_t FR_mutex;
sem_t r_mutex[MAX_SIZE], w_mutex[MAX_SIZE];



struct reader_args
{
	int no;
	int index;
	int operation;
};

struct writer_args
{
	int no;
	int index;
	int val;
	int operation;
};



void initialize()
{
	// Initial queue is of size 10, initialised to all 0s.
	F = 0;
	R = 9;

	int i;
	for(i = 0; i < MAX_SIZE; i++)
	{
		sq[i] = 0;
		r_num[i] = 0;
		w_num[i] = 0;
		sem_init(&r_mutex[i], 0, 1);
		sem_init(%w_mutex[i], 0, 1);
	}
}



void *dequeue(void *args)
{
	struct reader_args r_args = args;

	sem_wait(&FR_mutex);
	if(F == -1)
	{
		sem_post(&FR_mutex);
		printf("Reader %d: Underflow: Cannot dequeue.\n", r_args.no);
	}
	else
	{
		int pos = F;
		sem_wait(&r_mutex[pos]);
		int val = sq[F++];
		if(F > R)
			F = R = -1;
		sem_post(&FR_mutex);
		sleep(5);
		printf("Reader %d: The value of the dequeued element is %d.\n", r_args.no, val);
		sem_post(&r_mutex[pos]);
	}
}



void *enqueue(void *args)
{
	struct writer_args w_args = args;

	if(R == MAX_SIZE - 1)
		printf("Writer %d: Overflow: Cannot enqueue.\n", w_args.no);
	else
	{
		int pos = R + 1;
		sem_wait(&w_mutex[pos]);
		if(R == -1)
		{
			F = R = 0;
			sq[R] = w_args.val;
		}
		else
			sq[++R] = w_args.val;
		sleep(5);
		printf("Writer %d: Element having value %d has been enqueued.\n", w_args.no, w_args.val);
		sem_post(&w_mutex[pos]);
	}
}



void *read_element(void *args)
{
	struct reader_args r_args = args;

	if(F + r_args.index)
}



void *write_element(void *args)
{

}



int main()
{
	int i;
	int num_readers, num_writers;

	// Initialising shared queue and semaphores.
	initialize();

	pthread_t r_pthread[MAX_PTHREADS];
	pthread_t w_pthread[MAX_PTHREADS];
	struct reader_args r_args[MAX_PTHREADS];
	struct writer_args w_args[MAX_PTHREADS];

	while(1)
	{
		printf("Enter the number of readers: ");
		scanf("%d", &num_readers);
		if(num_readers < 0 || num_readers > MAX_PTHREADS)
			printf("Number of readers must be an integer between 0 and %d, try again.\n", MAX_PTHREADS);
		else
			break;
	}
	i = 0;
	while(i < num_readers)
	{
		r_args[i].no = i;
		while(1)
		{
			printf("Select the operation to be performed:\n1. Read element\n2. Dequeue\n");
			scanf("%d", &r_args[i].operation);
			if(r_args[i].operation != 1 && r_args[i].operation != 2)
				printf("Option selected must either be 1 or 2, try again.\n");
			else
				break;
		}
		if(r_args[i].operation == 1)
		{
			while(1)
			{
				printf("Enter the index of the element to be read from the front of the shared queue: ");
				scanf("%d", r_args[i].index);
				if(r_args[i].index < 0)
					printf("Index must be non-negative, try again.\n");
				else
					break;
			}
		}
	}

	while(1)
	{
		printf("Enter the number of writers: ");
		scanf("%d", &num_writers);
		if(num_writers < 0 || num_writers > MAX_PTHREADS)
			printf("Number of writers must be an integer between 0 and %d, try again.\n", MAX_PTHREADS);
		else
			break;
	}
	i = 0;
	while(i < num_writers)
	{
		w_args[i].no = i;
		while(1)
		{
			printf("Select the operation to be performed:\n1. Write element\n2. Enqueue\n");
			scanf("%d", &w_args[i].operation);
			if(w_args[i].operation != 1 && w_args[i].operation != 2)
				printf("Option selected must either be 1 or 2, try again.\n");
			else
				break;
		}
		if(w_args[i].operation == 1)
		{
			while(1)
			{
				printf("Enter the index of the element to be written to at the rear of the shared queue: ");
				scanf("%d", w_args[i].index);
				if(w_args[i].index < 0)
					printf("Index must be non-negative, try again.\n");
				else
					break;
			}
		}
		printf("Enter the value to be written: ");
		scanf("%d", w_args[i].val);
	}

	for(i = 0; i < num_readers; i++)
	{
		if(r_args[i].operation == 1)
			pthread_create(&r_pthread[i], NULL, read_element, (void *)&r_args[i]);
		else
			pthread_create(&r_pthread[i], NULL, dequeue, (void *)&r_args[i]);
	}
	for(i = 0; i < num_writers; i++)
	{
		if(w_args[i].operation == 1)
			pthread_create(&w_pthread[i], NULL, write_element, (void *)&w_args[i]);
		else
			pthread_create(&w_pthread[i], NULL, enqueue, (void *)&w_args[i]);
	}

	for(i = 0; i < num_readers; i++)
		pthread_join(r_pthread[i], NULL);
	for(i = 0; i < num_writers, i++)
		pthread_join(w_pthread[i], NULL);

	return 0;
}