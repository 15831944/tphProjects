#pragma once

#include "NECRelatedBaseDlg.h"
#include "../InputAirside/ApproachLand.h"


class CAirportDatabase;
class CDlgApproachToLand : public CNECRelatedBaseDlg
{
public:
	CDlgApproachToLand(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgApproachToLand();

// Dialog Data

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void ReloadData();
	virtual void SetListColumn();
	virtual void SetListContent();
	virtual void CheckListContent();
	virtual void OnNewItem(FlightConstraint& fltType);
	virtual void OnDelItem();
	//virtual void OnEditItem();
	virtual void OnListItemChanged(int nItem, int nSubItem);
	virtual void OnNECItemChanged(int nItem, int nNewNECTableID);

private:
	ApproachLands * m_pApproachLands;

public:
	virtual BOOL OnInitDialog();
};
