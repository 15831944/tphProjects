#pragma once
#include "afxwin.h"
#include "../InputAirside\ApproachSeparationType.h"

// CDlgSelecteAircraftClassification dialog

class CDlgSelecteAircraftClassification : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelecteAircraftClassification)

public:
	CDlgSelecteAircraftClassification(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelecteAircraftClassification();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRSIDEREPORT_SELECTACCLASSIFICATION };

public:
	FlightClassificationBasisType m_selectType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lbAcClass;
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
