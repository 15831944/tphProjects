#include "StdAfx.h"
#include "resource.h"
#include ".\dlgstandservice.h"
#include "..\InputAirside\AirsideAircraft.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\InputAirside\StandService.h"
#include "../Database/DBElementCollection_Impl.h"

BEGIN_MESSAGE_MAP(CDlgStandService, CNECRelatedBaseDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND(ID_EDIT_NECRELATEDITEM, OnCmdEditItem)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_NEC,OnSelComboBox)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)
END_MESSAGE_MAP()

CDlgStandService::CDlgStandService(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry)
:CNECRelatedBaseDlg(pSelectFlightType, _T("Stand Service Performance Specification"))
,m_nProjID(nProjID)
,m_pAirportDB(pAirportDB)
,m_pInputAirside(pInputAirside)
,m_pSelectProbDistEntry(pSelectProbDistEntry)
{
	m_pStandServiceList = new StandServiceList(pAirportDB);
	if(m_pStandServiceList)
		m_pStandServiceList->ReadData(nProjID);	
}

CDlgStandService::~CDlgStandService(void)
{
	if (m_pStandServiceList)
	{
		delete m_pStandServiceList;
		m_pStandServiceList = NULL;
	}
}

void CDlgStandService::ReloadData()
{

}

void CDlgStandService::SetListColumn()
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
	m_wndListCtrl.DeleteColumn(1);
	m_wndListCtrl.DeleteColumn(1);

	m_wndListCtrl.SetColumnWidth(0, 150);
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	lvc.pszText = "Min Turn Around Time(mins)";
	lvc.cx = 250;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(1, &lvc);
}

void CDlgStandService::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_pStandServiceList->GetElementCount();	

	for (int n = 0; n < nCount; n++)
	{
		StandService* pStandService = m_pStandServiceList->GetItem(n);		

		char szFltType[1024] = {0};
		pStandService->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(n, szFltType);


		CString strDistName = _T("");
		strDistName = pStandService->GetTimeDistScreenPrint();
		if(strDistName.IsEmpty())
			strDistName = _T("Select Distribution Name...");
		m_wndListCtrl.SetItemText(n, 1,strDistName );

		int nID = pStandService->GetID();
		m_wndListCtrl.SetItemData(n, nID == -1?-2:nID);
	}	
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}
void CDlgStandService::CheckListContent()
{
	int nCount = m_pStandServiceList->GetElementCount();
	if(nCount == 0)
	{
		StandService* pStandService = new StandService;
		pStandService->SetAirportDB(m_pAirportDB);
		m_pStandServiceList->AddNewItem(pStandService);
		pStandService = NULL;
	}
}

LRESULT CDlgStandService::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;
	return (0);
}

void CDlgStandService::OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result )
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNotifyStruct;
	*result = 0;
	if(!dispinfo)
		return;

	if(m_pStandServiceList == NULL)
		return;	

	StandService* pStandService = m_pStandServiceList->GetItem(m_nRowSel);

	if(pStandService == NULL)
		return;

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
		pProbDist = pPDEntry->m_pProbDist;

		ASSERT( pProbDist );
		if(pProbDist != NULL)
		{
			CString strDistName = pPDEntry->m_csName;

			pProbDist->printDistribution(szBuffer);

			if(m_nColumnSel == 1)
			{
				pStandService->SetTimeDistributionDesc(strDistName,(ProbTypes)pProbDist->getProbabilityType(),szBuffer); 
				//pStandService->SetTimeProbTypes((ProbTypes)pProbDist->getProbabilityType());
				//pStandService->SetTimePrintDist(szBuffer);
			}  		
		}


	}
	else
	{
		//if(m_strDistName == strSel)
		//	return;
		CProbDistEntry* pPDEntry = NULL;

		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry->m_csName == strSel)
				break;
		}

		//ASSERT( i < nCount );
		if(pPDEntry != NULL && pPDEntry->m_pProbDist != NULL)
		{
			pProbDist = pPDEntry->m_pProbDist;
			ASSERT( pProbDist );

			CString strDistName = pPDEntry->m_csName;	
			pProbDist->printDistribution(szBuffer);

			if(m_nColumnSel == 1)
			{
				pStandService->SetTimeDistributionDesc(strDistName,(ProbTypes)pProbDist->getProbabilityType() ,szBuffer ); 
				//pStandService->SetTimeProbTypes((ProbTypes)pProbDist->getProbabilityType());
				//pStandService->SetTimePrintDist(szBuffer);
			} 
		}
 		 
	}
	SetListColumn();
	SetListContent();
}

void CDlgStandService::OnNewItem(FlightConstraint& fltType)
{
	StandService* pStandService = new StandService;
	pStandService->SetFltType(fltType);
	pStandService->SetAirportDB(m_pAirportDB);
	m_pStandServiceList->AddNewItem(pStandService); 

	SetListContent();
}

void CDlgStandService::OnDelItem()
{
	int nItem = GetSelectedListItem();
	m_pStandServiceList->DeleteItem(nItem);
	m_wndListCtrl.DeleteItemEx(nItem);
	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CDlgStandService::OnListItemChanged(int nItem, int nSubItem)
{	
}

void CDlgStandService::OnNECItemChanged(int nItem, int nNewNECTableID)
{
}

BOOL CDlgStandService::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_EDIT_NECRELATEDITEM, TRUE);
	m_wndListCtrl.ModifyStyle(0,LVS_SINGLESEL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStandService::OnBnClickedButtonSave()
{ 
	try
	{
		CADODatabase::BeginTransaction() ;
		if(m_pStandServiceList)
			m_pStandServiceList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CNECRelatedBaseDlg::OnBnClickedButtonSave();
}


void CDlgStandService::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		if(m_pStandServiceList)
			m_pStandServiceList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CNECRelatedBaseDlg::OnOK();
}

void CDlgStandService::OnCmdEditItem()
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