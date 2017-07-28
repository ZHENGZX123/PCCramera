// SEP2PAppSDKDemo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSEP2PAppSDKDemoApp:
// See SEP2PAppSDKDemo.cpp for the implementation of this class
//

class CSEP2PAppSDKDemoApp : public CWinApp
{
public:
	CSEP2PAppSDKDemoApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	static void GetAppPath(CString &strAppPath);

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSEP2PAppSDKDemoApp theApp;