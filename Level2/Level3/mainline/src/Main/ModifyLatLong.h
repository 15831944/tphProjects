#if !defined(AFX_MODIFYLATLONG_H__0FF6697C_6067_4E2F_9121_E14D5ACF0D51__INCLUDED_)
#define AFX_MODIFYLATLONG_H__0FF6697C_6067_4E2F_9121_E14D5ACF0D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyLatLong.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyLatLong dialog

class CModifyLatLong : public CDialog
{
// Construction
public:
	CModifyLatLong(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyLatLong)
	enum { IDD = IDD_MODIFYLATLONG };
	CEdit	m_edtLong;
	CEdit	m_edtLat;
	CString	m_strLat;
	CString	m_strLong;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyLatLong)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyLatLong)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYLATLONG_H__0FF6697C_6067_4E2F_9121_E14D5ACF0D51__INCLUDED_)
