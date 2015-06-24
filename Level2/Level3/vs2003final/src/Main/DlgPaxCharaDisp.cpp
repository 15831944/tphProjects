// DlgPaxCharaDisp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgPaxCharaDisp.h"
#include "../Common/ProbDistManager.h"
#include "../Common/AirportDatabase.h"
#include "../Common/NewPassengerType.h"
#include "../Inputs/IN_TERM.H"
#include "DlgNewPassengerType.h"
#include "../InputOnBoard/PaxPhysicalCharacteristics.h"
#include "DlgProbDist.h"
#include "../Common/WinMsg.h"


// CDlgPaxCharaDisp dialog

IMPLEMENT_DYNAMIC(CDlgPaxCharaDisp, CXTResizeDialog)
CDlgPaxCharaDisp::CDlgPaxCharaDisp(InputTerminal * pInterm,int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPaxCharaDisp::IDD, pParent)
	,m_pInterm(pInterm)
	,m_PaxCharaList(pInterm->inStrDict)
	,m_nProj(nProjID)
{
}

CDlgPaxCharaDisp::~CDlgPaxCharaDisp()
{
}

void CDlgPaxCharaDisp::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PAXDISP, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgPaxCharaDisp, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_PAXDISP_NEW,OnAddPaxType)
	ON_COMMAND(ID_PAXDISP_DEL,OnRemovePaxType)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_PAXDISP,OnEndlabeledit)
	ON_BN_CLICKED(IDC_BTNPAX_SAVE,OnSave)
	ON_MESSAGE(WM_NOEDIT_DBCLICK, OnDBClick)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_PAXDISP,OnSelChangePassengerType)
END_MESSAGE_MAP()


// CDlgPaxCharaDisp message handlers

BOOL CDlgPaxCharaDisp::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_PAXDISP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_PAXDISP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BTNPAX_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	
	SetWindowText("Passenger Physical Characteristics");

	m_PaxCharaList.ReadData(m_nProj);
	OnInitToolbar();
	OnInitListCtrl();
	DisplayData();
	OnUpdateToolbar();
	GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgPaxCharaDisp::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolbar.LoadToolBar(IDR_TOOLBAR_PAXDISP))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgPaxCharaDisp::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_DEL,TRUE);
}

void CDlgPaxCharaDisp::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Passenger Type", "Major (cm)", "Minor (cm)", "Height (cm)" ,\
		"Upper reach (cm)","Arm length reach (cm)","Floor reach (cm)","Back reach (cm)",\
		"Upright shape","Sitting shape","Bending shape","Reaching shape"};
	int DefaultColumnWidth[] = { 120, 120, 120, 120, 120,120,120,120,100,100,100,100};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT, 
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_BMP,
			LVCFMT_BMP,
			LVCFMT_BMP,
			LVCFMT_BMP
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
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 12; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CDlgPaxCharaDisp::OnAddPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_PaxCharaList.getCount();
		for (int i=0; i<nCount; i++)
		{
			PaxPhysicalCharatieristicsItem* pData = m_PaxCharaList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType))
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}

		PaxPhysicalCharatieristicsItem* pItem = new PaxPhysicalCharatieristicsItem(m_pInterm->inStrDict);
		pItem->setMobileElemCnt(paxType);
		m_PaxCharaList.addItem(pItem);
		nCount = m_wndListCtrl.GetItemCount();
		m_wndListCtrl.InsertPaxDispItem(nCount,pItem);
		m_wndListCtrl.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgPaxCharaDisp::OnRemovePaxType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		PaxPhysicalCharatieristicsItem* pItem = (PaxPhysicalCharatieristicsItem*)m_wndListCtrl.GetItemData(nSel);
		m_PaxCharaList.removeItem(pItem);
		m_wndListCtrl.DeleteItem(nSel);
		GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgPaxCharaDisp::DisplayData()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = 0;
	for (int i = 0; i < (int)m_PaxCharaList.getCount(); i++)
	{
		PaxPhysicalCharatieristicsItem* pItem = m_PaxCharaList.getItem(i);
		nCount = m_wndListCtrl.GetItemCount();
		m_wndListCtrl.InsertPaxDispItem(nCount,pItem);
	}
}

void CDlgPaxCharaDisp::OnOK()
{
	OnSave() ;
	CDialog::OnOK();
}

void CDlgPaxCharaDisp::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_PaxCharaList.SaveData(m_nProj);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(FALSE);
}

void CDlgPaxCharaDisp::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;
	if(!dispinfo)
		return;

	PaxPhysicalCharatieristicsItem* pItem = (PaxPhysicalCharatieristicsItem*)m_wndListCtrl.GetItemData(dispinfo->item.iItem);
	if(!pItem)return;
	// TODO: Add your control notification handler code here	 
	CString strSel;
	strSel = dispinfo->item.pszText;

	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInterm->m_pAirportDB->getProbDistMan();
	if(strSel == _T("New Probability Distribution..." ) )
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

			if(dispinfo->item.iSubItem == 1)
			{
				pItem->setMajor(pPDEntry);
			}
			else if(dispinfo->item.iSubItem == 2)
			{
				pItem->setMinor(pPDEntry);	
			}
			else if (dispinfo->item.iSubItem == 3)
			{
				pItem->setHeight(pPDEntry);
			}
			else if (dispinfo->item.iSubItem == 4)
			{
				pItem->setUppReach(pPDEntry);
			}
			else if (dispinfo->item.iSubItem == 5)
			{
				pItem->setArmLength(pPDEntry);
			}
			else if (dispinfo->item.iSubItem == 6)
			{
				pItem->setFloorReach(pPDEntry);
			}
			else if (dispinfo->item.iSubItem == 7)
			{
				pItem->setBackReach(pPDEntry);
			}
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

		if(dispinfo->item.iSubItem == 1)
		{
			pItem->setMajor(pPDEntry);
		}
		else if(dispinfo->item.iSubItem == 2)
		{
			pItem->setMinor(pPDEntry);	
		}
		else if (dispinfo->item.iSubItem == 3)
		{
			pItem->setHeight(pPDEntry);
		}
		else if (dispinfo->item.iSubItem == 4)
		{
			pItem->setUppReach(pPDEntry);
		}
		else if (dispinfo->item.iSubItem == 5)
		{
			pItem->setArmLength(pPDEntry);
		}
		else if (dispinfo->item.iSubItem == 6)
		{
			pItem->setFloorReach(pPDEntry);
		}
		else if (dispinfo->item.iSubItem == 7)
		{
			pItem->setBackReach(pPDEntry);
		} 
	}
	GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
}

LRESULT CDlgPaxCharaDisp::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	int nItem = (int)wParam;
	PaxPhysicalCharatieristicsItem* pItem = (PaxPhysicalCharatieristicsItem*)m_wndListCtrl.GetItemData(nItem);
	if (NULL == pItem)
	{
		return 0;
	}

	CPassengerType paxType = pItem->getMobileElemCnt();
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_PaxCharaList.getCount();
		for (int i=0; i<nCount; i++)
		{
			PaxPhysicalCharatieristicsItem* pData = m_PaxCharaList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType) && pItem != pData)
			{
				MessageBox(_T("The Passenger Type already exists!."));

				return FALSE;
			}
		}

		pItem->setMobileElemCnt(paxType);
		DisplayData();
		GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
	}
	return TRUE;
}

void CDlgPaxCharaDisp::OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnUpdateToolbar();
}

void CDlgPaxCharaDisp::OnUpdateToolbar()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_DEL,TRUE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_DEL,FALSE);
	}
}