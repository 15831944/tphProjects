#pragma once
#include "NECRelatedBaseDlg.h"
#include "..\InputAirside\StandService.h"

class StandServiceList;
class CAirportDatabase;
class CDlgStandService: public CNECRelatedBaseDlg
{
public:
	CDlgStandService(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry);
	~CDlgStandService(void);

protected:
	void ReloadData();
	void SetListColumn();
	void SetListContent();
	void CheckListContent();
	void OnNewItem(FlightConstraint& fltType);
	void OnDelItem();
	void OnListItemChanged(int nItem, int nSubItem);
	void OnNECItemChanged(int nItem, int nNewNECTableID);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdEditItem();
	afx_msg void OnBnClickedButtonSave();
	void OnOK();
	afx_msg void OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg LRESULT OnDbClickListItem( WPARAM wparam, LPARAM lparam);
protected:
	StandServiceList* m_pStandServiceList;
	int m_nProjID;

	int m_nRowSel;
	int m_nColumnSel;
	CAirportDatabase* m_pAirportDB;
	InputAirside* m_pInputAirside;
	PSelectProbDistEntry m_pSelectProbDistEntry;
public:
	virtual BOOL OnInitDialog();
};
