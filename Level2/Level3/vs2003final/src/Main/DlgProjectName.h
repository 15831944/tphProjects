#if !defined(AFX_DLGPROJECTNAME_H__9C5A6529_0D3C_4B2C_9E28_381F1814E173__INCLUDED_)
#define AFX_DLGPROJECTNAME_H__9C5A6529_0D3C_4B2C_9E28_381F1814E173__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProjectName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgProjectName dialog

class CDlgProjectName : public CDialog
{
// Construction
public:
	CDlgProjectName( const CString _csInitName, bool _bCanSelectDB = true, long _lInitDB = 0l, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProjectName)
	enum { IDD = IDD_DIALOG_PROJECTNAME };
	CComboBox	m_comboDB;
	CEdit	m_editName;
	CString	m_strProjectName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProjectName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProjectName)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEditchangeComboDb();
	afx_msg void OnChangeProjectname();
	afx_msg void OnSelchangeComboDb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	long	m_lSelectDB;
	CString m_sSelectedDBName;
	
private:
	bool	m_bCanSelectDB;

private:
	// init global db list
	void InitDBList( void );
	
	// set cursor select item by db index
	void setInitSelectItem( void );
	void EnableChildControl();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROJECTNAME_H__9C5A6529_0D3C_4B2C_9E28_381F1814E173__INCLUDED_)
