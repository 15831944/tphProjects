// ArrivalETAOffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ArrivalETAOffsetDlg.h"
#include "../InputAirside/ArrivalETAOffsetList.h"
#include "../InputAirside/ArrivalETAOffset.h"
#include "../InputAirside/DistanceAndTimeFromARP.h"
#include "../Common/ProbDistManager.h"
#include "../Common/WinMsg.h"
#include "../Database/DBElementCollection_Impl.h"


namespace
{
	const UINT ID_NEW_DISANDTIMEFROMARPITEM = 10;
	const UINT ID_DEL_DISANDTIMEFROMARPITEM = 11;

	const UINT ID_NEW_FLIGHTTYPEITEM  = 20;
	const UINT ID_DEL_FLIGHTTYPEITEM  = 21;
	const UINT ID_EDIT_FLIGHTTYPEITEM = 22;
}

// CArrivalETAOffsetDlg dialog

IMPLEMENT_DYNAMIC(CArrivalETAOffsetDlg, CXTResizeDialog)
CArrivalETAOffsetDlg::CArrivalETAOffsetDlg(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CAirportDatabase *pAirPortdb, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CArrivalETAOffsetDlg::IDD, pParent)
	, m_pSelectFlightType(pSelectFlightType)
	, m_nProjID(nProjID)
	, m_pSelectProbDistEntry(pSelectProbDistEntry)
	, m_pAirportDB(pAirPortdb)
	, m_pInputAirside(pInputAirside)
{
	m_pArrivalETAOffsetList = new CArrivalETAOffsetList;
	m_pArrivalETAOffsetList->SetAirportDatabase(pAirPortdb);
	m_pArrivalETAOffsetList->ReadData(nProjID);

	InitTimeList();

	//set default value
	ASSERT( m_pArrivalETAOffsetList->GetElementCount() );	
}

CArrivalETAOffsetDlg::~CArrivalETAOffsetDlg()
{
	delete m_pArrivalETAOffsetList;
}

void CArrivalETAOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DISANDTIME, m_mListCtrl);
	DDX_Control(pDX, IDC_LIST_FLIGHTTYPE, m_mFlightTypeListBox);
}


BEGIN_MESSAGE_MAP(CArrivalETAOffsetDlg, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_FLIGHTTYPEITEM, OnNewFlightType)
	ON_COMMAND(ID_DEL_FLIGHTTYPEITEM, OnDelFlightType)
	ON_COMMAND(ID_EDIT_FLIGHTTYPEITEM, OnEditFlightType)
	ON_COMMAND(ID_NEW_DISANDTIMEFROMARPITEM, OnNewDistanceAndTimeFromARP)
	ON_COMMAND(ID_DEL_DISANDTIMEFROMARPITEM, OnDelDistanceAndTimeFromARP)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHTTYPE, OnLbnSelchangeListFlightType)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_DISANDTIME, OnNMSetfocusListDistanceAndTimeFromARP)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISANDTIME, OnLvnItemchangedListDistanceAndTimeFromARP)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_DISANDTIME, OnNMKillfocusListDistanceAndTimeFromARP)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISANDTIME, OnLvnEndlabeleditListDistanceAndTimeFromARP)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
END_MESSAGE_MAP()


// CArrivalETAOffsetDlg message handlers
int CArrivalETAOffsetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//flight type toolbar
	if (!m_mFlightTypeToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_mFlightTypeToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& flightTypeToolBar = m_mFlightTypeToolBar.GetToolBarCtrl();
	flightTypeToolBar.SetCmdID(0, ID_NEW_FLIGHTTYPEITEM);
	flightTypeToolBar.SetCmdID(1, ID_DEL_FLIGHTTYPEITEM);
	flightTypeToolBar.SetCmdID(2, ID_EDIT_FLIGHTTYPEITEM);

	//disandtimefromARP toolbar
	if (!m_mDisAndTimeToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_mDisAndTimeToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	m_mDisAndTimeToolBar.SetDlgCtrlID(m_mFlightTypeToolBar.GetDlgCtrlID()+1);
	CToolBarCtrl& disAndTimeToolBar = m_mDisAndTimeToolBar.GetToolBarCtrl();
	disAndTimeToolBar.SetCmdID(0, ID_NEW_DISANDTIMEFROMARPITEM);
	disAndTimeToolBar.SetCmdID(1, ID_DEL_DISANDTIMEFROMARPITEM);

	return 0;
}

BOOL CArrivalETAOffsetDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolbar();
	InitListCtrl();
	InitListBox();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetResize(IDC_STATIC_FLIGHTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_DISANDTIME, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_LIST_FLIGHTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_DISANDTIME, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);


	SetResize(m_mFlightTypeToolBar.GetDlgCtrlID(), SZ_TOP_LEFT , SZ_TOP_LEFT);
	SetResize(m_mDisAndTimeToolBar.GetDlgCtrlID(), SZ_TOP_CENTER , SZ_TOP_CENTER);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CArrivalETAOffsetDlg::InitListBox()
{
	m_mFlightTypeListBox.ResetContent();

	for (int i=0; i<(int)m_pArrivalETAOffsetList->GetElementCount(); i++)
	{
		CArrivalETAOffset *pArrivalETAOffset = m_pArrivalETAOffsetList->GetItem(i);
		ASSERT(pArrivalETAOffset != NULL);

		CString strFlightType(_T(""));
		pArrivalETAOffset->GetFlightType().screenPrint(strFlightType.GetBuffer(1024));

		int nIndex = m_mFlightTypeListBox.AddString(strFlightType);
		m_mFlightTypeListBox.SetItemData(nIndex, (DWORD_PTR)pArrivalETAOffset);
	}
}

void CArrivalETAOffsetDlg::InitToolbar()
{
	// set the position of the toolbar
	CRect rectToolbar;
	m_mFlightTypeListBox.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 24;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_mFlightTypeToolBar.MoveWindow(&rectToolbar);

	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton(ID_NEW_FLIGHTTYPEITEM, TRUE);
	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton(ID_DEL_FLIGHTTYPEITEM, FALSE);
	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FLIGHTTYPEITEM, FALSE);

	m_mListCtrl.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 24;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_mDisAndTimeToolBar.MoveWindow(&rectToolbar);
	m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DISANDTIMEFROMARPITEM, FALSE);
	m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DISANDTIMEFROMARPITEM, FALSE);
}

void CArrivalETAOffsetDlg::InitListCtrl(void)
{
	// set list control window style
	DWORD dwStyle = m_mListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_mListCtrl.SetExtendedStyle(dwStyle);
	m_mListCtrl.SubClassHeadCtrl();

	m_mListCtrl.DeleteAllItems();

	while(m_mListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		m_mListCtrl.DeleteColumn(0);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption.LoadString(IDS_DISTANCEFROMARP);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 180;
	lvc.fmt = LVCFMT_EDIT;
	lvc.csList = &strList;
	m_mListCtrl.InsertColumn(0, &lvc, dtINT);

	strCaption.LoadString(IDS_TIMETOGATE);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 180;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &m_TimeList;
	m_mListCtrl.InsertColumn(1, &lvc, dtSTRING);
}

void CArrivalETAOffsetDlg::OnNewFlightType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	//find the flttype
	if (LB_ERR != m_mFlightTypeListBox.FindString(-1, szBuffer))
	{
		MessageBox(_T("The Flight Type already exists!"));

		return;
	}

	int nAddStringIndex = m_mFlightTypeListBox.AddString(szBuffer);
	m_mFlightTypeListBox.SetCurSel(nAddStringIndex);

	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTYPEITEM, TRUE );
	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTYPEITEM, TRUE );
	m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton( ID_NEW_DISANDTIMEFROMARPITEM, TRUE);

	m_pCurArrivalETAOffset = new CArrivalETAOffset;
	m_pCurArrivalETAOffset->SetFlightType(fltType);
	m_mFlightTypeListBox.SetItemData(nAddStringIndex, (DWORD_PTR)m_pCurArrivalETAOffset);

	m_pArrivalETAOffsetList->AddNewItem(m_pCurArrivalETAOffset);

	CDistanceAndTimeFromARP *pDistanceAndTimeFromARP = new CDistanceAndTimeFromARP;
	m_pCurArrivalETAOffset->AddNewItem(pDistanceAndTimeFromARP);

	SetListContent();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CArrivalETAOffsetDlg::OnDelFlightType()
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	int nCurSel = m_mFlightTypeListBox.GetCurSel();

	if (LB_ERR == nCurSel)
	{
		return;
	}

	CArrivalETAOffset* pArrivalETAOffset = (CArrivalETAOffset*)m_mFlightTypeListBox.GetItemData(nCurSel);
	ASSERT(pArrivalETAOffset != NULL);

	m_pArrivalETAOffsetList->DeleteItem(pArrivalETAOffset);

	m_mFlightTypeListBox.DeleteString(nCurSel);

	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTYPEITEM, FALSE );
	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTYPEITEM, FALSE );
	m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton( ID_NEW_DISANDTIMEFROMARPITEM, FALSE);
	m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_DISANDTIMEFROMARPITEM, FALSE);

	m_mListCtrl.DeleteAllItems();

	m_pCurArrivalETAOffset = NULL;
}

void CArrivalETAOffsetDlg::OnEditFlightType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	CString strDefault;
	strDefault.Format("%s", szBuffer);

	//the flight type is exist
	if (LB_ERR != m_mFlightTypeListBox.FindString(-1, szBuffer))
	{
		MessageBox(_T("The Flight Type already exists!"));

		return;
	}

	int nCurSel = m_mFlightTypeListBox.GetCurSel();

	m_pCurArrivalETAOffset = (CArrivalETAOffset*)m_mFlightTypeListBox.GetItemData(nCurSel);
	ASSERT(m_pCurArrivalETAOffset);

	m_pCurArrivalETAOffset->SetFlightType(fltType);

	m_mFlightTypeListBox.DeleteString(nCurSel);
	int nModifyCurSel = m_mFlightTypeListBox.InsertString(nCurSel, szBuffer);

	m_mFlightTypeListBox.SetCurSel(nModifyCurSel);

	m_mFlightTypeListBox.SetItemData(nModifyCurSel, (DWORD_PTR)m_pCurArrivalETAOffset);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CArrivalETAOffsetDlg::OnNewDistanceAndTimeFromARP()
{
	ASSERT(m_pCurArrivalETAOffset != NULL);

	CDistanceAndTimeFromARP *pDistanceAndTimeFromARP = new CDistanceAndTimeFromARP;
	m_pCurArrivalETAOffset->AddNewItem(pDistanceAndTimeFromARP);

	SetListContent();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CArrivalETAOffsetDlg::OnDelDistanceAndTimeFromARP()
{
	ASSERT(m_pCurArrivalETAOffset != NULL);

	if (m_mListCtrl.GetSelectedCount() == 0)
	{
		return;
	}

	int nCurSelect = m_mListCtrl.GetCurSel();
	CDistanceAndTimeFromARP *pDistanceAndTimeFromARP = (CDistanceAndTimeFromARP *)m_mListCtrl.GetItemData(nCurSelect);
	ASSERT(pDistanceAndTimeFromARP != NULL);

	m_pCurArrivalETAOffset->DeleteItem(pDistanceAndTimeFromARP);

	m_mListCtrl.DeleteItem(nCurSelect);

	UpdateDisAndTimeFromARPToolBar();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CArrivalETAOffsetDlg::SetListContent()
{
	ASSERT(m_pCurArrivalETAOffset != NULL);

	m_mListCtrl.DeleteAllItems();

	for (size_t i=0; i<m_pCurArrivalETAOffset->GetElementCount(); i++)
	{
		CDistanceAndTimeFromARP *pDistanceAndTimeFromARP = m_pCurArrivalETAOffset->GetItem(i);
		CString strItemText(_T(""));
		strItemText.Format(_T("%d"), pDistanceAndTimeFromARP->GetDistance());
		m_mListCtrl.InsertItem(i, strItemText);
		m_mListCtrl.SetItemData(i, (DWORD_PTR)pDistanceAndTimeFromARP);
		m_mListCtrl.SetItemText(i, 1, pDistanceAndTimeFromARP->GetDistScreenPrint());
	}
}

void CArrivalETAOffsetDlg::OnLbnSelchangeListFlightType()
{
	int nCurSel = m_mFlightTypeListBox.GetCurSel();

	if (LB_ERR == nCurSel
		|| m_mFlightTypeListBox.GetCount()-1 < nCurSel)
	{
		m_pCurArrivalETAOffset = NULL;
		m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTYPEITEM, FALSE );
		m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTYPEITEM, FALSE );

		return ;
	}

	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTYPEITEM, TRUE );
	m_mFlightTypeToolBar.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTYPEITEM, TRUE );

	m_pCurArrivalETAOffset = (CArrivalETAOffset*)m_mFlightTypeListBox.GetItemData(nCurSel);
	SetListContent();

	m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton( ID_NEW_DISANDTIMEFROMARPITEM, TRUE);
}

void CArrivalETAOffsetDlg::OnLvnItemchangedListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateDisAndTimeFromARPToolBar();
}

void CArrivalETAOffsetDlg::OnNMSetfocusListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateDisAndTimeFromARPToolBar();
}

void CArrivalETAOffsetDlg::OnNMKillfocusListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateDisAndTimeFromARPToolBar();
}

void CArrivalETAOffsetDlg::UpdateDisAndTimeFromARPToolBar()
{
	//select an Item
	if (m_mListCtrl.GetSelectedCount() == 1)
	{
		m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_DISANDTIMEFROMARPITEM, TRUE);
	}
	//Select nothing
	else
	{
		m_mDisAndTimeToolBar.GetToolBarCtrl().EnableButton( ID_DEL_DISANDTIMEFROMARPITEM, FALSE);
	}
}

void CArrivalETAOffsetDlg::OnLvnEndlabeleditListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (NULL == m_pCurArrivalETAOffset)
	{
		return;
	}

	LV_ITEM* plvItem = &pDispInfo->item;
	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	CDistanceAndTimeFromARP *pDisAndTimeFromARP = (CDistanceAndTimeFromARP *)m_mListCtrl.GetItemData(nItem);
	ASSERT(pDisAndTimeFromARP != NULL);

	int nItemData = (int)atoi(m_mListCtrl.GetItemText(nItem, nSubItem));
	CString strItemData(_T(""));
	strItemData.Format(_T("%d"), nItemData);
	m_mListCtrl.SetItemText(nItem, nSubItem, strItemData);

	switch(nSubItem) 
	{
	case 0:
		{
			pDisAndTimeFromARP->SetDistance(nItemData);
		}
		break;

	case 1:
		{
			//pDisAndTimeFromARP->SetTime(nItemData);
		}
		break;

	default:
		break;
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CArrivalETAOffsetDlg::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pArrivalETAOffsetList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	OnOK();
}

void CArrivalETAOffsetDlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_pArrivalETAOffsetList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CArrivalETAOffsetDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CArrivalETAOffsetDlg::InitTimeList(void)
{
	m_TimeList.RemoveAll();
	m_TimeList.AddTail("NEW PROBABILITY DISTRIBUTION");

	m_TimeMap.clear();

	m_TimeMap.insert(make_pair(0, m_TimeList.GetTail()));

	CProbDistManager* pProbDistMan = m_pAirportDB->getProbDistMan();
	int nCount = static_cast<int>(pProbDistMan ->getCount());

	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
		m_TimeList.AddTail(pPBEntry->m_csName);
		m_TimeMap[m+1]= m_TimeList.GetTail();
	}
}

LRESULT CArrivalETAOffsetDlg::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	if (NULL == m_pCurArrivalETAOffset)
	{
		return 0;
	}


	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return 0;

	CDistanceAndTimeFromARP *pDistanceAndTimeFromARP = (CDistanceAndTimeFromARP *)m_mListCtrl.GetItemData(nItem);
	ASSERT(pDistanceAndTimeFromARP != NULL);
	if (NULL == pDistanceAndTimeFromARP)
	{
		return 0;
	}

	switch(nSubItem)
	{
	case 1:
		{
			ProbabilityDistribution* pProbDist = NULL;
			CProbDistManager* pProbDistMan = m_pAirportDB->getProbDistMan();

			//if select the first item
			if( nComboxSel == 0 )
			{
				CProbDistEntry* pPDEntry = NULL;
				pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
				if(pPDEntry == NULL)
					return 0;
				pProbDist = pPDEntry->m_pProbDist;
				assert( pProbDist );

				CString strDistName = pPDEntry->m_csName;

				char szBuffer[1024] = {0};
				pProbDist->screenPrint(szBuffer);
				pDistanceAndTimeFromARP->SetDistScreenPrint(szBuffer);
				pDistanceAndTimeFromARP->SetProbTypes((ProbTypes)pProbDist->getProbabilityType());
				pProbDist->printDistribution(szBuffer);
				pDistanceAndTimeFromARP->SetPrintDist(szBuffer);

				InitTimeList();

				m_mListCtrl.SetItemText(plvItem->iItem, plvItem->iSubItem, pDistanceAndTimeFromARP->GetDistScreenPrint());
				pDistanceAndTimeFromARP->SetTime(strDistName);

			}
			else
			{
				map<int, CString>::const_iterator iter = m_TimeMap.find(nComboxSel);
				CString strServiceTime = iter->second;
				pDistanceAndTimeFromARP->SetTime(strServiceTime);

				CProbDistEntry* pPDEntry = NULL;
				int nCount = pProbDistMan->getCount();
				for( int i=0; i<nCount; i++ )
				{
					pPDEntry = pProbDistMan->getItem( i );
					if( strcmp( pPDEntry->m_csName, strServiceTime ) == 0 )
						break;
				}
				//assert( i < nCount );
				pProbDist = pPDEntry->m_pProbDist;
				assert( pProbDist );
				char szBuffer[1024] = {0};
				pProbDist->screenPrint(szBuffer);
				pDistanceAndTimeFromARP->SetDistScreenPrint(szBuffer);
				pDistanceAndTimeFromARP->SetProbTypes((ProbTypes)pProbDist->getProbabilityType());
				pProbDist->printDistribution(szBuffer);
				pDistanceAndTimeFromARP->SetPrintDist(szBuffer);

				m_mListCtrl.SetItemText(plvItem->iItem, plvItem->iSubItem, pDistanceAndTimeFromARP->GetDistScreenPrint());
			}
		}
		break;

	default:
		break;
	}

	InitListCtrl();
	SetListContent();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	return 0;
}