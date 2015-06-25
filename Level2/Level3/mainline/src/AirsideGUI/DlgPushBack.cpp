#include "StdAfx.h"
#include "resource.h"
#include ".\dlgpushback.h"
#include "..\InputAirside\PushBack.h"
#include "..\InputAirside\AirsideAircraft.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"
#include "..\common\ProbabilityDistribution.h"

BEGIN_MESSAGE_MAP(CDlgPushBack, CNECRelatedBaseDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND(ID_EDIT_NECRELATEDITEM, OnCmdEditItem)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_NEC,OnSelComboBox)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)
END_MESSAGE_MAP()

CDlgPushBack::CDlgPushBack(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry)
	:CNECRelatedBaseDlg(pSelectFlightType, _T("Push Back Performance Specification"))
	,m_nProjID(nProjID)
	,m_pAirportDB(pAirportDB)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
{
	m_pPushBack = new CPushBack(pAirportDB);
	if(m_pPushBack)
		m_pPushBack->ReadData(nProjID);	
}

CDlgPushBack::~CDlgPushBack(void)
{
	if(m_pPushBack)
	{
		delete m_pPushBack;
	}
}

void CDlgPushBack::ReloadData()
{
	
}

void CDlgPushBack::SetListColumn()
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

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	lvc.pszText = "Push Back Operation Speed(m/s)";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.csList = &strlist;
	lvc.pszText = "Unhook And Taxi Time(mins)";
	m_wndListCtrl.InsertColumn(2, &lvc);

}

void CDlgPushBack::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_pPushBack->GetElementCount();	
	
	for (int n = 0; n < nCount; n++)
	{
		CPushBackNEC* pPushBackNEC = m_pPushBack->GetItem(n);		
		
		char szFltType[1024] = {0};
		pPushBackNEC->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(n, szFltType);


		CString strDistName = _T("");
		strDistName = pPushBackNEC->GetSpeedDistName();
		if(strDistName.IsEmpty())
			strDistName = _T("Select Distribution Name...");
		m_wndListCtrl.SetItemText(n, 1,strDistName );
		strDistName = pPushBackNEC->GetTimeDistName();
		if(strDistName.IsEmpty())
			strDistName = _T("Select Distribution Name...");
		m_wndListCtrl.SetItemText(n, 2,strDistName );

		int nID = pPushBackNEC->GetID();
		m_wndListCtrl.SetItemData(n, nID == -1?-2:nID);
	}	
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}
void CDlgPushBack::CheckListContent()
{
	int nCount = m_pPushBack->GetElementCount();
	if(nCount == 0)
	{
		CPushBackNEC* pbNEC = new CPushBackNEC;
		pbNEC->SetAirportDB(m_pAirportDB);
		m_pPushBack->AddNewItem(pbNEC);
		pbNEC = 0;
	}
}

LRESULT CDlgPushBack::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;
	return (0);
}

void CDlgPushBack::OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result )
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNotifyStruct;
	*result = 0;
	if(!dispinfo)
		return;

	if(!m_pPushBack)return;	
	CPushBackNEC* pPushBackNEC = m_pPushBack->GetItem(m_nRowSel);
	if(!pPushBackNEC)return;
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
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		CString strDistName = pPDEntry->m_csName;
		
		pProbDist->printDistribution(szBuffer);
		if(m_nColumnSel == 1)
		{
			pPushBackNEC->SetSpeedDistName(strDistName);
			pPushBackNEC->SetSpeedProbTypes((ProbTypes)pProbDist->getProbabilityType());
			pPushBackNEC->SetSpeedPrintDist(szBuffer);
		}
		else if(m_nColumnSel == 2)
		{
			pPushBackNEC->SetTimeDistName(strDistName); 
			pPushBackNEC->SetTimeProbTypes((ProbTypes)pProbDist->getProbabilityType());
			pPushBackNEC->SetTimePrintDist(szBuffer);
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
		//assert( i < nCount );
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		
		CString strDistName = pPDEntry->m_csName;	
		pProbDist->printDistribution(szBuffer);
		if(m_nColumnSel == 1)
		{
			pPushBackNEC->SetSpeedDistName(strDistName);
			pPushBackNEC->SetSpeedProbTypes((ProbTypes)pProbDist->getProbabilityType());
			pPushBackNEC->SetSpeedPrintDist(szBuffer);
		}
		else if(m_nColumnSel == 2)
		{
			pPushBackNEC->SetTimeDistName(strDistName); 
			pPushBackNEC->SetTimeProbTypes((ProbTypes)pProbDist->getProbabilityType());
			pPushBackNEC->SetTimePrintDist(szBuffer);
		}  		 
	}
	SetListColumn();
	SetListContent();
}

void CDlgPushBack::OnNewItem(FlightConstraint& fltType)
{
	CPushBackNEC* pbNEC = new CPushBackNEC;
	pbNEC->SetFltType(fltType);
	pbNEC->SetAirportDB(m_pAirportDB);
	m_pPushBack->AddNewItem(pbNEC); 

	SetListContent();
}

void CDlgPushBack::OnDelItem()
{
	int nItem = GetSelectedListItem();
	m_pPushBack->DeleteItem(nItem);
	m_wndListCtrl.DeleteItemEx(nItem);
	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CDlgPushBack::OnListItemChanged(int nItem, int nSubItem)
{	
}

void CDlgPushBack::OnNECItemChanged(int nItem, int nNewNECTableID)
{
}

BOOL CDlgPushBack::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_EDIT_NECRELATEDITEM, TRUE);
	m_wndListCtrl.ModifyStyle(0,LVS_SINGLESEL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPushBack::OnBnClickedButtonSave()
{ 
	if(m_pPushBack)m_pPushBack->SaveData(m_nProjID);
	CNECRelatedBaseDlg::OnBnClickedButtonSave();
}


void CDlgPushBack::OnOK()
{
	if(m_pPushBack)m_pPushBack->SaveData(m_nProjID);
	CNECRelatedBaseDlg::OnOK();
}

void CDlgPushBack::OnCmdEditItem()
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
