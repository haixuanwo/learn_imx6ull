#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

static void *my_thread_write_func (void *data)
{
	int i = (int)data;
	while (1)
	{
		pthread_rwlock_wrlock(&rwlock );
		shared_data++;
		printf("Thread %d is modifying the shared data: %d\n", i, shared_data);
		pthread_rwlock_unlock(&rwlock );
		sleep(1);		
	}

	return NULL;
}

static void *my_thread_read_func (void *data)
{
	int i = (int)data;
	while (1)
	{
		pthread_rwlock_rdlock(&rwlock );
		printf("Thread %d is reading the shared data: %d\n", i, shared_data);
		pthread_rwlock_unlock(&rwlock );
		sleep(1);		
	}

	return NULL;
}


int main(int argc, char **argv)
{
	pthread_t tid;
	int ret;
	int i;
		
	/* 创建写线程 */
	for (i =0; i < 10; i++)
	{
		ret = pthread_create(&tid, NULL, my_thread_write_func, (void *)i);
		if (ret)
		{
			printf("pthread_create err!\n");
			return -1;
		}
	}

	/* 创建读线程 */
	for (i =0; i < 10; i++)
	{
		ret = pthread_create(&tid, NULL, my_thread_read_func, (void *)i);
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
	
	pthread_spin_destroy(&rwlock );
	return 0;
}

