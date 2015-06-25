#pragma once
#include "Landside/LandsideLayoutOptions.h"

// CDlgLandsideLayoutOptions dialog

class CDlgLandsideLayoutOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgLandsideLayoutOptions)

public:
	CDlgLandsideLayoutOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideLayoutOptions();

// Dialog Data
	enum { IDD = IDD_DIALOG_LAYOUTOPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void OnOK();

	DECLARE_MESSAGE_MAP()

	void InitCombox();

private:
	CComboBox m_wndComboxCtrl;

	LandsideLayoutOptions m_layoutOptions;
public:
	afx_msg void OnCbnSelchangeComboData();
};
