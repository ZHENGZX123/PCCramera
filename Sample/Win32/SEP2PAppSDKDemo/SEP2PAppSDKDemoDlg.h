// SEP2PAppSDKDemoDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "AudioSample.h"
#include "SEP2P_API.h"


#define MAX_NUM_SEARCH_ON_LAN	100
#define MAX_NUM_CHANNEL			4

class CPicture;
class CCamObj;
typedef struct tag_DeviceSearched
{
	char chDID[32];
	char chIPAddr[16];
	char product_type[2];
	char bUsed;
	char reserve[5];
}DEVICE_SEARCHED;

typedef struct tag_ChannelInfo
{
	UCHAR nChannelNo;
	CHAR  bConnecting;
	CHAR  bStartVideo;
	CHAR  bStartAudio;
	CHAR  bStartTalk;
	CHAR  reserve[3];

	CHAR  chDID[32];
	TCHAR tchUsername[16];
	TCHAR tchPasswd[16];

	AV_PARAMETER stAVParam;
	CWnd  *pCtrlEdit;
	CWnd  *pCtrlConnect;
	CWnd  *pCtrlStartVideo;
	CWnd  *pCtrlStartAudio;
	CWnd  *pCtrlStartTalk;
	CWnd  *pCtrlStatus;

	HDC	  hDC;
	CPicture  *pObjPicture;
	CRect *pObjRect;
	BITMAPINFOHEADER bmiHead;
	BYTE  *pBufBmp24;

}CHANNEL_INFO;

typedef struct tag_MsgInfo
{
	UINT32 nDeviceNo;
	UINT32 nMsgType;
	UINT32 nMsgSize;
	CHAR*  pMsg;
}MSG_INFO;



// CSEP2PAppSDKDemoDlg dialog
class CSEP2PAppSDKDemoDlg : public CDialog
{
// Construction
public:
	CSEP2PAppSDKDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SEP2P_API_SAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	static CString ms_csTitleStart[MAX_NUM_CHANNEL];
	static CString ms_csTitleStop[MAX_NUM_CHANNEL];

	char m_bStartLanSearch;
	DEVICE_SEARCHED m_arrDevSearched[MAX_NUM_SEARCH_ON_LAN];

	CCamObj *m_pObjCams[MAX_NUM_CHANNEL];
	
	CHAR  m_chFirstFilePath[120];

	CAudioSample m_objSample;
	UCHAR *m_pHandleG726, *m_pHandleAdpcm;
	CString m_csAppPath;
	UCHAR *m_pHandleH264;
	volatile CHAR  m_bExitingApp;
	INT32  m_nCruiseCount;

	static INT32 OnSampleCallback(int index, CHAR*	pData, UINT32  nDataSize, void*	pUserData);
	//void DoSampleCallback(int index, CHAR*	pData, UINT32  nDataSize);

	void ReadDevListFromTxt();
	void WriteDevListToTxt();
	void SetBtnTitle(int nChanNo, char bStart);
	INT32 set15MinutesFlagAlarmTime(MSG_SET_ALARM_INFO_REQ *pAlarm,INT32 nWeek ,INT32 nBitIndex, INT32 bSel);
	INT32 get15MinutesFlagAlarmTime(MSG_SET_ALARM_INFO_REQ *pAlarm, INT32 nWeek, INT32 nBitIndex);
	void TRACE_Msg(MSG_INFO *pMsgInfo);	

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg LRESULT OnUpdateDeviceSearched(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateUI(WPARAM wParam, LPARAM lParam);
	void OnButtonsRange(UINT nID);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

public:
	CEdit m_edtLog;
	CComboBox m_ctlComboDev;
	CComboBox m_ctlComboReqStr;
	CComboBox m_ctlComboChn;

	void DoSearch(CHAR*	pData, UINT32  nDataSize);
	void DoRecvMsg(UCHAR nChanNo, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize);
	BOOL IsExistThisDID(const CHAR *pDID);
	
	void WriteLog(CHAR *format, ...);
	
	afx_msg void OnBnClickedLansearch();
	afx_msg void OnBnClickedClearLog();
	afx_msg void OnBnClickedGetReq();
	afx_msg void OnBnClickedSetReq();
};
