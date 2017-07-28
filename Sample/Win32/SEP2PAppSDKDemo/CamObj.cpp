
#include "StdAfx.h"
#include "SEP2PAppSDKDemo.h"
#include "CamObj.h"
#include "Picture.h"
#include "SE_AudioCodec.h"
#include "SE_VideoCodec.h"
#include "wave_out.h"

#include "SEP2PAppSDKDemoDlg.h"


#define MAX_SIZE_VIDEO_BMP24		6220800 //1920*1080*3

char CCamObj::m_bOnlyAudio=0;
CCamObj::CCamObj(UCHAR nChanNo, CSEP2PAppSDKDemoDlg *pMainWnd)
{
	m_pMainWnd  =pMainWnd;
	m_nChannelNo=nChanNo;
	ASSERT(pMainWnd!=NULL);

	m_bConnecting=1;
	m_bStartVideo=1;
	m_bStartAudio=1;
	m_bStartTalk =1;

	memset(m_chDID,0,sizeof(m_chDID));
	memset(m_tchUsername, 0, sizeof(m_tchUsername));
	memset(m_tchPasswd,0, sizeof(m_tchPasswd));
	memset(m_chProductSeries, 0, sizeof(m_chProductSeries));

	m_bConnectedDev=0;
	memset(&m_stAVParam, 0, sizeof(AV_PARAMETER));
	m_pCtrlEdit=NULL;
	m_pCtrlConnect=NULL;
	m_pCtrlStartVideo=NULL;
	m_pCtrlStartAudio=NULL;
	m_pCtrlStartTalk=NULL;
	m_pCtrlStatus=NULL;
	m_hDC=NULL;

	m_pObjPicture=NULL;
	m_pObjRect=new CRect();

	memset(&m_bmiHead, 0, sizeof(BITMAPINFOHEADER));
	m_bmiHead.biSize  = sizeof(BITMAPINFOHEADER);
	m_bmiHead.biPlanes= 1;
	m_bmiHead.biBitCount = 24;
	m_bmiHead.biCompression = BI_RGB;

	m_pBufBmp24=NULL;

	m_nTalkAudioSizeTmp=0;
	m_pTalkAudioDataTmp=NULL;

	m_pHandleAudioCodec =NULL;
	SEVideo_Create(VIDEO_CODE_TYPE_H264, &m_pHandleH264);

	m_bPlayingVideo=0;
	m_bPlayingAudio=0;
	m_hThreadPlayVideo=NULL;
	m_hThreadPlayAudio=NULL;

	MUTEX_Init(m_mutexVideo);
	MUTEX_Init(m_mutexAudio);
	m_dequeVideo.RemoveAll();
	m_dequeAudio.RemoveAll();
	ResetAudioVar();
	ResetVideoVar();
}

CCamObj::~CCamObj(void)
{
	StopPlayAudio();
	StopPlayVideo();

	if(m_pObjPicture){
		delete m_pObjPicture;
		m_pObjPicture=NULL;
	}
	if(m_pObjRect){
		delete m_pObjRect;
		m_pObjRect=NULL;
	}
	if(m_pBufBmp24){
		delete m_pBufBmp24;
		m_pBufBmp24=NULL;
	}

	//release audio handle
	if(m_pHandleAudioCodec) {
		SEAudio_Destroy(&m_pHandleAudioCodec);
		m_pHandleAudioCodec=NULL;
	}
	if(m_pHandleH264) {
		SEVideo_Destroy(&m_pHandleH264);
		m_pHandleH264=NULL;
	}
	if(m_pTalkAudioDataTmp) {
		delete []m_pTalkAudioDataTmp;
		m_pTalkAudioDataTmp=NULL;
	}
	VideoDataClear();
	AudioDataClear();

	MUTEX_Destroy(m_mutexVideo);
	MUTEX_Destroy(m_mutexAudio);
}

//static
UINT32 CCamObj::P2PAPI_Version()
{
	char chDesc[256]={0};
	UINT32 nAPIVer=SEP2P_GetSDKVersion(chDesc, sizeof(chDesc));
	char *pVer=(char *)&nAPIVer;
	TRACE("SEP2P_API ver=%d.%d.%d.%d, %s\n", *(pVer+3), *(pVer+2), *(pVer+1), *(pVer+0), chDesc);
	return nAPIVer;
}

//static 
void CCamObj::P2PAPI_Init()
{
	ST_InitStr stInitStr;
	memset(&stInitStr, 0, sizeof(stInitStr));
	strcpy(stInitStr.chInitStr, "EFGBFFBJKFJKGGJBENHHFOEHGPJJDJNBGNFLEIDOEFJFPJPBHNBKGPPGHAOAMMLOAJNDPPCCLDNLFNGNMHNMNFBOMM");
	strcpy(stInitStr.chPrefix, "VIEW");
	INT32 nRet=SEP2P_Initialize(&stInitStr, 1);

	TRACE(_T("P2PAPI_Init=%d\n"), nRet);
}

//static 
void CCamObj::P2PAPI_DeInit()
{
	INT32 nRet=SEP2P_DeInitialize();
	TRACE(_T("P2PAPI_DeInit=%d\n"), nRet);
}

INT32 CCamObj::P2PAPI_StartSearch()
{
	return SEP2P_StartSearch(OnSearchCallback, this);
}

void  CCamObj::P2PAPI_StopSearch()
{
	INT32 nRet=SEP2P_StopSearch();
}

INT32 CCamObj::P2PAPI_Connect()
{
 	INT32 nRet=ERR_P2PAPI_BASE;
	if(m_chDID[0]=='\0' || m_tchUsername[0]=='\0') return ERR_P2PAPI_BASE;

	CHAR chUsername[64]={0};
	CharFromWSTRU(CP_OEMCP, m_tchUsername, wcslen(m_tchUsername), chUsername);
	CHAR chPasswd[64]={0};
	CharFromWSTRU(CP_OEMCP, m_tchPasswd, wcslen(m_tchPasswd), chPasswd);
	nRet=SEP2P_Connect(m_chDID, chUsername, chPasswd);
	if(nRet==ERR_SEP2P_SUCCESSFUL){
		SEP2P_SetStreamCallback(m_chDID,  OnStreamCallback,  this);
		SEP2P_SetRecvMsgCallback(m_chDID, OnRecvMsgCallback, this);
		SEP2P_SetEventCallback(m_chDID,   OnEventCallback,   this);
	}
	TRACE(_T("SEP2P_Connect=%d\n"), nRet);
	return nRet;
}

void  CCamObj::P2PAPI_Disconnect()
{
	if(m_chDID[0]!='\0') SEP2P_Disconnect(m_chDID);
}

INT32 CCamObj::P2PAPI_SendMsg(INT32 nMsgType, const CHAR* pMsgData, INT32 nMsgDataSize)
{
	if(m_chDID[0]=='\0') return ERR_P2PAPI_BASE;
	INT32 nRet=SEP2P_SendMsg(m_chDID, nMsgType, pMsgData, nMsgDataSize);
	if(nRet>=0){
		if(nMsgType==SEP2P_MSG_START_TALK){
			m_nTalkAudioSizeTmp=0;
			if(m_pTalkAudioDataTmp==NULL) m_pTalkAudioDataTmp=new CHAR[MAX_SIZE_TMP_TALK];

			if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_G711A){
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_G711A, &m_pHandleAudioCodec);

			}else if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_G726){
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_G726, &m_pHandleAudioCodec);

			}else if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_ADPCM){
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_ADPCM, &m_pHandleAudioCodec);

			}else if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_AAC){
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_AAC, &m_pHandleAudioCodec);

			}
		}else if(nMsgType==SEP2P_MSG_STOP_TALK){
			if(m_pTalkAudioDataTmp) {
				delete []m_pTalkAudioDataTmp;
				m_pTalkAudioDataTmp=NULL;
			}
		}else if(nMsgType==SEP2P_MSG_START_VIDEO) nRet=StartPlayVideo();
		else  if(nMsgType==SEP2P_MSG_STOP_VIDEO)  StopPlayVideo();
		else  if(nMsgType==SEP2P_MSG_START_AUDIO) nRet=StartPlayAudio();
		else  if(nMsgType==SEP2P_MSG_STOP_AUDIO)  StopPlayAudio();
	}
	return nRet;
}

INT32 CCamObj::P2PAPI_TalkData(CHAR* pData, UINT32  nDataSize)
{
	UCHAR chAudioEncoded[1024];
	memset(chAudioEncoded, 0, sizeof(chAudioEncoded));
	UINT32 outLen=0, i=0;
	UCHAR *pHandleEncode=NULL;
	INT32 nRet=ERR_P2PAPI_BASE, nRetTmp=0;
	if(!m_bConnectedDev) return ERR_P2PAPI_BASE;

	CHAR   *pAudioData=pData;
	UINT32 nAudioSize=nDataSize, nAudioSizeLeft=0;

	if(m_pHandleAudioCodec!=NULL) pHandleEncode=m_pHandleAudioCodec;
	if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_ADPCM){
		//must be 1024 PCM data
		if((MAX_SIZE_TMP_TALK-m_nTalkAudioSizeTmp)>=nDataSize){
			memcpy(m_pTalkAudioDataTmp+m_nTalkAudioSizeTmp, pData, nDataSize);
			m_nTalkAudioSizeTmp+=nDataSize;
			return ERR_SEP2P_SUCCESSFUL;
		}else{
			memcpy(m_pTalkAudioDataTmp+m_nTalkAudioSizeTmp, pData, MAX_SIZE_TMP_TALK-m_nTalkAudioSizeTmp);
			pAudioData=m_pTalkAudioDataTmp;
			nAudioSize=MAX_SIZE_TMP_TALK;

			nAudioSizeLeft=nDataSize-(MAX_SIZE_TMP_TALK-m_nTalkAudioSizeTmp);
		}
	}

	if(pHandleEncode){
		if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_AAC){
			outLen=sizeof(chAudioEncoded);
			nRetTmp=SEAudio_Encode(pHandleEncode, (UCHAR *)pAudioData, nAudioSize, chAudioEncoded, &outLen);
			if(nRetTmp>0 && outLen>0) {
				DWORD nTickCount=GetTickCount();
				nRet=SEP2P_SendTalkData(m_chDID, (CHAR *)chAudioEncoded, outLen, nTickCount);
				TRACE(_T("P2PAPI_TalkData] SEAudio_Encode=%d nAudioCodecID=0x%X nAudioSize=%d outLen=%d, nTickCount=%u SEP2P_SendTalkData=%d\n"), 
						nRetTmp, m_stAVParam.nAudioCodecID, nAudioSize,outLen, nTickCount, nRet);
			}
		}else{
			outLen=sizeof(chAudioEncoded);
			nRetTmp=SEAudio_Encode(pHandleEncode, (UCHAR *)pAudioData, nAudioSize, chAudioEncoded, &outLen);
			nRet=SEP2P_SendTalkData(m_chDID, (CHAR *)chAudioEncoded, outLen, GetTickCount());

			CString csDID;
			BSTRFromCharU(CP_OEMCP, m_chDID, csDID);
			TRACE(_T("CSEP2PAppSDKDemoDlg::OnSampleCallback] %s nAudioCodecID=0x%X %d, outLen=%d, SEP2P_SendTalkData=%d\n"), 
				csDID, m_stAVParam.nAudioCodecID, nDataSize,outLen, nRet);
		}
		
	}else nRet=ERR_P2PAPI_UNKNOWN_CODEC_WHEN_TALK;

	if(m_stAVParam.nAudioCodecID==AV_CODECID_AUDIO_ADPCM){
		memcpy(m_pTalkAudioDataTmp, pData+MAX_SIZE_TMP_TALK-m_nTalkAudioSizeTmp, nAudioSizeLeft);
		m_nTalkAudioSizeTmp=nAudioSizeLeft;
	}
	
	return nRet;
}


//static 
INT32  CCamObj::OnStreamCallback(CHAR* pDID, CHAR*	pData, UINT32  nDataSize, VOID*	pUserData)
{
	CCamObj *pTHIS=(CCamObj *)pUserData;
	pTHIS->DoStreamCallback(pDID, pData, nDataSize);
	return 0;
}

UINT32 g_nTimestampPrev=0;
void CCamObj::DoStreamCallback(CHAR* pDID, CHAR* pData, UINT32 nDataSize)
{
	STREAM_HEAD *pStreamHead=(STREAM_HEAD *)pData;
	if(pDID[0]=='\0' || pData==NULL) return;
	if(memcmp(m_chDID, pDID, MAX_LEN_DID)!=0) return;

	if(pStreamHead->nCodecID==AV_CODECID_UNKNOWN) return;
	if(pStreamHead->nCodecID<AV_CODECID_AUDIO_ADPCM){
		//video data
		VideoDataAdd(pData, nDataSize);
		
		if(pStreamHead->nLivePlayback==1)
		{
			CHAR chTmp[256]={0};
			sprintf(chTmp, "\tLiveView] V nParameter=%d nTimestamp=%u span_ts=%d size=%d\n", 
				pStreamHead->nParameter, pStreamHead->nTimestamp, pStreamHead->nTimestamp-g_nTimestampPrev, pStreamHead->nStreamDataLen);
			OutputDebugStringA(chTmp);
			g_nTimestampPrev=pStreamHead->nTimestamp;
		}
	}else { //audio data
		AudioDataAdd(pData, nDataSize);
// 		if(pStreamHead->nLivePlayback==1){
// 			CHAR chTmp[256]={0};
// 			sprintf(chTmp, "Playback] A nParameter=%d nTimestamp=%d nLivePlayback=%d size=%d\n", pStreamHead->nParameter, pStreamHead->nTimestamp, pStreamHead->nLivePlayback, pStreamHead->nStreamDataLen);
// 			OutputDebugStringA(chTmp);			
// 		}
	}
}

//static 
INT32 CCamObj::OnSearchCallback(CHAR* pData, UINT32 nDataSize, VOID* pUserData)
{
	CCamObj *pTHIS=(CCamObj *)pUserData;
	pTHIS->DoSearchCallback(pData, nDataSize);
	return 0;
}

void CCamObj::DoSearchCallback(CHAR* pData, UINT32 nDataSize)
{
	m_pMainWnd->DoSearch(pData, nDataSize);
}

//static 
INT32  CCamObj::OnRecvMsgCallback(CHAR* pDID, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize, VOID* pUserData)
{
	CCamObj *pTHIS=(CCamObj *)pUserData;
	pTHIS->DoRecvMsgCallback(pDID, nMsgType, pMsg, nMsgSize);
	return 0;
}

void CCamObj::DoRecvMsgCallback(CHAR* pDID, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize)
{
	if(pDID==NULL || memcmp(m_chDID, pDID, MAX_LEN_DID)!=0) return;

	if(nMsgType==SEP2P_MSG_CONNECT_STATUS){
		MSG_CONNECT_STATUS *pConnectStatus=(MSG_CONNECT_STATUS *)pMsg;
		if(pConnectStatus->eConnectStatus==CONNECT_STATUS_CONNECTED){
			m_bConnectedDev=1;
			P2PAPI_GetAVParameter();
			P2PAPI_SendMsg(SEP2P_MSG_GET_USER_INFO2_REQ, NULL, 0);
			P2PAPI_SendMsg(SEP2P_MSG_GET_DEVICE_VERSION_REQ, NULL, 0);

			CHAR chTmp[258]={0}, chIP[3][24];
			strcpy(chIP[0],inet_ntoa(pConnectStatus->stRemoteAddr.sin_addr));
			strcpy(chIP[1],inet_ntoa(pConnectStatus->stMyLocalAddr.sin_addr));
			strcpy(chIP[2],inet_ntoa(pConnectStatus->stMyWanAddr.sin_addr));
			sprintf(chTmp, "DoRecvMsgCallback]  CONNECT_STATUS_CONNECTED: %s; Remote:%s:%d   MyLAN: %s:%d  MyWAN: %s:%d\n\n", 
				pConnectStatus->eConnectMode==CONNECT_MODE_P2P ? "P2P" : "Relay",
				chIP[0],  ntohs(pConnectStatus->stRemoteAddr.sin_port),
				chIP[1],  ntohs(pConnectStatus->stMyLocalAddr.sin_port),
				chIP[2],  ntohs(pConnectStatus->stMyWanAddr.sin_port));
			OutputDebugStringA(chTmp);

		}else m_bConnectedDev=0;

	}else if(nMsgType==SEP2P_MSG_CONNECT_MODE){
		CONNECT_MODE *pConnMode=(CONNECT_MODE *)pMsg;
		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback]  pConnMode=%d\n", *pConnMode);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_DEVICE_VERSION_RESP){
		MSG_GET_DEVICE_VERSION_RESP *pResp=(MSG_GET_DEVICE_VERSION_RESP *)pMsg;
		memcpy(m_chProductSeries, pResp->product_series, sizeof(m_chProductSeries));

		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_GET_DEVICE_VERSION_RESP m_chProductSeries=%s chVendor=%s\n", m_chProductSeries, pResp->chVendor);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_USER_INFO_RESP){
		MSG_GET_USER_INFO_RESP *pResp=(MSG_GET_USER_INFO_RESP *)pMsg;

		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_GET_USER_INFO_RESP pResp->chAdmin=%s, pResp->nCurUserRoleID=%d\n", pResp->chAdmin, pResp->nCurUserRoleID);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_EMAIL_GET_TEST_RESULT_RESP){
		MSG_EMAIL_GET_TEST_RESULT_RESP *pResp=(MSG_EMAIL_GET_TEST_RESULT_RESP *)pMsg;

		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_EMAIL_GET_TEST_RESULT_RESP result=%d\n", pResp->result);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_USER_INFO2_RESP){
		MSG_GET_USER_INFO2_RESP *pResp=(MSG_GET_USER_INFO2_RESP *)pMsg;
		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_GET_USER_INFO2_RESP nUserNum=%d\n", pResp->nUserNum);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_SET_USER_INFO2_RESP){
		MSG_SET_USER_INFO2_RESP *pResp=(MSG_SET_USER_INFO2_RESP *)pMsg;
		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_SET_USER_INFO2_RESP result=%d\n", pResp->result);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_CAMERA_PARAM_RESP){
		MSG_GET_CAMERA_PARAM_RESP *pResp=(MSG_GET_CAMERA_PARAM_RESP *)pMsg;
		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_SET_CAMERA_PARAM_RESP bOSD=0x%X\n", pResp->bOSD);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_SET_CAMERA_PARAM_RESP){
		MSG_SET_CAMERA_PARAM_RESP *pResp=(MSG_SET_CAMERA_PARAM_RESP *)pMsg;
		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_SET_CAMERA_PARAM_RESP result=%d\n", pResp->result);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_WIFI_LIST_RESP){
		MSG_GET_WIFI_LIST_RESP *pResp=(MSG_GET_WIFI_LIST_RESP *)pMsg;
		CHAR chTmp[258]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_GET_WIFI_LIST_RESP wifi num=%d\n", pResp->nResultCount);
		OutputDebugStringA(chTmp);
	}

	m_pMainWnd->DoRecvMsg(m_nChannelNo, nMsgType, pMsg, nMsgSize);
}

//static 
INT32  CCamObj::OnEventCallback(CHAR* pDID, UINT32 nEventType, CHAR* pEventData, UINT32 nEventDataSize, VOID* pUserData)
{
	CCamObj *pTHIS=(CCamObj *)pUserData;
	pTHIS->DoEventCallback(pDID, nEventType, pEventData, nEventDataSize);
	return 0;
}

void CCamObj::DoEventCallback(CHAR* pDID, UINT32 nEventType, CHAR* pEventData, UINT32 nEventDataSize)
{
	if(pDID==NULL || memcmp(m_chDID, pDID, MAX_LEN_DID)!=0) return;
	
	if(nEventType<EVENT_TYPE_DOORBELL_ALARM){
		//EVENT_TYPE_MOTION_ALARM = 0x01,
		//EVENT_TYPE_INPUT_ALARM  = 0x02,
		//EVENT_TYPE_AUDIO_ALARM  = 0x03,
		//EVENT_TYPE_MANUAL_ALARM = 0x04,
		TRACE(_T("DoEventCallback nEventType=%d\n"), nEventType);

	}else{
		if((nEventType & EVENT_TYPE_DOORBELL_ALARM)==EVENT_TYPE_DOORBELL_ALARM){
			//EVENT_TYPE_DOORBELL_ALARM
		}
		if((nEventType & EVENT_TYPE_TEMPERATURE_ALARM)==EVENT_TYPE_TEMPERATURE_ALARM){
			//EVENT_TYPE_TEMPERATURE_ALARM
		}
		if((nEventType & EVENT_TYPE_HUMIDITY_ALARM)==EVENT_TYPE_HUMIDITY_ALARM){
			//EVENT_TYPE_HUMIDITY_ALARM
		}
		EVENT_DATA *pStEvent=(EVENT_DATA *)pEventData;
		TRACE(_T("DoEventCallback nEventType=%d nChannel=%d nTypeNotify=%d\n"), nEventType, pStEvent->nChannel, pStEvent->nTypeNotify);
	}
}


INT32 CCamObj::VideoDataAdd(CHAR *pData, INT32 nDataSize)
{
	if(pData==NULL || nDataSize<=0) return ERR_P2PAPI_BASE;

	MUTEX_Lock(m_mutexVideo);
		CHAR *pVideoData=new CHAR[nDataSize];
		memcpy(pVideoData, pData, nDataSize);
		m_dequeVideo.AddTail(pVideoData);
	MUTEX_Unlock(m_mutexVideo);
	return m_dequeVideo.GetSize();
}

CHAR*  CCamObj::VideoDataGetAndRemove()
{
	CHAR *pVideo=NULL;
	MUTEX_Lock(m_mutexVideo);
		if(m_dequeVideo.GetSize()>0){
			pVideo=(CHAR *)m_dequeVideo.RemoveHead();
		}
	MUTEX_Unlock(m_mutexVideo);
	return pVideo; 
}

void CCamObj::VideoDataClear()
{
	int k=0;
	CHAR *pVideo=NULL;
	MUTEX_Lock(m_mutexVideo);
		for(k=0; k<m_dequeVideo.GetSize(); k++){
			pVideo=(CHAR *)m_dequeVideo.RemoveHead();
			if(pVideo) delete []pVideo;
		}
	MUTEX_Unlock(m_mutexVideo);
}

INT32 CCamObj::AudioDataAdd(CHAR *pData, INT32 nDataSize)
{
	if(pData==NULL || nDataSize<=0) return ERR_P2PAPI_BASE;

	MUTEX_Lock(m_mutexAudio);
		//TRACE(_T("AudioDataAdd, nDataSize=%d\n"), nDataSize);

		CHAR *pAudioData=new CHAR[nDataSize];
		memcpy(pAudioData, pData, nDataSize);
		m_dequeAudio.AddTail(pAudioData);
	MUTEX_Unlock(m_mutexAudio);
	return m_dequeAudio.GetSize();
}

CHAR* CCamObj::AudioDataGetAndRemove()
{
	CHAR *pAudio=NULL;
	MUTEX_Lock(m_mutexAudio);
		if(m_dequeAudio.GetSize()>0){
			pAudio=(CHAR *)m_dequeAudio.RemoveHead();
		}
	MUTEX_Unlock(m_mutexAudio);
	return pAudio; 
}

void  CCamObj::AudioDataClear()
{
	int k=0;
	CHAR *pAudio=NULL;
	MUTEX_Lock(m_mutexAudio);
		for(k=0; k<m_dequeAudio.GetSize(); k++){
			pAudio=(CHAR *)m_dequeAudio.RemoveHead();
			if(pAudio) delete []pAudio;
		}
	MUTEX_Unlock(m_mutexAudio);
}

INT32 CCamObj::StartPlayVideo()
{
	if(m_hThreadPlayVideo==NULL){
		DWORD nThreadID=0;
		m_hThreadPlayVideo=CreateThread(NULL, 0,ThreadPlayVideo, this, 0, &nThreadID);
		if(m_hThreadPlayVideo==NULL) return -1;
	}
	return 0;
}

void  CCamObj::StopPlayVideo()
{
	m_bPlayingVideo=0;
	if(m_hThreadPlayVideo!=NULL){
		WaitForSingleObject(m_hThreadPlayVideo, INFINITE); 
		CloseHandle(m_hThreadPlayVideo); 
		m_hThreadPlayVideo=NULL;
	}
}

INT32 CCamObj::StartPlayAudio()
{
	if(m_bOnlyAudio!=0) return ERR_P2PAPI_ALREADY_OPEN_AUDIO;
	m_bOnlyAudio=1;

	if(m_hThreadPlayAudio==NULL){
		DWORD nThreadID=0;
		m_hThreadPlayAudio=CreateThread(NULL, 0,ThreadPlayAudio, this, 0, &nThreadID);
		if(m_hThreadPlayVideo==NULL) return ERR_P2PAPI_BASE;
	}
	return 0;
}

void  CCamObj::StopPlayAudio()
{
	m_bPlayingAudio=0;
	if(m_hThreadPlayAudio!=NULL){
		WaitForSingleObject(m_hThreadPlayAudio, INFINITE); 
		CloseHandle(m_hThreadPlayAudio); 
		m_hThreadPlayAudio=NULL;

		m_bOnlyAudio=0;
	}
}

void CCamObj::ResetAudioVar()
{

	AudioDataClear();
}

void CCamObj::ResetVideoVar()
{
	m_nFirstTickLocal_video=0L;
	m_nTick2_video=0L;
	m_nFirstTimestampDevice_video=0L;

	VideoDataClear();
}

ULONG WINAPI CCamObj::ThreadPlayVideo(void* lpPara)
{
	CCamObj* pTHIS=(CCamObj *)lpPara;
	pTHIS->DoPlayVideo();
	TRACE(_T("---ThreadPlayVideo exited----\n"));
	return 0L;
}

void CCamObj::DoPlayVideo()
{
	m_bPlayingVideo=1;
	ULONG nDiffTimeStamp=0L;
	STREAM_HEAD *pStreamHead=NULL;
	CHAR *pData=NULL, *pVideoData;

	UCHAR *outYUV420=(UCHAR *)malloc(MAX_SIZE_VIDEO_BMP24);
	int  nRet=0, nWidth=0, nHeigh=0, nWxH=0;
	ULONG in_outLen=MAX_SIZE_VIDEO_BMP24, nYUVSize=0;

	UINT32 nTimestampPrev=0L;

	TRACE(_T("---ThreadPlayVideo goning......\n"));
	while(m_bPlayingVideo){
		pData=VideoDataGetAndRemove();
		if(pData==NULL){
			Sleep(10);
			continue;
		}
		pStreamHead=(STREAM_HEAD *)pData;
		pVideoData=pData+sizeof(STREAM_HEAD);
		m_nTick2_video=GetTickCount();
		if(m_nFirstTimestampDevice_video==0 || m_nFirstTickLocal_video==0){
			m_nFirstTimestampDevice_video=pStreamHead->nTimestamp;
			m_nFirstTickLocal_video		 =m_nTick2_video;
		}
		if(m_nTick2_video<m_nFirstTickLocal_video || pStreamHead->nTimestamp<m_nFirstTimestampDevice_video)
		{
			m_nFirstTimestampDevice_video=pStreamHead->nTimestamp;
			m_nFirstTickLocal_video		 =m_nTick2_video;
		}

		nDiffTimeStamp=(pStreamHead->nTimestamp-m_nFirstTimestampDevice_video) - (m_nTick2_video-m_nFirstTickLocal_video);
		nTimestampPrev=pStreamHead->nTimestamp;

// 		if(nDiffTimeStamp>0 && nDiffTimeStamp<=1000){
// 			for(int kk=0; kk<nDiffTimeStamp; kk++){
// 				if(!m_bPlayingVideo) break;
// 				Sleep(1);
// 			}
// 		}

		TRACE(_T("   DoPlayVideo(), nNumConnected=%d nNumLiveView=%d\n"), pStreamHead->nNumConnected, pStreamHead->nNumLiveView);
		switch(pStreamHead->nCodecID){
			case AV_CODECID_VIDEO_H264:{
				if(m_pBufBmp24==NULL) m_pBufBmp24=new BYTE[MAX_SIZE_VIDEO_BMP24];
				if(m_pObjPicture==NULL)m_pObjPicture=new CPicture();
				in_outLen=MAX_SIZE_VIDEO_BMP24;
				nRet=SEVideo_Decode2YUV(m_pHandleH264,(UCHAR *)pVideoData,pStreamHead->nStreamDataLen, (UCHAR *)outYUV420, &in_outLen, &nWidth,&nHeigh);
				if(nRet>0){
					/*
					nWxH=nWidth*nHeigh;
					UCHAR *pU=outYUV420+nWxH, *pV=outYUV420+nWxH*5/4;
					memcpy(m_pBufBmp24, pU, nWxH>>2);
					memcpy(pU,pV,nWxH>>2);
					memcpy(pV,m_pBufBmp24, nWxH>>2);
					*/

					nYUVSize=in_outLen;
					in_outLen=MAX_SIZE_VIDEO_BMP24;
					SEVideo_YUV420toRGB24(outYUV420, nYUVSize, m_pBufBmp24,&in_outLen, nWidth, nHeigh);
					m_bmiHead.biWidth	=nWidth;
					m_bmiHead.biHeight	=-nHeigh;
					m_bmiHead.biSizeImage=in_outLen;
					m_pObjPicture->PushData(m_pBufBmp24, &m_bmiHead);
					m_pObjPicture->Show(m_hDC, m_pObjRect);
				}
				}break;

			case AV_CODECID_VIDEO_MJPEG:{
				if(m_pObjPicture==NULL)m_pObjPicture=new CPicture();
				m_pObjPicture->PushData((BYTE *)pVideoData, pStreamHead->nStreamDataLen);
				m_pObjPicture->Show(m_hDC, m_pObjRect);
				}break;

			default:
				Sleep(10);
		}
		if(pData){
			delete []pData;
			pData=NULL;
		}
	}

	if(outYUV420){
		free(outYUV420);
		outYUV420=NULL;
	}
}

//static 
ULONG WINAPI CCamObj::ThreadPlayAudio(void* lpPara)
{
	CCamObj* pTHIS=(CCamObj *)lpPara;
	pTHIS->DoPlayAudio();
	TRACE(_T("---ThreadPlayAudio exited----\n"));
	return 0L;
}

#define MAX_SIZE_PCM_BUFFER	5120 //5K

FILE *gp_file=NULL;
INT32 g_nCount=0;
void CCamObj::DoPlayAudio()
{
	CHAR *pData=NULL, *pAudio=NULL;
	STREAM_HEAD* pStreamHead=NULL;
	CHAR *pPCMData=(CHAR *)malloc(MAX_SIZE_PCM_BUFFER);
	UINT32 nPCMDataSize=0;

	if(m_chProductSeries[0]=='X') {
		INT32 nAudioParam=m_stAVParam.nAudioParameter;
		INT32 nSampleRate=nAudioParam>>2;
		switch(nSampleRate){
			case AUDIO_SAMPLE_8K:
				Set_WIN_Params(INVALID_FILEDESC, AUDIO_SAMPLE_RATE_8K, 16, 1);
				break;
			default:
				Set_WIN_Params(INVALID_FILEDESC, AUDIO_SAMPLE_RATE_16K, 16, 1);
		}
	}else Set_WIN_Params(INVALID_FILEDESC, AUDIO_SAMPLE_RATE_8K, 16, 1);

	m_bPlayingAudio=1;
	TRACE(_T("---ThreadPlayAudio goning......\n"));
	while(m_bPlayingAudio){
		pData=AudioDataGetAndRemove();
		if(pData==NULL){
			Sleep(10);
			continue;
		}
		pStreamHead=(STREAM_HEAD *)pData;
		pAudio=pData+sizeof(STREAM_HEAD);
		switch(pStreamHead->nCodecID){
			case AV_CODECID_AUDIO_ADPCM:{
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_ADPCM, &m_pHandleAudioCodec);
				SEAudio_Decode(m_pHandleAudioCodec, (UCHAR *)pAudio, pStreamHead->nStreamDataLen, (UCHAR *)pPCMData, &nPCMDataSize);
				//TRACE(_T("DoPlayAudio] adpcm nPCMDataSize=%d\n"), nPCMDataSize);
				if(nPCMDataSize>0) WIN_Play_Samples(pPCMData, nPCMDataSize);
				}break;

			case AV_CODECID_AUDIO_G726:{
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_G726, &m_pHandleAudioCodec);
				SEAudio_Decode(m_pHandleAudioCodec, (UCHAR *)pAudio, pStreamHead->nStreamDataLen, (UCHAR *)pPCMData, &nPCMDataSize);
				//TRACE(_T("DoPlayAudio] G726 nPCMDataSize=%d\n"), nPCMDataSize);
				if(nPCMDataSize>0) WIN_Play_Samples(pPCMData, nPCMDataSize);
				}break;

			case AV_CODECID_AUDIO_G711A:{
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_G711A, &m_pHandleAudioCodec);
				SEAudio_Decode(m_pHandleAudioCodec, (UCHAR *)pAudio, pStreamHead->nStreamDataLen, (UCHAR *)pPCMData, &nPCMDataSize);
				//TRACE(_T("DoPlayAudio] G711 nPCMDataSize=%d\n"), nPCMDataSize);
				if(nPCMDataSize>0) WIN_Play_Samples(pPCMData, nPCMDataSize);
				}break;

			case  AV_CODECID_AUDIO_AAC:{
/*
				if(gp_file==NULL && g_nCount==0){
					gp_file=fopen("d:\\x_audio.aac","wb");
				}
				if(gp_file){
					fwrite(pAudio, 1, pStreamHead->nStreamDataLen, gp_file);
					fflush(gp_file);
					g_nCount++;
				}
				if(g_nCount==100){
					fclose(gp_file);
					gp_file=NULL;
					g_nCount++;
				}
*/
				if(m_pHandleAudioCodec==NULL) SEAudio_Create(AUDIO_CODE_TYPE_AAC, &m_pHandleAudioCodec);
				TRACE(_T("DoPlayAudio] AAC 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X \n"), 
					pAudio[0]&0xFF, pAudio[1]&0xFF, pAudio[2]&0xFF, pAudio[3]&0xFF, pAudio[4]&0xFF, pAudio[5]&0xFF, pAudio[6]&0xFF, pAudio[7]&0xFF);
					nPCMDataSize=MAX_SIZE_PCM_BUFFER;
				INT32 nRet=SEAudio_Decode(m_pHandleAudioCodec, (UCHAR *)pAudio, pStreamHead->nStreamDataLen, (UCHAR *)pPCMData, &nPCMDataSize);
				TRACE(_T("DoPlayAudio] AAC ret=%d nPCMDataSize=%d\n"), nRet, nPCMDataSize);

				if(nPCMDataSize>0 && nRet>0) WIN_Play_Samples(pPCMData, nPCMDataSize);
				}break;

			default:
				Sleep(10);
		}

		if(pData){
			delete []pData;
			pData=NULL;
		}
	}//while-end

	if(pPCMData){
		free(pPCMData);
		pPCMData=NULL;
	}
}
