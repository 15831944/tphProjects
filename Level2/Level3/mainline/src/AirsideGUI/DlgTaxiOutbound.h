#pragma once

#include "NECRelatedBaseDlg.h"
#include "..\InputAirside\TaxiOutbound.h"

class CTaxiOutbound;
class CAirportDatabase;
class CDlgTaxiOutbound : public CNECRelatedBaseDlg
{
public:
	CDlgTaxiOutbound(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB);
	virtual ~CDlgTaxiOutbound(void);

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
protected:
	CTaxiOutbound* m_pTaxiOutbound;
public:
	virtual BOOL OnInitDialog();
};
