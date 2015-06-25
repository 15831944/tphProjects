#if !defined(AFX_PICKCONVEYORTREE_H__F85293DC_F2C1_4D61_86B0_3F464E94CA78__INCLUDED_)
#define AFX_PICKCONVEYORTREE_H__F85293DC_F2C1_4D61_86B0_3F464E94CA78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PickConveyorTree.h : header file
//
#include "Processor2.h"

/////////////////////////////////////////////////////////////////////////////
// CPickConveyorTree window
#define UM_PCTREE_SELECT_PROC WM_USER+255
class CPickConveyorTree : public CTreeCtrl
{
// Construction
public:
	CPickConveyorTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickConveyorTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nInsertType;
	CWnd* m_pParentWnd;
	CImageList m_ImageList;
	CProcessor2* m_pProc2;
	CArray<ProcessorID*,ProcessorID*> m_arrayProcessorID;
	void Init(CProcessor2* pProc2,int nInsertType);
	BOOL Create(DWORD dwStyle,CWnd* pParentWnd);
	virtual ~CPickConveyorTree();

	bool m_bSendMessage;

	// Generated message map functions
protected:
	//{{AFX_MSG(CPickConveyorTree)
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICKCONVEYORTREE_H__F85293DC_F2C1_4D61_86B0_3F464E94CA78__INCLUDED_)
