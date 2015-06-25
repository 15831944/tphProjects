#pragma once

// CDlgTBTSameRunway dialog

#include "../MFCExControl/ListCtrlEx.h"
//#include "../InputAirside/TakeoffClearances.h"
#include "../InputAirside/AircraftClassifications.h"
#include "../InputAirside/TakeoffClearanceItem.h"

#include "afxwin.h"
#include "afxcmn.h"


class CDlgTBTSameRunway : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTBTSameRunway)

public:
	CDlgTBTSameRunway(int nProjID, TakeoffClearanceItem* pItem);
	virtual ~CDlgTBTSameRunway();

	int m_nProjID;


// Dialog Data
	enum { IDD = IDD_DIALOG_TBTSAMERUNWAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nTimeAfter;
	int m_nFtDownRunway;
	int m_nDistApproach;
	int m_nTimeApproach;
	int m_nTimeSlot;
	CComboBox m_wndCmbBasis;
	CListCtrlEx m_wndListBasis;

	CStringArray m_strClasifiBasisType;

	TakeoffBehindTakeoffSameRunwayOrCloseParalled* m_pTBTSameRunway;

	AircraftClassifications* m_pClsNone;
	AircraftClassifications* m_pClsWakeVortexWightBased;
	AircraftClassifications* m_pClsWingspanBased;
	AircraftClassifications* m_pClsSurfaceBearingWeightBased;
	AircraftClassifications* m_pClsApproachSpeedBased;
	AircraftClassifications* GetAircraftClsByType(FlightClassificationBasisType nType);


	void InitListCtrl();
	void InitComboBox();

	void ResetListCtrl();
	void InitEditBox();

	void GetValueBeforSave();

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();


	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboBasis();
	afx_msg void OnLvnEndlabeleditListBasis(NMHDR *pNMHDR, LRESULT *pResult);
};
