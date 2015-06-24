#pragma once
#include "afxwin.h"


// CDlgModifyThickness dialog

class CDlgModifyThickness : public CDialog
{
	DECLARE_DYNAMIC(CDlgModifyThickness)

public:
	CDlgModifyThickness(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgModifyThickness();

// Dialog Data
	enum { IDD = IDD_DIALOG_MODIFYTHICKNESS };

	double getThickness()const;
	void setThickness(double dthick);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editThick;
	double m_dThick;
	CStatic m_staticUnit;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
