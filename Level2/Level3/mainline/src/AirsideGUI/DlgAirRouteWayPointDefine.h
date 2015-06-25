#pragma once


// CDlgAirRouteWayPointDefine dialog
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/AirWayPoint.h"
#include "MFCExControl/FloarEdit.h"

class ARWaypoint;
class CDlgAirRouteWayPointDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgAirRouteWayPointDefine)

public:
	CDlgAirRouteWayPointDefine(ARWaypoint* pARWayPoint,
		std::vector<AirWayPoint>& vWaypoint, bool bStar,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirRouteWayPointDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_ARWAYPOINTDEFINE };

public:
	std::vector<AirWayPoint> m_vWaypoint;
	ARWaypoint* m_pARWayPoint;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbWayPoint;
	CFloarEdit m_edtMinSpeed;
	CFloarEdit m_edtMaxSpeed;
	CFloarEdit m_edtMinHeight;
	CFloarEdit m_edtMaxHeight;
	CComboBox m_cmbHeadingChoice;
	CEdit m_edtDegrees;
	CFloarEdit m_edtVisDistance;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnDeltaposSpinMinspeed(NMHDR *pNMHDR, LRESULT *pResult);
	double m_lMinSpeed;
	double m_lMaxSpeed;
	double m_lMinHeight;
	double m_lMaxHeight;
	long m_lDegrees;
	double m_lVisDistance;
	bool m_bSTAR;
	afx_msg void OnDeltaposSpinMaxspeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMinheight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMaxheight(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedRadioMinspeeddefine();
	afx_msg void OnBnClickedRadioMinspeednolimit();
	afx_msg void OnBnClickedRadioMaxspeednolimit();
	afx_msg void OnBnClickedRadioMaxspeeddefine();
	afx_msg void OnBnClickedRadioMinheightdefine();
	afx_msg void OnBnClickedRadioMaxheightdefine();
	afx_msg void OnBnClickedRadioMinheightnolimit();
	afx_msg void OnBnClickedRadioMaxheightnolimit();
	afx_msg void OnCbnSelchangeComboWaypoint();
	
	CSpinButtonCtrl m_spinDegrees;
	afx_msg void OnBnClickedRadioheading();
	afx_msg void OnBnClickedRadionextwaypoint();

};
