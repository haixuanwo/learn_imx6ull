
# 在开发板上运行
# ./freetype_show_font_angle simsun.ttc 180 96

all:
	arm-buildroot-linux-gnueabihf-gcc -o freetype_show_font_angle freetype_show_font_angle.c -lfreetype -lm

clean:
	rm -f freetype_show_font_angle
