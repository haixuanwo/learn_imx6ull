
#ifndef _UI_H
#define _UI_H

#include <common.h>
#include <disp_manager.h>
#include <input_manager.h>

typedef struct Button {
	char *name;
	Region tRegion;
	int (*OnDraw)(struct Button *ptButton, PDispBuff ptDispBuff);
	int (*OnPressed)(struct Button *ptButton, PDispBuff ptDispBuff, PInputEvent ptInputEvent);
};

#endif

