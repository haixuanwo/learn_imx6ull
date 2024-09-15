
ts_test: ts_test.o
	arm-buildroot-linux-gnueabihf-gcc -o $@ $^ -lts

%.o : %.c
	arm-buildroot-linux-gnueabihf-gcc -I include -c -o $@ $^

clean:
	rm *.o ts_test

