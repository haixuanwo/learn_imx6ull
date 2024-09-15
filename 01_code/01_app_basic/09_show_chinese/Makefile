
all:
	arm-buildroot-linux-gnueabihf-gcc -o show_chinese show_chinese.c
	gcc -finput-charset=UTF-8 -fexec-charset=GB2312 -o test_charset_ansi test_charset_ansi.c
	gcc -finput-charset=UTF-8 -fexec-charset=GB2312 -o test_charset_utf8 test_charset_utf8.c

clean:
	rm show_chinese
	rm test_charset_ansi
	rm test_charset_utf8
