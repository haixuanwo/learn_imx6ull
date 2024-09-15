/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-09-08 08:45:05
 * @LastEditors: Clark
 * @LastEditTime: 2024-09-08 10:50:35
 * @Description: file content
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void *fun(void *arg)
{
	printf("Pthread:%d Come !\n",(int )(long)arg+1);
	pthread_exit(arg);
}

int main()
{
	int ret,i,flag = 0;
	void *Tmp = NULL;
	pthread_t tid[3];

	for(i = 0;i < 3;i++)
  {
		ret = pthread_create(&tid[i], NULL, fun, (void *)(long)i);
		if(ret != 0)
    {
			perror("pthread_create");
			return -1;
		}
	}

	while (1)
  {
		for (i = 0; i <3; i++)
    {
      // int pthread_tryjoin_np(pthread_t thread, void **retval);
      // int pthread_timedjoin_np(pthread_t thread, void **retval, const struct timespec *abstime);
			if(pthread_tryjoin_np(tid[i],&Tmp) == 0)
      {
				printf("Pthread : %d exit !\n",(int )(long )Tmp+1);
				flag++;
			}
		}

		if(flag >= 3)
    {
      break;
    }
	}

	return 0;
}
