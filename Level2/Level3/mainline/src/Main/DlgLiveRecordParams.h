#if !defined(AFX_DLGLIVERECORDPARAMS_H__FA7BEDE1_2098_42A8_A425_7230959B3BB4__INCLUDED_)
#define AFX_DLGLIVERECORDPARAMS_H__FA7BEDE1_2098_42A8_A425_7230959B3BB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLiveRecordParams.h : header file
//
#include "..\MFCExControl\CoolTree.h"


class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CDlgLiveRecordParams dialog

class CDlgLiveRecordParams : public CDialog
{
// Construction
public:
	CDlgLiveRecordParams(CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLiveRecordParams)
	enum { IDD = IDD_DIALOG_RECORDOPTIONS2 };
	CCoolTree	m_treeParams;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLiveRecordParams)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTermPlanDoc* m_pDoc;
	HTREEITEM m_hFPS;
	HTREEITEM m_hChooseFile;
	HTREEITEM m_hQuality;
	HTREEITEM m_hWidth;
	HTREEITEM m_hHeight;


	void InitTree();

	// Generated message map functions
	//{{AFX_MSG(CDlgLiveRecordParams)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLIVERECORDPARAMS_H__FA7BEDE1_2098_42A8_A425_7230959B3BB4__INCLUDED_)
