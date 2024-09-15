#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

/*
pthread_rwlock_t（读写锁）是Linux中提供的另一个线程同步机制，它允许多个读者同时访问共享资源，但限制了写入访问。读写锁的主要功能和作用有：

并发读取优化：读取锁允许多个线程同时获取读取锁，这极大地提高了性能，特别是当读取操作远多于写入操作时。

执行顺序控制：读操作不会互相阻塞，但写操作会确保在任何时候只有一个写操作是活跃的。这就确保了写操作的原子性，不会被多个读写操作干扰。

降低写锁竞争：读取锁允许多线程同时访问，减少了对写锁的阻塞次序，提高了系统的并发性能。

防止死锁：读写锁的使用通常与pthread_mutex_t配合使用，可以有效减轻死锁问题，因为它允许多个读取线程同时存在，但只允许一个线程在写入锁被其他线程持有时进行写入操作。

一般情况下，在满足以下条件的应用场景中会使用pthread_rwlock_t：

读取操作远多于写入操作：这可以显著提高性能，允许多个读取线程同时访问，而不会因为写入操作被阻塞而受限。
共享数据结构读写频繁：在读多写少的情况下，读写锁能提供更高的并发性。
读写数据结构更新不频繁：尽管读写锁支持多个并发读者，但写入操作仍然是独占的，避免了乱序访问带来的复杂性。
需要保证数据一致性，但读取不会影响其他任务：读写锁允许并发读，但确保了写入时的完整性。
在设计并发系统时，了解和正确使用pthread_rwlock_t对于优化性能和避免锁竞争至关重要。
*/

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

