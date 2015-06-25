#pragma once
#include "NECRelatedBaseDlg.h"
#include "..\InputAirside\PerformanceTakeoffs.h"
class CAirportDatabase;

class CDlgPerformanceTakeoff : public CNECRelatedBaseDlg
{
public:
	CDlgPerformanceTakeoff(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPerformanceTakeoff();
protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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
	PerformTakeOffs *m_pCompsiteACTakeoffs;
public:
	virtual BOOL OnInitDialog();
};
