
#ifndef _UI_H
#define _UI_H

#include <common.h>
#include <disp_manager.h>
#include <input_manager.h>

#define BUTTON_DEFAULT_COLOR 0xff0000
#define BUTTON_PRESSED_COLOR 0x00ff00
#define BUTTON_PERCENT_COLOR 0x0000ff
#define BUTTON_TEXT_COLOR    0x000000

struct Button;

typedef int (*ONDRAW_FUNC)(struct Button *ptButton, PDispBuff ptDispBuff);
typedef int (*ONPRESSED_FUNC)(struct Button *ptButton, PDispBuff ptDispBuff, PInputEvent ptInputEvent);


typedef struct Button {
	char *name;
	int status;
	Region tRegion;
	ONDRAW_FUNC OnDraw;
	ONPRESSED_FUNC OnPressed;
}Button, *PButton;

void InitButton(PButton ptButton, char *name, PRegion ptRegion, ONDRAW_FUNC OnDraw, ONPRESSED_FUNC OnPressed);

#endif

