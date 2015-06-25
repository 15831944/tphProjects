#pragma once
#include "NECRelatedBaseDlg.h"
#include "..\InputAirside\TaxiInbound.h"

class CTaxiInbound;
class CAirportDatabase;
class CDlgTaxiInbound : public CNECRelatedBaseDlg
{
public:
	CDlgTaxiInbound(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaxiInbound();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void ReloadData();
	virtual void SetListColumn();
	virtual void SetListContent();
	virtual void CheckListContent();
	virtual void OnNewItem(FlightConstraint& fltType);
	//virtual void OnNewItem(CString& fltType);
	virtual void OnDelItem();
	//virtual void OnEditItem();
	virtual void OnListItemChanged(int nItem, int nSubItem);
	virtual void OnNECItemChanged(int nItem, int nNewNECTableID);

private:
	CTaxiInbound* m_pTaxiInbound;
public:
	virtual BOOL OnInitDialog();
};
