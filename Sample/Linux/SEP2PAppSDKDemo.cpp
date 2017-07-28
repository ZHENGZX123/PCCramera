
#include "SEP2P_Type.h"
#include "SEP2P_API.h"
#include "SE_VideoCodec.h"

/*
How to run this demo SEP2PAppSDKDemo program.

1>enter right 'demo did', 'demo username', 'demo password' to var 
  #define USER_DATA_DID			("demo did")
  #define USER_DATA_USERNAME	("demo username")
  #define USER_DATA_PASSWD		("demo password")
2>copy libSEP2P_AppSDK.so to the folder of SEP2PAppSDKDemo file
3>#./SEP2PAppSDKDemo

*/

#pragma pack(push,2)
    typedef struct tagBITMAPFILEHEADER {
            WORD    bfType;
            DWORD   bfSize;
            WORD    bfReserved1;
            WORD    bfReserved2;
            DWORD   bfOffBits;
    } BITMAPFILEHEADER;

    typedef struct tagBITMAPINFOHEADER{
            DWORD      biSize;
            DWORD      biWidth;
            DWORD      biHeight;
            WORD       biPlanes;
            WORD       biBitCount;
            DWORD      biCompression;
            DWORD      biSizeImage;
            DWORD      biXPelsPerMeter;
            DWORD      biYPelsPerMeter;
            DWORD      biClrUsed;
            DWORD      biClrImportant;
    }BITMAPINFOHEADER;
#pragma pack(pop)

#define BI_RGB 0L



#define MY_PRINT	printf

#define USER_DATA_DID		("demo did")
#define USER_DATA_USERNAME	("demo username")
#define USER_DATA_PASSWD	("demo password")
#define DID_LENGTH			17

#define MAX_SIZE_VIDEO_BMP24 6220800 //1920*1080*3

//---------------------------------------------------------------------------------------
INT32 g_eConnectStatus=0; //enum CONNECT_STATUS


UCHAR *g_handleVideoCodec=NULL;
CHAR  g_bFoundFirstIFrame=0;

UCHAR *g_outYUV420=NULL;
UCHAR *g_pBufBmp24=NULL;
BITMAPINFOHEADER g_bmiHead;
FILE  *g_fpWrite=NULL;
CHAR  g_bWriteOneBmp=1;

//---------------------------------------------------------------------------------------

INT32 OnSearchCallback(CHAR* pData, UINT32 nDataSize, VOID* pUserData)
{
	SEARCH_RESP *pSearchResp=(SEARCH_RESP *)pData;
	MY_PRINT("\tOnSearchCallback] %s %s FW_DeviceVersion=%s FW_P2PVersion=%s\n", pSearchResp->szDeviceID, pSearchResp->szIpAddr, pSearchResp->szFWDeviceVer, pSearchResp->szFWP2PVer);
	
	return 0;
}

INT32  OnRecvMsgCallback(CHAR* pDID, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize, VOID* pUserData)
{
	//nMsgType refer to SEP2P_ENUM_MSGTYPE of include\SEP2P_Define.h
	MY_PRINT("\tOnRecvMsgCallback] %s  nMsgType=0x%X, nMsgSize=%d\n", pDID, nMsgType, nMsgSize);
	if(nMsgType==SEP2P_MSG_CONNECT_STATUS){
		MSG_CONNECT_STATUS *pConnectStatus=(MSG_CONNECT_STATUS *)pMsg;
		g_eConnectStatus=pConnectStatus->eConnectStatus;
		MY_PRINT("\tOnRecvMsgCallback] SEP2P_MSG_CONNECT_STATUS eConnectStatus=%d\n", pConnectStatus->eConnectStatus);
	}

	return 0;
}

INT32  OnEventCallback(CHAR* pDID, UINT32 nEventType, CHAR* pEventData, UINT32 nEventDataSize, VOID* pUserData)
{
	//nEventType refer to SEP2P_ENUM_EVENT_TYPE of 'include\SEP2P_API.h'
	MY_PRINT("\tOnEventCallback] %s nEventDataSize=%d\n", pDID, nEventDataSize);
	return 0;
}

//Parameter:
//	pDID:		the same as USER_DATA_DID
//	pData:		STREAM_HEAD + body data.
//	nDataSize:	length of (STREAM_HEAD + body data)
//	pUserData:	the pUserData of SEP2P_SetStreamCallback(const CHAR* pDID, POnStreamCallback pStreamCallback, VOID *pUserData);
//
INT32  OnStreamCallback(CHAR* pDID, CHAR* pData, UINT32 nDataSize, VOID* pUserData)
{
	if(pDID==NULL) return -1;
	if(memcmp(pDID, USER_DATA_DID, DID_LENGTH)!=0) return -1;

	STREAM_HEAD *pStreadHead=(STREAM_HEAD *)pData;
    CHAR *pVideoData=pData+sizeof(STREAM_HEAD);

	//NOTES: stream data must append FIFO buffer!!!!! here is only one demo.
	if(pStreadHead->nCodecID<AV_CODECID_AUDIO_ADPCM){ //refer to SEP2P_ENUM_AV_CODECID
		//callback video data
		switch(pStreadHead->nCodecID){
			case AV_CODECID_VIDEO_MJPEG:
				break;

			case AV_CODECID_VIDEO_H264:{
				CHAR ch=(pStreadHead->nParameter==VIDEO_FRAME_FLAG_I) ? 'I' : 'P'; //pStreadHead->nParameter: Video: refer to SEP2P_ENUM_VIDEO_FRAME.
				if(!g_bFoundFirstIFrame){
					if(pStreadHead->nParameter==VIDEO_FRAME_FLAG_I) g_bFoundFirstIFrame=1;
					else break;
				}
			#ifdef APP_VIDEO_SUPPORT_DECODE
				if(g_handleVideoCodec==NULL) SEVideo_Create(VIDEO_CODE_TYPE_H264, &g_handleVideoCodec);
                
                INT32 nRet=0, nWidth=0, nHeigh=0, nWxH=0;
                ULONG in_outLen=MAX_SIZE_VIDEO_BMP24, nYUVSize=0;    
				nRet=SEVideo_Decode2YUV(g_handleVideoCodec,(UCHAR *)pVideoData, pStreadHead->nStreamDataLen, (UCHAR *)g_outYUV420, &in_outLen, &nWidth,&nHeigh);
				if(nRet>0){
					nYUVSize=in_outLen;
					in_outLen=MAX_SIZE_VIDEO_BMP24;
					SEVideo_YUV420toRGB24(g_outYUV420, nYUVSize, g_pBufBmp24, &in_outLen, nWidth, nHeigh);

                    if(g_fpWrite==NULL && g_bWriteOneBmp) g_fpWrite=fopen("./rgb24.bmp", "wb");
                    if(g_fpWrite){
                        g_bWriteOneBmp=0;

                        BITMAPFILEHEADER bfh;
                        memset( &bfh, 0, sizeof( bfh));
                    	bfh.bfType = 'MB';
                    	bfh.bfSize = sizeof( bfh ) + in_outLen + sizeof( BITMAPINFOHEADER );
                    	bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER);
                    	fwrite(&bfh, sizeof( bfh ), 1, g_fpWrite);

                        g_bmiHead.biWidth	=nWidth;
					    g_bmiHead.biHeight	=-nHeigh;
    					g_bmiHead.biSizeImage=0;                
                        fwrite(&g_bmiHead, 1, sizeof(g_bmiHead), g_fpWrite);
                        
                        fwrite(g_pBufBmp24, 1, in_outLen, g_fpWrite);
                        fflush(g_fpWrite);
                        fclose(g_fpWrite);
                        g_fpWrite=NULL;
                    }
				}
				MY_PRINT("Video: H264] %C frame, frame_len=%d, nWidth=%d nHeigh=%d\n", ch, pStreadHead->nStreamDataLen, nWidth, nHeigh);
			#endif

				}break;
			default:;
		}
		
	}else { //callback audio data
		switch(pStreadHead->nCodecID){
			case AV_CODECID_AUDIO_G711A:
				break;

			case AV_CODECID_AUDIO_AAC:{ //samplerate=16K,16bit,mono; head with 7 bytes
				MY_PRINT("\tAudio: AAC] frame_len=%d\n", pStreadHead->nStreamDataLen);
				}
				break;
			default:;
		}
	}
	
	return 0;
}


void *ThreadPlayVideo(void *arg)
{
	//NOTES: get stream data from FIFO buffer, then decode them!!!!!
	usleep(10000000);

	MY_PRINT("ThreadPlayVideo has exited.\n");
	pthread_exit(0);
}

int main(int argc, char **argv)
{
	char chDID[24]={0};
	char chUsername[16]={0};
	char chPassword[16]={0};
    char chDesc[248]={0};
	INT32 nRet=-1;
	pthread_t threadID=0;
	ST_InitStr stInitStr;

    //get the sdk version information
	UINT32 nAPIVer=SEP2P_GetSDKVersion(chDesc, sizeof(chDesc));
	char *pVer=(char *)&nAPIVer;
    MY_PRINT("SEP2P_API ver=%d.%d.%d.%d, %s\n", *(pVer+3), *(pVer+2), *(pVer+1), *(pVer+0), chDesc);
    
	strcpy(chDID, USER_DATA_DID);
	strcpy(chUsername, USER_DATA_USERNAME);
	strcpy(chPassword, USER_DATA_PASSWD);

	memset(&stInitStr, 0, sizeof(stInitStr));
	strcpy(stInitStr.chInitStr, "EFGBFFBJKFJKGGJBENHHFOEHGPJJDJNBGNFLEIDOEFJFPJPBHNBKGPPGHAOAMMLOAJNDPPCCLDNLFNGNMHNMNFBOMM");
	strcpy(stInitStr.chPrefix, "VIEW");
	SEP2P_Initialize(&stInitStr, 1);
    
	//Search the devices on the LAN
    SEP2P_StartSearch(OnSearchCallback, NULL);
    usleep(500000);
	SEP2P_StopSearch();

	//connect the specified device
	nRet=SEP2P_Connect(chDID, chUsername, chPassword);
	MY_PRINT("main] SEP2P_Connect(%s,..)=%d\n", chDID, nRet);
	if(nRet==ERR_SEP2P_SUCCESSFUL){
		SEP2P_SetStreamCallback(chDID,  OnStreamCallback,  NULL);
		SEP2P_SetRecvMsgCallback(chDID, OnRecvMsgCallback, NULL);
		SEP2P_SetEventCallback(chDID,   OnEventCallback,   NULL);
	}
	
	while(1){
		usleep(100000);
		if(g_eConnectStatus==CONNECT_STATUS_CONNECTED) break;
	}

    g_outYUV420=(UCHAR *)malloc(MAX_SIZE_VIDEO_BMP24);
    g_pBufBmp24=(UCHAR *)malloc(MAX_SIZE_VIDEO_BMP24);

    memset(&g_bmiHead, 0, sizeof(BITMAPINFOHEADER));
	g_bmiHead.biSize  = sizeof(BITMAPINFOHEADER);
	g_bmiHead.biPlanes= 1;
	g_bmiHead.biBitCount = 24;
	g_bmiHead.biCompression = BI_RGB;
    
	//request the video of this device
	if(g_eConnectStatus==CONNECT_STATUS_CONNECTED){
		//get audio/video parameter of this camera
		AV_PARAMETER stAVParameter;
		memset(&stAVParameter, 0, sizeof(AV_PARAMETER));
		nRet=SEP2P_GetAVParameterSupported(chDID, &stAVParameter);
		INT32 nSampleRate=(stAVParameter.nAudioParameter >> 2)&0x03; //Audio:(samplerate << 2) | (databits << 1) | (channel), samplerate refer to SEP2P_ENUM_AUDIO_SAMPLERATE; databits refer to SEP2P_ENUM_AUDIO_DATABITS; channel refer to SEP2P_ENUM_AUDIO_CHANNEL
		INT32 nDataBits  =(stAVParameter.nAudioParameter >> 1)&0x01;
		INT32 nChannel   =stAVParameter.nAudioParameter & 0x01;
		if(nRet>=0){
			MY_PRINT("main] Audio supported] nAudioCodecID_enum=0x%X nSampleRate_enum=%d nDataBits_enum=%d, nChannel_enum=%d\n", 
					stAVParameter.nAudioCodecID, nSampleRate, nDataBits, nChannel);
			for(nRet=0; nRet<sizeof(stAVParameter.nVideoParameter)/sizeof(CHAR); nRet++){
				if((stAVParameter.nVideoParameter[nRet]&0xFF)==VIDEO_RESO_UNKNOWN) break;
				MY_PRINT("main] Video supported] nVideoCodecID_enum=0x%X Video resolution_enum=%d\n", stAVParameter.nVideoCodecID, stAVParameter.nVideoParameter[nRet]);
			}
		}
		MY_PRINT("\n");
		//Import: must request video then request audio
		nRet=SEP2P_SendMsg(chDID, SEP2P_MSG_START_VIDEO, NULL, 0);  //request default parameter video to the camera
		nRet=SEP2P_SendMsg(chDID, SEP2P_MSG_START_AUDIO, NULL, 0);  //request default parameter audio to the camera
		MY_PRINT("main] SEP2P_SendMsg(%s SEP2P_MSG_START_AUDIO.)=%d\n", chDID, nRet);
		if(nRet>=0){
			if(pthread_create(&threadID, NULL, &ThreadPlayVideo, (void *)NULL)==0) {
				pthread_join(threadID, NULL);
			}
		}
		nRet=SEP2P_SendMsg(chDID, SEP2P_MSG_STOP_VIDEO, NULL, 0);
		nRet=SEP2P_SendMsg(chDID, SEP2P_MSG_STOP_AUDIO, NULL, 0);
	}

    SEP2P_DeInitialize();
    
#ifdef APP_VIDEO_SUPPORT_DECODE
    if(g_handleVideoCodec) SEVideo_Destroy(&g_handleVideoCodec);
#endif
    if(g_outYUV420) {
        free(g_outYUV420);
        g_outYUV420=NULL;
    }
    if(g_pBufBmp24) {
        free(g_pBufBmp24);
        g_pBufBmp24=NULL;
    }
    
	MY_PRINT("main] main is over\n");
}
