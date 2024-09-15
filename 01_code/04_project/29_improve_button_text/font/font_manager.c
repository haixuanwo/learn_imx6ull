
#include <font_manager.h>
#include <string.h>


static PFontOpr g_ptFonts = NULL;
static PFontOpr g_ptDefaulFontOpr = NULL;


void RegisterFont(PFontOpr ptFontOpr)
{
	ptFontOpr->ptNext = g_ptFonts;
	g_ptFonts = ptFontOpr;
}

void FontsRegister(void)
{
	extern void FreetypeRegister(void);
	FreetypeRegister();
}

int SelectAndInitFont(char *aFontOprName, char *aFontFileName)
{
	PFontOpr ptTmp = g_ptFonts;
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, aFontOprName) == 0)
			break;
		ptTmp = ptTmp->ptNext;
	}

	if (!ptTmp)
		return -1;

	g_ptDefaulFontOpr = ptTmp;
	return ptTmp->FontInit(aFontFileName);
}

int SetFontSize(int iFontSize)
{
	return g_ptDefaulFontOpr->SetFontSize(iFontSize);
}

int GetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap)
{
	return g_ptDefaulFontOpr->GetFontBitMap(dwCode, ptFontBitMap);
}

int GetStringRegionCar(char *str, PRegionCartesian ptRegionCar)
{
	return g_ptDefaulFontOpr->GetStringRegionCar(str, ptRegionCar);
}


