all:
	arm-buildroot-linux-gnueabihf-gcc -c -o ap3216c_test.o ap3216c_test.c -I include/
	arm-buildroot-linux-gnueabihf-gcc -c -o ap3216c_lib.o ap3216c_lib.c -I include/
	arm-buildroot-linux-gnueabihf-gcc -c -o i2cbusses.o i2cbusses.c  -I include/
	arm-buildroot-linux-gnueabihf-gcc -o ap3216c_test  ap3216c_lib.o  ap3216c_test.o  i2cbusses.o
clean:
	rm *.o ap3216c_test
	