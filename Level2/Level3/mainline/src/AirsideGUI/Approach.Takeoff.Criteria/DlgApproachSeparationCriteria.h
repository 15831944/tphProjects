#pragma once
#include "DlgApproachSeparationItems.h"
#include "../../MFCExControl/TabCtrlSSL.h"
#include "../../InputAirside/ApproachSeparationCirteria.h"
#include "RunwayImageStatic.h"
#include "afxcmn.h"
#include "../../InputAirside/ALTObject.h"
// CDlgApproachSeparationCriteria dialog

class CDlgApproachSeparationCriteria : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgApproachSeparationCriteria)

public:
	CDlgApproachSeparationCriteria(int nProjID,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgApproachSeparationCriteria();

protected:
	void UpdateUIState();
public:
// Dialog Data
	enum { IDD = IDD_DIALOG_APPROCHASEPARATION_CRITERIA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButton1();
	CRunwayImageStatic m_wndRunwayImage;
	CString m_strNear;
	CString m_strFar;
	CString m_strConvergentDegree;
	CString m_strDivergentDegree;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit10();
	afx_msg void OnEnChangeEdit11();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	CSpinButtonCtrl m_wndConvergentSpin;
	CSpinButtonCtrl m_wndDivergentSpin;
	CSpinButtonCtrl m_wndNearSpin;
	CSpinButtonCtrl m_wndFarSpin;
	CTabCtrlSSL		m_wndTabCtrl;

protected:
	int m_nProjID;
	int m_nAirportID;
	ALTObjectList m_vRunways;
	CApproachSeparationCriteria* m_pAppSepCriteria;
	CDlgApproachSeparationItems m_wndTabLandingBehindLanded;
	CDlgApproachSeparationItems m_wndTabLandingBehindTakeoff;		
	bool m_bInitialize;
};
