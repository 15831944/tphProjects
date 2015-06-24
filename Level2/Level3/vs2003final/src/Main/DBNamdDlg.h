#if !defined(AFX_DBNAMDDLG_H__37874452_06B3_48D8_A38F_9AA92EC89C04__INCLUDED_)
#define AFX_DBNAMDDLG_H__37874452_06B3_48D8_A38F_9AA92EC89C04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBNamdDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBNamdDlg dialog

class CDBNamdDlg : public CDialog
{
// Construction
public:
	CDBNamdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDBNamdDlg)
	enum { IDD = IDD_DIALOG_DBNAME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBNamdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDBNamdDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBNAMDDLG_H__37874452_06B3_48D8_A38F_9AA92EC89C04__INCLUDED_)
