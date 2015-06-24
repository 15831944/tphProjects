#if !defined(AFX_USEDPROCINFOBOX_H__6BAD6ED6_9313_45D7_B45A_0821F452A8D7__INCLUDED_)
#define AFX_USEDPROCINFOBOX_H__6BAD6ED6_9313_45D7_B45A_0821F452A8D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UsedProcInfoBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUsedProcInfoBox dialog
#include "UsedProcInfo.h"

class CUsedProcInfoBox : public CDialog
{
// Construction
public:
	CUsedProcInfoBox(CWnd* pParent = NULL);   // standard constructor
    
// Dialog Data
	//{{AFX_DATA(CUsedProcInfoBox)
	enum { IDD = IDD_DIALOG_USEDPROCINFO };
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUsedProcInfoBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	public:
		std::vector<CUsedProcInfo> m_vInfoList;
		virtual BOOL OnInitDialog();
		void AddInfo(CUsedProcInfo info)
		{
			m_vInfoList.push_back(info);
		}
		bool IsNoInfo(void);
protected:

	// Generated message map functions
	//{{AFX_MSG(CUsedProcInfoBox)
	afx_msg void OnYes();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USEDPROCINFOBOX_H__6BAD6ED6_9313_45D7_B45A_0821F452A8D7__INCLUDED_)
