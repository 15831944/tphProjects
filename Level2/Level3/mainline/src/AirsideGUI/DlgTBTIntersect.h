#pragma once

// CDlgTBTIntersect dialog

#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/AircraftClassifications.h"
#include "../InputAirside/TakeoffClearanceItem.h"
#include "afxwin.h"
#include "afxcmn.h"

class CDlgTBTIntersect : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTBTIntersect)

public:
	CDlgTBTIntersect(int nProjID, TakeoffClearanceItem* pItem);
	virtual ~CDlgTBTIntersect();

	int m_nProjID;

	CStringArray m_strClasifiBasisType;

	TakeoffBehindTakeoffIntersectingConvergngRunway* m_pTBTIntersect;

	AircraftClassifications* m_pClsNone;
	AircraftClassifications* m_pClsWakeVortexWightBased;
	AircraftClassifications* m_pClsWingspanBased;
	AircraftClassifications* m_pClsSurfaceBearingWeightBased;
	AircraftClassifications* m_pClsApproachSpeedBased;
	AircraftClassifications* GetAircraftClsByType(FlightClassificationBasisType nType);


// Dialog Data
	enum { IDD = IDD_DIALOG_TBTINTERSECT };

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
	int m_nTimeApproach;
	int m_nDistApproach;
	int m_nTimeSlot;
	CComboBox m_wndCmbBasis;
	CListCtrlEx m_wndListBasis;
	afx_msg void OnCbnSelchangeComboBasis();
	afx_msg void OnLvnEndlabeleditListBasis(NMHDR *pNMHDR, LRESULT *pResult);
};
