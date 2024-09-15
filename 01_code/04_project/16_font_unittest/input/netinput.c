

#include <input_manager.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/* socket
 * bind
 * sendto/recvfrom
 */

#define SERVER_PORT 8888

static int g_iSocketServer;

static int NetinputGetInputEvent(PInputEvent ptInputEvent)
{
	struct sockaddr_in tSocketClientAddr;
	int iRecvLen;
	char aRecvBuf[1000];
	
	unsigned int iAddrLen = sizeof(struct sockaddr);
	
	iRecvLen = recvfrom(g_iSocketServer, aRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
	if (iRecvLen > 0)
	{
		aRecvBuf[iRecvLen] = '\0';
		//printf("Get Msg From %s : %s\n", inet_ntoa(tSocketClientAddr.sin_addr), ucRecvBuf);
		ptInputEvent->iType 	= INPUT_TYPE_NET;
		gettimeofday(&ptInputEvent->tTime, NULL);
		strncpy(ptInputEvent->str, aRecvBuf, 1000);
		ptInputEvent->str[999] = '\0';
		return 0;
	}
	else
		return -1;
}

static int NetinputDeviceInit(void)
{
	struct sockaddr_in tSocketServerAddr;
	int iRet;
	
	g_iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == g_iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family      = AF_INET;
	tSocketServerAddr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
 	tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(tSocketServerAddr.sin_zero, 0, 8);
	
	iRet = bind(g_iSocketServer, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	return 0;
}

static int NetinputDeviceExit(void)
{
	close(g_iSocketServer);	
	return 0;
}


static InputDevice g_tNetinputDev ={
	.name = "touchscreen",
	.GetInputEvent  = NetinputGetInputEvent,
	.DeviceInit     = NetinputDeviceInit,
	.DeviceExit     = NetinputDeviceExit,
};


void NetInputRegister(void)
{
	RegisterInputDevice(&g_tNetinputDev);
}

