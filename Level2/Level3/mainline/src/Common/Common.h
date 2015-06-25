// Common.h : main header file for the COMMON DLL
//

#if !defined(AFX_COMMON_H__E7B3EA55_B662_4056_801D_B53A103E8E4A__INCLUDED_)
#define AFX_COMMON_H__E7B3EA55_B662_4056_801D_B53A103E8E4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCommonApp
// See Common.cpp for the implementation of this class
//

class CCommonApp : public CWinApp
{
public:
	CCommonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCommonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMON_H__E7B3EA55_B662_4056_801D_B53A103E8E4A__INCLUDED_)
