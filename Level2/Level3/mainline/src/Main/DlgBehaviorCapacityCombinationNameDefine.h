#pragma once
#include "afxwin.h"


// CDlgBehaviorCapacityCombinationNameDefine dialog

class CDlgBehaviorCapacityCombinationNameDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgBehaviorCapacityCombinationNameDefine)

public:
	CDlgBehaviorCapacityCombinationNameDefine(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBehaviorCapacityCombinationNameDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_COMBINATION_NAMEDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CEdit m_Name;
	CString m_StrName ;
public:
	CString GetSelectName( ) { return m_StrName ;} ;
protected:
	void OnOK() ;
};
