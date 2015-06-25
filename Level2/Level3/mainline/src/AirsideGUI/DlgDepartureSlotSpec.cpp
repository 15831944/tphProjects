#include "stdafx.h"
#include "Resource.h"
#include "../Common/ProbDistManager.h"
#include "DlgDepartureSlotSpec.h"
#include "../Common/AirportDatabase.h"
#include "../InputAirside/DepartureSlotSepcification.h"
#include "DlgTimeRange.h"
#include "InputAirside/AirRoute.h"
#include "..\common\WinMsg.h"
#include "../Database//DBElementCollection_Impl.h"


IMPLEMENT_DYNAMIC(CDlgDepartureSlotSpec, CXTResizeDialog)
CDlgDepartureSlotSpec::CDlgDepartureSlotSpec(int nProjID, PSelectFlightType pSelectFlightType,
											 CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent /*=NULL*/)
											 : CXTResizeDialog(CDlgDepartureSlotSpec::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pAirportDB(pAirportDB)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
{
	m_nRowSel = -1;
	m_nColumnSel = -1;
	m_pDepSlotSpec = new DepartureSlotSepcifications(m_pAirportDB);
	m_pDepSlotSpec->ReadData(m_nProjID);
}

CDlgDepartureSlotSpec::~CDlgDepartureSlotSpec()
{
	delete m_pDepSlotSpec;
	m_vrSID.clear();
}

void CDlgDepartureSlotSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgDepartureSlotSpec, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_DEPSLOT_ADD, OnNewDepartureSlotItem)
	ON_COMMAND(ID_DEPSLOT_DEL, OnDelDepartureSlotItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_NEC,OnSelComboBox)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnEndDbClickNoEditListItem)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS,OnKillFocusComboBoxOfCtrlList)
END_MESSAGE_MAP()

void   CDlgDepartureSlotSpec::OnSelComboBox(NMHDR*   pNMHDR,   LRESULT*   pResult)     
{     	
	
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	if(m_nColumnSel == 1)
		return;
	*pResult = 0;
	if(!dispinfo)
		return;

	if(!m_pDepSlotSpec)return;	
	DepartureSlotItem* pItem = m_pDepSlotSpec->GetItem(m_nRowSel);
	if(!pItem)return;
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
			if(m_nColumnSel == 3)
				m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,pItem->GetFirstSlotDelayProDisName());
			else if(m_nColumnSel == 4)
				m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,pItem->GetInterSlotDelayProDisName());
			return;
		}
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		CString strDistName = pPDEntry->m_csName;

		pProbDist->printDistribution(szBuffer);
		if(m_nColumnSel == 3)//First Slot Delay
		{
			pItem->SetFirstSlotDelayProDisName(strDistName);
			pItem->SetFirstSlotDelayProDisType((ProbTypes)pProbDist->getProbabilityType());
			pItem->SetFirstSlotDelayPrintDis(szBuffer);
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
		}
		else if(m_nColumnSel == 4)//Inter Slot Delay
		{
			pItem->SetInterSlotDelayProDisName(strDistName); 
			pItem->SetInterSlotDelayProDisType((ProbTypes)pProbDist->getProbabilityType());
			pItem->SetInterSlotDelayPrintDis(szBuffer);
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
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
		if(m_nColumnSel == 3)//First Slot Delay
		{
			pItem->SetFirstSlotDelayProDisName(strDistName);
			pItem->SetFirstSlotDelayProDisType((ProbTypes)pProbDist->getProbabilityType());
			pItem->SetFirstSlotDelayPrintDis(szBuffer);
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
		}
		else if(m_nColumnSel == 4)//Inter Slot Delay
		{
			pItem->SetInterSlotDelayProDisName(strDistName); 
			pItem->SetInterSlotDelayProDisType((ProbTypes)pProbDist->getProbabilityType());
			pItem->SetInterSlotDelayPrintDis(szBuffer);
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
		}  			 
	} 
}

LRESULT CDlgDepartureSlotSpec::OnKillFocusComboBoxOfCtrlList( WPARAM wparam, LPARAM lparam)
{
	if(m_nColumnSel != 1)return (0);
	int nCurSel = (int)wparam;
	if(!m_pDepSlotSpec)return (0);	
	DepartureSlotItem* pItem = m_pDepSlotSpec->GetItem(m_nRowSel);
	if(!pItem)return (0);
	pItem->SetSID(m_vrSID[nCurSel]);
	m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,pItem->GetSIDName());

	return (0);
}

LRESULT CDlgDepartureSlotSpec::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;
	return (0);
}

LRESULT CDlgDepartureSlotSpec::OnEndDbClickNoEditListItem( WPARAM wparam, LPARAM lparam)
{
	if(!m_pDepSlotSpec)return (0);	
	DepartureSlotItem* pItem = m_pDepSlotSpec->GetItem(m_nRowSel);
	if(!pItem)return (0);

	if(m_nColumnSel == 0)//Flight Type
	{
		if (m_pSelectFlightType == NULL)
			return (0);
		FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
		char szBuffer[1024]={0};
		int xCount = m_wndListCtrl.GetItemCount();
		for(int n = 0;n<xCount;n++)
		{
			if(!m_wndListCtrl.GetItemText(n,0).Compare(szBuffer))
			{
				MessageBox(_T("This Flight Type had existed."));
				return (0);
			}
		}

		pItem->SetFltType(fltType);
		CString strFltType;
		pItem->GetFlightConstraint().screenPrint(strFltType);
		CString strString;		
		strString.Format("%s \t ", strFltType);
		m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strString);
	}
	else if(m_nColumnSel == 2)//Time Range
	{
		CDlgTimeRange dlg(pItem->GetFromTime(), pItem->GetToTime());
		if(IDOK == dlg.DoModal())
		{
			pItem->SetFromTime(dlg.GetStartTime());
			pItem->SetToTime(dlg.GetEndTime());
			
			ElapsedTime estFromTime;
			estFromTime = pItem->GetFromTime();
			ElapsedTime estToTime;
			estToTime = pItem->GetToTime();
			CString strFromTime;
			strFromTime.Format("Day%d %02d:%02d:%02d", estFromTime.GetDay(), 
				estFromTime.GetHour(),
				estFromTime.GetMinute(),
				estFromTime.GetSecond());
			CString strToTime;
			strToTime.Format("Day%d %02d:%02d:%02d", estToTime.GetDay(), 
				estToTime.GetHour(),
				estToTime.GetMinute(),
				estToTime.GetSecond());
			CString strfromToTime;
			strfromToTime.Format("%s - %s", strFromTime, strToTime);
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strfromToTime);
		}
	}

	return (0);
}

int CDlgDepartureSlotSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_DEPSLOT))
	{
		return -1;
	}
	return 0;
}

int CDlgDepartureSlotSpec::GetSelectedListItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_wndListCtrl.GetNextSelectedItem(pos);

	return -1;
}
BOOL CDlgDepartureSlotSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	m_wndFltToolbar.MoveWindow(&rectFltToolbar, true);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEPSLOT_ADD, TRUE);
	m_wndFltToolbar.GetToolBarCtrl().HideButton(ID_DEPSLOT_EDIT, TRUE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEPSLOT_DEL, TRUE);

	SetResize(m_wndFltToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LISTBOX, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDSAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitListCtrl();
	GetDlgItem(IDSAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgDepartureSlotSpec::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);
	CStringList strlist;	
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;

	//flight type
	lvc.pszText = "Flight Type";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(0,&lvc);

	//SID
	strlist.RemoveAll();
	strlist.AddTail(_T("All"));
	m_vrSID.clear();
	m_vrSID.push_back(-2);
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute *pAirRoute = 0;
		pAirRoute = airRouteList.GetAirRoute(i);
		ASSERT(pAirRoute != NULL);
		if (pAirRoute && CAirRoute::SID == pAirRoute->getRouteType() )
		{
			strlist.AddTail(pAirRoute->getName());
			m_vrSID.push_back(pAirRoute->getID());
		}
	}	
	lvc.pszText = "SID";
	lvc.cx = 40;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(1,&lvc);

	//time range
	strlist.RemoveAll();
	lvc.pszText = "Time Range";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(2,&lvc);
	
	 //distribution
	strlist.RemoveAll();
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
	 lvc.pszText = "First slot delay(mins)";
	 lvc.cx = 120;
	 lvc.fmt = LVCFMT_DROPDOWN;
	 lvc.csList = &strlist;
	 m_wndListCtrl.InsertColumn(3, &lvc);

	 lvc.pszText = "Inter slot delay(mins)";
	 lvc.cx = 200;
	 m_wndListCtrl.InsertColumn(4, &lvc);

	 SetListContent();
	 
	 GetDlgItem(IDSAVE)->EnableWindow(FALSE);
}

void CDlgDepartureSlotSpec::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	//get values
	size_t nDepSlotCount = m_pDepSlotSpec->GetElementCount();
	for (size_t i = 0; i< nDepSlotCount; i++)
	{
		DepartureSlotItem* pItem = m_pDepSlotSpec->GetItem(i);

		//flight type
		FlightConstraint fltType = pItem->GetFlightConstraint();
		CString strFltType;
		fltType.screenPrint(strFltType);
		CString strString;		
		strString.Format("%s \t ", strFltType);
		m_wndListCtrl.InsertItem(i, strString);

		//SID
		m_wndListCtrl.SetItemText(i,1,pItem->GetSIDName());

		//Time Range
		ElapsedTime estFromTime;
		estFromTime = pItem->GetFromTime();
		ElapsedTime estToTime;
		estToTime = pItem->GetToTime();
		CString strFromTime;
		strFromTime.Format("Day%d %02d:%02d:%02d", estFromTime.GetDay(), 
			estFromTime.GetHour(),
			estFromTime.GetMinute(),
			estFromTime.GetSecond());
		CString strToTime;
		strToTime.Format("Day%d %02d:%02d:%02d", estToTime.GetDay(), 
			estToTime.GetHour(),
			estToTime.GetMinute(),
			estToTime.GetSecond());
		CString strfromToTime;
		strfromToTime.Format("%s - %s", strFromTime, strToTime);
		m_wndListCtrl.SetItemText(i, 2,strfromToTime);

		//first slot delay distribution
		m_wndListCtrl.SetItemText(i, 3,pItem->GetFirstSlotDelayProDisName());

		//inter slot delay distribution
		m_wndListCtrl.SetItemText(i, 4,pItem->GetInterSlotDelayProDisName());

		m_wndListCtrl.SetItemData(i, (DWORD_PTR)pItem);
	}
	m_wndListCtrl.SetItemState(nDepSlotCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CDlgDepartureSlotSpec::OnNewDepartureSlotItem()
{
	DepartureSlotItem* pItem = new DepartureSlotItem();
	if(!pItem)return;
	pItem->SetProjID(m_nProjID);
	pItem->setAirportDB(m_pAirportDB);
	FlightConstraint fltType;
	pItem->SetFltType(fltType);
	pItem->SetSID(-2);

	ElapsedTime elapsedtimeFrom;
	elapsedtimeFrom.set(0);
	pItem->SetFromTime(elapsedtimeFrom);
	ElapsedTime elapsedtimeTo;
	elapsedtimeTo.set(86399);
	pItem->SetToTime(elapsedtimeTo);

	pItem->SetFirstSlotDelayProDisName(_T("U[20~30]"));
	pItem->SetFirstSlotDelayProDisType(UNIFORM);
	pItem->SetFirstSlotDelayPrintDis(_T("Uniform:20;30"));
	pItem->SetInterSlotDelayProDisName(_T("U[20~30]"));
	pItem->SetInterSlotDelayProDisType(UNIFORM);
	pItem->SetInterSlotDelayPrintDis(_T("Uniform:20;30"));

	
	CString strFltType;
	fltType.screenPrint(strFltType);
	CString strString;
	strString.Format("%s \t ", strFltType);
	int nCount = m_wndListCtrl.GetItemCount();
	m_wndListCtrl.InsertItem(nCount, strString);
	m_wndListCtrl.SetItemText(nCount, 1,_T("All"));
	CString strInit;
	strInit.Format("Day1 00:00:00 - Day1 23:59:59");
	m_wndListCtrl.SetItemText(nCount, 2,strInit);
	m_wndListCtrl.SetItemText(nCount,3,pItem->GetFirstSlotDelayProDisName());
	m_wndListCtrl.SetItemText(nCount,4,pItem->GetInterSlotDelayProDisName());
	m_wndListCtrl.SetItemData(nCount, (DWORD_PTR)pItem);
	m_pDepSlotSpec->AddNewItem(pItem);

	m_wndListCtrl.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	GetDlgItem(IDSAVE)->EnableWindow(TRUE);
}

void CDlgDepartureSlotSpec::OnDelDepartureSlotItem()
{
	int nSel =  GetSelectedListItem();
	if (nSel == -1)
		return;
	
	DepartureSlotItem* pItem = (DepartureSlotItem*)m_wndListCtrl.GetItemData(nSel);

	m_pDepSlotSpec->DeleteItem(pItem);
	m_wndListCtrl.SetItemData(nSel,0);
	m_wndListCtrl.DeleteItem(nSel);
	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	GetDlgItem(IDSAVE)->EnableWindow(TRUE);
}

void CDlgDepartureSlotSpec::OnBnClickedOk()
{
	OnBnClickedSave();
	CXTResizeDialog::OnOK();
}

void CDlgDepartureSlotSpec::OnBnClickedSave()
{	
	try
	{
		CADODatabase::BeginTransaction();
		m_pDepSlotSpec->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e) 
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		return;
	}
	GetDlgItem(IDSAVE)->EnableWindow(FALSE);
}

//
//void CDlgDepartureSlotSpec::OnDbClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
//	if(IDOK!= dlg.DoModal())
//		return;
//}
