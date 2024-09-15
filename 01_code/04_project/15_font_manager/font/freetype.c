#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <font_manager.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


static FT_Face g_tFace;
static int g_iDefaultFontSize = 12;

static int FreeTypeFontInit(char *aFineName)
{
    FT_Library    library;
    int error;

    error = FT_Init_FreeType( &library );                 /* initialize library */    
	if (error)
	{
		printf("FT_Init_FreeType err\n");
		return -1;
	}
	
    error = FT_New_Face(library, aFineName, 0, &g_tFace ); /* create face object */
	if (error)
	{
		printf("FT_New_Face err\n");
		return -1;
	}

    FT_Set_Pixel_Sizes(g_tFace, g_iDefaultFontSize, 0);

	return 0;
}

static int FreeTypeSetFontSize(int iFontSize)
{
    FT_Set_Pixel_Sizes(g_tFace, iFontSize, 0);
	return 0;
}

static int FreeTypeGetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap)
{
	int error;
    FT_Vector pen;
    FT_Glyph  glyph;
    FT_GlyphSlot slot = g_tFace->glyph;

    pen.x = ptFontBitMap->iCurOriginX * 64; /* 单位: 1/64像素 */
    pen.y = ptFontBitMap->iCurOriginY * 64; /* 单位: 1/64像素 */

	/* 转换：transformation */
	FT_Set_Transform(g_tFace, 0, &pen);

	/* 加载位图: load glyph image into the slot (erase previous one) */
	error = FT_Load_Char(g_tFace, dwCode, FT_LOAD_RENDER);
	if (error)
	{
		printf("FT_Load_Char error\n");
		return -1;
	}

	ptFontBitMap->pucBuffer = slot->bitmap.buffer;

	ptFontBitMap->iLeftUpX = slot->bitmap_left;
	ptFontBitMap->iLeftUpY = ptFontBitMap->iCurOriginY*2 - slot->bitmap_top;
	ptFontBitMap->iWidth   = slot->bitmap.width;
	ptFontBitMap->iRows    = slot->bitmap.rows;
	ptFontBitMap->iNextOriginX = ptFontBitMap->iCurOriginX + slot->advance.x / 64;
	ptFontBitMap->iNextOriginY = ptFontBitMap->iCurOriginY;

	return 0;
}


static FontOpr g_tFreetypeOpr = {
	.name          = "freetype",
	.FontInit      = FreeTypeFontInit,
	.SetFontSize   = FreeTypeSetFontSize,
	.GetFontBitMap = FreeTypeGetFontBitMap,
};

void FreetypeRegister(void)
{
	RegisterFont(&g_tFreetypeOpr);
}

