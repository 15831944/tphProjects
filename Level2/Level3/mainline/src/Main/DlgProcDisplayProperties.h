#if !defined(AFX_DLGPROCDISPLAYPROPERTIES_H__E157BEBD_547C_4B51_924C_E1BEF5029EC6__INCLUDED_)
#define AFX_DLGPROCDISPLAYPROPERTIES_H__E157BEBD_547C_4B51_924C_E1BEF5029EC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProcDisplayProperties.h : header file
//

#include "ColorBox.h"
#include "XPStyle/CJColorPicker.h"

class CProcessor2;
/////////////////////////////////////////////////////////////////////////////
// CDlgProcDisplayProperties dialog

class CDlgProcDisplayProperties : public CDialog
{
// Construction
public:
	CDlgProcDisplayProperties(CPROCESSOR2LIST* pProc2List, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProcDisplayProperties)
	enum { IDD = IDD_PROCDISPPROPERTIES };
	CButton m_chkDisplay[6];
	int		m_nDisplay[6];
	CCJColorPicker m_btnColor[6];
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	COLORREF	m_cColor[6];
	BOOL m_bMultiColor[6];
	BOOL m_bApplyDisplay[6];
	BOOL m_bApplyColor[6];

	CPROCESSOR2LIST* m_pProc2List;

	BOOL MakeChkTriState(UINT nID, BOOL bTriState);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcDisplayProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProcDisplayProperties)
	afx_msg void OnChangeColor(UINT nID);
	afx_msg void OnChkDisplay(UINT nID);
	//}}AFX_MSG
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
	afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
	afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
	afx_msg LONG OnCloseUp(UINT lParam, LONG wParam);
	afx_msg LONG OnDropDown(UINT lParam, LONG wParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROCDISPLAYPROPERTIES_H__E157BEBD_547C_4B51_924C_E1BEF5029EC6__INCLUDED_)
