// DeviceInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "SEP2PAppSDKDemo.h"
#include "DeviceInfo.h"
#include "SEP2PAppSDKDemoDlg.h"

// CDeviceInfo 对话框

IMPLEMENT_DYNAMIC(CDeviceInfo, CDialog)

CDeviceInfo::CDeviceInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceInfo::IDD, pParent)
{
	m_nChanNo=-1;
}

CDeviceInfo::~CDeviceInfo()
{
}

void CDeviceInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDeviceInfo, CDialog)
	ON_BN_CLICKED(IDC_OK, &CDeviceInfo::OnBnClickedOk)
END_MESSAGE_MAP()


// CDeviceInfo 消息处理程序
BOOL CDeviceInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString csTitle;
	csTitle.Format(_T("Edit Device Information(#%d)"), m_nChanNo);
	SetWindowText(csTitle);

	SetDlgItemText(IDC_EDIT1, m_csDID);
	SetDlgItemText(IDC_EDIT2, m_csUsername);
	SetDlgItemText(IDC_EDIT3, m_csPassword);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDeviceInfo::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT1, m_csDID);
	if(m_csDID.IsEmpty()){
		MessageBox(_T("Please enter DID"), _T("Tips"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	GetDlgItemText(IDC_EDIT2, m_csUsername);
	if(m_csUsername.IsEmpty()){
		MessageBox(_T("Please enter Username"), _T("Tips"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	GetDlgItemText(IDC_EDIT3, m_csPassword);

// 	CHAR chDID[128]={0};
// 	CharFromWSTRU(CP_OEMCP, m_csDID, m_csDID.GetLength(), chDID);
// 	if(m_pSampleDlg && m_pSampleDlg->IsExistThisDID(chDID)){
// 		MessageBox(_T("This DID has already existed."), _T("Tips"), MB_OK|MB_ICONINFORMATION);
// 		return;
// 	}
	CDialog::OnOK();
}