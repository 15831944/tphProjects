#pragma once

// CDlgTBLIntersect dialog

#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/AircraftClassifications.h"
#include "../InputAirside/TakeoffClearanceItem.h"


class CDlgTBLIntersect : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTBLIntersect)

public:
	CDlgTBLIntersect(int nProjID, TakeoffClearanceItem* pItem);
	virtual ~CDlgTBLIntersect();

	int m_nProjID;

	CStringArray m_strClasifiBasisType;

	TakeoffBehindLandingIntersectingConvergingRunway* m_pTBLIntersect;

	AircraftClassifications* m_pClsNone;
	AircraftClassifications* m_pClsWakeVortexWightBased;
	AircraftClassifications* m_pClsWingspanBased;
	AircraftClassifications* m_pClsSurfaceBearingWeightBased;
	AircraftClassifications* m_pClsApproachSpeedBased;
	AircraftClassifications* GetAircraftClsByType(FlightClassificationBasisType nType);



// Dialog Data
	enum { IDD = IDD_DIALOG_TBLINTERSECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void InitListCtrl();
	void InitComboBox();

	void ResetListCtrl();
	void InitEditBox();

	void GetValueBeforSave();

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();


	int m_nDistApproach;
	int m_nTimeApproach;
	int m_nTimeSlot;
	CListCtrlEx m_wndListBasis;
	CComboBox m_wndCmbBasis;

	afx_msg void OnCbnSelchangeComboBasis();
	afx_msg void OnLvnEndlabeleditListBasis(NMHDR *pNMHDR, LRESULT *pResult);
};
