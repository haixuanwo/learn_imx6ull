#ifndef _FONT_MANAGER_H
#define _FONT_MANAGER_H

#ifndef NULL
#define NULL (void *)0
#endif

typedef struct FontBitMap {
	int iLeftUpX;
	int iLeftUpY;
	int iWidth;
	int iRows;
	int iCurOriginX;
	int iCurOriginY;
	int iNextOriginX;
	int iNextOriginY;
	unsigned char *pucBuffer;
}FontBitMap, *PFontBitMap;

typedef struct FontOpr {
	char *name;
	int (*FontInit)(char *aFineName);
	int (*SetFontSize)(int iFontSize);
	int (*GetFontBitMap)(unsigned int dwCode, PFontBitMap ptFontBitMap);
	struct FontOpr *ptNext;
}FontOpr, *PFontOpr;


#endif




