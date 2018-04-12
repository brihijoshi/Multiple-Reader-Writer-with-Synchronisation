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
int r_num[MAX_SIZE];
sem_t r_mutex[MAX_SIZE];
sem_t w_mutex;
sem_t x_mutex[MAX_SIZE];
sem_t deq_mutex, enq_mutex;



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
	// Initial queue is of size 10.
	F = 0;
	R = 10;

	int i;

	//Initialising queue to {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}.
	for(i = F; i < R; i++)
		sq[i] = i;
	for(i = R; i < MAX_SIZE; i++)
		sq[i] = 0;

	for(i = 0; i < MAX_SIZE; i++)
	{
		r_num[i] = 0;
		sem_init(&r_mutex[i], 0, 1);
		sem_init(&x_mutex[i], 0, 1);
	}
	sem_init(&w_mutex, 0, 1);
	sem_init(&deq_mutex, 0, 1);
	sem_init(&enq_mutex, 0, 1);
}



void *dequeue(void *args)
{
	struct reader_args *r_args = args;

	sem_wait(&deq_mutex);
	if(F == R)
	{
		printf("Reader %d: Underflow: Cannot dequeue.\n", r_args->no);
	}
	else
	{
		int pos = F;
		sem_wait(&x_mutex[pos]);
		sleep(2);
		int val = sq[F++];
		printf("Reader %d: The value of the dequeued element is %d.\n", r_args->no, val);
		sem_post(&x_mutex[pos]);
	}
	sem_post(&deq_mutex);
}



void *enqueue(void *args)
{
	struct writer_args *w_args = args;

	sem_wait(&w_mutex);
	sem_wait(&enq_mutex);
	if(R == MAX_SIZE)
	{
		printf("Writer %d: Overflow: Cannot enqueue.\n", w_args->no);
	}
	else
	{
		int pos = R;
		sem_wait(&x_mutex[pos]);
		sleep(2);
		sq[R++] = w_args->val;
		printf("Writer %d: Element having value %d has been enqueued.\n", w_args->no, sq[pos]);
		sem_post(&x_mutex[pos]);
	}
	sem_post(&enq_mutex);
	sem_post(&w_mutex);
}



void *read_element(void *args)
{
	struct reader_args *r_args = args;

	int pos = F + r_args->index;
	if(pos >= MAX_SIZE)
	{
		printf("Reader %d: Index out of bounds.\n", r_args->no);
	}
	else
	{
		sem_wait(&r_mutex[pos]);
		if(r_num[pos] == 0)
		{
			sem_wait(&x_mutex[pos]);
			if(pos < F || pos >= R)
			{
				printf("Reader %d: Index out of bounds.\n", r_args->no);
				sem_post(&x_mutex[pos]);
				sem_post(&r_mutex[pos]);
				return NULL;
			}
		}

		r_num[pos]++;
		sem_post(&r_mutex[pos]);

		int val = sq[pos];
		sleep(2);
		printf("Reader %d: The value of the element read is %d.\n", r_args->no, val);

		sem_wait(&r_mutex[pos]);
		r_num[pos]--;
		if(r_num[pos] == 0)
		{
			sem_post(&x_mutex[pos]);
		}
		sem_post(&r_mutex[pos]);
	}
}



void *write_element(void *args)
{
	struct writer_args *w_args = args;

	int pos = F + w_args->index;
	if(pos >= MAX_SIZE)
	{
		printf("Writer %d: Index out of bounds.\n", w_args->no);
	}
	else
	{
		sem_wait(&w_mutex);
		sem_wait(&x_mutex[pos]);

		if(pos < F || pos >= R)
		{
			printf("Writer %d: Index out of bounds.\n", w_args->no);
			sem_post(&x_mutex[pos]);
			sem_post(&w_mutex);
			return NULL;
		}

		sq[pos] = w_args->val;
		sleep(2);
		printf("Writer %d: Value %d has been written to the element.\n", w_args->no, sq[pos]);

		sem_post(&x_mutex[pos]);
		sem_post(&w_mutex);
	}
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

	while(1)
	{
		printf("Enter the number of writers: ");
		scanf("%d", &num_writers);
		if(num_writers < 0 || num_writers > MAX_PTHREADS)
			printf("Number of writers must be an integer between 0 and %d, try again.\n", MAX_PTHREADS);
		else
			break;
	}

	for(i = 0; i < num_readers; i++)
	{
		r_args[i].no = i + 1;
		while(1)
		{
			printf("Select the operation to be performed by reader %d:\n1. Read element\n2. Dequeue\n", i + 1);
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
				printf("Enter the index of the element to be read from the front of the shared queue by reader %d: ", i + 1);
				scanf("%d", &r_args[i].index);
				if(r_args[i].index < 0)
					printf("Index must be non-negative, try again.\n");
				else
					break;
			}
		}
	}

	for(i = 0; i < num_writers; i++)
	{
		w_args[i].no = i + 1;
		while(1)
		{
			printf("Select the operation to be performed by writer %d:\n1. Write element\n2. Enqueue\n", i + 1);
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
				printf("Enter the index of the element to be written to from the front of the shared queue by writer %d: ", i + 1);
				scanf("%d", &w_args[i].index);
				if(w_args[i].index < 0)
					printf("Index must be non-negative, try again.\n");
				else
					break;
			}
		}
		printf("Enter the value to be written by writer %d: ", i + 1);
		scanf("%d", &w_args[i].val);
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
	for(i = 0; i < num_writers; i++)
		pthread_join(w_pthread[i], NULL);

	return 0;
}
