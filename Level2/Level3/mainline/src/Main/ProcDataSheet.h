#if !defined(AFX_PROCDATASHEET_H__9ECBE747_7CD7_4C9B_8440_935199D79E79__INCLUDED_)
#define AFX_PROCDATASHEET_H__9ECBE747_7CD7_4C9B_8440_935199D79E79__INCLUDED_

#include "ProcDataPage.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcDataSheet.h : header file
//

enum RESULT_CODE { ENUM_TO_NONE, ENUM_TO_SAVE, ENUM_TO_CANCEL };
/////////////////////////////////////////////////////////////////////////////
// CProcDataSheet
#include "ResizePS\ResizableSheet.h"

class CProcDataSheet : public CResizableSheet
{
	DECLARE_DYNAMIC(CProcDataSheet)

// Construction
public:
	CProcDataSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage = 0);
	CProcDataSheet(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage=0);
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcDataSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	CWnd* m_pParent;
	CButton m_btnLeft;
	virtual ~CProcDataSheet();

	// Generated message map functions
protected:
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	// modeify by bird 2003/5/20, to add moving side walk page
	//CProcDataPage* m_pProcDataPage[PROC_TYPES-3];
	//CProcDataPage* m_pProcDataPage[PROC_TYPES-2];	
	// Modified by Tim In 2/6/2004 For Adding Station
	CProcDataPage* m_pProcDataPage[PROC_TYPES-1];	
	//{{AFX_MSG(CProcDataSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnBtnProcDataSheetLeft();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCDATASHEET_H__9ECBE747_7CD7_4C9B_8440_935199D79E79__INCLUDED_)
