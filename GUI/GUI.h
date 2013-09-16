// GUI.h : main header file for the GUI application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CGUIApp:
// See GUI.cpp for the implementation of this class
//

class CGUIApp : public CWinApp
{
public:
	CGUIApp();
private:

	HINSTANCE m_hMdl;
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CGUIApp theApp;