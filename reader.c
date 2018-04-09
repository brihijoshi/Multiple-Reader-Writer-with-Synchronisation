#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define SHM_SIZE 1024
pthread_mutex_t x,wsem;
pthread_t tid;
int readcount;
key_t key;
int shmid;
int *data;
int mode;
void initialize()
{
	 pthread_mutex_init(&x,NULL);
	 pthread_mutex_init(&wsem,NULL);
	 readcount=0;
}
void* reader(void* j)
{
	 int waittime;
	 int i =(int) j;
	 waittime = rand() % 5;
	 printf("reader %d is trying to read\n",i);
	 pthread_mutex_lock(&x);
	 printf("reader %d is reading\n",i);
	 readcount++;
	 if(readcount == 1)
	 pthread_mutex_lock(&wsem);
	 printf("Number of readers = %d\n",readcount);
	 //pthread_mutex_unlock(&x);
	 sleep(waittime);
	 //pthread_mutex_lock(&x);
	 readcount--;
	 if(readcount==0)
	 pthread_mutex_unlock(&wsem);
	 pthread_mutex_unlock(&x);
	 printf("Reader %d is leaving\n",i);
}
void* writer(void* j)
{
	 int waittime;
	 int i = (int) j;
	 waittime = rand() % 3;
	 printf("writer %d is trying to write\n",i);
	 pthread_mutex_lock(&wsem);
	 printf("Writer %d is writing\n",i);
	 printf("Data is now %d\n",(*data)++);
	 sleep(waittime);
	 pthread_mutex_unlock(&wsem);
	 printf("\n Writer %d is leaving\n",i);
	 sleep(30);
	 exit(0);
}
int main()
{
	 int n1,n2,i;
	 if((key = ftok("reader.c",'R')) == -1)
	 {
	  perror("ftok");
	  exit(1);
	 }
	 if((shmid = shmget(key,SHM_SIZE,0644|IPC_CREAT)) == -1)
	 {
		  perror("shmget");
		  //exit(1);
	 }
	 data = shmat(shmid,(void*)0,0);
	 if(data == (int*)(-1))
	 {
		  perror("shmat");
		  exit(1);
	 }
	 *data = 0;
	 printf("Enter no.of Readers : ");
	 scanf("%d",&n1);
	 printf("Enter no.of Writers :");
	 scanf("%d",&n2);
	 for(i=0;i<n1;i++)
	 pthread_create(&tid,NULL,reader,(void*)i);
	 for(i=0;i<n2;i++)
	 pthread_create(&tid,NULL,writer,(void*)i);
	 sleep(5);
	 exit(0);
}