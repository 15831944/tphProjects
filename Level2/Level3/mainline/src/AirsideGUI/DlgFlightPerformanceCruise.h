#pragma once
#include "necrelatedbasedlg.h"
#include "../InputAirside/FlightPerformanceCruise.h"
#include "../InputAirside/AirsideAircraft.h"

class CAirportDatabase;
class CDlgFlightPerformanceCruise :	public CNECRelatedBaseDlg
{
public:
	CDlgFlightPerformanceCruise(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB);
	~CDlgFlightPerformanceCruise(void);

private:
	ns_AirsideInput::vFlightPerformanceCruise *m_pFlightperformanceCrusie;

protected:
	virtual void ReloadData();
	virtual void SetListColumn();
	virtual void SetListContent();
	virtual void CheckListContent();
	virtual void OnNewItem(FlightConstraint& fltType);
	virtual void OnDelItem();
	//virtual void OnEditItem();
	virtual void OnListItemChanged(int nItem, int nSubItem) ;
	virtual void OnNECItemChanged(int nItem, int nNewNECTableID);

private:
	void LoadItemData(int nItem,ns_AirsideInput::CFlightPerformanceCruise *pTerminal);

public:
	virtual BOOL OnInitDialog();
};
