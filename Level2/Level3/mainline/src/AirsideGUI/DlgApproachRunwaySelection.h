#pragma once
#include "afxcmn.h"
#include "../InputAirside/ApproachSeparationCirteria.h"
#include "../InputAirside/ALT_BIN.h"

// CDlgApproachRunwaySelection dialog

class CDlgApproachRunwaySelection : public CDialog
{
	DECLARE_DYNAMIC(CDlgApproachRunwaySelection)

public:
	CDlgApproachRunwaySelection(int nProjID,AIRCRAFTOPERATIONTYPE emAppType,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgApproachRunwaySelection();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECT_TWO_RUNWAY };
protected:
	int m_nProjID;
	AIRCRAFTOPERATIONTYPE m_emAppType;

	int m_nFlag;//0x1,allow same runway mark;
public:
	int m_nFirstRunwayID;
	RUNWAY_MARK m_emFirstRunwayMark;
	int m_nSecondRunwayID;
	RUNWAY_MARK m_emSecondRunwayMark;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SetFlag(int nFlag);
	virtual BOOL OnInitDialog();
	CTreeCtrl m_wndFirstTree;
	CTreeCtrl m_wndSecondTree;
	afx_msg void OnBnClickedOk();
};
