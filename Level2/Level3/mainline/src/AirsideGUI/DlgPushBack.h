#pragma once

#include "NECRelatedBaseDlg.h"
#include "..\InputAirside\PushBack.h"

class CPushBack;
class CAirportDatabase;
class CDlgPushBack : public CNECRelatedBaseDlg
{
public:
	CDlgPushBack(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry);
	virtual ~CDlgPushBack(void);

protected:
	virtual void ReloadData();
	virtual void SetListColumn();
	virtual void SetListContent();
	virtual void CheckListContent();
	virtual void OnNewItem(FlightConstraint& fltType);
	virtual void OnDelItem();
	virtual void OnListItemChanged(int nItem, int nSubItem);
	virtual void OnNECItemChanged(int nItem, int nNewNECTableID);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdEditItem();
	afx_msg void OnBnClickedButtonSave();
	virtual void OnOK();
	afx_msg void OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg LRESULT OnDbClickListItem( WPARAM wparam, LPARAM lparam);
protected:
	CPushBack* m_pPushBack;
	int m_nProjID;
	 
	int m_nRowSel;
	int m_nColumnSel;
	CAirportDatabase* m_pAirportDB;
	InputAirside* m_pInputAirside;
	PSelectProbDistEntry m_pSelectProbDistEntry;
public:
	virtual BOOL OnInitDialog();
};
