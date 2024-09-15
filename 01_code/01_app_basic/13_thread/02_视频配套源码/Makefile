
CC=gcc
ACROSS_COMPILE=arm-buildroot-linux-gnueabihf-

all:
	$(CC) -o pthread1 pthread1.c -lpthread
	$(CC) -o pthread2 pthread2.c -lpthread
	$(CC) -o pthread3 pthread3.c -lpthread
	$(CC) -o pthread4 pthread4.c -lpthread
	$(CC) -o pthread5 pthread5.c -lpthread
	$(CC) -o pthread6_spinlock pthread6_spinlock.c -lpthread
	$(CC) -o pthread7_rwlock pthread7_rwlock.c -lpthread

	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread1 pthread1.c	-lpthread
	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread2 pthread2.c	-lpthread
	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread3 pthread3.c	-lpthread
	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread4 pthread4.c	-lpthread
	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread5 pthread5.c	-lpthread
	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread6_spinlock pthread6_spinlock.c -lpthread
	$(ACROSS_COMPILE)$(CC) -o imx6ull_pthread7_rwlock pthread7_rwlock.c -lpthread

clean:
	rm -f pthread1
	rm -f pthread2
	rm -f pthread3
	rm -f pthread4
	rm -f pthread5
	rm -f pthread6_spinlock
	rm -f pthread7_rwlock

	rm -f imx6ull_pthread1
	rm -f imx6ull_pthread2
	rm -f imx6ull_pthread3
	rm -f imx6ull_pthread4
	rm -f imx6ull_pthread5
	rm -f imx6ull_pthread6_spinlock
	rm -f imx6ull_pthread7_rwlock
