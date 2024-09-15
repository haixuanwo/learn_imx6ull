

#include <config.h>
#include <stdio.h>

static ItemCfg g_tItemCfgs[ITEMCFG_MAX_NUM];
static int g_iItemCfgCount = 0;

int ParseConfigFile(char *strFileName)
{
	FILE *fp;
	char buf[100];
	char *p = buf;
	
	/* 1. open config file */
	fp = fopen(CFG_FILE, "r");
	if (!fp)
	{
		printf("can not open cfg file %s\n", CFG_FILE);
		return -1;
	}

	while (fgets(buf, 100, fp))
	{
		/* 2.1 read each line */
		buf[99] = '\0';		

		/* 2.2 吃掉开头的空格或TAB */
		p = buf;
		while (*p == ' ' || *p =='\t')
			p++;

		/* 2.3 忽略注释 */
		if (*p == '#')
			continue;

		/* 2.4 处理 */
		g_tItemCfgs[g_iItemCfgCount].command[0] = '\0';
		g_tItemCfgs[g_iItemCfgCount].index = g_iItemCfgCount;
		sscanf(p, "%s %d %s", g_tItemCfgs[g_iItemCfgCount].name, &g_tItemCfgs[g_iItemCfgCount].bCanBeTouched, \
		                      g_tItemCfgs[g_iItemCfgCount].command);
		g_iItemCfgCount++;		
	}
	return 0;
}

int GetItemCfgCount(void)
{
	return g_iItemCfgCount;
}

PItemCfg GetItemCfgByIndex(int index)
{
	if (index < g_iItemCfgCount)
		return &g_tItemCfgs[index];
	else
		return NULL;
}

PItemCfg GetItemCfgByName(char *name)
{
	int i;
	for (i = 0; i < g_iItemCfgCount; i++)
	{
		if (strcmp(name, g_tItemCfgs[i].name) == 0)
			return &g_tItemCfgs[index];
	}
	return NULL;
}


