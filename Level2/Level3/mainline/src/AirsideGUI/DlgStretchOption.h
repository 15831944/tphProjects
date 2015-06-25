#pragma once

// CDlgStretchOption dialog
#include "..\InputAirside\stretchsetoption.h"
/*struct DriveInt 
{
	CString  str;
	int i;
};*/
class CDlgStretchOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgStretchOption)

public:
	CDlgStretchOption( CWnd* pParent = NULL);   // standard constructor
	CDlgStretchOption(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgStretchOption();

// Dialog Data
	//enum { IDD = IDD_DIALOG_STRETCHOPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:

public:
	CStretchSetOption m_stretchSetOption;
	int m_nProjID;
	int m_strDrive;
	double m_dLaneWidth;
	int m_iLaneNumber;
	
	//	afx_msg void OnInitMenu(CMenu* pMenu);
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
