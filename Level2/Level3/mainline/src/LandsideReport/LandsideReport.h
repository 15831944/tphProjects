// LandsideReport.h : main header file for the LandsideReport DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CLandsideReportApp
// See LandsideReport.cpp for the implementation of this class
//

class CLandsideReportApp : public CWinApp
{
public:
	CLandsideReportApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
