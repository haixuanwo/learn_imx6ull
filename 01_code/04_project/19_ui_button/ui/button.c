

#include <ui.h>


static int DefaultOnDraw(struct Button *ptButton, PDispBuff ptDispBuff)
{
	/* 绘制底色 */
	DrawRegion(&ptButton->tRegion, BUTTON_DEFAULT_COLOR);

	/* 居中写文字 */
	DrawTextInRegionCentral(ptButton->name, &ptButton->tRegion, BUTTON_TEXT_COLOR);

	/* flush to lcd/web */
	FlushDisplayRegion(&ptButton->tRegion, ptDispBuff);
}

static int DefaultOnPressed(struct Button *ptButton, PDispBuff ptDispBuff, PInputEvent ptInputEvent)
{
	unsigned int dwColor = BUTTON_DEFAULT_COLOR;
	
	ptButton->status = !ptButton->status;
	if (ptButton->status)
		dwColor = BUTTON_PRESSED_COLOR;

	/* 绘制底色 */
	DrawRegion(&ptButton->tRegion, dwColor);

	/* 居中写文字 */
	DrawTextInRegionCentral(ptButton->name, &ptButton->tRegion, BUTTON_TEXT_COLOR);

	/* flush to lcd/web */
	FlushDisplayRegion(&ptButton->tRegion, ptDispBuff);
	
}


void InitButton(PButton ptButton, char *name, PRegion ptRegion, ONDRAW_FUNC OnDraw, ONPRESSED_FUNC OnPressed)
{
	ptButton->status = 0;
	ptButton->name = name;
	ptButton->tRegion = *ptRegion;
	ptButton->OnDraw    = OnDraw ? OnDraw : DefaultOnDraw;
	ptButton->OnPressed = OnPressed ? OnPressed : DefaultOnPressed;
}


