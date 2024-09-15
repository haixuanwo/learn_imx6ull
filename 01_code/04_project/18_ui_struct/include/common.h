

#ifndef _COMMON_H
#define _COMMON_H

#ifndef NULL
#define NULL (void *)0
#endif

typedef struct Region {
	int iLeftUpX;
	int iLeftUpY;
	int iWidth;
	int iHeigh;
}Region, *PRegion;

#endif

