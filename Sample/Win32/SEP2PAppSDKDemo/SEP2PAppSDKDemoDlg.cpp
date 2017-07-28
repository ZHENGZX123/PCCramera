// SEP2PAppSDKDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SEP2PAppSDKDemo.h"
#include "SEP2PAppSDKDemoDlg.h"
#include "DeviceInfo.h"
#include "AudioSample.h"
#include "SE_AudioCodec.h"
#include "SE_VideoCodec.h"
#include "Picture.h"
#include "CamObj.h"
//#include "../../../SEP2P_Define_Ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define OM_UPDATE_DEVICE	WM_USER+10
#define OM_UPDATE_UI		WM_USER+11

#define UPDATE_UI_WPARAM_StatusBar	1
#define UPDATE_UI_WPARAM_Connect	2
#define UPDATE_UI_WPARAM_Talk		3
#define UPDATE_UI_WPARAM_MsgArrived	4


CString CSEP2PAppSDKDemoDlg::ms_csTitleStart[MAX_NUM_CHANNEL]={_T("Connect"), _T("StartVideo"), _T("StartAudio"), _T("StartTalk")};
CString CSEP2PAppSDKDemoDlg::ms_csTitleStop[MAX_NUM_CHANNEL] ={_T("Disconnect"), _T("StopVideo"), _T("StopAudio"), _T("StopTalk")};


// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



//----{{User defined msg----------------------
#define SEP2P_MSG_EXT_CMD1		0x1000
#define SEP2P_MSG_EXT_CMD2		0x1001

typedef struct tag_SEP2P_MSG_EXT_SDFILE_REQ{
	CHAR  nAction;	//0=list; 1=del; 
	CHAR  reserve[7];
	//CHAR chFilename[64];
}SEP2P_MSG_EXT_SDFILE_REQ;

typedef struct tag_SEP2P_MSG_EXT_SDFILE_RESP{
	CHAR  nAction;	//0=list; 1=del; 
	CHAR  nResult;	//0=success; 1=fail
	CHAR  reserve[2];
	INT32 nFileNum;
	//CHAR chFilename[64];
}SEP2P_MSG_EXT_SDFILE_RESP;
//----}}User defined msg----------------------

// CSEP2PAppSDKDemoDlg dialog
CSEP2PAppSDKDemoDlg::CSEP2PAppSDKDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSEP2PAppSDKDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bStartLanSearch=1;
	memset(m_arrDevSearched, 0, sizeof(m_arrDevSearched));

	m_pHandleG726 =NULL;
	m_pHandleAdpcm=NULL;
	m_pHandleH264=NULL;
	m_bExitingApp=0;
	for(int i=0; i<MAX_NUM_CHANNEL; i++) m_pObjCams[i]=new CCamObj(i, this);
	m_chFirstFilePath[0]='\0';
	m_nCruiseCount=0;
}

void CSEP2PAppSDKDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_edtLog);
	DDX_Control(pDX, IDC_COMBO1, m_ctlComboDev);
	DDX_Control(pDX, IDC_COMBO2, m_ctlComboReqStr);
	DDX_Control(pDX, IDC_COMBO3, m_ctlComboChn);
}

BEGIN_MESSAGE_MAP(CSEP2PAppSDKDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_WM_DESTROY()

	ON_MESSAGE(OM_UPDATE_DEVICE,	&CSEP2PAppSDKDemoDlg::OnUpdateDeviceSearched)
	ON_MESSAGE(OM_UPDATE_UI,		&CSEP2PAppSDKDemoDlg::OnUpdateUI)

	ON_BN_CLICKED(IDC_LANSEARCH,	&CSEP2PAppSDKDemoDlg::OnBnClickedLansearch)
	ON_BN_CLICKED(IDC_CLEAR_LOG,	&CSEP2PAppSDKDemoDlg::OnBnClickedClearLog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONNECT1, IDC_EDIT_DEVICE4, OnButtonsRange)

	ON_BN_CLICKED(IDC_GET_REQ, &CSEP2PAppSDKDemoDlg::OnBnClickedGetReq)
	ON_BN_CLICKED(IDC_SET_REQ, &CSEP2PAppSDKDemoDlg::OnBnClickedSetReq)
END_MESSAGE_MAP()


// CSEP2PAppSDKDemoDlg message handlers
BOOL CSEP2PAppSDKDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString csText;
	UINT32 nVer=SEP2P_GetSDKVersion(NULL, 0);
	CHAR *pVer=(CHAR *)&nVer;
	csText.Format(_T("AppSDK v%d.%d.%d.%d"), *(pVer+3),*(pVer+2),*(pVer+1),*(pVer+0));
	SetDlgItemText(IDC_APIVER, csText);

	CWnd *pVideo=NULL;
	pVideo=GetDlgItem(IDC_VIDEO0);
	if(pVideo) {
		m_pObjCams[0]->SetHDC(pVideo->GetDC()->GetSafeHdc());
		pVideo->GetClientRect(m_pObjCams[0]->GetRect());
	}
	m_pObjCams[0]->SetCtrlEdit(GetDlgItem(IDC_EDIT_DEVICE1));
	m_pObjCams[0]->SetCtrlConnect(GetDlgItem(IDC_CONNECT1));
	m_pObjCams[0]->SetCtrlStartVideo(GetDlgItem(IDC_START_VIDEO1));
	m_pObjCams[0]->SetCtrlStartAudio(GetDlgItem(IDC_START_AUDIO1));
	m_pObjCams[0]->SetCtrlStartTalk(GetDlgItem(IDC_START_TALK1));
	m_pObjCams[0]->SetCtrlStatus(GetDlgItem(IDC_STATUS1));
	m_pObjCams[0]->GetCtrlStartVideo()->EnableWindow(FALSE);
	m_pObjCams[0]->GetCtrlStartAudio()->EnableWindow(FALSE);
	m_pObjCams[0]->GetCtrlStartTalk()->EnableWindow(FALSE);
	m_ctlComboChn.EnableWindow(FALSE);
	m_ctlComboChn.SetCurSel(0);

	pVideo=GetDlgItem(IDC_VIDEO1);
	if(pVideo) {
		m_pObjCams[1]->SetHDC(pVideo->GetDC()->GetSafeHdc());
		pVideo->GetClientRect(m_pObjCams[1]->GetRect());
	}
	m_pObjCams[1]->SetCtrlEdit(GetDlgItem(IDC_EDIT_DEVICE2));
	m_pObjCams[1]->SetCtrlConnect(GetDlgItem(IDC_CONNECT2));
	m_pObjCams[1]->SetCtrlStartVideo(GetDlgItem(IDC_START_VIDEO2));
	m_pObjCams[1]->SetCtrlStartAudio(GetDlgItem(IDC_START_AUDIO2));
	m_pObjCams[1]->SetCtrlStartTalk(GetDlgItem(IDC_START_TALK2));
	m_pObjCams[1]->SetCtrlStatus(GetDlgItem(IDC_STATUS2));
	m_pObjCams[1]->GetCtrlStartVideo()->EnableWindow(FALSE);
	m_pObjCams[1]->GetCtrlStartAudio()->EnableWindow(FALSE);
	m_pObjCams[1]->GetCtrlStartTalk()->EnableWindow(FALSE);

	pVideo=GetDlgItem(IDC_VIDEO2);
	if(pVideo) {
		m_pObjCams[2]->SetHDC(pVideo->GetDC()->GetSafeHdc());
		pVideo->GetClientRect(m_pObjCams[2]->GetRect());
	}
	m_pObjCams[2]->SetCtrlEdit(GetDlgItem(IDC_EDIT_DEVICE3));
	m_pObjCams[2]->SetCtrlConnect(GetDlgItem(IDC_CONNECT3));
	m_pObjCams[2]->SetCtrlStartVideo(GetDlgItem(IDC_START_VIDEO3));
	m_pObjCams[2]->SetCtrlStartAudio(GetDlgItem(IDC_START_AUDIO3));
	m_pObjCams[2]->SetCtrlStartTalk(GetDlgItem(IDC_START_TALK3));
	m_pObjCams[2]->SetCtrlStatus(GetDlgItem(IDC_STATUS3));
	m_pObjCams[2]->GetCtrlStartVideo()->EnableWindow(FALSE);
	m_pObjCams[2]->GetCtrlStartAudio()->EnableWindow(FALSE);
	m_pObjCams[2]->GetCtrlStartTalk()->EnableWindow(FALSE);

	pVideo=GetDlgItem(IDC_VIDEO3);
	if(pVideo) {
		m_pObjCams[3]->SetHDC(pVideo->GetDC()->GetSafeHdc());
		pVideo->GetClientRect(m_pObjCams[3]->GetRect());
	}
	m_pObjCams[3]->SetCtrlEdit(GetDlgItem(IDC_EDIT_DEVICE4));
	m_pObjCams[3]->SetCtrlConnect(GetDlgItem(IDC_CONNECT4));
	m_pObjCams[3]->SetCtrlStartVideo(GetDlgItem(IDC_START_VIDEO4));
	m_pObjCams[3]->SetCtrlStartAudio(GetDlgItem(IDC_START_AUDIO4));
	m_pObjCams[3]->SetCtrlStartTalk(GetDlgItem(IDC_START_TALK4));
	m_pObjCams[3]->SetCtrlStatus(GetDlgItem(IDC_STATUS4));
	m_pObjCams[3]->GetCtrlStartVideo()->EnableWindow(FALSE);
	m_pObjCams[3]->GetCtrlStartAudio()->EnableWindow(FALSE);
	m_pObjCams[3]->GetCtrlStartTalk()->EnableWindow(FALSE);

	SetBtnTitle(0,1);
	SetBtnTitle(1,1);
	SetBtnTitle(2,1);
	SetBtnTitle(3,1);

	CSEP2PAppSDKDemoApp::GetAppPath(m_csAppPath);
	ReadDevListFromTxt();
	m_ctlComboDev.SetCurSel(0);
	m_ctlComboReqStr.SetCurSel(0);
	
	CCamObj::P2PAPI_Version();
	CCamObj::P2PAPI_Init();

	int n=sizeof(8);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSEP2PAppSDKDemoDlg::OnDestroy()
{
	m_bExitingApp=1;
	CDialog::OnDestroy();
	WriteDevListToTxt();

	int i=0, nMsgDataSize=0;
	CHAR *pMsgData=NULL;
	for(i=0;i<MAX_NUM_CHANNEL; i++){
		if(i==0){
			MSG_STOP_AUDIO stStopAudio;
			memset(&stStopAudio, 0, sizeof(stStopAudio));
			stStopAudio.nChannel=m_ctlComboChn.GetCurSel();

			MSG_STOP_VIDEO stStopVideo;
			memset(&stStopVideo, 0, sizeof(stStopVideo));
			stStopVideo.nChannel=stStopAudio.nChannel;

			MSG_STOP_TALK stStopTalk;
			memset(&stStopTalk, 0, sizeof(stStopTalk));
			stStopTalk.nChannel=stStopAudio.nChannel;

			m_pObjCams[i]->P2PAPI_SendMsg(SEP2P_MSG_STOP_AUDIO, (CHAR *)&stStopAudio, sizeof(stStopAudio));
			m_pObjCams[i]->P2PAPI_SendMsg(SEP2P_MSG_STOP_VIDEO, (CHAR *)&stStopVideo, sizeof(stStopVideo));
			m_pObjCams[i]->P2PAPI_SendMsg(SEP2P_MSG_STOP_TALK,  (CHAR *)&stStopTalk, sizeof(stStopTalk));
		}else{
			m_pObjCams[i]->P2PAPI_SendMsg(SEP2P_MSG_STOP_AUDIO, NULL, 0);
			m_pObjCams[i]->P2PAPI_SendMsg(SEP2P_MSG_STOP_VIDEO, NULL, 0);
			m_pObjCams[i]->P2PAPI_SendMsg(SEP2P_MSG_STOP_TALK, NULL, 0);
		}
		m_pObjCams[i]->P2PAPI_Disconnect();
	}
	CCamObj::P2PAPI_DeInit();

	for(i=0; i<MAX_NUM_CHANNEL; i++)
	{
		if(m_pObjCams[i]){
			delete m_pObjCams[i];
			m_pObjCams[i]=NULL;
		}
	}
}

void CSEP2PAppSDKDemoDlg::ReadDevListFromTxt()
{
	CStdioFile objFile;
	CString csFileName, csLine;
	csFileName.Format(_T("%sSEP2P_API_DEV.txt"), m_csAppPath);
	BOOL bRet=objFile.Open(csFileName, CFile::modeRead);
	if(bRet){
		int iRow=0;
		char chDID[128]={0};
		while(objFile.ReadString(csLine)){
			switch(iRow){
				case 0:
					CharFromWSTRU(CP_OEMCP, csLine, csLine.GetLength(), chDID);
					strcpy(m_pObjCams[0]->GetDID(), chDID);
					break;
				case 1:
					wcscpy(m_pObjCams[0]->GetUsername(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;
				case 2:
					wcscpy(m_pObjCams[0]->GetPasswd(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;


				case 3:
					CharFromWSTRU(CP_OEMCP, csLine, csLine.GetLength(), chDID);
					strcpy(m_pObjCams[1]->GetDID(), chDID);
					break;
				case 4:
					wcscpy(m_pObjCams[1]->GetUsername(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;
				case 5:
					wcscpy(m_pObjCams[1]->GetPasswd(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;


				case 6:
					CharFromWSTRU(CP_OEMCP, csLine, csLine.GetLength(), chDID);
					strcpy(m_pObjCams[2]->GetDID(), chDID);
					break;
				case 7:
					wcscpy(m_pObjCams[2]->GetUsername(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;
				case 8:
					wcscpy(m_pObjCams[2]->GetPasswd(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;

				case 9:
					CharFromWSTRU(CP_OEMCP, csLine, csLine.GetLength(), chDID);
					strcpy(m_pObjCams[3]->GetDID(), chDID);
					break;
				case 10:
					wcscpy(m_pObjCams[3]->GetUsername(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;
				case 11:
					wcscpy(m_pObjCams[3]->GetPasswd(), csLine.GetBuffer());
					csLine.ReleaseBuffer();
					break;
				default:;
			}
			iRow++;
			if(iRow>=12) break;
		}
		objFile.Close();
	}
}

void CSEP2PAppSDKDemoDlg::WriteDevListToTxt()
{
	CStdioFile objFile;
	CString csFileName, csLine, csCrLr=_T("\n");
	csFileName.Format(_T("%sSEP2P_API_DEV.txt"), m_csAppPath);
	BOOL bRet=objFile.Open(csFileName, CFile::modeReadWrite | CFile::modeCreate);
	if(bRet){
		CString csDID;
		BSTRFromCharU(CP_OEMCP, m_pObjCams[0]->GetDID(), csDID);
		objFile.WriteString(csDID);
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[0]->GetUsername()));
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[0]->GetPasswd()));
		objFile.WriteString(csLine);
		objFile.WriteString(csCrLr);

		BSTRFromCharU(CP_OEMCP, m_pObjCams[1]->GetDID(), csDID);
		objFile.WriteString(csDID);
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[1]->GetUsername()));
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[1]->GetPasswd()));
		objFile.WriteString(csLine);
		objFile.WriteString(csCrLr);

		BSTRFromCharU(CP_OEMCP, m_pObjCams[2]->GetDID(), csDID);
		objFile.WriteString(csDID);
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[2]->GetUsername()));
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[2]->GetPasswd()));
		objFile.WriteString(csLine);
		objFile.WriteString(csCrLr);

		BSTRFromCharU(CP_OEMCP, m_pObjCams[3]->GetDID(), csDID);
		objFile.WriteString(csDID);
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[3]->GetUsername()));
		objFile.WriteString(csCrLr);
		objFile.WriteString(CString(m_pObjCams[3]->GetPasswd()));
		objFile.WriteString(csLine);
		objFile.WriteString(csCrLr);
		objFile.Close();
	}
}

void CSEP2PAppSDKDemoDlg::SetBtnTitle(int nDeviceNo, char bStart)
{
	if(nDeviceNo<0 || nDeviceNo>=MAX_NUM_CHANNEL) return;
	if(bStart){
		m_pObjCams[nDeviceNo]->GetCtrlConnect()->SetWindowText(ms_csTitleStart[0]);
		m_pObjCams[nDeviceNo]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStart[1]);
		m_pObjCams[nDeviceNo]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStart[2]);
		m_pObjCams[nDeviceNo]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStart[3]);
	}else{
		m_pObjCams[nDeviceNo]->GetCtrlConnect()->SetWindowText(ms_csTitleStop[0]);
		m_pObjCams[nDeviceNo]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStop[1]);
		m_pObjCams[nDeviceNo]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStop[2]);
		m_pObjCams[nDeviceNo]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStop[3]);
	}
}

void CSEP2PAppSDKDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID & 0xFFF0) == IDM_ABOUTBOX){
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}else{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSEP2PAppSDKDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}else{
		CDialog::OnPaint();
	}
}

LRESULT CSEP2PAppSDKDemoDlg::OnUpdateDeviceSearched(WPARAM wParam, LPARAM lParam)
{
	CHAR chText[255]={0};
	CString csOld, csText, csTmp;
	GetDlgItemText(IDC_EDIT3, csOld);
	sprintf(chText, "%C \t%s\t%s\r\n",m_arrDevSearched[wParam].product_type[0]==0?'L':'M', m_arrDevSearched[wParam].chDID, m_arrDevSearched[wParam].chIPAddr);
	BSTRFromCharU(CP_OEMCP, chText, csTmp);
	csText.Format(_T("%s%s"), csOld, csTmp);

	SetDlgItemText(IDC_EDIT3, csText);
	m_edtLog.LineScroll(m_edtLog.GetLineCount(), 0);

	return 0L;
}

LRESULT CSEP2PAppSDKDemoDlg::OnUpdateUI(WPARAM wParam, LPARAM lParam)
{
	switch(wParam){
		case UPDATE_UI_WPARAM_StatusBar:
			break;

		case UPDATE_UI_WPARAM_Connect:{
			int nDeviceNo=HIWORD(lParam);
			int nConnectStatus=LOWORD(lParam);
			if(nDeviceNo<0 || nDeviceNo>=MAX_NUM_CHANNEL) break;
			
			CString csText;
			CHAR chText[128]={0};
			if(nConnectStatus==CONNECT_STATUS_CONNECTED){
				sprintf(chText, "%s Online", m_pObjCams[nDeviceNo]->GetDID());
				BSTRFromCharU(CP_OEMCP, chText, csText);
				m_pObjCams[nDeviceNo]->GetCtrlStatus()->SetWindowText(csText);
				m_pObjCams[nDeviceNo]->GetCtrlStartVideo()->EnableWindow(TRUE);
				m_pObjCams[nDeviceNo]->GetCtrlStartAudio()->EnableWindow(TRUE);
				m_pObjCams[nDeviceNo]->GetCtrlStartTalk()->EnableWindow(TRUE);
				if(nDeviceNo==0) m_ctlComboChn.EnableWindow(TRUE);

				//m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_DEVICE_VERSION_REQ, NULL, 0);
			}else {
				if(nConnectStatus==CONNECT_STATUS_CONNECTING) sprintf(chText, "%s Connecting...", m_pObjCams[nDeviceNo]->GetDID());
				else sprintf(chText, "%s Connect failed(%d)", m_pObjCams[nDeviceNo]->GetDID(), nConnectStatus);
				BSTRFromCharU(CP_OEMCP, chText, csText);
				m_pObjCams[nDeviceNo]->GetCtrlStatus()->SetWindowText(csText);

				m_pObjCams[nDeviceNo]->GetCtrlStartVideo()->EnableWindow(FALSE);
				m_pObjCams[nDeviceNo]->GetCtrlStartAudio()->EnableWindow(FALSE);
				m_pObjCams[nDeviceNo]->GetCtrlStartTalk()->EnableWindow(FALSE);
				if(nDeviceNo==0) m_ctlComboChn.EnableWindow(FALSE);
			}
			}break;

		case UPDATE_UI_WPARAM_Talk:{
			int nDeviceNo=HIWORD(lParam);
			int nResult=LOWORD(lParam);
			if(nResult==0) m_pObjCams[nDeviceNo]->GetCtrlStatus()->SetWindowText(_T("Talking..."));
			else if(nResult==1) m_pObjCams[nDeviceNo]->GetCtrlStatus()->SetWindowText(_T("Other user is talking"));
			else if(nResult==2) m_pObjCams[nDeviceNo]->GetCtrlStatus()->SetWindowText(_T("Talk failed"));
			else m_pObjCams[nDeviceNo]->GetCtrlStatus()->SetWindowText(_T("Talk stopped"));
			}break;

		case UPDATE_UI_WPARAM_MsgArrived:{
			MSG_INFO *pMsgInfo=(MSG_INFO *)lParam;
			if(pMsgInfo->nMsgType==SEP2P_MSG_SNAP_PICTURE_RESP){
				CHAR chTmp[256]={0};
				sprintf(chTmp, "SEP2P_MSG_SNAP_PICTURE_RESP, pMsgInfo->nMsgSize=%d\n", pMsgInfo->nMsgSize);
				OutputDebugStringA(chTmp);

				FILE *fp=fopen("d:\\snapshot.jpg", "wb");
				if(fp && pMsgInfo->nMsgSize>0){
					fwrite(pMsgInfo->pMsg, 1, pMsgInfo->nMsgSize, fp);
					fclose(fp);	
				}
			}

			TRACE_Msg(pMsgInfo);
			}break;
		default:;
	}
	return 0L;
}

void CSEP2PAppSDKDemoDlg::TRACE_Msg(MSG_INFO *pMsgInfo)
{
	if(pMsgInfo==NULL) return;
	CHAR chTmp[1024]={0};
	switch(pMsgInfo->nMsgType)
	{
		case SEP2P_MSG_GET_CAMERA_PARAM_RESP:{
			MSG_GET_CAMERA_PARAM_RESP *pResp=(MSG_GET_CAMERA_PARAM_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_CAMERA_PARAM_RESP, nResolution=%d bOSD=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), pResp->nResolution, pResp->bOSD);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_CURRENT_WIFI_RESP:{
			MSG_GET_CURRENT_WIFI_RESP *pResp=(MSG_GET_CURRENT_WIFI_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_CURRENT_WIFI_RESP, nResolution=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), pResp->chSSID);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_WIFI_LIST_RESP:{
			MSG_GET_WIFI_LIST_RESP *pResp=(MSG_GET_WIFI_LIST_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s MSG_GET_WIFI_LIST_RESP, nResultCount=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), pResp->nResultCount);
			OutputDebugStringA(chTmp);
			for(int i=0; i<pResp->nResultCount; i++){
				sprintf(chTmp, "i=%d %s\n",i, pResp->wifi[i].chSSID);
				OutputDebugStringA(chTmp);
			}
			}break;

		case SEP2P_MSG_GET_USER_INFO_RESP:{
			MSG_GET_USER_INFO_RESP *pResp=(MSG_GET_USER_INFO_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_USER_INFO_RESP, user1=%s, curUser=%s roleid=%d, chAdmin=%s\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), 
				pResp->chVisitor, pResp->chCurUser, pResp->nCurUserRoleID, pResp->chAdmin);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_DATETIME_RESP:{
			MSG_GET_DATETIME_RESP *pResp=(MSG_GET_DATETIME_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s MSG_GET_DATETIME_RESP, nSecToNow=%d nSecTimeZone=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(),pResp->nSecToNow, pResp->nSecTimeZone);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_FTP_INFO_RESP:{
			MSG_GET_FTP_INFO_RESP *pResp=(MSG_GET_FTP_INFO_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_FTP_INFO_RESP, chFTPSvr=%s chDir=%s chUser=%s\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), 
				pResp->chFTPSvr,pResp->chDir,pResp->chUser);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_EMAIL_INFO_RESP:{
			MSG_GET_EMAIL_INFO_RESP *pResp=(MSG_GET_EMAIL_INFO_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_EMAIL_INFO_RESP, bHasTestFunction=%d chSMTPSvr=%s sender=%s subject=%s\n", 
				m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), 
				pResp->bHasTestFunction,
				pResp->chSMTPSvr,pResp->chSender,pResp->chSubject);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_ALARM_INFO_RESP:{
			MSG_GET_ALARM_INFO_RESP *pResp=(MSG_GET_ALARM_INFO_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s MSG_GET_ALARM_INFO_RESP, bHasTempHumiFunction=%d bMDEnable=%d,%d,%d,%d nMDSensitivity=%d,%d,%d,%d position=(%d,%d,%d,%d),(%d,%d,%d,%d),(%d,%d,%d,%d),(%d,%d,%d,%d)\n\n", 
				m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(),
				pResp->bHasTempHumiFunction,
				pResp->bMDEnable[0],pResp->bMDEnable[1],pResp->bMDEnable[2],pResp->bMDEnable[3],
				pResp->nMDSensitivity[0],pResp->nMDSensitivity[1],pResp->nMDSensitivity[2],pResp->nMDSensitivity[3],
				pResp->md_x[0], pResp->md_y[0],pResp->md_width[0],pResp->md_height[0],
				pResp->md_x[1], pResp->md_y[1],pResp->md_width[1],pResp->md_height[1],
				pResp->md_x[2], pResp->md_y[2],pResp->md_width[2],pResp->md_height[2],
				pResp->md_x[3], pResp->md_y[3],pResp->md_width[3],pResp->md_height[3]);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_SDCARD_REC_PARAM_RESP:{
			MSG_GET_SDCARD_REC_PARAM_RESP *pResp=(MSG_GET_SDCARD_REC_PARAM_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s MSG_GET_SDCARD_REC_PARAM_RESP, bRecordCoverInSDCard=%d nRecordTimeLen=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), 
				pResp->bRecordCoverInSDCard, pResp->nRecordTimeLen);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_DEVICE_VERSION_RESP:{
			MSG_GET_DEVICE_VERSION_RESP *pResp=(MSG_GET_DEVICE_VERSION_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_DEVICE_VERSION_RESP, product_series=%C, fwddns_app_ver=%s fwp2p_app_ver=%s fwp2p_app_buildtime=%s imn_ver=0x%X is_push_function=%d %s\n", 
				m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), pResp->product_series[0],
				pResp->chFwddns_app_ver, pResp->chFwp2p_app_ver, pResp->chFwp2p_app_buildtime, 
				pResp->imn_ver_of_device, pResp->is_push_function, pResp->imn_server_port);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_GET_IPUSH_INFO_RESP:{
			MSG_GET_IPUSH_INFO_RESP *pResp=(MSG_GET_IPUSH_INFO_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s GET_IPUSH_INFO_RESP bEnable=%d nResult=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), pResp->bEnable, pResp->nResult);
			OutputDebugStringA(chTmp);
			}break;

		case SEP2P_MSG_SET_IPUSH_INFO_RESP:{
			MSG_GET_IPUSH_INFO_RESP *pResp=(MSG_GET_IPUSH_INFO_RESP *)pMsgInfo->pMsg;
			sprintf(chTmp, "%s SET_IPUSH_INFO_RESP bEnable=%d nResult=%d\n", m_pObjCams[pMsgInfo->nDeviceNo]->GetDID(), pResp->bEnable, pResp->nResult);
			OutputDebugStringA(chTmp);
			}break;
		default:;
	}

	if(pMsgInfo){
		if(pMsgInfo->pMsg) {
			free(pMsgInfo->pMsg);
			pMsgInfo->pMsg=NULL;
		}
		free(pMsgInfo);
		pMsgInfo=NULL;
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSEP2PAppSDKDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSEP2PAppSDKDemoDlg::DoSearch(CHAR* pData, UINT32  nDataSize)
{
	SEARCH_RESP *pSearchResp=(SEARCH_RESP *)pData;
	int i=0, iPosInserted=-1, bExist=0;
	DEVICE_SEARCHED *pDevSearch=NULL;

	for(i=0; i<MAX_NUM_SEARCH_ON_LAN; i++){
		pDevSearch=&m_arrDevSearched[i];
		if(iPosInserted==-1 && !pDevSearch->bUsed) iPosInserted=i;
		if(pDevSearch->bUsed && memcmp(pDevSearch->chDID, pSearchResp->szDeviceID, MAX_LEN_DID)==0){
			bExist=1;
			break;
		}
	}

	if(!bExist){
		pDevSearch=&m_arrDevSearched[iPosInserted];
		strcpy(pDevSearch->chDID, pSearchResp->szDeviceID);
		strcpy(pDevSearch->chIPAddr, pSearchResp->szIpAddr);
		pDevSearch->product_type[0]=pSearchResp->product_type[0];
		pDevSearch->product_type[1]=pSearchResp->product_type[1];
		pDevSearch->bUsed=1;

		//TRACE(_T("DoSearchCallback,i=%d %d %s\n"), i, iPosInserted, pDevSearch->chDID);
		PostMessage(OM_UPDATE_DEVICE, iPosInserted, 0L);
	}
}

void CSEP2PAppSDKDemoDlg::WriteLog(CHAR *format, ...)
{
#ifndef _DEBUG
	CHAR chLog[400]={0};
	CString csFilename;
	FILE *fp=NULL;

	csFilename.Format(_T("%s\\connect.log"), m_csAppPath);
	CharFromWSTRU(CP_OEMCP, csFilename, csFilename.GetLength(), chLog);
	fp=fopen(chLog, "a+");
	if(fp){
		memset(chLog, 0, sizeof(chLog));

		va_list ap;
		time_t timep;
		struct tm *p=NULL;
		time (&timep);
		p = localtime (&timep);
		
		sprintf(chLog,"%04d-%02d-%02d %02d:%02d:%02d  ",p->tm_year+1900,p->tm_mon+1,p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
		va_start(ap, format);
		vsprintf(&chLog[20], format,ap);
		va_end(ap);
		
		fwrite(chLog, 1, strlen(chLog), fp);
		fclose(fp);
	}
#endif
}

void CSEP2PAppSDKDemoDlg::DoRecvMsg(UCHAR nDeviceNo, UINT32  nMsgType, CHAR* pMsg, UINT32 nMsgSize)
{
	TRACE(_T("DoRecvMsgCallback] nDeviceNo=%d, nMsgType=0x%X, nMsgSize=%d\n"),nDeviceNo, nMsgType, nMsgSize);
	if(nMsgType%2==1){
		int nRespResult=pMsg[0];
		//TRACE(_T("DoRecvMsgCallback] nDeviceNo=%d, nMsgType=0x%X, nRespResult=%d\n"),nDeviceNo, nMsgType, nRespResult);
	}

	if(nMsgType==SEP2P_MSG_CONNECT_STATUS){
		MSG_CONNECT_STATUS *pConnectStatus=(MSG_CONNECT_STATUS *)pMsg;
		PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_Connect, MAKELPARAM(pConnectStatus->eConnectStatus, nDeviceNo));
		TRACE(_T("DoRecvMsgCallback] SEP2P_MSG_CONNECT_STATUS eConnectStatus=%d\n"), pConnectStatus->eConnectStatus);

		if(pConnectStatus->eConnectStatus==CONNECT_STATUS_CONNECTED){
			CHAR chIP[3][24];
			strcpy(chIP[0],inet_ntoa(pConnectStatus->stRemoteAddr.sin_addr));
			strcpy(chIP[1],inet_ntoa(pConnectStatus->stMyLocalAddr.sin_addr));
			strcpy(chIP[2],inet_ntoa(pConnectStatus->stMyWanAddr.sin_addr));
			WriteLog("%s CONNECT_STATUS_CONNECTED: %s; Remote:%s:%d   MyLAN: %s:%d  MyWAN: %s:%d\n",
				m_pObjCams[nDeviceNo]->GetDID(),
				pConnectStatus->eConnectMode==CONNECT_MODE_P2P ? "P2P" : "Relay",
				chIP[0],  ntohs(pConnectStatus->stRemoteAddr.sin_port),
				chIP[1],  ntohs(pConnectStatus->stMyLocalAddr.sin_port),
				chIP[2],  ntohs(pConnectStatus->stMyWanAddr.sin_port));
		}

	}else if(nMsgType==SEP2P_MSG_CONNECT_MODE){
		INT32 *pConnectMode =(INT32 *)pMsg;
		if(*pConnectMode==CONNECT_MODE_P2P) TRACE(_T("DoRecvMsgCallback] SEP2P_MSG_CONNECT_MODE=P2P\n"));
		else TRACE(_T("DoRecvMsgCallback] SEP2P_MSG_CONNECT_MODE=RLY\n"));

	}else if(nMsgType==SEP2P_MSG_START_TALK_RESP){
		MSG_START_TALK_RESP *pTalkResp=(MSG_START_TALK_RESP *)pMsg;
		TRACE(_T("DoRecvMsgCallback] TalkResp=%d\n"), pTalkResp->result);

		if(pTalkResp->result==0) {
			//1>stop talk of the other device
			//......
			//2>start this talk
			AV_PARAMETER *pAVParam=m_pObjCams[nDeviceNo]->GetAVParameter();
			if(pAVParam->nAudioCodecID==AV_CODECID_AUDIO_AAC) m_objSample.StartSample(nDeviceNo,AUDIO_SAMPLE_RATE_16K, OnSampleCallback, this);
			else m_objSample.StartSample(nDeviceNo,AUDIO_SAMPLE_RATE_8K, OnSampleCallback, this);
		}
		PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_Talk, MAKELPARAM(pTalkResp->result, nDeviceNo));

	}else if(nMsgType==SEP2P_MSG_GET_WIFI_LIST_RESP){
		MSG_GET_WIFI_LIST_RESP *pResp=(MSG_GET_WIFI_LIST_RESP *)pMsg;
		TRACE(_T("DoRecvMsgCallback] nResultCount=%d\n"),pResp->nResultCount);

	}else if(nMsgType==SEP2P_MSG_GET_CAMERA_PARAM_RESP){
		MSG_GET_CAMERA_PARAM_RESP *pResp=(MSG_GET_CAMERA_PARAM_RESP *)pMsg;
		TRACE(_T("DoRecvMsgCallback] MSG_GET_CAMERA_PARAM_RESP, nFlip=%d, nIRLed=%d bOSD=0x%X micvol=%d spkvol=%d\n"), 
					pResp->nFlip, pResp->nIRLed, pResp->bOSD, pResp->nMICVolume, pResp->nSPKVolume);

	}else if(nMsgType==SEP2P_MSG_SET_CAMERA_PARAM_RESP){
		MSG_SET_CAMERA_PARAM_RESP *pResp=(MSG_SET_CAMERA_PARAM_RESP *)pMsg;
		TRACE(_T("DoRecvMsgCallback] result=%d,nBitMaskToSet=0x%X\n"), pResp->result, pResp->nBitMaskToSet);

	}else if(nMsgType==SEP2P_MSG_GET_USER_INFO_RESP){
			MSG_GET_USER_INFO_RESP *pResp=(MSG_GET_USER_INFO_RESP *)pMsg;
			OutputDebugStringA(pResp->chAdmin);

	}else if(nMsgType==SEP2P_MSG_GET_UART_CTRL_RESP){
		MSG_GET_UART_CTRL_RESP *pResp=(MSG_GET_UART_CTRL_RESP *)pMsg;
		char chTmp[256]={0};
		sprintf(chTmp, "DoRecvMsgCallback] SEP2P_MSG_GET_UART_CTRL_RESP, chUartAlarmServer=%s,nUartAlarmServerPort=%d\n", pResp->chUartAlarmServer, pResp->nUartAlarmServerPort);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_EXT_CMD1){ //User defined msg response
		OutputDebugStringA(pMsg);

	}else if(nMsgType==SEP2P_MSG_EXT_CMD2){ //User defined msg response
		SEP2P_MSG_EXT_SDFILE_RESP *pResp=(SEP2P_MSG_EXT_SDFILE_RESP *)pMsg;
		CHAR *pFile=pMsg+sizeof(SEP2P_MSG_EXT_SDFILE_RESP);
		CHAR chTmp[128];
		for(int kk=0; kk<pResp->nFileNum; kk++){
			sprintf(chTmp, "file%d=%s\n", kk, pFile+kk*64);
			OutputDebugStringA(chTmp);
		}
	}else if(nMsgType==SEP2P_MSG_GET_CUSTOM_PARAM_RESP){
			MSG_GET_CUSTOM_PARAM_RESP *pResp=(MSG_GET_CUSTOM_PARAM_RESP *)pMsg;
			CHAR chTmp[128];
			sprintf(chTmp,"result=%d %s=%s\n", pResp->result, pResp->chParamName, pResp->chParamValue);
			OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_SET_CUSTOM_PARAM_RESP){
		MSG_SET_CUSTOM_PARAM_RESP *pResp=(MSG_SET_CUSTOM_PARAM_RESP *)pMsg;
		CHAR chTmp[128];
		sprintf(chTmp,"result=%d %s=%s\n", pResp->result, pResp->chParamName, pResp->chParamValue);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_ALARM_INFO_RESP){
		MSG_GET_ALARM_INFO_RESP *pResp=(MSG_GET_ALARM_INFO_RESP *)pMsg;
		CHAR chTmp[128];
		sprintf(chTmp,"MSG_GET_ALARM_INFO_RESP,md_name=%s\n", pResp->md_name);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP){
		MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP *pResp=(MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP *)pMsg;
		CHAR chTmp[128];
		sprintf(chTmp,"\tMSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP,chDay=%s\n", pResp->chDay);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP){
		MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP *pResp=(MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP *)pMsg;
		CHAR chTmp[1024];
		sprintf(chTmp,"\tMSG_GET_REMOTE_REC_FILE_BY_DAY_RESP,res=%d TNum=%d no=[%d,%d]\n", pResp->nResult, pResp->nFileTotalNum, pResp->nBeginNoOfThisTime, pResp->nEndNoOfThisTime);
		OutputDebugStringA(chTmp);

		INT32 ii=0, nNum=pResp->nEndNoOfThisTime - pResp->nBeginNoOfThisTime + 1;
		REC_FILE_INFO *pRecFile=(REC_FILE_INFO *)(pMsg+sizeof(MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP));
		strcpy(m_chFirstFilePath, pRecFile->chFilePath); //only test
		for(ii=0; ii<nNum; ii++){
			//1 /mnt/mmc/201412/19/nrc20141219235248.mp4 2014-12-19 23:52:48 2014-12-19 23:57:49, 301s 78418KB
			sprintf(chTmp,"\t %d %s %s %s, %ds %dKB\n", 
				    ii+pResp->nBeginNoOfThisTime, pRecFile->chFilePath, pRecFile->chStartTime, pRecFile->chEndTime,
					pRecFile->nTimeLen_sec, pRecFile->nFileSize_KB);
			OutputDebugStringA(chTmp);
			pRecFile+=1;
		}
	}else if(nMsgType==SEP2P_MSG_START_PLAY_REC_FILE_RESP){
		MSG_START_PLAY_REC_FILE_RESP *pResp=(MSG_START_PLAY_REC_FILE_RESP *)pMsg;
		CHAR chTmp[256];
		sprintf(chTmp,"\tMSG_START_PLAY_REC_FILE_RESP,res=%d fpath=%s, playbackID=%d\n", pResp->nResult, pResp->chFilePath, pResp->nPlaybackID);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_STOP_PLAY_REC_FILE_RESP){
		MSG_STOP_PLAY_REC_FILE_RESP *pResp=(MSG_STOP_PLAY_REC_FILE_RESP *)pMsg;
		CHAR chTmp[128];
		sprintf(chTmp,"\tMSG_STOP_PLAY_REC_FILE_RESP,res=%d fpath=%s\n", pResp->nResult, pResp->chFilePath);
		OutputDebugStringA(chTmp);

	}else if(nMsgType==SEP2P_MSG_GET_DATETIME_RESP){
		MSG_GET_DATETIME_RESP *pResp=(MSG_GET_DATETIME_RESP *)pMsg;
		
		char szDateTime[128]={0};
		time_t nTime=(time_t)(pResp->nSecToNow+pResp->nSecTimeZone);
		struct tm *ptm1=NULL;
		ptm1= gmtime(&nTime);
		sprintf(szDateTime, "%4d-%02d-%02d %02d:%02d:%02d", 
			ptm1->tm_year+1900,
			ptm1->tm_mon+1,
			ptm1->tm_mday,
			ptm1->tm_hour,
			ptm1->tm_min,
			ptm1->tm_sec);
		printf("szDateTime=%s", szDateTime);

	}/*else if(nMsgType==SEP2P_MSG_GET_EXT_APP_RESP){
		MSG_GET_EXT_APP_RESP *pResp=(MSG_GET_EXT_APP_RESP *)pMsg;
		printf("%s\n", pResp->chUrlPrefix);
	}
	*/

	if(!m_bExitingApp){
		MSG_INFO *pMsgInfo=(MSG_INFO *)malloc(sizeof(MSG_INFO));
		memset(pMsgInfo, 0, sizeof(MSG_INFO));
		pMsgInfo->nDeviceNo=nDeviceNo;
		pMsgInfo->nMsgType=nMsgType;
		pMsgInfo->nMsgSize=nMsgSize;
		if(nMsgSize>0){
			pMsgInfo->pMsg=(CHAR *)malloc(nMsgSize);
			memcpy(pMsgInfo->pMsg, pMsg, nMsgSize);
		}
		PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_MsgArrived, (LPARAM)pMsgInfo);
	}
}

void CSEP2PAppSDKDemoDlg::OnButtonsRange(UINT nID)
{
	switch(nID)
	{
//edit device
		case IDC_EDIT_DEVICE1:{
			CHAR *pMyDID=m_pObjCams[0]->GetDID();
			CDeviceInfo dlg;
			dlg.m_nChanNo=1;
			if(pMyDID[0]!='\0') BSTRFromCharU(CP_OEMCP, pMyDID, dlg.m_csDID);
			dlg.m_csUsername.Format(_T("%s"), m_pObjCams[0]->GetUsername());
			dlg.m_csPassword.Format(_T("%s"), m_pObjCams[0]->GetPasswd());
			dlg.m_pSampleDlg=this;
			if(IDOK==dlg.DoModal()){
				CHAR chMyDID[128]={0};
				CharFromWSTRU(CP_OEMCP, dlg.m_csDID, dlg.m_csDID.GetLength(), chMyDID);
				strcpy(m_pObjCams[0]->GetDID(), chMyDID);
				wcscpy(m_pObjCams[0]->GetUsername(), dlg.m_csUsername.GetBuffer());
				wcscpy(m_pObjCams[0]->GetPasswd(), dlg.m_csPassword.GetBuffer());
				dlg.m_csUsername.ReleaseBuffer();
				dlg.m_csPassword.ReleaseBuffer();
			}
			}
			break;

		case IDC_EDIT_DEVICE2:{
			CHAR *pMyDID=m_pObjCams[1]->GetDID();
			CDeviceInfo dlg;
			dlg.m_nChanNo=2;
			if(pMyDID[0]!='\0') BSTRFromCharU(CP_OEMCP, pMyDID, dlg.m_csDID);
			dlg.m_csUsername.Format(_T("%s"), m_pObjCams[1]->GetUsername());
			dlg.m_csPassword.Format(_T("%s"), m_pObjCams[1]->GetPasswd());
			dlg.m_pSampleDlg=this;
			if(IDOK==dlg.DoModal()){
				CHAR chMyDID[128]={0};
				CharFromWSTRU(CP_OEMCP, dlg.m_csDID, dlg.m_csDID.GetLength(), chMyDID);
				strcpy(m_pObjCams[1]->GetDID(), chMyDID);
				wcscpy(m_pObjCams[1]->GetUsername(), dlg.m_csUsername.GetBuffer());
				wcscpy(m_pObjCams[1]->GetPasswd(), dlg.m_csPassword.GetBuffer());
				dlg.m_csUsername.ReleaseBuffer();
				dlg.m_csPassword.ReleaseBuffer();
			}
			}
			break;

		case IDC_EDIT_DEVICE3:{
			CHAR *pMyDID=m_pObjCams[2]->GetDID();
			CDeviceInfo dlg;
			dlg.m_nChanNo=3;
			if(pMyDID[0]!='\0') BSTRFromCharU(CP_OEMCP, pMyDID, dlg.m_csDID);
			dlg.m_csUsername.Format(_T("%s"), m_pObjCams[2]->GetUsername());
			dlg.m_csPassword.Format(_T("%s"), m_pObjCams[2]->GetPasswd());
			dlg.m_pSampleDlg=this;
			if(IDOK==dlg.DoModal()){
				CHAR chMyDID[128]={0};
				CharFromWSTRU(CP_OEMCP, dlg.m_csDID, dlg.m_csDID.GetLength(), chMyDID);
				strcpy(m_pObjCams[2]->GetDID(), chMyDID);
				wcscpy(m_pObjCams[2]->GetUsername(), dlg.m_csUsername.GetBuffer());
				wcscpy(m_pObjCams[2]->GetPasswd(), dlg.m_csPassword.GetBuffer());
				dlg.m_csUsername.ReleaseBuffer();
				dlg.m_csPassword.ReleaseBuffer();
			}
			}
			break;

		case IDC_EDIT_DEVICE4:{
			CHAR *pMyDID=m_pObjCams[3]->GetDID();
			CDeviceInfo dlg;
			dlg.m_nChanNo=4;
			if(pMyDID[0]!='\0') BSTRFromCharU(CP_OEMCP, pMyDID, dlg.m_csDID);
			dlg.m_csUsername.Format(_T("%s"), m_pObjCams[3]->GetUsername());
			dlg.m_csPassword.Format(_T("%s"), m_pObjCams[3]->GetPasswd());
			dlg.m_pSampleDlg=this;
			if(IDOK==dlg.DoModal()){
				CHAR chMyDID[128]={0};
				CharFromWSTRU(CP_OEMCP, dlg.m_csDID, dlg.m_csDID.GetLength(), chMyDID);
				strcpy(m_pObjCams[3]->GetDID(), chMyDID);
				wcscpy(m_pObjCams[3]->GetUsername(), dlg.m_csUsername.GetBuffer());
				wcscpy(m_pObjCams[3]->GetPasswd(), dlg.m_csPassword.GetBuffer());
				dlg.m_csUsername.ReleaseBuffer();
				dlg.m_csPassword.ReleaseBuffer();
			}
			}
			break;

//connect device
		case IDC_CONNECT1:{
			CHAR *pMyStr=m_pObjCams[0]->GetDID();
			if(pMyStr[0]=='\0') {
				MessageBox(_T("DID don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			TCHAR* pMyTStr=m_pObjCams[0]->GetUsername();
			if(pMyTStr[0]=='\0') {
				MessageBox(_T("Username don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			if(m_pObjCams[0]->m_bConnecting){
				m_pObjCams[0]->m_bConnecting=0;
				m_pObjCams[0]->GetCtrlConnect()->SetWindowText(ms_csTitleStop[0]);
				m_pObjCams[0]->P2PAPI_Connect();
			}else{
				m_pObjCams[0]->m_bConnecting=1;
				m_pObjCams[0]->GetCtrlConnect()->SetWindowText(ms_csTitleStart[0]);
				m_pObjCams[0]->P2PAPI_Disconnect();
				m_pObjCams[0]->GetCtrlStartVideo()->EnableWindow(FALSE);
				m_pObjCams[0]->GetCtrlStartAudio()->EnableWindow(FALSE);
				m_pObjCams[0]->GetCtrlStartTalk()->EnableWindow(FALSE);
				m_ctlComboChn.EnableWindow(FALSE);
			}
			}break;

		case IDC_CONNECT2:{
			CHAR *pMyStr=m_pObjCams[1]->GetDID();
			if(pMyStr[0]=='\0') {
				MessageBox(_T("DID don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			TCHAR* pMyTStr=m_pObjCams[1]->GetUsername();
			if(pMyTStr[0]=='\0') {
				MessageBox(_T("Username don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			if(m_pObjCams[1]->m_bConnecting){
				m_pObjCams[1]->m_bConnecting=0;
				m_pObjCams[1]->GetCtrlConnect()->SetWindowText(ms_csTitleStop[0]);
				m_pObjCams[1]->P2PAPI_Connect();

			}else{
				m_pObjCams[1]->m_bConnecting=1;
				m_pObjCams[1]->GetCtrlConnect()->SetWindowText(ms_csTitleStart[0]);
				m_pObjCams[1]->P2PAPI_Disconnect();
				m_pObjCams[1]->GetCtrlStartVideo()->EnableWindow(FALSE);
				m_pObjCams[1]->GetCtrlStartAudio()->EnableWindow(FALSE);
				m_pObjCams[1]->GetCtrlStartTalk()->EnableWindow(FALSE);
			}
			}break;

		case IDC_CONNECT3:{
			CHAR *pMyStr=m_pObjCams[2]->GetDID();
			if(pMyStr[0]=='\0') {
				MessageBox(_T("DID don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			TCHAR* pMyTStr=m_pObjCams[2]->GetUsername();
			if(pMyTStr[0]=='\0') {
				MessageBox(_T("Username don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			if(m_pObjCams[2]->m_bConnecting){
				m_pObjCams[2]->m_bConnecting=0;
				m_pObjCams[2]->GetCtrlConnect()->SetWindowText(ms_csTitleStop[0]);
				m_pObjCams[2]->P2PAPI_Connect();

			}else{
				m_pObjCams[2]->m_bConnecting=1;
				m_pObjCams[2]->GetCtrlConnect()->SetWindowText(ms_csTitleStart[0]);
				m_pObjCams[2]->P2PAPI_Disconnect();
				m_pObjCams[2]->GetCtrlStartVideo()->EnableWindow(FALSE);
				m_pObjCams[2]->GetCtrlStartAudio()->EnableWindow(FALSE);
				m_pObjCams[2]->GetCtrlStartTalk()->EnableWindow(FALSE);
			}
			}break;

		case IDC_CONNECT4:{
			CHAR *pMyStr=m_pObjCams[3]->GetDID();
			if(pMyStr[0]=='\0') {
				MessageBox(_T("DID don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			TCHAR* pMyTStr=m_pObjCams[3]->GetUsername();
			if(pMyTStr[0]=='\0') {
				MessageBox(_T("Username don't enter."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				break;
			}
			if(m_pObjCams[3]->m_bConnecting){
				m_pObjCams[3]->m_bConnecting=0;
				m_pObjCams[3]->GetCtrlConnect()->SetWindowText(ms_csTitleStop[0]);
				m_pObjCams[3]->P2PAPI_Connect();

			}else{
				m_pObjCams[3]->m_bConnecting=1;
				m_pObjCams[3]->GetCtrlConnect()->SetWindowText(ms_csTitleStart[0]);
				m_pObjCams[3]->P2PAPI_Disconnect();
				m_pObjCams[3]->GetCtrlStartVideo()->EnableWindow(FALSE);
				m_pObjCams[3]->GetCtrlStartAudio()->EnableWindow(FALSE);
				m_pObjCams[3]->GetCtrlStartTalk()->EnableWindow(FALSE);
			}
			}break;

//request video
		case IDC_START_VIDEO1:{
			if(m_pObjCams[0]->m_bStartVideo){
				m_pObjCams[0]->m_bStartVideo=0;
				m_pObjCams[0]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStop[1]);
				
				MSG_START_VIDEO stStartVideo;
				memset(&stStartVideo, 0, sizeof(stStartVideo));
				stStartVideo.eVideoReso=VIDEO_RESO_VGA1;
				stStartVideo.nChannel=m_ctlComboChn.GetCurSel();
				stStartVideo.nVideoCodecID=AV_CODECID_VIDEO_H264;

				INT32 nRet=m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_START_VIDEO, (CHAR *)&stStartVideo, sizeof(stStartVideo)); //NULL:  The default is VIDEO_RESO_VGA1.
				TRACE(_T("VIDEO P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[0]->m_bStartVideo=1;
				m_pObjCams[0]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStart[1]);

				MSG_STOP_VIDEO stStopVideo;
				memset(&stStopVideo, 0, sizeof(stStopVideo));
				stStopVideo.nChannel=m_ctlComboChn.GetCurSel();
				m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_STOP_VIDEO, (CHAR *)&stStopVideo, sizeof(stStopVideo));
			}
			}break;

		case IDC_START_VIDEO2:{
			if(m_pObjCams[1]->m_bStartVideo){
				m_pObjCams[1]->m_bStartVideo=0;
				m_pObjCams[1]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStop[1]);
				INT32 nRet=m_pObjCams[1]->P2PAPI_SendMsg(SEP2P_MSG_START_VIDEO, NULL, 0);
				TRACE(_T("VIDEO P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[1]->m_bStartVideo=1;
				m_pObjCams[1]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStart[1]);
				m_pObjCams[1]->P2PAPI_SendMsg(SEP2P_MSG_STOP_VIDEO, NULL, 0);
			}
			}break;

		case IDC_START_VIDEO3:{
			if(m_pObjCams[2]->m_bStartVideo){
				m_pObjCams[2]->m_bStartVideo=0;
				m_pObjCams[2]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStop[1]);
				INT32 nRet=m_pObjCams[2]->P2PAPI_SendMsg(SEP2P_MSG_START_VIDEO, NULL, 0);
				TRACE(_T("VIDEO P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[2]->m_bStartVideo=1;
				m_pObjCams[2]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStart[1]);
				m_pObjCams[2]->P2PAPI_SendMsg(SEP2P_MSG_STOP_VIDEO, NULL, 0);
			}
			}break;

		case IDC_START_VIDEO4:{
			if(m_pObjCams[3]->m_bStartVideo){
				m_pObjCams[3]->m_bStartVideo=0;
				m_pObjCams[3]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStop[1]);
				INT32 nRet=m_pObjCams[3]->P2PAPI_SendMsg(SEP2P_MSG_START_VIDEO, NULL, 0);
				TRACE(_T("VIDEO P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[3]->m_bStartVideo=1;
				m_pObjCams[3]->GetCtrlStartVideo()->SetWindowText(ms_csTitleStart[1]);
				m_pObjCams[3]->P2PAPI_SendMsg(SEP2P_MSG_STOP_VIDEO, NULL, 0);
			}
			}break;

//request audio
		case IDC_START_AUDIO1:{
			if(m_pObjCams[0]->m_bStartAudio){
				m_pObjCams[0]->m_bStartAudio=0;
				m_pObjCams[0]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStop[2]);

				MSG_START_AUDIO stStartAudio;
				memset(&stStartAudio, 0, sizeof(stStartAudio));
				stStartAudio.nAudioCodecID=AV_CODECID_AUDIO_AAC;
				stStartAudio.nChannel=m_ctlComboChn.GetCurSel();
				INT32 nRet=m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_START_AUDIO, (CHAR *)&stStartAudio, sizeof(stStartAudio));
				TRACE(_T("AUDIO P2PAPI_SendMsg=%d\n"), nRet);

				if(nRet==ERR_P2PAPI_ALREADY_OPEN_AUDIO){
					MessageBox(_T("The other audio has already been opened."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				}
			}else{
				m_pObjCams[0]->m_bStartAudio=1;
				m_pObjCams[0]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStart[2]);

				MSG_STOP_AUDIO stStopAudio;
				memset(&stStopAudio,0,sizeof(stStopAudio));
				stStopAudio.nChannel=m_ctlComboChn.GetCurSel();
				m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_STOP_AUDIO, (CHAR *)&stStopAudio, sizeof(stStopAudio));
			}
			}break;

		case IDC_START_AUDIO2:{
			if(m_pObjCams[1]->m_bStartAudio){
				m_pObjCams[1]->m_bStartAudio=0;
				m_pObjCams[1]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStop[2]);
				INT32 nRet=m_pObjCams[1]->P2PAPI_SendMsg(SEP2P_MSG_START_AUDIO, NULL, 0);
				TRACE(_T("AUDIO P2PAPI_SendMsg=%d\n"), nRet);
				if(nRet==ERR_P2PAPI_ALREADY_OPEN_AUDIO){
					MessageBox(_T("The other audio has already been opened."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				}
			}else{
				m_pObjCams[1]->m_bStartAudio=1;
				m_pObjCams[1]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStart[2]);
				m_pObjCams[1]->P2PAPI_SendMsg(SEP2P_MSG_STOP_AUDIO, NULL, 0);
			}
			}break;

		case IDC_START_AUDIO3:{
			if(m_pObjCams[2]->m_bStartAudio){
				m_pObjCams[2]->m_bStartAudio=0;
				m_pObjCams[2]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStop[2]);
				INT32 nRet=m_pObjCams[2]->P2PAPI_SendMsg(SEP2P_MSG_START_AUDIO, NULL, 0);
				TRACE(_T("AUDIO P2PAPI_SendMsg=%d\n"), nRet);
				if(nRet==ERR_P2PAPI_ALREADY_OPEN_AUDIO){
					MessageBox(_T("The other audio has already been opened."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				}
			}else{
				m_pObjCams[2]->m_bStartAudio=1;
				m_pObjCams[2]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStart[2]);
				m_pObjCams[2]->P2PAPI_SendMsg(SEP2P_MSG_STOP_AUDIO, NULL, 0);
			}
			}break;

		case IDC_START_AUDIO4:{
			if(m_pObjCams[3]->m_bStartAudio){
				m_pObjCams[3]->m_bStartAudio=0;
				m_pObjCams[3]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStop[2]);
				INT32 nRet=m_pObjCams[3]->P2PAPI_SendMsg(SEP2P_MSG_START_AUDIO, NULL, 0);
				TRACE(_T("AUDIO P2PAPI_SendMsg=%d\n"), nRet);
				if(nRet==ERR_P2PAPI_ALREADY_OPEN_AUDIO){
					MessageBox(_T("The other audio has already been opened."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
				}
			}else{
				m_pObjCams[3]->m_bStartAudio=1;
				m_pObjCams[3]->GetCtrlStartAudio()->SetWindowText(ms_csTitleStart[2]);
				m_pObjCams[3]->P2PAPI_SendMsg(SEP2P_MSG_STOP_AUDIO, NULL, 0);
			}
			}break;

//request talk
		case IDC_START_TALK1:{
			if(m_pObjCams[0]->m_bStartTalk){
				m_pObjCams[0]->m_bStartTalk=0;
				m_pObjCams[0]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStop[3]);
				
				MSG_START_TALK stStartTalk;
				memset(&stStartTalk, 0, sizeof(stStartTalk));
				stStartTalk.nChannel=m_ctlComboChn.GetCurSel();
				INT32 nRet=m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_START_TALK, (CHAR *)&stStartTalk, sizeof(stStartTalk));
				TRACE(_T("TALK P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[0]->m_bStartTalk=1;
				m_pObjCams[0]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStart[3]);
				m_objSample.StopSample();

				PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_Talk, MAKELPARAM(10, 0));

				MSG_STOP_TALK stStopTalk;
				memset(&stStopTalk, 0, sizeof(stStopTalk));
				stStopTalk.nChannel=m_ctlComboChn.GetCurSel();
				INT32 nRet=m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_START_TALK, (CHAR *)&stStopTalk, sizeof(stStopTalk));
				m_pObjCams[0]->P2PAPI_SendMsg(SEP2P_MSG_STOP_TALK, (CHAR *)&stStopTalk, sizeof(stStopTalk));
			}
			}break;

		case IDC_START_TALK2:{
			if(m_pObjCams[1]->m_bStartTalk){
				m_pObjCams[1]->m_bStartTalk=0;
				m_pObjCams[1]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStop[3]);
				INT32 nRet=m_pObjCams[1]->P2PAPI_SendMsg(SEP2P_MSG_START_TALK, NULL, 0);
				TRACE(_T("TALK P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[1]->m_bStartTalk=1;
				m_pObjCams[1]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStart[3]);
				m_objSample.StopSample();
				PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_Talk, MAKELPARAM(10, 1));
				m_pObjCams[1]->P2PAPI_SendMsg(SEP2P_MSG_STOP_TALK, NULL, 0);
			}
			}break;

		case IDC_START_TALK3:{
			if(m_pObjCams[2]->m_bStartTalk){
				m_pObjCams[2]->m_bStartTalk=0;
				m_pObjCams[2]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStop[3]);
				INT32 nRet=m_pObjCams[2]->P2PAPI_SendMsg(SEP2P_MSG_START_TALK, NULL, 0);
				TRACE(_T("TALK P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[2]->m_bStartTalk=1;
				m_pObjCams[2]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStart[3]);
				m_objSample.StopSample();
				PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_Talk, MAKELPARAM(10, 2));
				m_pObjCams[2]->P2PAPI_SendMsg(SEP2P_MSG_STOP_TALK, NULL, 0);
			}
			}break;

		case IDC_START_TALK4:{
			if(m_pObjCams[3]->m_bStartTalk){
				m_pObjCams[3]->m_bStartTalk=0;
				m_pObjCams[3]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStop[3]);
				INT32 nRet=m_pObjCams[3]->P2PAPI_SendMsg(SEP2P_MSG_START_TALK, NULL, 0);
				TRACE(_T("TALK P2PAPI_SendMsg=%d\n"), nRet);
			}else{
				m_pObjCams[3]->m_bStartTalk=1;
				m_pObjCams[3]->GetCtrlStartTalk()->SetWindowText(ms_csTitleStart[3]);
				m_objSample.StopSample();
				PostMessage(OM_UPDATE_UI, UPDATE_UI_WPARAM_Talk, MAKELPARAM(10, 3));
				m_pObjCams[3]->P2PAPI_SendMsg(SEP2P_MSG_STOP_TALK, NULL, 0);
			}
			}break;
		default:;
	}
}

BOOL CSEP2PAppSDKDemoDlg::IsExistThisDID(const CHAR *pDID)
{
	if(pDID==NULL) return FALSE;
	int i=0;
	CHAR *pMyDID=NULL;
	for(i=0; i<MAX_NUM_CHANNEL; i++){
		pMyDID=m_pObjCams[i]->GetDID();
		if(memcmp(pMyDID, pDID, MAX_LEN_DID)==0){
			return TRUE;
		}
	}
	return FALSE;
}

void CSEP2PAppSDKDemoDlg::OnBnClickedLansearch()
{
	INT32 nRet=0;
	if(m_bStartLanSearch){
		m_bStartLanSearch=0;
		SetDlgItemText(IDC_LANSEARCH, _T("Stop LAN Search"));
		memset(m_arrDevSearched, 0, sizeof(m_arrDevSearched));
		m_pObjCams[0]->P2PAPI_StartSearch();

	}else{
		m_bStartLanSearch=1;
		SetDlgItemText(IDC_LANSEARCH, _T("Start LAN Search"));
		m_pObjCams[0]->P2PAPI_StopSearch();
	}
	TRACE("OnBnClickedLansearch] SEP2P_...=%d\n", nRet);
}

//static 
INT32 CSEP2PAppSDKDemoDlg::OnSampleCallback(int index, CHAR* pData, UINT32  nDataSize, void*	pUserData)
{
	CSEP2PAppSDKDemoDlg* pThis=(CSEP2PAppSDKDemoDlg *)pUserData;
	pThis->m_pObjCams[index]->P2PAPI_TalkData(pData, nDataSize);
	return 0;
}

void CSEP2PAppSDKDemoDlg::OnBnClickedClearLog()
{
	SetDlgItemText(IDC_EDIT3, _T(""));
}

void CSEP2PAppSDKDemoDlg::OnBnClickedGetReq()
{
	int nChannel=m_ctlComboChn.GetCurSel();
	int nDeviceNo=m_ctlComboDev.GetCurSel(), nCmdIndex=0;
	if(nDeviceNo==CB_ERR) return;
	if(m_pObjCams[nDeviceNo]->m_bConnecting){
		MessageBox(_T("Please first connect the device"), _T("Tips"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	nCmdIndex=m_ctlComboReqStr.GetCurSel();
	switch(nCmdIndex)
	{
		case 0:
			TRACE("GET_CAMERA_PARAM------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_CAMERA_PARAM_REQ, NULL, 0);
			break;

		case 1:
			TRACE("GET_CURRENT_WIFI------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_CURRENT_WIFI_REQ, NULL, 0);
			break;

		case 2:
			TRACE("GET_WIFI_LIST------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_WIFI_LIST_REQ, NULL, 0);
			break;

		case 3:
			TRACE("GET_USER_INFO------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_USER_INFO_REQ, NULL, 0);
			break;

		case 4:
			TRACE("GET_DATETIME------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_DATETIME_REQ, NULL, 0);
			break;

		case 5:
			TRACE("GET_FTP_INFO------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_FTP_INFO_REQ, NULL, 0);
			break;

		case 6:
			TRACE("GET_EMAIL_INFO------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_EMAIL_INFO_REQ, NULL, 0);
			break;

		case 7:
			TRACE("GET_ALARM_INFO------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_ALARM_INFO_REQ, NULL, 0);
			break;

		case 8:
			TRACE("GET_SDCARD_REC_PARAM------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_SDCARD_REC_PARAM_REQ, NULL, 0);

			//m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_IPUSH_INFO_REQ, NULL, 0);
			break;

		case 9:
			TRACE("GET_DEVICE_VERSION------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_DEVICE_VERSION_REQ, NULL, 0);
			break;

		case 10:{
			TRACE("QUERY_CHANNEL_INFO_OF_NVR_REQ------\n");
			MSG_QUERY_CHANNEL_INFO_OF_NVR_REQ stReq;
			memset(&stReq, 0, sizeof(stReq));
			stReq.nChannel=0xFF;
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_QUERY_CHANNEL_INFO_OF_NVR_REQ, (CHAR *)&stReq, sizeof(stReq));
			}break;

		case 12:{
			TRACE("SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ------\n");
			MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ stReq;
			memset(&stReq, 0, sizeof(stReq));
			stReq.nYearMon=201412;
			stReq.nRecType=1;
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ, (CHAR *)&stReq, sizeof(stReq));
			}break;

		case 13:
			TRACE("MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ------\n");
			{
				MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ stReq;
				memset(&stReq, 0, sizeof(stReq));
				stReq.nYearMonDay=20161205;
				stReq.nRecType=1;
				stReq.nBeginNoOfThisTime=0;
				stReq.nEndNoOfThisTime=0;
				m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ, (CHAR *)&stReq, sizeof(stReq));
			}break;

		case 14:
			TRACE("MSG_START_PLAY_REC_FILE_REQ------%s\n", m_chFirstFilePath);
			{
				MSG_START_PLAY_REC_FILE_REQ stReq;
				memset(&stReq, 0, sizeof(stReq));
				stReq.nBeginPos_sec=0;
				strcpy(stReq.chFilePath, m_chFirstFilePath);
				m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_START_PLAY_REC_FILE_REQ, (CHAR *)&stReq, sizeof(stReq));
			}break;

		case 15:
			TRACE("MSG_STOP_PLAY_REC_FILE_REQ------%s\n", m_chFirstFilePath);
			{
				MSG_STOP_PLAY_REC_FILE_REQ stReq;
				memset(&stReq, 0, sizeof(stReq));
				strcpy(stReq.chFilePath, m_chFirstFilePath);
				m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_STOP_PLAY_REC_FILE_REQ, (CHAR *)&stReq, sizeof(stReq));
			}break;

		case 16:{
			TRACE("get ipush info------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_IPUSH_INFO_REQ, NULL, 0);
			}break;

		case 17:{
			TRACE("get USER_INFO2------\n");
			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_USER_INFO2_REQ, NULL, 0);

// 			MSG_GET_EXT_APP_REQ stReq;
// 			memset(&stReq, 0, sizeof(stReq));
// 			stReq.nAppType=1;
// 			m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_EXT_APP_REQ, (CHAR*)&stReq, sizeof(stReq));
			}break;

		default:;
	}
}

void CSEP2PAppSDKDemoDlg::OnBnClickedSetReq()
{
	int nDeviceNo=m_ctlComboDev.GetCurSel(), nCmdIndex=0;
	if(nDeviceNo==CB_ERR) return;
	if(m_pObjCams[nDeviceNo]->m_bConnecting){
		MessageBox(_T("Please first connect the device"), _T("Tips"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	nCmdIndex=m_ctlComboReqStr.GetCurSel();
	switch(nCmdIndex)
	{
	case 0:{
		TRACE("SET_CAMERA_PARAM------\n");
		MSG_SET_CAMERA_PARAM_REQ setReq;
		CHAR *pTimeNameOSD=NULL;
		memset(&setReq, 0, sizeof(setReq));
		setReq.nResolution=1;
		setReq.nBright  =0;
		setReq.nContrast=128;
		setReq.nIRLed=2;
		
		pTimeNameOSD=(CHAR *)&(setReq.bOSD);
		pTimeNameOSD[0]=1; //time region OSD
		pTimeNameOSD[1]=1; //name region OSD
		pTimeNameOSD[2]=0; //show Temperature&Humidity
		if(pTimeNameOSD[2]) strcpy(setReq.chOSDName, "TEMP_HUM"); //TEMP_HUM
		else strcpy(setReq.chOSDName, "IPC");

		setReq.nMode=0;
		setReq.nFlip=3;
		setReq.nMICVolume=80;
		setReq.nSPKVolume=60;
		setReq.nBitMaskToSet=BIT_MASK_CAM_PARAM_MODE;//BIT_MASK_CAM_PARAM_MIC_VOLUME|BIT_MASK_CAM_PARAM_SPK_VOLUME;
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_CAMERA_PARAM_REQ, (CHAR *)&setReq, sizeof(setReq));

		//or
		//m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_CAMERA_PARAM_DEFAULT_REQ, NULL, 0);
		}break;

	case 1:{
		TRACE("SET_CURRENT_WIFI------\n");
		MSG_SET_CURRENT_WIFI_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		setReq.bEnable=1;
		strcpy(setReq.chSSID, "TPLINK_soft");
		setReq.nAuthtype=5;
		strcpy(setReq.chWPAPsk,"11111111");
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_CURRENT_WIFI_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 2: //none
		//TRACE("SET_WIFI_LIST------\n");
		break;

	case 3:{
		TRACE("SET_USER_INFO------\n");
		MSG_SET_USER_INFO_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		strcpy(setReq.chAdmin, "admin");
		strcpy(setReq.chAdminPwd, "1234567");
		int nRet=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_USER_INFO_REQ, (CHAR *)&setReq, sizeof(setReq));
		TRACE("SET_USER_INFO------, P2PAPI_SendMsg=%d\n", nRet);
		}break;

	case 4:{
		TRACE("SET_DATETIME------\n");
		MSG_SET_DATETIME_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		setReq.nSecToNow=0;
		setReq.nSecTimeZone=28800;
		strcpy(setReq.chNTPServer, "time.nist.gov");
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_DATETIME_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 5:{
		TRACE("SET_FTP_INFO------\n");
// 		MSG_SET_FTP_INFO_REQ setReq;
// 		memset(&setReq, 0, sizeof(setReq));
// 		strcpy(setReq.chFTPSvr, "192.168.1.100");
// 		strcpy(setReq.chUser,"ftp");
// 		strcpy(setReq.chPwd, "ftp");
// 		strcpy(setReq.chDir,"/device");
// 		setReq.nPort=2121;
// 		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_FTP_INFO_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 6:{
		TRACE("SET_EMAIL_INFO------\n");
		MSG_SET_EMAIL_INFO_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));

		strcpy(setReq.chSMTPSvr,"smtp.126.com");
		strcpy(setReq.chSender,"sender");
		//strcpy(setReq.chUser,"user");
		//strcpy(setReq.chPwd,"user");
		strcpy(setReq.chReceiver1,"recv@126.com");
		strcpy(setReq.chReceiver2, " ");
		strcpy(setReq.chText, "email content");
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_EMAIL_INFO_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 7:{
		TRACE("SET_ALARM_INFO------\n");
		MSG_SET_ALARM_INFO_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		setReq.bMDEnable[0]=0;
		setReq.nMDSensitivity[0]=8;
		setReq.md_x[0]=10;
		setReq.md_y[0]=10;
		setReq.md_width[0]=10;
		setReq.md_height[0]=10;

		setReq.bMDEnable[1]=0;
		setReq.nMDSensitivity[1]=8;
		setReq.md_x[1]=19;
		setReq.md_y[1]=19;
		setReq.md_width[1]=19;
		setReq.md_height[1]=19;

		setReq.bMDEnable[2]=1;
		setReq.nMDSensitivity[2]=7;
		setReq.md_x[2]=30;
		setReq.md_y[2]=30;
		setReq.md_width[2]=30;
		setReq.md_height[2]=30;

		setReq.bMDEnable[3]=1;
		setReq.nMDSensitivity[3]=6;
		setReq.md_x[3]=40;
		setReq.md_y[3]=40;
		setReq.md_width[3]=40;
		setReq.md_height[3]=40;

		setReq.bMailWhenAlarm=1;
		setReq.nAudioAlarmSensitivity =1;
		setReq.bSnapshotToFTPWhenAlarm=1;
		
		//setReq.nPresetbitWhenAlarm=5;

		setReq.bIOLinkageWhenAlarm=1;
		setReq.nTimeSecOfIOOut=11;
		setReq.bSpeakerWhenAlarm=1;
		setReq.nTimeSecOfSpeaker=12;
		
		setReq.nTriggerAlarmType=1;
		setReq.bTemperatureAlarm=0;
		setReq.bHumidityAlarm=1;
		setReq.nTempMinValueWhenAlarm=2;
		setReq.nTempMaxValueWhenAlarm=3;
		setReq.nHumiMinValueWhenAlarm=4;
		setReq.nHumiMaxValueWhenAlarm=5;

		//alarm schedule(alarmed time)
// 		setReq.nAlarmTime_sun_0=0x10000000; //sunday,07:00:00---07:14:59
// 		setReq.nAlarmTime_sun_1=0x00000001;	//sunday,08:00:00---08:14:59
// 		setReq.nAlarmTime_sun_2=0x80000001; //sunday,16:00:00---16:14:59, 23:45:00---23:59:59
// 		setReq.nAlarmTime_sat_2=0x80000001; //satday,16:00:00---16:14:59, 23:45:00---23:59:59

		//every 00:00:00---07:59:59,  0bit----8*60=480/15=32bit
		INT32 i=0;
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 0, i, 1);
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 1, i, 1);
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 2, i, 1);
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 3, i, 1);
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 4, i, 1);
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 5, i, 1);
		for(i=0; i<32; i++) set15MinutesFlagAlarmTime(&setReq, 6, i, 1);

		//every 22:00:00---23:59:59, 22*60/15=88bit----24*60/15=96bit
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 0, i, 1);
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 1, i, 1);
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 2, i, 1);
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 3, i, 1);
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 4, i, 1);
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 5, i, 1);
		for(i=88; i<96; i++) set15MinutesFlagAlarmTime(&setReq, 6, i, 1);
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_ALARM_INFO_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 8:{
		TRACE("SET_SDCARD_REC_PARAM------\n");
		MSG_SET_SDCARD_REC_PARAM_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		setReq.bRecordCoverInSDCard=1;
		setReq.nRecordTimeLen=4;
		setReq.nCurChnRecording=1;
 		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_SDCARD_REC_PARAM_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 9:{//format SDCard
		TRACE("Format SDCard------\n");
		//m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_FORMAT_SDCARD_REQ, NULL, 0);
		
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_RESTORE_FACTORY, NULL, 0);

//1.get		
//		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_UART_CTRL_REQ, NULL, 0);//(CHAR *)&stReq, sizeof(stReq));

//2.set
// 		MSG_SET_UART_CTRL_REQ stReq;
// 		memset(&stReq, 0, sizeof(stReq));
// 		memset(&stReq.bUartAlarmEnable, 0xFF, sizeof(stReq.bUartAlarmEnable));
// 		stReq.bUartAlarmEnable[0]=0;
// 		stReq.nBitMaskToSet=BIT_MASK_UART_ALARM_ENABLE;
// 		int n=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_UART_CTRL_REQ,(CHAR *)&stReq, sizeof(stReq));
// 		TRACE(_T("SEP2P_MSG_SET_UART_CTRL_REQ, n=%d\n"), n);
		}break;
	
	case 10:{//PT
		TRACE("PT Control------\n");
		/*
		MSG_PTZ_CONTROL_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		setReq.nCtrlCmd=PTZ_CTRL_PRESET_BIT_SET;
		setReq.nCtrlParam=5; //PTZ goto preset position 5
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_PTZ_CONTROL_REQ, (CHAR *)&setReq, sizeof(setReq));
		*/

		/*
		//control PTZ by one step
		MSG_PTZ_CONTROL_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		if(nDeviceNo==0) setReq.nChannel=m_ctlComboChn.GetCurSel();

		setReq.nCtrlCmd=PTZ_CTRL_LEFT;
		//setReq.nCtrlParam=5; //PTZ goto preset position 5
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_PTZ_CONTROL_REQ, (CHAR *)&setReq, sizeof(setReq));
		
		Sleep(500);
		setReq.nCtrlCmd=PTZ_CTRL_STOP;
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_PTZ_CONTROL_REQ, (CHAR *)&setReq, sizeof(setReq));
		*/

		//cruise horizontal
		MSG_PTZ_CONTROL_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		if(nDeviceNo==0) setReq.nChannel=m_ctlComboChn.GetCurSel();

		setReq.nCtrlCmd=PTZ_CTRL_LEFT;
		setReq.nCtrlParam=1;
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_PTZ_CONTROL_REQ, (CHAR *)&setReq, sizeof(setReq));
		/*
		if(m_nCruiseCount%2==0) setReq.nCtrlCmd=PTZ_CTRL_CRUISE_H;
		else setReq.nCtrlCmd=PTZ_CTRL_STOP;
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_PTZ_CONTROL_REQ, (CHAR *)&setReq, sizeof(setReq));
		m_nCruiseCount++;
		*/
		}break;

	case 11:{ //User defined msg req
		TRACE("User defined msg------\n");
		
		CHAR chData[]={"My msg defined"};
		//int n=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_EXT_CMD1, chData, sizeof(chData));
		int n=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_REBOOT_DEVICE, NULL, 0);
		TRACE(_T("UserMsg, n=%d\n"), n);
		
		/*
		SEP2P_MSG_EXT_SDFILE_REQ stReq;
		memset(&stReq, 0, sizeof(stReq));
		int n=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_EXT_CMD2, (CHAR *)&stReq, sizeof(stReq));
		TRACE(_T("UserMsg, n=%d\n"), n);
		*/

		Sleep(500);
		MSG_SET_CUSTOM_PARAM_REQ stReq;
		memset(&stReq, 0, sizeof(stReq));
		strcpy(stReq.chParamName, "param1"); //param0 to param39 at [admin] of the device
		strcpy(stReq.chParamValue, "value1");
		n=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_CUSTOM_PARAM_REQ, (CHAR *)&stReq, sizeof(stReq));
		TRACE(_T("param req, n=%d\n"), n);
		
		Sleep(500);
		MSG_GET_CUSTOM_PARAM_REQ stReq1;
		memset(&stReq1, 0, sizeof(stReq1));
		strcpy(stReq1.chParamName, "param1"); //param0 to param39 at [admin] of the device
		int n1=m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_GET_CUSTOM_PARAM_REQ, (CHAR *)&stReq1, sizeof(stReq1));
		TRACE(_T("param req, n=%d\n"), n1);
		}break;

	case 16:{
		//SEP2P_MSG_SET_IPUSH_INFO_REQ
		TRACE("set ipush info------\n");
		MSG_SET_IPUSH_INFO_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));
		setReq.bEnable=0;
		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_IPUSH_INFO_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	case 17:{
		TRACE("set USER_INFO2------\n");

		MSG_SET_USER_INFO2_REQ setReq;
		memset(&setReq, 0, sizeof(setReq));

		//L series
		setReq.nOpCode=OP_USER_INFO_EDIT;
		setReq.nOpUserNum=1;
		strcpy(setReq.arrUserInfo[0].chUsername, "admin");
		strcpy(setReq.arrUserInfo[0].chUserPwd, "123456");
		setReq.arrUserInfo[0].nUserID=1;
		setReq.arrUserInfo[0].nUserRoleID=ROLE_ID_ADMIN;

// 		strcpy(setReq.arrUserInfo[0].chUsername, "guest");
// 		strcpy(setReq.arrUserInfo[0].chUserPwd, "guest");
// 		setReq.arrUserInfo[0].nUserID=3;
// 		setReq.arrUserInfo[0].nUserRoleID=ROLE_ID_GUEST;

		m_pObjCams[nDeviceNo]->P2PAPI_SendMsg(SEP2P_MSG_SET_USER_INFO2_REQ, (CHAR *)&setReq, sizeof(setReq));
		}break;

	default:;
	}	
}


INT32 CSEP2PAppSDKDemoDlg::get15MinutesFlagAlarmTime(MSG_SET_ALARM_INFO_REQ *pAlarm, INT32 nWeek, INT32 nBitIndex)
{
	INT32 bSelected = 0;
	if(nWeek<0 || nWeek>=7) return 0;
	INT32 nBitValue = 0;
	switch(nWeek){
		case 0:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_sun_0>>nBitIndex)&0x01;
			}else if (nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_sun_1>>(nBitIndex-32))&0x01;
			}else if(nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_sun_2>>(nBitIndex-64))&0x01;
			}
			break;

		case 1:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_mon_0>>nBitIndex)&0x01;
			}else if (nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_mon_1>>(nBitIndex-32))&0x01;
			}else if(nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_mon_2>>(nBitIndex-64))&0x01;
			}
			break;

		case 2:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_tue_0>>nBitIndex)&0x01;
			}else if ( nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_tue_1>>(nBitIndex-32))&0x01;
			}else if( nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_tue_2>>(nBitIndex-64))&0x01;
			}
			break;

		case 3:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_wed_0>>nBitIndex)&0x01;
			}else if (nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_wed_1>>(nBitIndex-32))&0x01;
			}else if(nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_wed_2>>(nBitIndex-64))&0x01;
			}
			break;

		case 4:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_thu_0>>nBitIndex)&0x01;
			}else if (nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_thu_1>>(nBitIndex-32))&0x01;
			}else if( nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_thu_2>>(nBitIndex-64))&0x01;
			}
			break;

		case 5:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_fri_0>>nBitIndex)&0x01;
			}else if (nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_fri_1>>(nBitIndex-32))&0x01;
			}else if(nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_fri_2>>(nBitIndex-64))&0x01;
			}
			break;

		case 6:
			if(nBitIndex<32){
				nBitValue = (pAlarm->nAlarmTime_sat_0>>nBitIndex)&0x01;
			}else if (nBitIndex<64){
				nBitValue = (pAlarm->nAlarmTime_sat_1>>(nBitIndex-32))&0x01;
			}else if( nBitValue<96){
				nBitValue = (pAlarm->nAlarmTime_sat_2>>(nBitIndex-64))&0x01;
			}
			break;
	}
	bSelected = (nBitValue==1) ? 1:0;
	return bSelected;
}

INT32 CSEP2PAppSDKDemoDlg::set15MinutesFlagAlarmTime(MSG_SET_ALARM_INFO_REQ *pAlarm,INT32 nWeek ,INT32 nBitIndex, INT32 bSel)
{
	INT32 bResult = 0;
	if(nWeek<0||nWeek>7) return bResult;
	if(nBitIndex<0 || nBitIndex>=96) return bResult;
	switch (nWeek){
		case 0:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_sun_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_sun_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_sun_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_sun_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_sun_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_sun_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;

		case 1:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_mon_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_mon_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_mon_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_mon_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_mon_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_mon_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;

		case 2:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_tue_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_tue_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_tue_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_tue_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_tue_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_tue_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;

		case 3:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_wed_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_wed_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_wed_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_wed_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_wed_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_wed_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;

		case 4:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_thu_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_thu_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_thu_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_thu_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_thu_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_thu_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;

		case 5:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_fri_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_fri_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_fri_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_fri_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_fri_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_fri_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;

		case 6:
			if(nBitIndex<32){
				pAlarm->nAlarmTime_sat_0 &=(~(0x01<<nBitIndex));
				if(bSel){
					pAlarm->nAlarmTime_sat_0 |= (0x1<<nBitIndex);
				}
			}else if(nBitIndex>=32 && nBitIndex<64){
				pAlarm->nAlarmTime_sat_1 &=(~(0x01<<(nBitIndex-32)));
				if(bSel){
					pAlarm->nAlarmTime_sat_1 |= (0x1<<(nBitIndex-32));
				}
			}else if(nBitIndex>=64 && nBitIndex<96){
				pAlarm->nAlarmTime_sat_2 &=(~(0x01<<(nBitIndex-64)));
				if(bSel){
					pAlarm->nAlarmTime_sat_2 |= (0x1<<(nBitIndex-64));
				}
			}
			break;
	}
	return bResult;
}
