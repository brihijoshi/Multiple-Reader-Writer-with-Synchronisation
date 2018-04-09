#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_SIZE 1024

struct data{
	int elem;
	pthread_mutex_t elem_lock;
};

struct data arr[5];

void start(){
	
}