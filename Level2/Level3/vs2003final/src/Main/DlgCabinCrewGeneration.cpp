#include "StdAfx.h"
#include ".\dlgcabincrewgeneration.h"
#include "../Common/ProbDistManager.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/CabinCrewGeneration.h"
#include "DlgProbDist.h"
#include "../Common/WinMsg.h"
#include "../Database/ADODatabase.h"
#include "TermPlanDoc.h"
#include "../InputOnBoard/CInputOnboard.h"
IMPLEMENT_DYNAMIC(DlgCabinCrewGeneration, CXTResizeDialog)
DlgCabinCrewGeneration::DlgCabinCrewGeneration(InputTerminal * pInterm,CTermPlanDoc* pDoc ,CWnd* pParent /*=NULL*/)
: CXTResizeDialog(DlgCabinCrewGeneration::IDD, pParent)
,m_pInterm(pInterm)
{
	try
	{	
		m_pCabinCrewGeneration =pDoc->GetInputOnboard()->GetCrewGenerater();
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pCabinCrewGeneration)
		{
			delete m_pCabinCrewGeneration;
			m_pCabinCrewGeneration = NULL;
		}
		return;
	}
}

DlgCabinCrewGeneration::~DlgCabinCrewGeneration(void)
{

}

void DlgCabinCrewGeneration::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PAXDISP, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(DlgCabinCrewGeneration, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_PAXDISP_NEW,OnAddCrew)
	ON_COMMAND(ID_PAXDISP_DEL,OnRemoveCrew)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_PAXDISP,OnEndlabeledit)
	ON_BN_CLICKED(IDC_BTNPAX_SAVE,OnSave)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_PAXDISP,OnSelChangeListCtrl)
END_MESSAGE_MAP()


// CDlgPaxCharaDisp message handlers

BOOL DlgCabinCrewGeneration::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_PAXDISP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_PAXDISP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BTNPAX_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetWindowText("Cabin Crew Generation");
	
	OnInitToolbar();
	OnInitListCtrl();
	DisplayData();
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PAXDISP_DEL,FALSE);

	GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int DlgCabinCrewGeneration::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void DlgCabinCrewGeneration::OnInitToolbar()
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

void DlgCabinCrewGeneration::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Rank","ID","M/F", "Visibility(m)","Aware", "Major (cm)", "Minor (cm)", "Height (cm)" ,\
		"Upper reach (cm)","Arm length reach (cm)","Floor reach (cm)","Back reach (cm)",\
		"Upright shape","Sitting shape","Bending shape","Reaching shape"};

	int DefaultColumnWidth[] = { 120, 80, 80, 80, 80, 120, 120, 120, 120, 120, 120, 120, 100, 100, 100, 100 };

	int nColFormat[] = 
	{	
			LVCFMT_EDIT,
			LVCFMT_NUMBER,
			LVCFMT_DROPDOWN,
			LVCFMT_NUMBER,
			LVCFMT_DROPDOWN,
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

	CStringList strContent[5];
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	strContent[2].AddTail("F");
	strContent[2].AddTail("M");

	strContent[4].AddTail("Yes");
	strContent[4].AddTail("No");

	for (int i =0; i <5; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.csList = &strContent[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}

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
	lvc.csList = &strList;

	for (int i = 5; i < 16; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void DlgCabinCrewGeneration::OnAddCrew()
{
	CabinCrewPhysicalCharacteristics* pItem = new CabinCrewPhysicalCharacteristics;
	int nCount = m_wndListCtrl.GetItemCount();
	CString strRank;
	strRank.Format("Rank%d",nCount+1);
	pItem->setRank(strRank);
	pItem->setID(nCount+1);
	m_pCabinCrewGeneration->AddData(pItem);
	m_wndListCtrl.InsertCrewDispItem(nCount,pItem);
	m_wndListCtrl.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
	GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
}

void DlgCabinCrewGeneration::OnRemoveCrew()
{
	if(m_wndListCtrl.GetItemCount() ==0)
		return;

	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CabinCrewPhysicalCharacteristics* pItem = (CabinCrewPhysicalCharacteristics*)m_wndListCtrl.GetItemData(nSel);
		m_pCabinCrewGeneration->DelData(pItem);
		m_wndListCtrl.DeleteItem(nSel);
		GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
	}
}

void DlgCabinCrewGeneration::DisplayData()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_pCabinCrewGeneration->getDataCount();
	for (int i = 0; i < nCount; i++)
	{
		CabinCrewPhysicalCharacteristics* pItem = m_pCabinCrewGeneration->getData(i);
		int nIdx = m_wndListCtrl.GetItemCount();
		m_wndListCtrl.InsertCrewDispItem(nIdx,pItem);
	}
}

void DlgCabinCrewGeneration::OnOK()
{
	OnSave();
	CDialog::OnOK();
}

void DlgCabinCrewGeneration::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pCabinCrewGeneration->SaveData();
		CADODatabase::CommitTransaction() ;
		GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(FALSE);
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

void DlgCabinCrewGeneration::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	if(!dispinfo)
		return;

	// TODO: Add your control notification handler code here

	int nItem = dispinfo->item.iItem;
	int nSubItem = dispinfo->item.iSubItem;
	if ( nItem < 0|| nSubItem >11)
		return;

	if (nSubItem == 0 || nSubItem == 1 || nSubItem == 3)
	{
		OnListItemChanged(nItem, nSubItem);
		GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
		*pResult = 0;
		return;
	}

	CabinCrewPhysicalCharacteristics* pItem = (CabinCrewPhysicalCharacteristics*)m_wndListCtrl.GetItemData(nItem);
	if(!pItem)
		return;

	CString strSel;
	strSel = dispinfo->item.pszText;

	if (nSubItem == 2 )
	{
		Sex eSex;
		if (!strSel.IsEmpty() && !strSel.CompareNoCase(_T("F")))
			eSex = Female;
		else 
			eSex = Male;
		pItem->setSex(eSex);

		*pResult = 0;			
		return;
	}

	if (nSubItem == 4 )
	{
		Awareness eAware;
		if (!strSel.IsEmpty() && !strSel.CompareNoCase(_T("Yes")))
			eAware = YES;
		else 
			eAware = NO;
		pItem->setAwareness(eAware);

		*pResult = 0;			
		return;
	}

	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInterm->m_pAirportDB->getProbDistMan();
	CProbDistEntry* pPDEntry = NULL;

	if(strSel == _T("New Probability Distribution..." ) )
	{
		CDlgProbDist dlg(m_pInterm->m_pAirportDB, false);
		if(dlg.DoModal() != IDOK)
			return;

		pPDEntry = dlg.GetSelectedPD();
		if(pPDEntry == NULL)
			return;
	}
	else
	{
		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry->m_csName == strSel)
				break;
		}
	}

	switch(nSubItem)
	{
	case 5:
		pItem->setMajor(pPDEntry);
		break;
	case 6:
		pItem->setMinor(pPDEntry);	
		break;
	case 7:
		pItem->setHeight(pPDEntry);
		break;
	case 8:
		pItem->setUppReach(pPDEntry);
		break;
	case 9:
		pItem->setArmLength(pPDEntry);
		break;
	case 10:
		pItem->setFloorReach(pPDEntry);
		break;
	case 11:
		pItem->setBackReach(pPDEntry);
		break;
	default:
		break;
	}

	GetDlgItem(IDC_BTNPAX_SAVE)->EnableWindow(TRUE);
}

void DlgCabinCrewGeneration::OnListItemChanged(int nItem, int nSubItem)
{
	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	CabinCrewPhysicalCharacteristics* pData = (CabinCrewPhysicalCharacteristics*)m_wndListCtrl.GetItemData(nItem);

	if (nSubItem == 0)
		pData->setRank(strCurVal);

	int dCurVal = atoi(strCurVal.GetBuffer(0));	
	strCurVal.ReleaseBuffer();

	if (nSubItem == 1)
		pData->setID(dCurVal);

	if (nSubItem == 3)
		pData->setVisibility(dCurVal);

}

void DlgCabinCrewGeneration::OnSelChangeListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnUpdateToolbar();
}

void DlgCabinCrewGeneration::OnUpdateToolbar()
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