

CC=gcc
ACROSS_COMPILE=arm-buildroot-linux-gnueabihf-

all:
	$(CC) -o udp_server2 server.c
	$(CC) -o udp_client2 client.c

	$(ACROSS_COMPILE)$(CC) -o im6ull_udp_server2 server.c
	$(ACROSS_COMPILE)$(CC) -o im6ull_udp_client2 client.c

clean:
	rm -f udp_server2 udp_client2 im6ull_udp_server2 im6ull_udp_client2
