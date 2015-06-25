#include "StdAfx.h"
#include "resource.h"
#include ".\dlgflightperformanceterminal.h"
#include "..\Common\AirportDatabase.h"

using namespace ns_AirsideInput;

CDlgFlightPerformanceTerminal::CDlgFlightPerformanceTerminal(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB) 
 : CNECRelatedBaseDlg(pSelectFlightType,"Terminal Maneuvering Performance Specification")
{
	m_pFlightPerformanceTerminal = new vFlightPerformanceTerminal(pAirportDB);
	m_pFlightPerformanceTerminal->ReadData(nProjID);
}

CDlgFlightPerformanceTerminal::~CDlgFlightPerformanceTerminal(void)
{
}

void CDlgFlightPerformanceTerminal::ReloadData()
{
}

void CDlgFlightPerformanceTerminal::SetListColumn()
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
	lvc.pszText ="Fuel Burn(avg)";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 1, &lvc );

	//min speed
	lvc.csList = &strList;
	lvc.pszText ="Min Speed(kts)";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 2, &lvc );

	//max speed
	lvc.csList = &strList;
	lvc.pszText ="Max Speed(kts)";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 3, &lvc );

	//max Acceleration
	lvc.csList = &strList;
	lvc.pszText ="Max Acceleration(ft/sec2)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 4, &lvc );

	//max deceleration
	lvc.csList = &strList;
	lvc.pszText ="Max deceleration(ft/sec2)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 5, &lvc );
	
	//Min speed holding
	lvc.csList = &strList;
	lvc.pszText ="Min speed holding(kts)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 6, &lvc );
	
	//max Speed holding 
	lvc.csList = &strList;
	lvc.pszText ="max Speed holding(kts)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 7, &lvc );

	//max down vertical speed
	lvc.csList = &strList;
	lvc.pszText ="max down vertical speed(ft/min)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 8, &lvc );
	
	//max up vertical speed
	lvc.csList = &strList;
	lvc.pszText ="max up vertical speed(ft/min)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 9, &lvc );

	//cost
	lvc.csList = &strList;
	lvc.pszText ="cost($/hr)";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn( 10, &lvc );
}

void CDlgFlightPerformanceTerminal::SetListContent()
{
	int nCount = static_cast<int>(m_pFlightPerformanceTerminal->size());
	for (int i = 0; i< nCount; i++)
	{
		CFlightPerformanceTerminal *pTerminal = (*m_pFlightPerformanceTerminal)[i];

		char szFltType[1024] = {0};
		pTerminal->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(i, szFltType);

		LoadItemData(i,pTerminal);
	}
}
void CDlgFlightPerformanceTerminal::CheckListContent()
{
	int nCount = static_cast<int>(m_pFlightPerformanceTerminal->size());
	if(nCount == 0)
	{
		CFlightPerformanceTerminal *pTerminal = new CFlightPerformanceTerminal();
		pTerminal->setProjID(m_pFlightPerformanceTerminal->getProjID());
	
		pTerminal->SaveData();
		m_pFlightPerformanceTerminal->push_back(pTerminal);
	}
}
void CDlgFlightPerformanceTerminal::LoadItemData(int nItem,CFlightPerformanceTerminal *pTerminal)
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

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMinSpeedHolding());
	m_wndListCtrl.SetItemText(nItem,6,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMaxSpeedHolding());
	m_wndListCtrl.SetItemText(nItem,7,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMaxDownVSpeed());
	m_wndListCtrl.SetItemText(nItem,8,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getMaxUpVSpeed());
	m_wndListCtrl.SetItemText(nItem,9,strValue);

	strValue.Empty();
	strValue.Format("%.2f",pTerminal->getCost());
	m_wndListCtrl.SetItemText(nItem,10,strValue);
}

void CDlgFlightPerformanceTerminal::OnNewItem(FlightConstraint& fltType)
{
	CFlightPerformanceTerminal *pTerminal = new CFlightPerformanceTerminal();
	pTerminal->setProjID(m_pFlightPerformanceTerminal->getProjID());
	pTerminal->SetFltType(fltType);

	pTerminal->SaveData();
	m_pFlightPerformanceTerminal->push_back(pTerminal);
	LoadItemData(m_wndListCtrl.GetItemCount() - 1, pTerminal);
}

void CDlgFlightPerformanceTerminal::OnDelItem()
{
	int nItem =GetSelectedListItem();
	m_wndListCtrl.DeleteItem(nItem);
	
	vFlightPerformanceTerminal::iterator iter = m_pFlightPerformanceTerminal->begin();
	while (nItem)
	{
		iter++;
		nItem -= 1;  
	}
	(*iter)->DeleteData();
	delete *iter;
	m_pFlightPerformanceTerminal->erase(iter);
}

//void CDlgFlightPerformanceTerminal::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//}

void CDlgFlightPerformanceTerminal::OnListItemChanged(int nItem, int nSubItem) 
{
	CFlightPerformanceTerminal *pTerminal = m_pFlightPerformanceTerminal->at(nItem);
	ASSERT(pTerminal);
	CString strValue = m_wndListCtrl.GetItemText(nItem,nSubItem);
	double dValue = atof(strValue);
	if (nSubItem == 1)
		pTerminal->setFuelBurn(dValue);
	else if(nSubItem == 2)
		pTerminal->setMinSpeed(dValue);
	else if(nSubItem == 3)
		pTerminal->setMaxSpeed(dValue);
	else if(nSubItem == 4)
		pTerminal->setMaxAcceleration(dValue);
	else if(nSubItem == 5)
		pTerminal->setMaxDeceleration(dValue);
	else if(nSubItem == 6)
		pTerminal->setMinSpeedHolding(dValue);
	else if(nSubItem == 7)
		pTerminal->setMaxSpeedHolding(dValue);
	else if(nSubItem == 8)
		pTerminal->setMaxDownVSpeed(dValue);
	else if(nSubItem == 9)
		pTerminal->setMaxUpVSpeed(dValue);
	else if(nSubItem == 10)
		pTerminal->setCost(dValue);

	pTerminal->UpdateData();

}

void CDlgFlightPerformanceTerminal::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//CFlightPerformanceTerminal *pTerminal = m_pFlightPerformanceTerminal->at(nItem);
	//ASSERT(pTerminal);
	//pTerminal->setCompositeID(nNewNECTableID);
	//pTerminal->UpdateData();
}
BOOL CDlgFlightPerformanceTerminal::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
