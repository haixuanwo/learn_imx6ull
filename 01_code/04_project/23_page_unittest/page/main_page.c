
#include <page_manager.h>
#include <stdio.h>

static void MainPageRun(void *pParams)
{
	printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}

static PageAction g_tMainPage = {
	.name = "main",
	.Run  = MainPageRun,
};

void MainPageRegister(void)
{
	PageRegister(&g_tMainPage);
}


