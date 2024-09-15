#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

/*
pthread_mutex_t（互斥锁）和pthread_spinlock_t（自旋锁）都是Linux多线程并发控制中常见的同步机制，它们的区别如下：

阻塞机制：
pthread_mutex_t 在尝试获取锁时，如果锁被其他线程锁定，它会阻塞当前线程，直至锁被释放。这种阻塞会切换到操作系统提供的上下文，允许其他线程运行。
pthread_spinlock_t 是非阻塞的，线程会一直尝试获取锁，如果无法立即获取，会进入自旋循环等待，直到锁被释放。

适用场景：
pthread_mutex_t 通常适用于共享资源或数据结构的完整访问，当一个线程在获取锁后，其他所有尝试获取锁的线程都会被阻塞，直到锁被释放。
pthread_spinlock_t 更适合于锁持有时间短、竞争不激烈的情况，尤其在高吞吐量或实时环境中，避免不必要的上下文切换，提高性能。

CPU开销：
pthread_mutex_t 的阻塞使得它对CPU的消耗较小，除非长时间阻塞。
pthread_spinlock_t 在等不到锁的情况下，会消耗CPU，请注意避免造成无限自旋，否则可能导致CPU空转。

中断处理：
pthread_mutex_t 在中断处理时通常是安全的，内核会正确处理中断。
pthread_spinlock_t 在中断处理中通常会变为阻塞，避免了可能的锁竞争问题，但也可能会导致中断延迟。

资源使用：
pthread_mutex_t 在等待期间不需要额外的数据结构（如忙等待计数器）。
pthread_spinlock_t 需要额外的资源来支持自旋等待，可能会浪费更多内存。
在选择使用哪种锁时，要根据实际应用场景、性能需求以及中断行为来决定。通常情况下，pthread_mutex_t 更加通用和可靠，而pthread_spinlock_t 可能提供更高的效率，但在竞争激烈的多核环境中需谨慎使用。
*/

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

