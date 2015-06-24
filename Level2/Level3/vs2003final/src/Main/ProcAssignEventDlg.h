#if !defined(AFX_PROCASSIGNEVENTDLG_H__2BDA7321_A493_42B2_9617_82E01E8AC86A__INCLUDED_)
#define AFX_PROCASSIGNEVENTDLG_H__2BDA7321_A493_42B2_9617_82E01E8AC86A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcAssignEventDlg.h : header file
//

#include <vector>
//#include "inputs\pax_cnst.h"
#include "inputs\MultiMobConstraint.h"

/////////////////////////////////////////////////////////////////////////////
// CProcAssignEventDlg dialog

class CProcAssignEventDlg : public CDialog
{
// Construction
public:
	CMultiMobConstraint& GetPaxConstr();
	bool IsOpen();
	COleDateTime& GetTime();
	void SetPaxConstr( const CMultiMobConstraint& _paxConstr );
	void SetOpen( bool _bOpen );
	void SetTime( const COleDateTime& _oTime );
	CProcAssignEventDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcAssignEventDlg)
	enum { IDD = IDD_DIALOG_ASSIGNMENTEVENT };
	CButton	m_btnAdd;
	CButton	m_btnDel;
	CListBox	m_listBoxPaxConstr;
	COleDateTime	m_oTime;
	int		m_nStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcAssignEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadList();
	CMultiMobConstraint m_multiPaxConstr;
	std::vector<CString> m_vectRawString;

	// Generated message map functions
	//{{AFX_MSG(CProcAssignEventDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnSelchangeListPaxconstr();
	afx_msg void OnRadioOpen();
	afx_msg void OnRadioClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCASSIGNEVENTDLG_H__2BDA7321_A493_42B2_9617_82E01E8AC86A__INCLUDED_)
