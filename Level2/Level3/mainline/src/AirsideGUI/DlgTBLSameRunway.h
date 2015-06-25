#pragma once
#include "Resource.h"

// CDlgTBLSameRunway dialog
#include "../InputAirside/AircraftClassifications.h"
#include "../InputAirside/TakeoffClearanceItem.h"

class CDlgTBLSameRunway : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTBLSameRunway)

public:
	CDlgTBLSameRunway(int nProjID, TakeoffClearanceItem* pItem);
	virtual ~CDlgTBLSameRunway();

	int m_nProjID;
	TakeoffBehindLandingSameRunway* m_pTBLSameRunway;

// Dialog Data
	enum { IDD = IDD_DIALOG_TBLSAMERUNWAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void InitEditBox();

	void GetValueBeforSave();

	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();

	int m_nDistApproach;
	int m_nTimeApproach;
	int m_nTimeSlot;
	int m_nFtAfterLander;
};
