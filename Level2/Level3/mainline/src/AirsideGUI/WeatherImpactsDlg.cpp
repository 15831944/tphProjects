// WeatherImpactsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WeatherImpactsDlg.h"
#include "resource.h"
#include "../common/flt_cnst.h"
#include "../Database/ARCportDatabaseConnection.h"
#include "InputAirside/WeatherImpact.h"
#include "../Database/DBElementCollection_Impl.h"


// CWeatherImpactsDlg dialog
using namespace ADODB;

namespace
{
	const UINT ID_NEW_WEATHERIMPACTS = 10;
	const UINT ID_DEL_WEATHERIMPACTS = 11;
}

IMPLEMENT_DYNAMIC(CWeatherImpactsDlg, CXTResizeDialog)
CWeatherImpactsDlg::CWeatherImpactsDlg(int nProjID, PSelectFlightType pSelectFlightType, CWnd* pParent)
	: CXTResizeDialog(IDD_DIALOG_WEATHERIMPACT, pParent)
	, m_pSelectFlightType(pSelectFlightType)
	, m_WeatherCondition(DEFAULT_WEATHERCONDITION)
	, m_nProjID(nProjID)
{
	m_WeatherImpactList = new CWeatherImpactList();
	m_WeatherImpactList->ReadData(m_nProjID);

	m_WeatherImpact = NULL;
}

CWeatherImpactsDlg::~CWeatherImpactsDlg()
{
	//Delete all data
	delete m_WeatherImpactList;
	//delete m_WeatherImpactList;
}

void CWeatherImpactsDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_WEATHERCONDITION, m_WeatherConditionTreeCtrl);
	DDX_Control(pDX, IDC_LIST_WEATHERIMPACTS, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CWeatherImpactsDlg, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_WEATHERIMPACTS, OnCmdNewItem)
	ON_COMMAND(ID_DEL_WEATHERIMPACTS, OnCmdDeleteItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WEATHERCONDITION, OnTvnSelchangedTreeWeathercondition)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_WEATHERIMPACTS, OnLvnEndlabeleditListWeatherimpacts)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_WEATHERIMPACTS, OnNMSetfocusListWeatherimpacts)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WEATHERIMPACTS, OnLvnItemchangedListWeatherimpacts)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_WEATHERIMPACTS, OnNMKillfocusListWeatherimpacts)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CWeatherImpactsDlg message handlers

int CWeatherImpactsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_ToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_WEATHERIMPACTS);
	toolbar.SetCmdID(1, ID_DEL_WEATHERIMPACTS);

	return 0;
}


void CWeatherImpactsDlg::InitToolBar()
{
	CRect rect;
	m_ListCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBar.MoveWindow(&rect);

	m_ToolBar.ShowWindow( SW_SHOW );

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW_WEATHERIMPACTS, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL_WEATHERIMPACTS, FALSE );
}

void CWeatherImpactsDlg::InitWeatherConditionTree()
{
	HTREEITEM hTreeItem       = NULL;
	HTREEITEM hTreeItemChild  = NULL;

	CString strItemCaption;

	//VFR
	strItemCaption.LoadString(IDS_VFR);
	hTreeItem = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption);
	strItemCaption.LoadString(IDS_NOFREEZINGPRECITITATION);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(VFR_NONFREEZINGPRECP));
	strItemCaption.LoadString(IDS_FREEZINGDRIZZLE);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(VFR_FREEZINGDRIZZLE));
	m_WeatherConditionTreeCtrl.Expand(hTreeItem, TVE_EXPAND);

	//IFR
	strItemCaption.LoadString(IDS_IFR);
	hTreeItem = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption);
	strItemCaption.LoadString(IDS_NOFREEZINGPRECITITATION);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_NONFREEZINGPRECP));
	strItemCaption.LoadString(IDS_FREEZINGDRIZZLE);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_FREEZINGDRIZZLE));
	strItemCaption.LoadString(IDS_LIGHTSNOW);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_LIGHTSONW));
	strItemCaption.LoadString(IDS_MEDIUMSNOW);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_MEDIUMSNOW));
	strItemCaption.LoadString(IDS_HEAVYSNOW);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_HEAVYSONW));
	m_WeatherConditionTreeCtrl.Expand(hTreeItem, TVE_EXPAND);

	//CAT II/III
	strItemCaption.LoadString(IDS_CAT);
	hTreeItem = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption);
	strItemCaption.LoadString(IDS_NOFREEZINGPRECITITATION);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_NONFREEZINGPRECP));
	strItemCaption.LoadString(IDS_FREEZINGDRIZZLE);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_FREEZINGDRIZZLE));
	strItemCaption.LoadString(IDS_LIGHTSNOW);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_LIGHTSONW));
	strItemCaption.LoadString(IDS_MEDIUMSNOW);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_MEDIUMSNOW));
	strItemCaption.LoadString(IDS_HEAVYSNOW);
	hTreeItemChild = m_WeatherConditionTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_WeatherConditionTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_HEAVYSONW));
	m_WeatherConditionTreeCtrl.Expand(hTreeItem, TVE_EXPAND);

	m_WeatherConditionTreeCtrl.SelectItem(0);
}

void CWeatherImpactsDlg::InitListCtrl()
{
	SetListColumn();
	SetListContent();
}

void CWeatherImpactsDlg::SetListColumn()
{
	// set list control window style
	DWORD dwStyle = m_ListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCtrl.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	strCaption.LoadString(IDS_FLIGHTTYPE);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_ListCtrl.InsertColumn(0, &lvc);

	strCaption.LoadString(IDS_LANDINGDISPERF);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_ListCtrl.InsertColumn(1, &lvc);

	strCaption.LoadString(IDS_TAXISPDPERF);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_ListCtrl.InsertColumn(2, &lvc);

	strCaption.LoadString(IDS_HOLDSHRTLINEOFF);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_ListCtrl.InsertColumn(3, &lvc);

	strCaption.LoadString(IDS_TAKEOFFDISPERF);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_ListCtrl.InsertColumn(4, &lvc);


	strCaption.LoadString(IDS_DEICINGTIMECHANGE);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_ListCtrl.InsertColumn(5, &lvc);
}

void CWeatherImpactsDlg::SetListContent()
{
	//clear the list control
	m_ListCtrl.DeleteAllItems();

	if (NULL == m_WeatherImpact)
	{
		return;
	}
	if (m_WeatherImpact->GetElementCount() == (size_t)0)
	{
		return;
	}

	size_t flightNum = m_WeatherImpact->GetElementCount();
	for (int i=0; i<(int)flightNum; i++)
	{
		CFlightPerformance *pFlightPerf = NULL;
		pFlightPerf = m_WeatherImpact->GetItem(i);

		m_ListCtrl.InsertItem(i, pFlightPerf->GetFlightType());
		CString itemText;
		itemText.Format("%.2f",pFlightPerf->GetLandingDisPerf());
		m_ListCtrl.SetItemText(i, 1, itemText);
		itemText.Format("%.2f",pFlightPerf->GetTaxiSpedPerf());
		m_ListCtrl.SetItemText(i, 2, itemText);
		itemText.Format("%.2f",pFlightPerf->GetHoldShortLineOffset());
		m_ListCtrl.SetItemText(i, 3, itemText);
		itemText.Format("%.2f",pFlightPerf->GetTakeoffDisPerf());
		m_ListCtrl.SetItemText(i, 4, itemText);
		itemText.Format("%.2f",pFlightPerf->GetDeicingTimeChange());
		m_ListCtrl.SetItemText(i, 5, itemText);
	}
}

BOOL CWeatherImpactsDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitWeatherConditionTree();
	InitToolBar();
	InitListCtrl();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	SetResize(IDC_STATIC_WEATHERCONDITION, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_WEATHERIMPACTS, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_TREE_WEATHERCONDITION, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_WEATHERIMPACTS, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);


	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_CENTER , SZ_TOP_CENTER);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CWeatherImpactsDlg::OnCmdNewItem()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	//haven't select the flight type
	//if (!strcmp(szBuffer, "DEFAULT"))
	//{
	//	return;
	//}

	//If the Flight Type is exist
	int nCount = m_ListCtrl.GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		if (!m_ListCtrl.GetItemText(i, 0).Compare(szBuffer))
		{
			MessageBox(_T("The Flight Type already exists!"));

			return;
		}
	}

	if (NULL == m_WeatherImpact)
	{
		m_WeatherImpact = new CWeatherImpact();
		m_WeatherImpact->SetWeatherCondition(m_WeatherCondition);
		m_WeatherImpactList->AddNewItem(m_WeatherImpact);
	}

	m_WeatherImpactList->GetItemByWeatherCondition(m_WeatherCondition, &m_WeatherImpact);

	CFlightPerformance *pFlightPerf = new CFlightPerformance();
	pFlightPerf->SetFlightType(szBuffer);

	m_WeatherImpact->AddNewItem(pFlightPerf);

	int newItem = m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), szBuffer);
	CString itemText;
	itemText.Format("%.2f",pFlightPerf->GetLandingDisPerf());
	m_ListCtrl.SetItemText(newItem, 1, itemText);
	itemText.Format("%.2f",pFlightPerf->GetTaxiSpedPerf());
	m_ListCtrl.SetItemText(newItem, 2, itemText);
	itemText.Format("%.2f",pFlightPerf->GetHoldShortLineOffset());
	m_ListCtrl.SetItemText(newItem, 3, itemText);
	itemText.Format("%.2f",pFlightPerf->GetTakeoffDisPerf());
	m_ListCtrl.SetItemText(newItem, 4, itemText);
	itemText.Format("%.2f",pFlightPerf->GetDeicingTimeChange());
	m_ListCtrl.SetItemText(newItem, 5, itemText);

	UpdateToolBarState();
	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CWeatherImpactsDlg::OnCmdDeleteItem()
{
	int selectItem = m_ListCtrl.GetCurSel();

	if (selectItem < 0 )
	{
		return;
	}

	CString strFlightType = m_ListCtrl.GetItemText(selectItem, 0);
	CFlightPerformance *pFlightPerf;
	m_WeatherImpact->GetItemByFlightType(strFlightType, &pFlightPerf);
	if (NULL != pFlightPerf)
	{
		m_WeatherImpact->DeleteItem(pFlightPerf);
	}

	m_ListCtrl.DeleteItemEx(selectItem);

	UpdateToolBarState();
	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CWeatherImpactsDlg::UpdateToolBarState()
{
	if (!::IsWindow(m_ListCtrl.m_hWnd) || !::IsWindow(m_ListCtrl.m_hWnd))
		return;

	BOOL bEnable = TRUE;

	switch(m_WeatherCondition) 
	{
	case VFR_NONFREEZINGPRECP:
	case VFR_FREEZINGDRIZZLE:
	case IFR_NONFREEZINGPRECP:
	case IFR_FREEZINGDRIZZLE:
	case IFR_LIGHTSONW:
	case IFR_MEDIUMSNOW:
	case IFR_HEAVYSONW:
	case CAT_NONFREEZINGPRECP:
	case CAT_FREEZINGDRIZZLE:
	case CAT_LIGHTSONW:
	case CAT_MEDIUMSNOW:
	case CAT_HEAVYSONW:
		{
			//new button
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_NEW_WEATHERIMPACTS, bEnable);
			// delete button
			int selectItem = m_ListCtrl.GetSelectedCount();
			if (selectItem > 0)
				bEnable = TRUE;
			else
				bEnable = FALSE;

			m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEL_WEATHERIMPACTS, bEnable);
		}
		break;
	default:
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_NEW_WEATHERIMPACTS, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEL_WEATHERIMPACTS, FALSE);
		}
	}
}


void CWeatherImpactsDlg::OnTvnSelchangedTreeWeathercondition(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM hItem = m_WeatherConditionTreeCtrl.GetSelectedItem();
	WEATHERCONDITION weatherCondtion = (WEATHERCONDITION)m_WeatherConditionTreeCtrl.GetItemData(hItem);

	switch(weatherCondtion) 
	{
	case VFR_NONFREEZINGPRECP:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(VFR_NONFREEZINGPRECP, &m_WeatherImpact);
			m_WeatherCondition = VFR_NONFREEZINGPRECP;
		}
		break;

	case VFR_FREEZINGDRIZZLE:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(VFR_FREEZINGDRIZZLE, &m_WeatherImpact);
			m_WeatherCondition = VFR_FREEZINGDRIZZLE;
		}
		break;

	case IFR_NONFREEZINGPRECP:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(IFR_NONFREEZINGPRECP, &m_WeatherImpact);
			m_WeatherCondition = IFR_NONFREEZINGPRECP;
		}
		break;

	case IFR_FREEZINGDRIZZLE:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(IFR_FREEZINGDRIZZLE, &m_WeatherImpact);
			m_WeatherCondition = IFR_FREEZINGDRIZZLE;
		}
		break;

	case IFR_LIGHTSONW:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(IFR_LIGHTSONW, &m_WeatherImpact);
			m_WeatherCondition = IFR_LIGHTSONW;
		}
		break;

	case IFR_MEDIUMSNOW:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(IFR_MEDIUMSNOW, &m_WeatherImpact);
			m_WeatherCondition = IFR_MEDIUMSNOW;
		}
		break;

	case IFR_HEAVYSONW:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(IFR_HEAVYSONW, &m_WeatherImpact);
			m_WeatherCondition = IFR_HEAVYSONW;
		}
		break;

	case CAT_NONFREEZINGPRECP:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(CAT_NONFREEZINGPRECP, &m_WeatherImpact);
			m_WeatherCondition = CAT_NONFREEZINGPRECP;
		}
		break;

	case CAT_FREEZINGDRIZZLE:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(CAT_FREEZINGDRIZZLE, &m_WeatherImpact);
			m_WeatherCondition = CAT_FREEZINGDRIZZLE;
		}
		break;

	case CAT_LIGHTSONW:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(CAT_LIGHTSONW, &m_WeatherImpact);
			m_WeatherCondition = CAT_LIGHTSONW;
		}
		break;

	case CAT_MEDIUMSNOW:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(CAT_MEDIUMSNOW, &m_WeatherImpact);
			m_WeatherCondition = CAT_MEDIUMSNOW;
		}
		break;

	case CAT_HEAVYSONW:
		{
			m_WeatherImpactList->GetItemByWeatherCondition(CAT_HEAVYSONW, &m_WeatherImpact);
			m_WeatherCondition = CAT_HEAVYSONW;
		}
		break;

	default:
		{
			m_WeatherImpact = NULL;
			m_WeatherCondition = DEFAULT_WEATHERCONDITION;
		}
	}

	SetListContent();
	UpdateToolBarState();
}

void CWeatherImpactsDlg::OnLvnEndlabeleditListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (NULL == m_WeatherImpact)
	{
		return;
	}

    LV_ITEM* plvItem = &pDispInfo->item;
	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	CString strFlightType = m_ListCtrl.GetItemText(nItem, 0);
	CFlightPerformance *pFlightPerf;
	m_WeatherImpact->GetItemByFlightType(strFlightType, &pFlightPerf);
    float nItemData = (float)atof(m_ListCtrl.GetItemText(nItem, nSubItem));
	CString strItemData;
	strItemData.Format("%.2f", nItemData);
	m_ListCtrl.SetItemText(nItem, nSubItem, strItemData);

	switch(nSubItem) 
	{
	case 1:
		pFlightPerf->SetLandingDisPerf(nItemData);
		break;

	case 2:
		pFlightPerf->SetTaxiSpedPerf(nItemData);
		break;

	case 3:
		pFlightPerf->SetHoldShortLineOffset(nItemData);
		break;

	case 4:
		pFlightPerf->SetTakeoffDisPerf(nItemData);
		break;

	case 5:
		pFlightPerf->SetDeicingTimeChange(nItemData);
		break;

	default:
		break;
	}

 	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CWeatherImpactsDlg::OnNMSetfocusListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateToolBarState();
}

void CWeatherImpactsDlg::OnLvnItemchangedListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateToolBarState();
}

void CWeatherImpactsDlg::OnNMKillfocusListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateToolBarState();
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEL_WEATHERIMPACTS, FALSE);
}

void CWeatherImpactsDlg::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_WeatherImpactList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	OnOK();
}

void CWeatherImpactsDlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_WeatherImpactList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CWeatherImpactsDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
