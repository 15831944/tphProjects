#if !defined(AFX_DLGAREAPORTAL_H__EFB8D484_78B5_4C3C_8341_F6720FB124D3__INCLUDED_)
#define AFX_DLGAREAPORTAL_H__EFB8D484_78B5_4C3C_8341_F6720FB124D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAreaPortal.h : header file
//
#include "..\inputs\AreasPortals.h" 
#include "..\Landside\LandsidePortals.h"
#include "..\Landside\LandsideArea.h"
#include "TermPlanDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAreaPortal dialog
#define UM_DLGAREAPORTAL_SHOW_OR_HIDE WM_USER+512
class CDlgAreaPortal : public CDialog
{
// Construction
public:
	CTermPlanDoc* m_pDoc;
	CWnd* m_pOwner;
	int DoFakeModal();
	enum _areaportal {
		area,
		portal
	};

	CDlgAreaPortal(CTermPlanDoc* pDoc,_areaportal type, CNamedPointList* pNamedList, CWnd* pParent = NULL,CWnd* pOwner=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAreaPortal)
	enum { IDD = IDD_AREAPORTALDEF };
	CString	m_sCoords;
	CString	m_sName;
	CString	m_sNameTag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAreaPortal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual int DoModal(); // do not allow this to be called by others

	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);

	CNamedPointList* m_pNamedList;
	CWnd* m_pParent;
	_areaportal m_eType;
	std::vector<ARCPoint2> m_Points;

	// Generated message map functions
	//{{AFX_MSG(CDlgAreaPortal)
	afx_msg void OnPfmCoords();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditName();
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAREAPORTAL_H__EFB8D484_78B5_4C3C_8341_F6720FB124D3__INCLUDED_)
