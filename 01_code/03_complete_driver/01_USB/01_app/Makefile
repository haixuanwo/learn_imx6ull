
all:
	gcc -Wall -o dnw 01_dnw.c -I./include/libusb-1.0 -L./lib -lusb-1.0 -lpthread -static

	gcc -Wall -o readmouse_sync 02_readmouse_sync.c -I./include/libusb-1.0 -L./lib -lusb-1.0 -lpthread -static

	gcc -Wall -o readmouse_async 03_readmouse_async.c -I./include/libusb-1.0 -L./lib -lusb-1.0 -lpthread -static

	gcc -Wall -o zero_app 04_zero_app.c -I./include/libusb-1.0 -L./lib -lusb-1.0 -lpthread -static

	gcc -Wall -o serial_send_recv 05_serial_send_recv.c -I./include/libusb-1.0 -L./lib -lusb-1.0 -lpthread -static


clean:
	@rm -f dnw readmouse_sync readmouse_async zero_app serial_send_recv *.o
