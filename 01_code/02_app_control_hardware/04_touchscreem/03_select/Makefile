
ts_test: ts_test.o
	arm-buildroot-linux-gnueabihf-gcc -o $@ $^

%.o : %.c
	arm-buildroot-linux-gnueabihf-gcc -I include -c -o $@ $^


