// main.h : main header file for the main application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <main/resource.h>       // main symbols


// CmainApp:
// See main.cpp for the implementation of this class
//

class CmainApp : public CWinApp
{
public:
	CmainApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CmainApp theApp;