#if !defined(AFX_SMARTEDIT_H__61FB3A72_2E94_467E_95FC_23A31EDDF21F__INCLUDED_)
#define AFX_SMARTEDIT_H__61FB3A72_2E94_467E_95FC_23A31EDDF21F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MFCExControlAPI.h"
// SmartEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSmartEdit window

class MFCEXCONTROL_API CSmartEdit : public CEdit
{
// Construction
public:
	CSmartEdit( CString _csStr );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmartEdit();

	// Generated message map functions
protected:
	CString m_csStr;
	//{{AFX_MSG(CSmartEdit)
	afx_msg void OnKillfocus();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTEDIT_H__61FB3A72_2E94_467E_95FC_23A31EDDF21F__INCLUDED_)
