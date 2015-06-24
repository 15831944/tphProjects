#if !defined(AFX_MOBLIEELEMTIPS_H__C0DFA890_0E92_441D_8167_005D8F0173F5__INCLUDED_)
#define AFX_MOBLIEELEMTIPS_H__C0DFA890_0E92_441D_8167_005D8F0173F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoblieElemTips.h : header file
//

class CMobileElemConstraint;
/////////////////////////////////////////////////////////////////////////////
// CMoblieElemTips window

class CMoblieElemTips : public CToolTipCtrl
{
// Construction
public:
	CMoblieElemTips();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoblieElemTips)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowTips( CString& strTips );
	void InitTooltips( CWnd* pWnd, int iMaxWidth = 300 );
	virtual ~CMoblieElemTips();
private:
	CWnd* m_pWnd;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMoblieElemTips)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOBLIEELEMTIPS_H__C0DFA890_0E92_441D_8167_005D8F0173F5__INCLUDED_)
