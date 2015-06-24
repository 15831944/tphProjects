#include "StdAfx.h"
#include "resource.h"
#include ".\dlgkineticsandspace.h"
#include "../InputOnBoard/KineticsAndSpace.h"
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

IMPLEMENT_DYNAMIC(DlgKineticsAndSpace, CXTResizeDialog)
DlgKineticsAndSpace::DlgKineticsAndSpace(int nPrjID, KineticsAndSpaceDataType eDataType, CWnd* pParent/* = NULL*/)
:CXTResizeDialog(DlgKineticsAndSpace::IDD, pParent)
,m_nProjID(nPrjID)
,m_pParentWnd(pParent)
{
	try
	{
		m_pKineticsAndSpaceDataList = new KineticsAndSpaceDataList;
		m_pKineticsAndSpaceDataList->ReadData(nPrjID,eDataType,GetInputTerminal()->inStrDict);		
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pKineticsAndSpaceDataList)
		{
			delete m_pKineticsAndSpaceDataList;
			m_pKineticsAndSpaceDataList = NULL;
		}
		return;
	}

	m_strCaption = getDlgTitleAndUnit(eDataType);
}

DlgKineticsAndSpace::~DlgKineticsAndSpace(void)
{
	delete m_pKineticsAndSpaceDataList;
	m_pKineticsAndSpaceDataList = NULL;
}

void DlgKineticsAndSpace::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAINTENANCE, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(DlgKineticsAndSpace, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnNewItem)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDelItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MAINTENANCE_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL,OnBnClickedCancel)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_MAINTENANCE,OnSelComboBox)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)

END_MESSAGE_MAP()


void DlgKineticsAndSpace::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);
	CStringList strlist;	
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;

	//mobile element type
	lvc.pszText = "Passenger Type";
	lvc.cx = 200;
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
	CString strTitle = "Probability Distribution " + m_strUnit;
	lvc.pszText  = (LPSTR) ((LPCTSTR)strTitle);
	lvc.cx = 300;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;
	m_wndListCtrl.InsertColumn(1, &lvc);

	SetListContent();

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(FALSE);
}

void  DlgKineticsAndSpace::OnSelComboBox(NMHDR*   pNMHDR,   LRESULT*   pResult)     
{     	

	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	if(m_nColumnSel == 0)
		return;
	*pResult = 0;
	if(!dispinfo)
		return;

	// TODO: Add your control notification handler code here
	KineticsAndSpaceData* pData = m_pKineticsAndSpaceDataList->GetDataByIdx(m_nRowSel);
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
			CString strDistName = pData->GetDataProbDistribution()->getPrintDist() ;
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
			return;
		}

		CProbDistEntry* pPDEntry = dlg.GetSelectedPD();

		if(pPDEntry == NULL)
		{
			m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,pData->GetDataProbDistribution()->getDistName());
			return;
		}
		pProbDist = pPDEntry->m_pProbDist;
		ASSERT( pProbDist );	//the pProbDist cannot be null
		CString strDistName = pPDEntry->m_csName;

		pProbDist->printDistribution(szBuffer);
		
		pData->GetDataProbDistribution()->SetProDistrubution(pPDEntry);
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

		pData->GetDataProbDistribution()->SetProDistrubution(pPDEntry);
		m_wndListCtrl.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
		
	} 
	//SetListContent();
}

LRESULT DlgKineticsAndSpace::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;

	return NULL;
}

int DlgKineticsAndSpace::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndFltToolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		return -1;
	}
	return 0;
}

BOOL DlgKineticsAndSpace::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetWindowText(m_strCaption);

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_MAINTENANCETOOLBAR)->GetWindowRect(&rectFltToolbar);
	GetDlgItem(IDC_STATIC_MAINTENANCETOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rectFltToolbar);
	m_wndFltToolbar.MoveWindow(&rectFltToolbar, true);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, TRUE);
	m_wndFltToolbar.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONEDIT, TRUE);


	SetResize(m_wndFltToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_BARFRAME, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
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

void DlgKineticsAndSpace::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	//get values
	int nCount = m_pKineticsAndSpaceDataList->GetDataCount();
	for (int i =0; i < nCount; i++)
	{
		KineticsAndSpaceData* pData = m_pKineticsAndSpaceDataList->GetDataByIdx(i);
		CString strMobileType = pData->GetMobileElemType()->PrintStringForShow();
		CString strDistName = pData->GetDataProbDistribution()->getPrintDist();
		m_wndListCtrl.InsertItem(i, strMobileType);
		m_wndListCtrl.SetItemText(i, 1, strDistName);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pData);
	}

}

InputTerminal* DlgKineticsAndSpace::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

void DlgKineticsAndSpace::OnNewItem()
{
	KineticsAndSpaceDataType eType = m_pKineticsAndSpaceDataList->GetDataType();
	KineticsAndSpaceData* pData = new KineticsAndSpaceData(eType,GetInputTerminal()->inStrDict);
	CDlgNewPassengerType dlg(pData->GetMobileElemType(),GetInputTerminal(),this);
	if (IDOK != dlg.DoModal())
	{
		delete pData;
		return;
	}

	// check duplication
	if (m_pKineticsAndSpaceDataList->findSameData(*(pData->GetMobileElemType())))
	{
		MessageBox(_T("The passenger type has already been defined!"), "Error", MB_OK | MB_ICONWARNING);
		delete pData;
		return;
	}

	CString strMobileType = pData->GetMobileElemType()->PrintStringForShow();

	int nCount = m_wndListCtrl.GetItemCount();

	m_wndListCtrl.InsertItem(nCount, strMobileType);
	m_wndListCtrl.SetItemText(nCount, 1, pData->GetDataProbDistribution()->getPrintDist());
	m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pData);
	m_pKineticsAndSpaceDataList->AddData(pData);

	m_wndListCtrl.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(TRUE);	
}

void DlgKineticsAndSpace::OnDelItem()
{
	int nSel =  -1;

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		nSel = m_wndListCtrl.GetNextSelectedItem(pos);

	if (nSel == -1)
		return;
	
	KineticsAndSpaceData* pDelData = m_pKineticsAndSpaceDataList->GetDataByIdx(nSel);
	if (pDelData)
		m_pKineticsAndSpaceDataList->DelData(pDelData);

	m_wndListCtrl.DeleteItemEx(nSel);

	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
		m_wndListCtrl.SetItemState(nCount -1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(TRUE);
}

void DlgKineticsAndSpace::OnBnClickedOk()
{
	OnBnClickedSave();
	CXTResizeDialog::OnOK();
}

void DlgKineticsAndSpace::OnBnClickedCancel()
{
	CXTResizeDialog::OnCancel();
}

void DlgKineticsAndSpace::OnBnClickedSave()
{	
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pKineticsAndSpaceDataList->SaveData();
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

CString DlgKineticsAndSpace::getDlgTitleAndUnit(KineticsAndSpaceDataType eDataType)
{
	CString strTiTle = _T("");
	switch(eDataType)
	{
		case Speed:
			{
				strTiTle = "Speed";
				m_strUnit = "(m/s)";
				break;
			}

		case FreeInStep:
			{
				strTiTle = "Free In Step";
				m_strUnit = "(m)";
				break;
			}

		case CongestionInStep:
			{
				strTiTle = "Congestion In Step";
				m_strUnit = "(m)";
				break;
			}

		case QueueInStep:
			{
				strTiTle = "Queue In Step";
				m_strUnit = "(m)";
				break;
			}

		case FreeSideStep:
			{
				strTiTle = "Free Side Step";
				m_strUnit = "(m)";
				break;
			}

		case CongestionSideStep:
			{
				strTiTle = "Congestion Side Step";
				m_strUnit = "(m)";
				break;
			}

		case QueueSideStep:
			{
				strTiTle = "Queue Side Step";
				m_strUnit = "(m)";
				break;
			}

		case InteractionDistance:
			{
				strTiTle = "Interaction Distance";
				m_strUnit = "(m)";
				break;
			}

		case AffinityFreeDistance:
			{
				strTiTle = "Affinity Free Distance";
				m_strUnit = "(m)";
				break;
			}

		case AffinityCongestionDistance:
			{
				strTiTle = "Affinity Congestion Distance";
				m_strUnit = "(m)";
				break;
			}

		case Squeezability:
			{
				strTiTle = "Squeezability";
				m_strUnit = "(%)";
				break;
			}

		default:
			break;

	}

	return strTiTle;
}