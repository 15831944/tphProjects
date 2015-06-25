#pragma once
#include "necrelatedbasedlg.h"
#include "../InputAirside/FlightPerformanceTerminal.h"
#include "../InputAirside/AirsideAircraft.h"

class CAirportDatabase;
class CDlgFlightPerformanceTerminal :public CNECRelatedBaseDlg
{
public:
	CDlgFlightPerformanceTerminal(int nProjID,
		PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB);
	~CDlgFlightPerformanceTerminal(void);

private:
	ns_AirsideInput::vFlightPerformanceTerminal *m_pFlightPerformanceTerminal;
	CAirsideAircraftCat *m_pAircraftCat;
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
	void LoadItemData(int nItem,ns_AirsideInput::CFlightPerformanceTerminal *pTerminal);

public:
	virtual BOOL OnInitDialog();
};
