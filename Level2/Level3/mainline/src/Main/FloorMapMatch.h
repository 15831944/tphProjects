#if !defined(AFX_FLOORMAPMATCH_H__07332B04_065A_4F05_9F95_19FDC0131745__INCLUDED_)
#define AFX_FLOORMAPMATCH_H__07332B04_065A_4F05_9F95_19FDC0131745__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FloorMapMatch.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CFloorMapMatch dialog

class CFloorMapMatch : public CDialog
{
// Construction
public:
	CFloorMapMatch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFloorMapMatch)
	enum { IDD = IDD_DIALOG_FloorMapMatch };
	CString	m_edit;
	CString m_path;
	CString m_name;
	CString	m_static;
	CString m_filetype;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFloorMapMatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFloorMapMatch)
	afx_msg void OnCancelMode();
	afx_msg void OnBrowser();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOORMAPMATCH_H__07332B04_065A_4F05_9F95_19FDC0131745__INCLUDED_)
