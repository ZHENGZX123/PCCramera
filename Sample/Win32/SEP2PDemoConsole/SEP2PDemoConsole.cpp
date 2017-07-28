// SEP2PDemoConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SEP2P_Type.h"
#include "SEP2P_API.h"
#include "SEP2P_Define.h"
#include <signal.h>


typedef	 DWORD  pub_threadid;
typedef	 DWORD  handle_thread;
typedef handle_thread (WINAPI *PPUB_PTHREAD_START_ROUTINE)(VOID* lpThreadParameter);
INT32 pub_CreateThread(PPUB_PTHREAD_START_ROUTINE lpStartAddress, IN VOID* lpParameter, IN INT32 dwCreationFlags, IN CHAR bDetach, OUT VOID **hhThread, OUT pub_threadid* lpThreadId);
VOID  pub_DestroyThread(VOID** hThread);


#define pub_thread_join(hHandle, threadid){ WaitForSingleObject(hHandle, INFINITE); if(hHandle) { CloseHandle(hHandle);  hHandle=NULL;}}

//---------------------------------------------------------------------------------------
INT32 g_eConnectStatus=0; //enum CONNECT_STATUS
CHAR  g_bRunning=0;
//---------------------------------------------------------------------------------------

INT32 OnSearchCallback(CHAR* pData, UINT32 nDataSize, VOID* pUserData)
{
	SEARCH_RESP *pSearchResp=(SEARCH_RESP *)pData;
	printf("\tOnSearchCallback] %s %s\n", pSearchResp->szDeviceID, pSearchResp->szIpAddr);

	return 0;
}

INT32  OnRecvMsgCallback(CHAR* pDID, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize, VOID* pUserData)
{
	printf("\tOnRecvMsgCallback] %s nMsgSize=%d nMsgType=0x%X\n", pDID, nMsgSize, nMsgType);
	if(nMsgType==SEP2P_MSG_CONNECT_STATUS){
		MSG_CONNECT_STATUS *pConnectStatus=(MSG_CONNECT_STATUS *)pMsg;
		g_eConnectStatus=pConnectStatus->eConnectStatus;
		printf("\tOnRecvMsgCallback] SEP2P_MSG_CONNECT_STATUS eConnectStatus=%d\n", pConnectStatus->eConnectStatus);
	}

	return 0;
}

INT32  OnEventCallback(CHAR* pDID, UINT32 nEventType, CHAR* pEventData, UINT32 nEventDataSize, VOID* pUserData)
{
	printf("\tOnEventCallback] %s nEventDataSize=%d\n", pDID, nEventDataSize);
	return 0;
}

INT32  OnStreamCallback(CHAR* pDID, CHAR*	pData, UINT32  nDataSize, VOID*	pUserData)
{
	printf("\tOnStreamCallback] %s nDataSize=%d\n", pDID, nDataSize);
	return 0;
}

/*
void *ThreadPlayVideo(void *arg)
{
	Sleep(5000);

}
*/

void hand_signal(int i)
{
	g_bRunning=0;

	printf("hand_signal arrived. i=%d \n", i);
}

int _tmain(int argc, _TCHAR* argv[])
{
	char chDID[24]={"VIEW-000146-ZNEXT"}; //VIEW-000146-ZNEXT  VIEW-000254-WUDUL
	char chUsername[16]={"admin"};
	char chPassword[16]={"123456"};
	char chDesc[256]={0};
	INT32 nRet=-1;
	//handle_thread threadID=0;

	//get the sdk version information
	UINT32 nAPIVer=SEP2P_GetSDKVersion(chDesc, sizeof(chDesc));
	char *pVer=(char *)&nAPIVer;
	printf("SEP2P_API ver=%d.%d.%d.%d, %s\n", *(pVer+3), *(pVer+2), *(pVer+1), *(pVer+0), chDesc);

	ST_InitStr stInitStr;
	memset(&stInitStr, 0, sizeof(stInitStr));
	strcpy(stInitStr.chInitStr, "EFGBFFBJKFJKGGJBENHHFOEHGPJJDJNBGNFLEIDOEFJFPJPBHNBKGPPGHAOAMMLOAJNDPPCCLDNLFNGNMHNMNFBOMM");
	strcpy(stInitStr.chPrefix,  "VIEW");
	SEP2P_Initialize(&stInitStr, 1);
	
	g_bRunning=1;

	//Search the devices on the LAN
	/*
	SEP2P_StartSearch(OnSearchCallback, NULL);
	Sleep(1000);
	SEP2P_StopSearch();
	*/

	//connect the specified device
	signal(SIGINT, hand_signal);
	//while(g_bRunning)
	{
		nRet=SEP2P_Connect(chDID, chUsername, chPassword);
		printf("\tmain] SEP2P_Connect(%s,..)=%d\n", chDID, nRet);
		if(nRet==ERR_SEP2P_SUCCESSFUL){
			SEP2P_SetStreamCallback(chDID,  OnStreamCallback,  NULL);
			SEP2P_SetRecvMsgCallback(chDID, OnRecvMsgCallback, NULL);
			SEP2P_SetEventCallback(chDID,   OnEventCallback,   NULL);
		}
	
		Sleep(60000);
		//if(g_eConnectStatus==CONNECT_STATUS_CONNECTED) break;

		nRet=SEP2P_Disconnect(chDID);
		printf("\tmain] SEP2P_Disconnect(%s,..)=%d\n\n", chDID, nRet);
	}
	//request the video of this device
	if(g_eConnectStatus==CONNECT_STATUS_CONNECTED){
		nRet=SEP2P_SendMsg(chDID, SEP2P_MSG_START_VIDEO, NULL, 0);
		printf("\tmain] SEP2P_SendMsg(%s SEP2P_MSG_START_VIDEO.)=%d\n", chDID, nRet);
		if(nRet>=0){
			
			/*
			if(pthread_create(&threadID, NULL, &ThreadPlayVideo, (void *)NULL)==0) {
				pthread_join(threadID, NULL);
			}
			*/
		}
		nRet=SEP2P_SendMsg(chDID, SEP2P_MSG_STOP_VIDEO, NULL, 0);
	}

	Sleep(60000);
	SEP2P_DeInitialize();
	printf("\tmain] main is over\n");

	return 0;
}

