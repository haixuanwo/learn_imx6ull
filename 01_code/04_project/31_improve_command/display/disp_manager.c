#include <stdio.h>
#include <string.h>
#include <disp_manager.h>

/* 管理底层的LCD、WEB */
static PDispOpr g_DispDevs = NULL;
static PDispOpr g_DispDefault = NULL;
static DispBuff g_tDispBuff;
static int line_width;
static int pixel_width;

void DrawTextInRegionCentral(char *name, PRegion ptRegion, unsigned int dwColor)
{
	FontBitMap tFontBitMap;
	RegionCartesian tRegionCar;

	int iOriginX, iOriginY;
	int i = 0;
	int error;

	/* 计算字符串的外框 */
	GetStringRegionCar(name, &tRegionCar);

	/* 算出第一个字符的origin */
	iOriginX = ptRegion->iLeftUpX + (ptRegion->iWidth - tRegionCar.iWidth)/2 - tRegionCar.iLeftUpX;
	iOriginY = ptRegion->iLeftUpY + (ptRegion->iHeigh - tRegionCar.iHeigh)/2 + tRegionCar.iLeftUpY;


	/* 逐个绘制 */
	while (name[i])
	{
		/* get bitmap */
		tFontBitMap.iCurOriginX = iOriginX;
		tFontBitMap.iCurOriginY = iOriginY;
		error = GetFontBitMap(name[i], &tFontBitMap);
		if (error)
		{
			printf("SelectAndInitFont err\n");
			return;
		}

		/* draw on buffer */		
		DrawFontBitMap(&tFontBitMap, dwColor);		

		iOriginX = tFontBitMap.iNextOriginX;
		iOriginY = tFontBitMap.iNextOriginY;	
		i++;
	}
	
}

void DrawFontBitMap(PFontBitMap ptFontBitMap, unsigned int dwColor)
{
    int i, j, p, q;
	int x = ptFontBitMap->tRegion.iLeftUpX;
	int y = ptFontBitMap->tRegion.iLeftUpY;
    int x_max = x + ptFontBitMap->tRegion.iWidth;
    int y_max = y + ptFontBitMap->tRegion.iHeigh;
	int width = ptFontBitMap->tRegion.iWidth;
	unsigned char *buffer = ptFontBitMap->pucBuffer;

    //printf("x = %d, y = %d\n", x, y);

    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
        for ( i = x, p = 0; i < x_max; i++, p++ )
        {
            if ( i < 0      || j < 0       ||
                i >= g_tDispBuff.iXres || j >= g_tDispBuff.iYres )
            continue;

            //image[j][i] |= bitmap->buffer[q * bitmap->width + p];
            if (buffer[q * width + p])
	            PutPixel(i, j, dwColor);
        }
    }
	
}


void DrawRegion(PRegion ptRegion, unsigned int dwColor)
{
	int x = ptRegion->iLeftUpX;
	int y = ptRegion->iLeftUpY;
	int width = ptRegion->iWidth;
	int heigh = ptRegion->iHeigh;

	int i,j;

	for (j = y; j < y + heigh; j++)
	{
		for (i = x; i < x + width; i++)
			PutPixel(i, j, dwColor);
	}
}

int PutPixel(int x, int y, unsigned int dwColor)
{
	unsigned char *pen_8 = (unsigned char *)(g_tDispBuff.buff+y*line_width+x*pixel_width);
	unsigned short *pen_16;	
	unsigned int *pen_32;	

	unsigned int red, green, blue;	

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (g_tDispBuff.iBpp)
	{
		case 8:
		{
			*pen_8 = dwColor;
			break;
		}
		case 16:
		{
			/* 565 */
			red   = (dwColor >> 16) & 0xff;
			green = (dwColor >> 8) & 0xff;
			blue  = (dwColor >> 0) & 0xff;
			dwColor = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = dwColor;
			break;
		}
		case 32:
		{
			*pen_32 = dwColor;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", g_tDispBuff.iBpp);
			return -1;
			break;
		}
	}

	return 0;
}



void RegisterDisplay(PDispOpr ptDispOpr)
{
	ptDispOpr->ptNext = g_DispDevs;
	g_DispDevs = ptDispOpr;
}


int SelectDefaultDisplay(char *name)
{
	PDispOpr pTmp = g_DispDevs;
	while (pTmp) 
	{
		if (strcmp(name, pTmp->name) == 0)
		{
			g_DispDefault = pTmp;
			return 0;
		}

		pTmp = pTmp->ptNext;
	}

	return -1;
}

int InitDefaultDisplay(void)
{
	int ret;
	
	ret = g_DispDefault->DeviceInit();
	if (ret)
	{
		printf("DeviceInit err\n");
		return -1;
	}

	
	ret = g_DispDefault->GetBuffer(&g_tDispBuff);
	if (ret)
	{
		printf("GetBuffer err\n");
		return -1;
	}

	line_width  = g_tDispBuff.iXres * g_tDispBuff.iBpp/8;
	pixel_width = g_tDispBuff.iBpp/8;

	return 0;
}


PDispBuff GetDisplayBuffer(void)
{
	return &g_tDispBuff;
}


int FlushDisplayRegion(PRegion ptRegion, PDispBuff ptDispBuff)
{
	return g_DispDefault->FlushRegion(ptRegion, ptDispBuff);
}


void DisplaySystemRegister(void)
{
	
	extern void FramebufferRegister(void);
	FramebufferRegister();
}

