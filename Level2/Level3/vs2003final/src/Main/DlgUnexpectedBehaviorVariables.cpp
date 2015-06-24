#include "StdAfx.h"
#include "resource.h"
#include ".\dlgunexpectedbehaviorvariables.h"
#include "../inputs/in_term.h"
#include "../Inputs/probab.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/AirportDatabase.h"
#include "../Common/ProbDistManager.h"
#include "TermPlanDoc.h"
#include "DlgNewPassengerType.h"
#include "../Common/NewPassengerType.h"
#include "DlgProbDist.h"
#include "../Database/ADODatabase.h"
#include "../InputOnBoard/UnexpectedBehaviorVariables.h"

IMPLEMENT_DYNAMIC(DlgUnexpectedBehaviorVariables, CXTResizeDialog)
DlgUnexpectedBehaviorVariables::DlgUnexpectedBehaviorVariables(CWnd* pParent/* = NULL*/)
:CXTResizeDialog(DlgUnexpectedBehaviorVariables::IDD, pParent)
,m_pParentWnd(pParent)
{
	try
	{
		m_pUnexpectedBehaviorVariablesList = new UnexpectedBehaviorVariablesList;
		m_pUnexpectedBehaviorVariablesList->ReadData(GetInputTerminal()->inStrDict);		
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pUnexpectedBehaviorVariablesList)
		{
			delete m_pUnexpectedBehaviorVariablesList;
			m_pUnexpectedBehaviorVariablesList = NULL;
		}
		return;
	}

}

DlgUnexpectedBehaviorVariables::~DlgUnexpectedBehaviorVariables(void)
{
	delete m_pUnexpectedBehaviorVariablesList;
	m_pUnexpectedBehaviorVariablesList = NULL;
}

void DlgUnexpectedBehaviorVariables::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAINTENANCE, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(DlgUnexpectedBehaviorVariables, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnNewItem)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDelItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MAINTENANCE_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL,OnBnClickedCancel)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_MAINTENANCE,OnSelComboBox)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)

END_MESSAGE_MAP()


void DlgUnexpectedBehaviorVariables::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);
	CStringList strlist;	
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;

	//mobile element type
	lvc.pszText = "Passenger Type";
	lvc.cx = 180;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(0,&lvc);


	//distribution
	strlist.RemoveAll();
	CString s;
	s.LoadString( IDS_STRING_NEWDIST );
	strlist.InsertAfter( strlist.GetTailPosition(), s );
	int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
		strlist.InsertAfter( strlist.GetTailPosition(), pPBEntry->m_csName );
	}

	lvc.cx = 120;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;

	lvc.pszText  = "P(Wrong Aisle)";
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText  = "P(Wrong Door)";
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText  = "P(Wrong Deck)";
	m_wndListCtrl.InsertColumn(3, &lvc);
	SetListContent();

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(FALSE);
}

void  DlgUnexpectedBehaviorVariables::OnSelComboBox(NMHDR*   pNMHDR,   LRESULT*   pResult)     
{     	

	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	if(m_nColumnSel == 0)
		return;
	*pResult = 0;
	if(!dispinfo)
		return;

	// TODO: Add your control notification handler code here
	UnexpectedBehaviorVariables* pData = m_pUnexpectedBehaviorVariablesList->GetVariable(m_nRowSel);
	if (pData == NULL)
		return;

	CString strSel;
	strSel = dispinfo->item.pszText;

	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
	if(strSel == _T("NEW PROBABILITY DISTRIBUTION" ) )
	{
		CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB,true,this);
		if (dlg.DoModal() != IDOK)
		{
			CString strDistName = "";
			switch(m_nColumnSel)
			{
			case 1:
				strDistName = pData->GetWrongAisleDist()->getPrintDist();
				break;
			case 2:
				strDistName = pData->GetWrongDoorDist()->getPrintDist();
				break;
			case 3:
				strDistName = pData->GetSeatStagingDist()->getPrintDist();
				break;
			default:
				break;
			}
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
			return;
		}

		CProbDistEntry* pPDEntry = dlg.GetSelectedPD();

		if(pPDEntry == NULL)
			return;

		pProbDist = pPDEntry->m_pProbDist;
		ASSERT( pProbDist );	//the pProbDist cannot be null
		CString strDistName = pPDEntry->m_csName;

		pProbDist->printDistribution(szBuffer);

		switch(m_nColumnSel)
		{
		case 1:
			pData->GetWrongAisleDist()->SetProDistrubution(pPDEntry);
			break;
		case 2:
			pData->GetWrongDoorDist()->SetProDistrubution(pPDEntry);
			break;
		case 3:
			pData->GetSeatStagingDist()->SetProDistrubution(pPDEntry);
			break;
		default:
			break;
		}
		m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);

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
		pProbDist = pPDEntry->m_pProbDist;
		ASSERT( pProbDist );	//the pProbDist cannot be null

		CString strDistName = pPDEntry->m_csName;	
		pProbDist->printDistribution(szBuffer);

		switch(m_nColumnSel)
		{
		case 1:
			pData->GetWrongAisleDist()->SetProDistrubution(pPDEntry);
			break;
		case 2:
			pData->GetWrongDoorDist()->SetProDistrubution(pPDEntry);
			break;
		case 3:
			pData->GetSeatStagingDist()->SetProDistrubution(pPDEntry);
			break;
		default:
			break;
		}
		m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);

	} 
}

LRESULT DlgUnexpectedBehaviorVariables::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;

	return NULL;
}

int DlgUnexpectedBehaviorVariables::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		return -1;
	}
	return 0;
}

BOOL DlgUnexpectedBehaviorVariables::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetWindowText("Unexpected Behavior Variables Specification");

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_MAINTENANCETOOLBAR)->GetWindowRect(&rectFltToolbar);
	GetDlgItem(IDC_STATIC_MAINTENANCETOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rectFltToolbar);
	m_wndToolbar.MoveWindow(&rectFltToolbar, true);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, TRUE);
	m_wndToolbar.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONEDIT, TRUE);


	SetResize(m_wndToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_MAINTENANCE, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BTN_MAINTENANCE_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitListCtrl();
	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void DlgUnexpectedBehaviorVariables::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	//get values
	int nCount = 	m_pUnexpectedBehaviorVariablesList->GetCount();
	for (int i =0; i < nCount; i++)
	{
		UnexpectedBehaviorVariables* pData = m_pUnexpectedBehaviorVariablesList->GetVariable(i);
		CString strMobileType = pData->GetPaxType()->PrintStringForShow();
		CString strAisle = pData->GetWrongAisleDist()->getPrintDist();
		CString strDoor = pData->GetWrongDoorDist()->getPrintDist();
		CString strStaging = pData->GetSeatStagingDist()->getPrintDist();

		m_wndListCtrl.InsertItem(i, strMobileType);
		m_wndListCtrl.SetItemText(i, 1, strAisle);
		m_wndListCtrl.SetItemText(i, 2, strDoor);
		m_wndListCtrl.SetItemText(i, 3, strStaging);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pData);
	}

}

InputTerminal* DlgUnexpectedBehaviorVariables::GetInputTerminal()
{
	if (m_pParentWnd == NULL)
		return NULL;

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

void DlgUnexpectedBehaviorVariables::OnNewItem()
{

	UnexpectedBehaviorVariables* pData = new UnexpectedBehaviorVariables(GetInputTerminal()->inStrDict);

	CDlgNewPassengerType dlg(pData->GetPaxType(),GetInputTerminal(),this);
	if (IDOK != dlg.DoModal())
	{
		delete pData;
		return;
	}

	if (m_pUnexpectedBehaviorVariablesList->FindVariable(*(pData->GetPaxType())))
	{
		MessageBox(_T("The passenger type already exists!"), _T("Error"), MB_OK | MB_ICONWARNING);
		delete pData;
		return;
	}
	CString strMobileType = pData->GetPaxType()->PrintStringForShow();

	int nCount = m_wndListCtrl.GetItemCount();

	m_wndListCtrl.InsertItem(nCount, strMobileType);
	//CString strDistType = "Select a probability distribution...";
	m_wndListCtrl.SetItemText(nCount, 1, pData->GetWrongAisleDist()->getPrintDist());
	m_wndListCtrl.SetItemText(nCount, 2, pData->GetWrongDoorDist()->getPrintDist());
	m_wndListCtrl.SetItemText(nCount, 3, pData->GetSeatStagingDist()->getPrintDist());
	m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pData);
	m_pUnexpectedBehaviorVariablesList->AddData(pData);

	m_wndListCtrl.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(TRUE);	
}

void DlgUnexpectedBehaviorVariables::OnDelItem()
{
	int nSel = m_wndListCtrl.GetCurSel();

	if (nSel == -1)
		return;

	UnexpectedBehaviorVariables* pDelData = m_pUnexpectedBehaviorVariablesList->GetVariable(nSel);
	if (pDelData)
		m_pUnexpectedBehaviorVariablesList->DelData(pDelData);

	m_wndListCtrl.DeleteItemEx(nSel);

	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount -1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(TRUE);
}

void DlgUnexpectedBehaviorVariables::OnBnClickedOk()
{
	OnBnClickedSave();
	CXTResizeDialog::OnOK();
}

void DlgUnexpectedBehaviorVariables::OnBnClickedCancel()
{
	CXTResizeDialog::OnCancel();
}

void DlgUnexpectedBehaviorVariables::OnBnClickedSave()
{	
	try
	{
		CADODatabase::BeginTransaction();
		m_pUnexpectedBehaviorVariablesList->SaveData();
		CADODatabase::CommitTransaction() ;
		GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(FALSE);		
	}
	catch (CADOException* error)
	{
		CADODatabase::RollBackTransation() ;
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;

		return;
	}

}