#pragma once

#include "SEP2P_API.h"


#define MAX_LEN_DID				17
#define MAX_SIZE_TMP_TALK		1024

#define AUDIO_SAMPLE_RATE_8K	8000
#define AUDIO_SAMPLE_RATE_16K	16000

#define  ERR_P2PAPI_BASE					-400
#define  ERR_P2PAPI_ALREADY_OPEN_AUDIO		(ERR_P2PAPI_BASE-1)
#define  ERR_P2PAPI_UNKNOWN_CODEC_WHEN_TALK	(ERR_P2PAPI_BASE-2)

#define MUTEX_Init(theMutex)	InitializeCriticalSection(&theMutex)
#define MUTEX_Lock(theMutex)	EnterCriticalSection(&theMutex)
#define MUTEX_Unlock(theMutex)	LeaveCriticalSection(&theMutex)
#define MUTEX_Destroy(theMutex)	DeleteCriticalSection(&theMutex)


class CSEP2PAppSDKDemoDlg;
class CPicture;
class CCamObj
{
public:
	CCamObj(UCHAR nChanNo, CSEP2PAppSDKDemoDlg *pMainWnd);
	virtual ~CCamObj(void);

	CHAR  m_bConnecting;
	CHAR  m_bStartVideo;
	CHAR  m_bStartAudio;
	CHAR  m_bStartTalk;

private:
	CSEP2PAppSDKDemoDlg *m_pMainWnd;
	UCHAR m_nChannelNo;
	CHAR  m_chDID[32];
	TCHAR m_tchUsername[16];
	TCHAR m_tchPasswd[16];
	CHAR  m_chProductSeries[4];

	char  m_bConnectedDev;
	AV_PARAMETER m_stAVParam;
	CWnd  *m_pCtrlEdit;
	CWnd  *m_pCtrlConnect;
	CWnd  *m_pCtrlStartVideo;
	CWnd  *m_pCtrlStartAudio;
	CWnd  *m_pCtrlStartTalk;
	CWnd  *m_pCtrlStatus;

	HDC	  m_hDC;
	CPicture  *m_pObjPicture;
	CRect *m_pObjRect;
	BITMAPINFOHEADER m_bmiHead;
	BYTE  *m_pBufBmp24;

	UINT32 m_nTalkAudioSizeTmp;
	CHAR  *m_pTalkAudioDataTmp;
	UCHAR *m_pHandleAudioCodec;
	UCHAR *m_pHandleH264;

	CPtrList m_dequeVideo;
	CPtrList m_dequeAudio;

	CRITICAL_SECTION m_mutexVideo;
	CRITICAL_SECTION m_mutexAudio;

	static char m_bOnlyAudio;
	volatile char m_bPlayingVideo, m_bPlayingAudio;
	HANDLE m_hThreadPlayVideo, m_hThreadPlayAudio;
	ULONG  m_nFirstTickLocal_video, m_nTick2_video, m_nFirstTimestampDevice_video;

private:
	static ULONG WINAPI ThreadPlayVideo(void* lpPara);
	void DoPlayVideo();
	static ULONG WINAPI ThreadPlayAudio(void* lpPara);
	void DoPlayAudio();

public:
	void  SetDID(CHAR *pDID)			{ if(pDID){ strcpy(m_chDID, pDID); }}
	CHAR* GetDID()						{ return m_chDID;					}
	void  SetUsername(TCHAR* tchUser)	{ wcscpy(m_tchUsername, tchUser);	}
	TCHAR* GetUsername()				{ return m_tchUsername;				}
	void  SetPasswd(TCHAR* tchPasswd)	{ wcscpy(m_tchPasswd, tchPasswd);	}
	TCHAR* GetPasswd()					{ return m_tchPasswd;				}

	void SetCtrlEdit(CWnd *pCtrl)		{ m_pCtrlEdit=pCtrl;				}
	CWnd* GetCtrlEdit()					{ return m_pCtrlEdit;				}
	void SetCtrlConnect(CWnd *pCtrl)	{ m_pCtrlConnect=pCtrl;				}
	CWnd* GetCtrlConnect()				{ return m_pCtrlConnect;			}
	void SetCtrlStartVideo(CWnd *pCtrl) { m_pCtrlStartVideo=pCtrl;			}
	CWnd* GetCtrlStartVideo()			{ return m_pCtrlStartVideo;			}
	void SetCtrlStartAudio(CWnd *pCtrl) { m_pCtrlStartAudio=pCtrl;			}
	CWnd* GetCtrlStartAudio()			{ return m_pCtrlStartAudio;			}
	void SetCtrlStartTalk(CWnd *pCtrl)	{ m_pCtrlStartTalk=pCtrl;			}
	CWnd* GetCtrlStartTalk()			{ return m_pCtrlStartTalk;			}
	void SetCtrlStatus(CWnd *pCtrl)		{ m_pCtrlStatus=pCtrl;				}
	CWnd* GetCtrlStatus()				{ return m_pCtrlStatus;				}

	void SetHDC(HDC hDC)				{ m_hDC=hDC;						}
	HDC  GetHDC()						{ return m_hDC;						}
	void SetRect(LPCRECT pRect)			{ m_pObjRect->CopyRect(pRect);		}
	CRect* GetRect()					{ return m_pObjRect;				}

	AV_PARAMETER* GetAVParameter()		{ return &m_stAVParam;				}

public:
	static INT32 OnSearchCallback(CHAR*	pData, UINT32  nDataSize, VOID*	pUserData);
	void DoSearchCallback(CHAR*	pData, UINT32  nDataSize);

	static INT32  OnStreamCallback(CHAR* pDID, CHAR*	pData, UINT32  nDataSize, VOID*	pUserData);
	void DoStreamCallback(CHAR* pDID, CHAR*	pData, UINT32  nDataSize);

	static INT32  OnRecvMsgCallback(CHAR* pDID, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize, VOID* pUserData);
	void DoRecvMsgCallback(CHAR* pDID, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize);

	static INT32  OnEventCallback(CHAR* pDID, UINT32 nEventType, CHAR* pEventData, UINT32 nEventDataSize, VOID* pUserData);
	void DoEventCallback(CHAR* pDID, UINT32 nEventType, CHAR* pEventData, UINT32 nEventDataSize);

	INT32 VideoDataAdd(CHAR *pData, INT32 nDataSize);
	CHAR* VideoDataGetAndRemove();
	void  VideoDataClear();

	INT32 AudioDataAdd(CHAR *pData, INT32 nDataSize);
	CHAR* AudioDataGetAndRemove();
	void  AudioDataClear();

	void  ResetAudioVar();
	void  ResetVideoVar();

	INT32 StartPlayVideo();
	void  StopPlayVideo();
	INT32 StartPlayAudio();
	void  StopPlayAudio();

	BOOL  IsConnected() { return m_bConnectedDev; }
	static UINT32 P2PAPI_Version();
	static void P2PAPI_Init();
	static void P2PAPI_DeInit();

	INT32 P2PAPI_StartSearch();
	void  P2PAPI_StopSearch();
	INT32 P2PAPI_Connect();
	void  P2PAPI_Disconnect();

	INT32 P2PAPI_GetAVParameter(){
		if(m_chDID[0]=='\0') return ERR_P2PAPI_BASE;
		INT32 nRet=SEP2P_GetAVParameterSupported(m_chDID, &m_stAVParam);
		TRACE(_T("P2PAPI_GetAVParameter=%d, nAudioCodecID=0x%X nVideoParameter=%d,%d,%d,%d  nAudioParameter=%d\n"), nRet, m_stAVParam.nAudioCodecID, 
			  m_stAVParam.nVideoParameter[0],m_stAVParam.nVideoParameter[1],m_stAVParam.nVideoParameter[2],m_stAVParam.nVideoParameter[3],
			  m_stAVParam.nAudioParameter);
		return nRet;
	}
	INT32 P2PAPI_SendMsg(INT32 nMsgType, const CHAR* pMsgData, INT32 nMsgDataSize);
	INT32 P2PAPI_TalkData(CHAR* pData, UINT32  nDataSize);
};
