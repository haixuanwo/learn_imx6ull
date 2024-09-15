
#include <common.h>
#include <page_manager.h>
#include <string.h>

static PPageAction g_ptPages = NULL;

void PageRegister(PPageAction ptPageAction)
{
	ptPageAction->ptNext = g_ptPages;
	g_ptPages = ptPageAction;
}

PPageAction Page(char *name)
{
	PPageAction ptTmp = g_ptPages;

	while (ptTmp)
	{
		if (strcmp(name, ptTmp->name) == 0)
			return ptTmp;
		ptTmp = ptTmp->ptNext;
	}

	return NULL;
}


void PagesRegister(void)
{
	//MainPageRegister();
}

