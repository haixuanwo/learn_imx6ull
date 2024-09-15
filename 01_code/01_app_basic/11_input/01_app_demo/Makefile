
CC=arm-buildroot-linux-gnueabihf-gcc

all:
	${CC} 01_get_input_info.c -o 01_get_input_info
	${CC} 02_input_read.c -o 02_input_read
	${CC} 03_input_read_poll.c -o 03_input_read_poll
	${CC} 04_input_read_select.c -o 04_input_read_select
	${CC} 05_input_read_fasync.c -o 05_input_read_fasync

clean:
	rm -f 01_get_input_info
	rm -f 02_input_read
	rm -f 03_input_read_poll
	rm -f 04_input_read_select
	rm -f 05_input_read_fasync
