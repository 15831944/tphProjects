#pragma once

#include "NECRelatedBaseDlg.h"
#include "..\InputAirside\DepClimbOut.h"
// CDlgDepClimbOut dialog
class CAirportDatabase;
class CDlgDepClimbOut : public CNECRelatedBaseDlg
{
	DECLARE_DYNAMIC(CDlgDepClimbOut)

public:
	CDlgDepClimbOut(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDepClimbOut();


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
	DepClimbOuts* m_pDepClimbOuts;
public:
	virtual BOOL OnInitDialog();
};
