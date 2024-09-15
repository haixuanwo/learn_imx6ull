

CC=gcc
ACROSS_COMPILE=arm-buildroot-linux-gnueabihf-

all:
	$(CC) -o udp_server server.c
	$(CC) -o udp_client client.c

	$(ACROSS_COMPILE)$(CC) -o im6ull_udp_server server.c
	$(ACROSS_COMPILE)$(CC) -o im6ull_udp_client client.c

clean:
	rm -f udp_server udp_client im6ull_udp_server im6ull_udp_client
