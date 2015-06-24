#if !defined(AFX_PRINTABLELISTCTRL_H__EE20FA6E_EBA1_4F7A_9A4F_EFB5B684842C__INCLUDED_)
#define AFX_PRINTABLELISTCTRL_H__EE20FA6E_EBA1_4F7A_9A4F_EFB5B684842C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintableListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintableListCtrl window

class CPrintableListCtrl : public CListCtrl
{
// Construction
public:
	CPrintableListCtrl();

// Attributes
public:

// Operations
public:
void PrintList( CString _sHeader, CString _sFoot );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintableListCtrl)
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrintableListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrintableListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CString m_sHeader;
	CString m_sFoot;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTABLELISTCTRL_H__EE20FA6E_EBA1_4F7A_9A4F_EFB5B684842C__INCLUDED_)
