#include "StdAfx.h"
#include "resource.h"
#include ".\dlgflightperformancecruise.h"
#include "..\Common\AirportDatabase.h"

using namespace ns_AirsideInput;

CDlgFlightPerformanceCruise::CDlgFlightPerformanceCruise(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB)
 :CNECRelatedBaseDlg(pSelectFlightType, "Cruise Performance Specification")
{
	m_pFlightperformanceCrusie = new vFlightPerformanceCruise(pAirportDB);
	m_pFlightperformanceCrusie->ReadData(nProjID);
}

CDlgFlightPerformanceCruise::~CDlgFlightPerformanceCruise(void)
{
}
void CDlgFlightPerformanceCruise::ReloadData()
{

}
void CDlgFlightPerformanceCruise::SetListColumn()
{
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	CStringList strList;
	////Composite Index
	//lvc.csList = &strList;
	//lvc.pszText ="Composite index";
	//lvc.cx = 80;
	//lvc.fmt = LVCFMT_DROPDOWN;
	//m_wndListCtrl.InsertColumn( 0, &lvc );

	//Fuel Burn
	lvc.csList = &strList;
	lvc.pszText ="Fuel Burn(avg)(lbs/hr)";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 1, &lvc );
	
	//min speed
	lvc.csList = &strList;
	lvc.pszText ="Min Speed(kts)";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 2, &lvc );

	//max speed
	lvc.csList = &strList;
	lvc.pszText ="Max Speed(kts)";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 3, &lvc );

	//max Acceleration
	lvc.csList = &strList;
	lvc.pszText ="Max Acceleration(ft/sec2)";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 4, &lvc );

	//max deceleration
	lvc.csList = &strList;
	lvc.pszText ="Max deceleration(ft/sec2)";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 5, &lvc );

	//max deceleration
	lvc.csList = &strList;
	lvc.pszText ="Cost($/hr)";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 6, &lvc );
}
void CDlgFlightPerformanceCruise::SetListContent()
{
	int nCount = static_cast<int>(m_pFlightperformanceCrusie->size());
	for (int i = 0; i< nCount; i++)
	{
		CFlightPerformanceCruise *pCruise =(*m_pFlightperformanceCrusie)[i];
		
		char szFltType[1024] = {0};
		pCruise->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(i, szFltType);

		LoadItemData(i, pCruise);
	}
}
void CDlgFlightPerformanceCruise::CheckListContent()
{
	int nCount = static_cast<int>(m_pFlightperformanceCrusie->size());
	if(nCount == 0)
	{
		CFlightPerformanceCruise *pCruise = new CFlightPerformanceCruise();
		pCruise->setProjID(m_pFlightperformanceCrusie->getProjID());
		
		pCruise->SaveData();

		m_pFlightperformanceCrusie->push_back(pCruise);
	}
}
void CDlgFlightPerformanceCruise::LoadItemData(int nItem,CFlightPerformanceCruise *pTerminal)
{

	CString strValue;
	strValue.Format("%.2f",pTerminal->getFuelBurn());
	m_wndListCtrl.SetItemText(nItem,1,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMinSpeed());
	m_wndListCtrl.SetItemText(nItem,2,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMaxSpeed());
	m_wndListCtrl.SetItemText(nItem,3,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMaxAcceleration());
	m_wndListCtrl.SetItemText(nItem,4,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMaxDeceleration());
	m_wndListCtrl.SetItemText(nItem,5,strValue);
	//cost 
	strValue.Empty();
	strValue.Format("%.2f",2000.0);
	m_wndListCtrl.SetItemText(nItem,6,strValue);
}
void CDlgFlightPerformanceCruise::OnNewItem(FlightConstraint& fltType)
{
	CFlightPerformanceCruise *pCruise = new CFlightPerformanceCruise();
	pCruise->setProjID(m_pFlightperformanceCrusie->getProjID());
	pCruise->SetFltType(fltType);

	pCruise->SaveData();

	m_pFlightperformanceCrusie->push_back(pCruise);
	LoadItemData(m_wndListCtrl.GetItemCount() - 1, pCruise);
}
void CDlgFlightPerformanceCruise::OnDelItem()
{
	int nItem =GetSelectedListItem();
	m_wndListCtrl.DeleteItem(nItem);

	vFlightPerformanceCruise::iterator iter = m_pFlightperformanceCrusie->begin();
	while (nItem)
	{
		iter++;
		nItem -= 1;  
	}
	(*iter)->DeleteData();
	delete *iter;
	m_pFlightperformanceCrusie->erase(iter);
}
//void CDlgFlightPerformanceCruise::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//	{
//		int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
//		m_wndListCtrl.EditSubLabel(nItem, 1);
//	}
//}

void CDlgFlightPerformanceCruise::OnListItemChanged(int nItem, int nSubItem)
{
	CFlightPerformanceCruise *pCruise = m_pFlightperformanceCrusie->at(nItem);
	ASSERT(pCruise);
	CString strValue = m_wndListCtrl.GetItemText(nItem,nSubItem);
	double dValue = atof(strValue);
	if (nSubItem == 1)
		pCruise->setFuelBurn(dValue);
	else if(nSubItem == 2)
		pCruise->setMinSpeed(dValue);
	else if(nSubItem == 3)
		pCruise->setMaxSpeed(dValue);
	else if(nSubItem == 4)
		pCruise->setMaxAcceleration(dValue);
	else if(nSubItem == 5)
		pCruise->setMaxDeceleration(dValue);

	pCruise->UpdateData();
}
void CDlgFlightPerformanceCruise::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//CFlightPerformanceCruise *pCruise = m_pFlightperformanceCrusie->at(nItem);
	//ASSERT(pCruise);
	//pCruise->setCompositeID(nNewNECTableID);
	//pCruise->UpdateData();
}


BOOL CDlgFlightPerformanceCruise::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
