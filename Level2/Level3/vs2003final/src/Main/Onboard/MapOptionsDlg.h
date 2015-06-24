#pragma once

#include <common\CADInfo.h>

// CMapOptionsDlg dialog

class CMapOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CMapOptionsDlg)

public:
	CMapOptionsDlg(const CCADFileDisplayInfo& displayInfo, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMapOptionsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAPOPTIONS };

	double	m_dRotation;
	double	m_dXOffset;
	double	m_dYOffset;
	CString m_sUnits;
	CCADFileDisplayInfo m_cadInfos;	

	CButton	m_cBtnMapFileName;
	CComboBox m_cmbSourceUnits;
	BOOL m_bMapChanged;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLayeroptions();
	afx_msg void OnBnClickedMapfilename();
	afx_msg void OnBnClickedButtonRemovemap();
	afx_msg void OnBnClickedOk();
};
