#if !defined(AFX_ALLPROCESSORTREECTRL_H__ECF58874_170D_46B5_97C9_0AAB15F5CFE1__INCLUDED_)
#define AFX_ALLPROCESSORTREECTRL_H__ECF58874_170D_46B5_97C9_0AAB15F5CFE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllProcessorTreeCtrl.h : header file
//

#include "ProcessorTreeCtrl.h"
class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTreeCtrl window

class CAllProcessorTreeCtrl : public CProcessorTreeCtrl
{
// Construction
public:
	CAllProcessorTreeCtrl();

// Attributes
public:

// Operations
public:
	 bool IsSelected( CString strProcess, ProcessorDatabase* _pProcDB );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllProcessorTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void LoadData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB, int _nProcType =-1 );
	virtual ~CAllProcessorTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAllProcessorTreeCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLPROCESSORTREECTRL_H__ECF58874_170D_46B5_97C9_0AAB15F5CFE1__INCLUDED_)
