#include "StdAfx.h"
#include "Resource.h"
#include ".\dlgtowoperationperformance.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\Common\ProDistrubutionData.h"
#include "..\InputAirside\TowOperationPerformance.h"
#include "../Database/ADODatabase.h"
#include "../Inputs/probab.h"
#include "../Database/DBElementCollection_Impl.h"

BEGIN_MESSAGE_MAP(DlgTowOperationPerformance, CNECRelatedBaseDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND(ID_EDIT_NECRELATEDITEM, OnCmdEditItem)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_NEC,OnSelComboBox)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)
END_MESSAGE_MAP()


DlgTowOperationPerformance::DlgTowOperationPerformance(PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry)
:CNECRelatedBaseDlg(pSelectFlightType, _T("Pushback /Tow Operation Performance"))
,m_pAirportDB(pAirportDB)
,m_pInputAirside(pInputAirside)
,m_pSelectProbDistEntry(pSelectProbDistEntry)
{
	m_pTowOperationPerformance = new TowOperationPerformanceList(pAirportDB);
	try
	{
		m_pTowOperationPerformance->ReadData(-1);
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pTowOperationPerformance)
		{
			delete m_pTowOperationPerformance;
			m_pTowOperationPerformance = NULL;
		}
		return;
	}
}

DlgTowOperationPerformance::~DlgTowOperationPerformance(void)
{
	delete m_pTowOperationPerformance;
	m_pTowOperationPerformance = NULL;
}

void DlgTowOperationPerformance::ReloadData()
{

}

void DlgTowOperationPerformance::SetListColumn()
{ 
	CStringList strlist;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pAirportDB)
		pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	strlist.AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strlist.AddTail(pPDEntry->m_csName);
	}

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	lvc.pszText = "Push Back Operation Speed(m/s)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.csList = &strlist;
	lvc.pszText = "Unhook Time(mins)";
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.csList = &strlist;
	lvc.pszText = "Attached acceleration(m/s/s)";
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.csList = &strlist;
	lvc.pszText = "Attached deceleration(m/s/s)";
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.csList = &strlist;
	lvc.pszText = "Tow speed(m/s)";
	m_wndListCtrl.InsertColumn(5, &lvc);

}

void DlgTowOperationPerformance::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_pTowOperationPerformance->GetElementCount();	

	for (int n = 0; n < nCount; n++)
	{
		TowOperationPerformance* pData = m_pTowOperationPerformance->GetItem(n);		

		char szFltType[1024] = {0};
		pData->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(n, szFltType);


		CString strDistName = _T("");
		strDistName = pData->m_pPushbackSpeedDist->getDistName();
		m_wndListCtrl.SetItemText(n, 1,strDistName );
		strDistName = pData->m_pUnhookAndTaxiTimeDist->getDistName();
		m_wndListCtrl.SetItemText(n, 2,strDistName );
		strDistName = pData->m_pAttachedAccelerationDist->getDistName();
		m_wndListCtrl.SetItemText(n, 3, strDistName);
		strDistName = pData->m_pAttachedDecelerationDist->getDistName();
		m_wndListCtrl.SetItemText(n, 4, strDistName);
		strDistName = pData->m_pTowSpeedDist->getDistName();
		m_wndListCtrl.SetItemText(n, 5, strDistName);

		m_wndListCtrl.SetItemData(n,(DWORD_PTR)pData);
	}	
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}
void DlgTowOperationPerformance::CheckListContent()
{
	int nCount = m_pTowOperationPerformance->GetElementCount();
	if(nCount == 0)
	{
		TowOperationPerformance* pData = new TowOperationPerformance;
		pData->SetAirportDB(m_pAirportDB);
		m_pTowOperationPerformance->AddNewItem(pData);
	}
}

LRESULT DlgTowOperationPerformance::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;
	return (0);
}

void DlgTowOperationPerformance::OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result )
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNotifyStruct;
	*result = 0;
	if(!dispinfo)
		return;

	if(m_pTowOperationPerformance == NULL)
		return;	
	TowOperationPerformance* pData = (TowOperationPerformance*)m_wndListCtrl.GetItemData(m_nRowSel);

	if(pData == NULL)
		return;
	// TODO: Add your control notification handler code here	 
	CString strSel;
	strSel = dispinfo->item.pszText;

	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	if(strSel == _T("New Probability Distribution..." ) )
	{
		CProbDistEntry* pPDEntry = NULL;
		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
		if(pPDEntry == NULL)
		{
			SetListContent();
			return;
		}
		//pProbDist = pPDEntry->m_pProbDist;
		//assert( pProbDist );
		CString strDistName = pPDEntry->m_csName;

		//pProbDist->printDistribution(szBuffer);
		switch(m_nColumnSel)
		{
		case 1: 
			pData->m_pPushbackSpeedDist->SetProDistrubution(pPDEntry);
			break;
		case 2:
			pData->m_pUnhookAndTaxiTimeDist->SetProDistrubution(pPDEntry);
			break;
		case 3:
			pData->m_pAttachedAccelerationDist->SetProDistrubution(pPDEntry);
		    break;
		case 4:
			pData->m_pAttachedDecelerationDist->SetProDistrubution(pPDEntry);
		    break;
		case 5:
			pData->m_pTowSpeedDist->SetProDistrubution(pPDEntry);
			break;
		default:
		    break;
		}
	}
	else
	{
		CProbDistEntry* pPDEntry = NULL;

		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry->m_csName == strSel)
				break;
		}
		//assert( i < nCount );
		//pProbDist = pPDEntry->m_pProbDist;
		//assert( pProbDist );

		CString strDistName = pPDEntry->m_csName;	
		//pProbDist->printDistribution(szBuffer);
		switch(m_nColumnSel)
		{
		case 1: 
			pData->m_pPushbackSpeedDist->SetProDistrubution(pPDEntry);
			break;
		case 2:
			pData->m_pUnhookAndTaxiTimeDist->SetProDistrubution(pPDEntry);
			break;
		case 3:
			pData->m_pAttachedAccelerationDist->SetProDistrubution(pPDEntry);
			break;
		case 4:
			pData->m_pAttachedDecelerationDist->SetProDistrubution(pPDEntry);
			break;
		case 5:
			pData->m_pTowSpeedDist->SetProDistrubution(pPDEntry);
			break;
		default:
			break;
		}
	}
	SetListContent();
}

void DlgTowOperationPerformance::OnNewItem(FlightConstraint& fltType)
{
	TowOperationPerformance* pData = new TowOperationPerformance;
	pData->m_fltType = fltType ;
	pData->SetAirportDB(m_pAirportDB);
	m_pTowOperationPerformance->AddNewItem(pData); 

	SetListContent();
}

void DlgTowOperationPerformance::OnDelItem()
{
	int nItem = GetSelectedListItem();
	TowOperationPerformance* pData = (TowOperationPerformance*)m_wndListCtrl.GetItemData(nItem);
	m_pTowOperationPerformance->DeleteItem(pData);
	m_wndListCtrl.DeleteItemEx(nItem);
	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void DlgTowOperationPerformance::OnListItemChanged(int nItem, int nSubItem)
{	
}

void DlgTowOperationPerformance::OnNECItemChanged(int nItem, int nNewNECTableID)
{
}

BOOL DlgTowOperationPerformance::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_EDIT_NECRELATEDITEM, TRUE);
	m_wndListCtrl.ModifyStyle(0,LVS_SINGLESEL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DlgTowOperationPerformance::OnBnClickedButtonSave()
{ 
	try
	{
		CADODatabase::BeginTransaction() ;
		if(m_pTowOperationPerformance)
			m_pTowOperationPerformance->SaveData();
		CNECRelatedBaseDlg::OnBnClickedButtonSave();
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}


void DlgTowOperationPerformance::OnOK()
{
	OnBnClickedButtonSave() ;
	CNECRelatedBaseDlg::OnOK();
}

void DlgTowOperationPerformance::OnCmdEditItem()
{
	POINT point;
	point.x = 0;
	point.y = 0;
	GetCursorPos(&point);
	m_wndListCtrl.ScreenToClient(&point);
	DWORD dwLparam = 0;
	dwLparam = (DWORD)point.y;
	dwLparam <<= int(sizeof(DWORD)/2);
	dwLparam |= (DWORD)point.x;
	m_wndListCtrl.SendMessage(WM_LBUTTONDBLCLK,(WPARAM)MK_LBUTTON,(LPARAM)dwLparam);
}
