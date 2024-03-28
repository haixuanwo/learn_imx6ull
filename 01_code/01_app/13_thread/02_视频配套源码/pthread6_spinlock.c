#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

pthread_spinlock_t spinlock;
int shared_data = 0;

static void *my_thread_func (void *data)
{
	int i = (int)data;
	while (1)
	{
		pthread_spin_lock(&spinlock);
		shared_data++;
		printf("Thread %d is modifying the shared data: %d\n", i, shared_data);
		pthread_spin_unlock(&spinlock);
		sleep(1);		
	}

	return NULL;
}


int main(int argc, char **argv)
{
	pthread_t tid;
	int ret;
	int i;
	
	pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
	
	/* 创建线程 */
	for (i =0; i < 10; i++)
	{
		ret = pthread_create(&tid, NULL, my_thread_func, (void *)i);
		if (ret)
		{
			printf("pthread_create err!\n");
			return -1;
		}
	}


	while (1)
	{
		sleep(100);
	}
	
	pthread_spin_destroy(&spinlock);
	return 0;
}

