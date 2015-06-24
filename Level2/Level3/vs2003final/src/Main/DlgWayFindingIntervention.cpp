// DlgWayFindingIntervention.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgWayFindingIntervention.h"
#include ".\dlgwayfindingintervention.h"
#include "../Common/ProbDistManager.h"
#include "../Common/AirportDatabase.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewPassengerType.h"
#include "../Inputs/IN_TERM.H"
#include "DlgProbDist.h"
#include "../Common/WinMsg.h"


// CDlgWayFindingIntervention dialog

IMPLEMENT_DYNAMIC(CDlgWayFindingIntervention, CXTResizeDialog)
CDlgWayFindingIntervention::CDlgWayFindingIntervention(InputTerminal * pInterm,int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgWayFindingIntervention::IDD, pParent)
	,m_pInterm(pInterm)
	,m_nProjID(nProjID)
{

}

CDlgWayFindingIntervention::~CDlgWayFindingIntervention()
{
}

void CDlgWayFindingIntervention::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgWayFindingIntervention, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListData)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_DATA,OnEndlabeledit)
	ON_COMMAND(ID_MODEL_NEW,OnAddPaxType)
	ON_COMMAND(ID_MODEL_DELETE,OnRemovePaxType)
	ON_COMMAND(ID_MODEL_EDIT,OnEditPaxType)
	ON_MESSAGE(WM_NOEDIT_DBCLICK, OnDBClick)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
END_MESSAGE_MAP()


// CDlgWayFindingIntervention message handlers
BOOL CDlgWayFindingIntervention::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	OnInitToolbar();
	m_wayFindingInterventionList.ReadData(m_nProjID, m_pInterm->inStrDict);
	OnInitListCtrl();
	DisplayData();
	OnUpdateToolbar();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgWayFindingIntervention::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS ) ||
		!m_wndToolBar.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgWayFindingIntervention::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolBar.MoveWindow(&rect,true);
	m_wndToolBar.ShowWindow(SW_SHOW);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
}

void CDlgWayFindingIntervention::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Passenger Type","Crew Specific", "Leader Only", "Type","Duration"};
	int DefaultColumnWidth[] = { 150,100, 100,120, 160};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN
	};

	CStringList strList[5];
	strList[1].AddTail(_T("Yes"));
	strList[1].AddTail(_T("No"));

	strList[2].AddTail(_T("Yes"));
	strList[2].AddTail(_T("No"));

	strList[3].AddTail(_T("To seat"));
	strList[3].AddTail(_T("Interaction"));

	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pInterm)
		pProbDistMan = m_pInterm->m_pAirportDB->getProbDistMan();
	strList[4].AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strList[4].AddTail(pPDEntry->m_csName);
	}

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 5; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.csList = &strList[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CDlgWayFindingIntervention::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	OnUpdateToolbar();
}

void CDlgWayFindingIntervention::OnAddPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_wayFindingInterventionList.getCount();
		for (int i=0; i<nCount; i++)
		{
			CWayFindingInterventionItem* pData = m_wayFindingInterventionList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType))
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}

		CWayFindingInterventionItem* pItem = new CWayFindingInterventionItem(m_pInterm->inStrDict);
		pItem->setMobileElemCnt(paxType);
		m_wayFindingInterventionList.addItem(pItem);
		nCount = m_wndListCtrl.GetItemCount();
		m_wndListCtrl.InsertItem(nCount,pItem->getMobileElemCnt().PrintStringForShow());
		m_wndListCtrl.SetItemText(nCount,1,getString(pItem->getCrewSpecific()));
		m_wndListCtrl.SetItemText(nCount,2,getString(pItem->getLeaderOnly()));
		m_wndListCtrl.SetItemText(nCount,3,getTypeString(pItem->getType()));
		m_wndListCtrl.SetItemText(nCount,4,pItem->getDuration().getPrintDist());
		m_wndListCtrl.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgWayFindingIntervention::OnRemovePaxType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CWayFindingInterventionItem* pItem = (CWayFindingInterventionItem*)m_wndListCtrl.GetItemData(nSel);
		m_wayFindingInterventionList.removeItem(pItem);
		m_wndListCtrl.DeleteItem(nSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgWayFindingIntervention::OnEditPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	int nItem = m_wndListCtrl.GetCurSel();
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_wayFindingInterventionList.getCount();
		CWayFindingInterventionItem* pItem = (CWayFindingInterventionItem*)m_wndListCtrl.GetItemData(nItem);
		for (int i=0; i<nCount; i++)
		{
			CWayFindingInterventionItem* pData = m_wayFindingInterventionList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType) && pItem != pData)
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}

		pItem->setMobileElemCnt(paxType);
		DisplayData();
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgWayFindingIntervention::DisplayData()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_wayFindingInterventionList.getCount();
	for (int i = 0; i < nCount; i++)
	{
		CWayFindingInterventionItem* pItem = m_wayFindingInterventionList.getItem(i);
		m_wndListCtrl.InsertItem(i,pItem->getMobileElemCnt().PrintStringForShow());
		m_wndListCtrl.SetItemText(i,1,getString(pItem->getCrewSpecific()));
		m_wndListCtrl.SetItemText(i,2,getString(pItem->getLeaderOnly()));
		m_wndListCtrl.SetItemText(i,3,getTypeString(pItem->getType()));
		m_wndListCtrl.SetItemText(i,4,pItem->getDuration().getPrintDist());
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pItem);
	}
}

void CDlgWayFindingIntervention::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_wayFindingInterventionList.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CXTResizeDialog::OnOK();
}

CString CDlgWayFindingIntervention::getString(BOOL bSelect)
{
	if (bSelect)
	{
		return _T("Yes");
	}
	return _T("No");
}

BOOL CDlgWayFindingIntervention::getValueFromCString(CString strValue)
{
	if (!strValue.IsEmpty() && !strValue.CompareNoCase(_T("Yes")))
	{
		return TRUE;
	}
	return FALSE;
}
PaxType CDlgWayFindingIntervention::getPaxTypeFromCString(CString strPaxType)
{
	if (!strPaxType.IsEmpty() && !strPaxType.CompareNoCase(_T("To seat")))
	{
		return TOSEAT;
	}
	return INTERACTION;
}

CString CDlgWayFindingIntervention::getTypeString(PaxType emPaxType)
{
	CString szData = _T("");
	switch(emPaxType)
	{
	case TOSEAT:
		szData = _T("To seat");
		break;
	case INTERACTION:
		szData = _T("Interaction");
		break;
	default:
	    break;
	}
	return szData;
}

void CDlgWayFindingIntervention::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;

	CWayFindingInterventionItem* pItem = (CWayFindingInterventionItem*)m_wndListCtrl.GetItemData(dispinfo->item.iItem);
	if(!pItem)return;
	BOOL bTrue;
	CString strSel;
	strSel = dispinfo->item.pszText;
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInterm->m_pAirportDB->getProbDistMan();
	switch(dispinfo->item.iSubItem)
	{
	case 1:
		{
			bTrue = getValueFromCString(strSel);
			pItem->setCrewSpecific(bTrue);
		}
		break;
	case 2:
		{
			bTrue = getValueFromCString(strSel);
			pItem->setLeaderOnly(bTrue);
		}
	    break;
	case 3:
		{
			PaxType emPaxType = getPaxTypeFromCString(strSel);
			pItem->setType(emPaxType);
		}
		break;
	case 4:
		{
			if (strSel == _T("New Probability Distribution..." ) )
			{
				CProbDistEntry* pPDEntry = NULL;
				CDlgProbDist dlg(m_pInterm->m_pAirportDB, false);
				if(dlg.DoModal() == IDOK)
				{
					CProbDistEntry* pPDEntry = NULL;
					pPDEntry = dlg.GetSelectedPD();
					if(pPDEntry == NULL)
					{
						return;
					}

					pItem->setDuration(pPDEntry);
					m_wndListCtrl.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,pPDEntry->m_csName);
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
				pItem->setDuration(pPDEntry);
				m_wndListCtrl.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,pPDEntry->m_csName);
			}
		}
	default:
	    break;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}
LRESULT CDlgWayFindingIntervention::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	int nItem = (int)wParam;
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_wayFindingInterventionList.getCount();
		CWayFindingInterventionItem* pItem = (CWayFindingInterventionItem*)m_wndListCtrl.GetItemData(nItem);
		for (int i=0; i<nCount; i++)
		{
			CWayFindingInterventionItem* pData =m_wayFindingInterventionList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType) && pItem != pData)
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return FALSE;
			}
		}

		pItem->setMobileElemCnt(paxType);
		DisplayData();
	}
	return TRUE;
}

void CDlgWayFindingIntervention::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_wayFindingInterventionList.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgWayFindingIntervention::OnUpdateToolbar()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
	}
}