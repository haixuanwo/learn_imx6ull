#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

typedef struct Region {
	int iLeftUpX;
	int iLeftUpY;
	int iWidth;
	int iHeigh;
}Region, *PRegion;

typedef struct DispOpr {
	char *name;
	int DeviceInit(void);
	int DeviceExit(void);
	char *GetBuffer(int *pXres, int *pYres, int *pBpp);
	int FlushRegion(PRegion ptRegion, char *buffer);
	struct DispOpr *ptNext;
}DispOpr, *PDispOpr;

#endif




