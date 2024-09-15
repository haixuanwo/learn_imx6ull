

#include <input_manager.h>
#include <tslib.h>
#include <stdio.h>

static struct tsdev *g_ts;

static int TouchscreenGetInputEvent(PInputEvent ptInputEvent)
{
	struct ts_sample samp;
	int ret;
	
	ret = ts_read(g_ts, &samp, 1);
	
	if (ret != 1)
		return -1;

	ptInputEvent->iType     = INPUT_TYPE_TOUCH;
	ptInputEvent->iX        = samp.x;
	ptInputEvent->iY        = samp.y;
	ptInputEvent->iPressure = samp.pressure;
	ptInputEvent->tTime     = samp.tv;

	return 0;
}

static int TouchscreenDeviceInit(void)
{
	g_ts = ts_setup(NULL, 0);
	if (!g_ts)
	{
		printf("ts_setup err\n");
		return -1;
	}

	return 0;
}

static int TouchscreenDeviceExit(void)
{
	ts_close(g_ts);
	return 0;
}


static InputDevice g_tTouchscreenDev ={
	.name = "touchscreen",
	.GetInputEvent  = TouchscreenGetInputEvent,
	.DeviceInit     = TouchscreenDeviceInit,
	.DeviceExit     = TouchscreenDeviceExit,
};

void TouchscreenRegister(void)
{
	RegisterInputDevice(&g_tTouchscreenDev);
}

