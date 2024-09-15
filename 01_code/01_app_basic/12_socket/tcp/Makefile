

CC=gcc
ACROSS_COMPILE=arm-buildroot-linux-gnueabihf-

all:
	$(CC) -o tcp_server server.c
	$(CC) -o tcp_client client.c

	$(ACROSS_COMPILE)$(CC) -o im6ull_tcp_server server.c
	$(ACROSS_COMPILE)$(CC) -o im6ull_tcp_client client.c

clean:
	rm -f tcp_server tcp_client im6ull_tcp_server im6ull_tcp_client
