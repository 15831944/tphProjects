// DlgTargetLocations.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgTargetLocations.h"
#include "../Common/ProbDistManager.h"
#include "../Common/AirportDatabase.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewPassengerType.h"
#include "../Inputs/IN_TERM.H"
#include "DlgProbDist.h"
#include "../Common/WinMsg.h"

// CDlgTargetLocations dialog

IMPLEMENT_DYNAMIC(CDlgTargetLocations, CXTResizeDialog)
CDlgTargetLocations::CDlgTargetLocations(InputTerminal * pInterm,int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTargetLocations::IDD, pParent)
	,m_pInterm(pInterm)
	,m_nPorjID(nProjID)
{
}

CDlgTargetLocations::~CDlgTargetLocations()
{
}

void CDlgTargetLocations::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgTargetLocations, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnSelChangePassengerType)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_DATA,OnEndlabeledit)
	ON_COMMAND(ID_MODEL_NEW,OnAddPaxType)
	ON_COMMAND(ID_MODEL_DELETE,OnRemovePaxType)
	ON_COMMAND(ID_MODEL_EDIT,OnEditPaxType)
	ON_MESSAGE(WM_NOEDIT_DBCLICK, OnDBClick)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
END_MESSAGE_MAP()


// CDlgTargetLocations message handlers
BOOL CDlgTargetLocations::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_targetLocationList.ReadData(m_nPorjID,m_pInterm->inStrDict);
	OnInitToolbar();
	OnInitListCtrl();
	DisplayData();
	OnUpdateToolbar();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgTargetLocations::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolbar.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgTargetLocations::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
}

void CDlgTargetLocations::OnOK()
{
	OnSave() ;
	CDialog::OnOK();
}

void CDlgTargetLocations::DisplayData()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_targetLocationList.getCount();
	for (int i = 0; i < nCount; i++)
	{
		CTargetLocationsItem* pItem = m_targetLocationList.getItem(i);
		m_wndListCtrl.InsertItem(i,pItem->getMobileElemCnt().PrintStringForShow());
		CString strValue = _T("");
		strValue.Format(_T("%d"),pItem->getUnderShoot());
		m_wndListCtrl.SetItemText(i,1,strValue);
		strValue.Format(_T("%d"),pItem->getOnTarget());
		m_wndListCtrl.SetItemText(i,2,strValue);
		strValue.Format(_T("%d"),pItem->getOverShoot());
		m_wndListCtrl.SetItemText(i,3,strValue);
		strValue.Format(_T("%d"),pItem->getSeatTaging());
		m_wndListCtrl.SetItemText(i,4,strValue);
		m_wndListCtrl.SetItemText(i,5,pItem->getOffTargetDist().getPrintDist());
		m_wndListCtrl.SetItemText(i,6,pItem->getSearchRadiusDist().getPrintDist());
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pItem);
	}
}

void CDlgTargetLocations::OnSave()
{
	if (m_targetLocationList.checkSum())
	{
		try
		{
			CADODatabase::BeginTransaction() ;
            m_targetLocationList.SaveData(m_nPorjID);
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException e)
		{
			CADODatabase::RollBackTransation() ;
		}
		
	}
	else
	{
		::AfxMessageBox("Make sure the sum of percent is 100%");
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgTargetLocations::OnUpdateToolbar()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
	}
}

void CDlgTargetLocations::OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	OnUpdateToolbar();
}

void CDlgTargetLocations::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Passenger Type","P(undershoot)", "P(on target)","P(overshoot)","P(seat staging)",\
		"Off target Dist(m)","Search Radius(m)"};
	int DefaultColumnWidth[] = { 150,80,80,80,80,160,160};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_EDIT,
			LVCFMT_EDIT,
			LVCFMT_EDIT,
			LVCFMT_EDIT,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN
	};

	CStringList strList;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pInterm)
		pProbDistMan = m_pInterm->m_pAirportDB->getProbDistMan();
	strList.AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strList.AddTail(pPDEntry->m_csName);
	}

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 7; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.csList = &strList;
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CDlgTargetLocations::OnAddPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_targetLocationList.getCount();
		for (int i=0; i<nCount; i++)
		{
			CTargetLocationsItem* pData = m_targetLocationList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType))
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}

		CTargetLocationsItem* pItem = new CTargetLocationsItem(m_pInterm->inStrDict);
		pItem->setMobileElemCnt(paxType);
		m_targetLocationList.addItem(pItem);
		nCount = m_wndListCtrl.GetItemCount();

		CString strValue = _T("");
		m_wndListCtrl.InsertItem(nCount,pItem->getMobileElemCnt().PrintStringForShow());
		strValue.Format(_T("%d"),pItem->getUnderShoot());
		m_wndListCtrl.SetItemText(nCount,1,strValue);
		strValue.Format(_T("%d"),pItem->getOnTarget());
		m_wndListCtrl.SetItemText(i,2,strValue);
		strValue.Format(_T("%d"),pItem->getOverShoot());
		m_wndListCtrl.SetItemText(nCount,3,strValue);
		strValue.Format(_T("%d"),pItem->getSeatTaging());
		m_wndListCtrl.SetItemText(nCount,4,strValue);
		m_wndListCtrl.SetItemText(nCount,5,pItem->getOffTargetDist().getPrintDist());
		m_wndListCtrl.SetItemText(nCount,6,pItem->getSearchRadiusDist().getPrintDist());
		m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pItem);
		m_wndListCtrl.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgTargetLocations::OnRemovePaxType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CTargetLocationsItem* pItem = (CTargetLocationsItem*)m_wndListCtrl.GetItemData(nSel);
		m_targetLocationList.removeItem(pItem);
		m_wndListCtrl.DeleteItem(nSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgTargetLocations::OnEditPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	int nItem = m_wndListCtrl.GetCurSel();
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_targetLocationList.getCount();
		CTargetLocationsItem* pItem = (CTargetLocationsItem*)m_wndListCtrl.GetItemData(nItem);
		for (int i=0; i<nCount; i++)
		{
			CTargetLocationsItem* pData = m_targetLocationList.getItem(i);
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

LRESULT CDlgTargetLocations::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	int nItem = (int)wParam;
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_targetLocationList.getCount();
		CTargetLocationsItem* pItem = (CTargetLocationsItem*)m_wndListCtrl.GetItemData(nItem);
		for (int i=0; i<nCount; i++)
		{
			CTargetLocationsItem* pData =m_targetLocationList.getItem(i);
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

void CDlgTargetLocations::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;

	CTargetLocationsItem* pItem = (CTargetLocationsItem*)m_wndListCtrl.GetItemData(dispinfo->item.iItem);
	if(!pItem)return;
	CString strSel;
	strSel = dispinfo->item.pszText;
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInterm->m_pAirportDB->getProbDistMan();
	switch(dispinfo->item.iSubItem)
	{
	case 1:
		{
			pItem->setUnderShoot(atoi(strSel));
		}
		break;
	case 2:
		{
			pItem->setOnTarget(atoi(strSel));
		}
		break;
	case 3:
		{
			pItem->setOverShoot(atoi(strSel));
		}
		break;
	case 4:
		{
			pItem->setSeatTaging(atoi(strSel));
		}
		break;
	case 5:
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

					pItem->setOffTargetDist(pPDEntry);
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
				pItem->setOffTargetDist(pPDEntry);
			}
		}
		break;
	case 6:
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

					pItem->setSearchRadiusDist(pPDEntry);
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
				pItem->setSearchRadiusDist(pPDEntry);
				m_wndListCtrl.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,pPDEntry->m_csName);
			}
		}
	default:
		break;
	}
	if(!pItem->checkSum())
	{
		::AfxMessageBox("Make sure the sum of percent is 100%");
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}