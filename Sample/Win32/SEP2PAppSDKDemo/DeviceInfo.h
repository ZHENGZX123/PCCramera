#pragma once


// CDeviceInfo �Ի���
class CSEP2PAppSDKDemoDlg;
class CDeviceInfo : public CDialog
{
	DECLARE_DYNAMIC(CDeviceInfo)

public:
	CDeviceInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDeviceInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };
	
	int m_nChanNo;

	CString m_csDID;
	CString m_csUsername, m_csPassword;
	CSEP2PAppSDKDemoDlg *m_pSampleDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
