// DlgTaxiSpeedConstraints.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTaxiSpeedConstraints.h"

#include "Common/FLT_CNST.H"

#include "InputAirside/Taxiway.h"
#include "Common/WinMsg.h"
#include ".\dlgtaxispeedconstraints.h"
#include <algorithm>

static const int IDC_VERTICAL_SPLITTER_BAR = 30;
// CDlgTaxiSpeedConstraints dialog

IMPLEMENT_DYNAMIC(CDlgTaxiSpeedConstraints, CXTResizeDialogEx)
CDlgTaxiSpeedConstraints::CDlgTaxiSpeedConstraints(
	int nProjID, CAirportDatabase* pAirportDB, PFuncSelectFlightType pFuncSelectFlightType, CWnd* pParent /*=NULL*/)
	: CXTResizeDialogEx(CXTResizeDialogEx::styleUseInitSizeAsMinSize, CDlgTaxiSpeedConstraints::IDD, pParent)
	, m_nProjID(nProjID)
	, m_pAirportDB(pAirportDB)
	, m_pFuncSelectFlightType(pFuncSelectFlightType)
	, m_hTaxiwayLastSelFltTypeItem(NULL)
{
	ASSERT(m_pFuncSelectFlightType);

	m_TaxiwayConstraints.ReadData(nProjID, pAirportDB);
}

CDlgTaxiSpeedConstraints::~CDlgTaxiSpeedConstraints()
{
}

void CDlgTaxiSpeedConstraints::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAXIWAY, m_wndTaxiway);
	DDX_Control(pDX, IDC_LIST_CONSTRAINTS, m_wndConstraints);
}


BEGIN_MESSAGE_MAP(CDlgTaxiSpeedConstraints, CXTResizeDialogEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_TAXIWAY_NEW, OnMsgTaxiwayNew)
	ON_COMMAND(ID_TAXIWAY_EDIT, OnMsgTaxiwayEdit)
	ON_COMMAND(ID_TAXIWAY_DEL, OnMsgTaxiwayDel)
	ON_COMMAND(ID_CONSTRAINTS_NEW, OnMsgConstraintsNew)
	ON_COMMAND(ID_CONSTRAINTS_DEL, OnMsgConstraintsDel)

	ON_COMMAND(ID_FLIGHTTYPE_ADDFLIGHTTYPE, OnAddFlightType)
	ON_COMMAND(ID_FLIGHTTYPE_EDITFLIGHTTYPE, OnEditFlightType)
	ON_COMMAND(ID_FLIGHTTYPE_DELETEFLIGHTTYPE, OnDelFlightType)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TAXIWAY, OnSelChangedTaxiwayTree)

	ON_WM_CONTEXTMENU()

	ON_WM_SIZE()
END_MESSAGE_MAP()

int CDlgTaxiSpeedConstraints::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndTaxiwayToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1000)
		|| !m_wndTaxiwayToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	if (!m_wndConstraintsToolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1001)
		|| !m_wndConstraintsToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& taxiwayToolbar = m_wndTaxiwayToolbar.GetToolBarCtrl();
	taxiwayToolbar.SetCmdID(0, ID_TAXIWAY_NEW);
	taxiwayToolbar.SetCmdID(1, ID_TAXIWAY_DEL);
	taxiwayToolbar.SetCmdID(2, ID_TAXIWAY_EDIT);

	CToolBarCtrl& constraintsToolbar = m_wndConstraintsToolbar.GetToolBarCtrl();
	constraintsToolbar.SetCmdID(0, ID_CONSTRAINTS_NEW);
	constraintsToolbar.SetCmdID(1, ID_CONSTRAINTS_DEL);

	m_wndSplitterVer.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_VERTICAL_SPLITTER_BAR);

	return 0;
}

BOOL CDlgTaxiSpeedConstraints::OnInitDialog()
{
	CXTResizeDialogEx::OnInitDialog();

	InitToolbar();
	InitListCtrlHeader();

	m_wndSplitterVer.SetStyle(SPS_VERTICAL);
	m_wndSplitterVer.SetBuddies(GetDlgItem(IDC_STATIC_TAXIWAY), GetDlgItem(IDC_STATIC_CONSTRAINTS));

	UpdateSplitterRange();
	SetResizeItems();

	ResetTreeCtrlContent();
	UpdateRightListCtrl();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

// CDlgTaxiSpeedConstraints message handlers

void CDlgTaxiSpeedConstraints::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CString strError;
	if (FALSE == m_TaxiwayConstraints.CheckAllRoutesValid(strError))
	{
		MessageBox(strError);
		return;
	}

	m_TaxiwayConstraints.SaveData(m_nProjID);
	CXTResizeDialogEx::OnOK();
}




void CDlgTaxiSpeedConstraints::OnMsgTaxiwayNew()
{
	CDlgTaxiwaySelect dlg(m_nProjID, &TaxiwayChecker(&m_TaxiwayConstraints));
	if (IDOK == dlg.DoModal())
	{
		TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem = new TaxiwaySpeedConstraintTaxiwayItem;
		pConstraintTaxiwayItem->SetTaxiwayID(dlg.GetSelTaxiwayID());
		m_TaxiwayConstraints.AddNewItem(pConstraintTaxiwayItem);

		TaxiwaySpeedConstraintFlttypeItem* pConstraintFlttypeItem = new TaxiwaySpeedConstraintFlttypeItem;
		FlightConstraint fltType;
		fltType.SetAirportDB(m_pAirportDB);
		pConstraintFlttypeItem->SetFlightType(fltType);
		pConstraintTaxiwayItem->GetConstraintFltType().AddNewItem(pConstraintFlttypeItem);

		int nTaxiwayID = pConstraintTaxiwayItem->GetTaxiwayID();
		pConstraintFlttypeItem->GetSpeedConstraints().AddNewItem(GetOneConstraintItemByTaxiwayID(nTaxiwayID));


		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this, cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_STATIC;

		CString strTaxiway;
		strTaxiway.Format(_T("Taxiway: %s"), pConstraintTaxiwayItem->GetTaxiwayString());
		HTREEITEM hConstraintTaxewayItem = m_wndTaxiway.InsertItem(strTaxiway, cni, FALSE);
		m_wndTaxiway.SetItemData(hConstraintTaxewayItem, (DWORD)pConstraintTaxiwayItem);

		CString strFlttype;
		strFlttype.Format(_T("Flight Type: %s"), pConstraintFlttypeItem->GetFltTypeName());
		HTREEITEM hConstraintFlttypeItem = m_wndTaxiway.InsertItem(strFlttype, cni,
			FALSE, FALSE, hConstraintTaxewayItem);
		m_wndTaxiway.SetItemData(hConstraintFlttypeItem, (DWORD)pConstraintFlttypeItem);

		m_wndTaxiway.SelectItem(hConstraintFlttypeItem);
		UpdateRightListCtrl();
	}
}

void CDlgTaxiSpeedConstraints::OnMsgTaxiwayEdit()
{
	HTREEITEM hTreeItem = GetSelTreeItemByNodeType(TAXIWAY_NODE);
	if (hTreeItem)
	{
		TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem =
			(TaxiwaySpeedConstraintTaxiwayItem*)m_wndTaxiway.GetItemData(hTreeItem);
		CDlgTaxiwaySelect dlg(m_nProjID, &TaxiwayChecker(&m_TaxiwayConstraints, pConstraintTaxiwayItem->GetTaxiwayID()));
		if (IDOK == dlg.DoModal())
		{
			pConstraintTaxiwayItem->SetTaxiwayID(dlg.GetSelTaxiwayID());

			CString strTaxiway;
			strTaxiway.Format(_T("Taxiway: %s"), pConstraintTaxiwayItem->GetTaxiwayString());

			m_wndTaxiway.SetItemText(hTreeItem, strTaxiway);
		}
	}
}

void CDlgTaxiSpeedConstraints::OnMsgTaxiwayDel()
{
	HTREEITEM hTreeItem = GetSelTreeItemByNodeType(TAXIWAY_NODE);
	if (hTreeItem)
	{
		TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem =
			(TaxiwaySpeedConstraintTaxiwayItem*)m_wndTaxiway.GetItemData(hTreeItem);
		m_TaxiwayConstraints.DeleteItem(pConstraintTaxiwayItem);
		m_wndTaxiway.DeleteItem(hTreeItem);
	}
}

void CDlgTaxiSpeedConstraints::OnMsgConstraintsNew()
{
	m_wndConstraints.AddOneNewItem();
	UpdateConstraintsToolbar();
}

void CDlgTaxiSpeedConstraints::OnMsgConstraintsDel()
{
	m_wndConstraints.DeleteSelectedItem();
}

HTREEITEM CDlgTaxiSpeedConstraints::GetSelTreeItemByNodeType( TaxiwayTreeNodeType nodeType )
{
	HTREEITEM hTreeItem = m_wndTaxiway.GetSelectedItem();
	if (NULL == hTreeItem)
	{
		return NULL;
	}
	int nLevel = 0;
	HTREEITEM hTmpTreeItem = hTreeItem;
	while (1)
	{
		hTmpTreeItem = m_wndTaxiway.GetParentItem(hTmpTreeItem);
		if (NULL == hTmpTreeItem)
		{
			break;
		}
		nLevel++;
	}
	return nodeType == nLevel ? hTreeItem : NULL;
}

void CDlgTaxiSpeedConstraints::OnAddFlightType()
{
	HTREEITEM hTreeItem = GetSelTreeItemByNodeType(TAXIWAY_NODE);
	if(hTreeItem)
	{
		FlightConstraint fltConstraint;
		if ((*m_pFuncSelectFlightType)(NULL, fltConstraint))
		{
			TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem =
				(TaxiwaySpeedConstraintTaxiwayItem*)m_wndTaxiway.GetItemData(hTreeItem);
			TaxiwaySpeedConstraintFlttypeItem* pConstraintFltTypeItem = new TaxiwaySpeedConstraintFlttypeItem;
			pConstraintFltTypeItem->SetFlightType(fltConstraint);
			pConstraintFltTypeItem->GetSpeedConstraints().AddNewItem(GetOneConstraintItemByTaxiwayID(pConstraintTaxiwayItem->GetTaxiwayID()));
			pConstraintTaxiwayItem->GetConstraintFltType().AddNewItem(pConstraintFltTypeItem);

			CString strFltType;
			strFltType.Format(_T("Flight Type: %s"), pConstraintFltTypeItem->GetFltTypeName());

			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this, cni);
			cni.nt = NT_NORMAL;
			cni.net = NET_STATIC;
			HTREEITEM hConstraintFlttypeItem = m_wndTaxiway.InsertItem(strFltType, cni, FALSE, FALSE, hTreeItem);
			m_wndTaxiway.SetItemData(hConstraintFlttypeItem, (DWORD)pConstraintFltTypeItem);
			m_wndTaxiway.Expand(hTreeItem, TVE_EXPAND);

			m_wndTaxiway.SelectItem(hConstraintFlttypeItem);
			UpdateRightListCtrl();
		}
	}
}

void CDlgTaxiSpeedConstraints::OnEditFlightType()
{
	HTREEITEM hTreeItem = GetSelTreeItemByNodeType(FLTTYPE_NODE);
	if(hTreeItem)
	{
		FlightConstraint fltConstraint;
		if (m_pFuncSelectFlightType(NULL, fltConstraint))
		{
			TaxiwaySpeedConstraintFlttypeItem* pConstraintFltTypeItem =
				(TaxiwaySpeedConstraintFlttypeItem*)m_wndTaxiway.GetItemData(hTreeItem);
			pConstraintFltTypeItem->SetFlightType(fltConstraint);

			CString strFltType;
			strFltType.Format(_T("Flight Type: %s"), pConstraintFltTypeItem->GetFltTypeName());

			m_wndTaxiway.SetItemText(hTreeItem, strFltType);
		}
	}
}

void CDlgTaxiSpeedConstraints::OnDelFlightType()
{
	HTREEITEM hTreeItem = GetSelTreeItemByNodeType(FLTTYPE_NODE);
	if(hTreeItem)
	{
		HTREEITEM hParentItem = m_wndTaxiway.GetParentItem(hTreeItem);
		ASSERT(hParentItem);
		TaxiwaySpeedConstraintFlttypeItem* pConstraintFltTypeItem =
			(TaxiwaySpeedConstraintFlttypeItem*)m_wndTaxiway.GetItemData(hTreeItem);
		TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem =
			(TaxiwaySpeedConstraintTaxiwayItem*)m_wndTaxiway.GetItemData(hParentItem);

		pConstraintTaxiwayItem->GetConstraintFltType().DeleteItem(pConstraintFltTypeItem);
		m_wndTaxiway.DeleteItem(hTreeItem);

		UpdateRightListCtrl();
	}
}

void CDlgTaxiSpeedConstraints::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CRect  rtTaxiwayTree;
	m_wndTaxiway.GetWindowRect(&rtTaxiwayTree);
	if( rtTaxiwayTree.PtInRect(point))
	{
		m_wndTaxiway.SetFocus();
		CPoint pt = point;
		m_wndTaxiway.ScreenToClient( &pt );
		UINT iRet;
		HTREEITEM hRClickItem = m_wndTaxiway.HitTest( pt, &iRet );
		if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
			return;
		m_wndTaxiway.SelectItem(hRClickItem);

		CMenu menuPopup;
		menuPopup.LoadMenu( IDR_MENU_FLTTYPE );
		CMenu* pMenu=NULL;
		pMenu = menuPopup.GetSubMenu(0);

		if(GetSelTreeItemByNodeType(TAXIWAY_NODE))
		{
			pMenu->EnableMenuItem( ID_FLIGHTTYPE_EDITFLIGHTTYPE, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_FLIGHTTYPE_DELETEFLIGHTTYPE, MF_GRAYED | MF_DISABLED );
		}
		else if (GetSelTreeItemByNodeType(FLTTYPE_NODE))
		{
			pMenu->EnableMenuItem( ID_FLIGHTTYPE_ADDFLIGHTTYPE, MF_GRAYED | MF_DISABLED );
		}

		if (pMenu)
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
}

void CDlgTaxiSpeedConstraints::UpdateRightListCtrl()
{
	m_wndConstraints.DeleteAllItems();

	HTREEITEM hTreeItem = GetSelTreeItemByNodeType(FLTTYPE_NODE);
	if (NULL == hTreeItem)
	{
		m_wndConstraints.ResetListContent(NULL, -1);
		m_wndConstraints.EnableWindow(FALSE);
		UpdateConstraintsToolbar();
		return;
	}
	m_hTaxiwayLastSelFltTypeItem = hTreeItem;

	TaxiwaySpeedConstraintFlttypeItem* pConstraintFlttypeItem =
		(TaxiwaySpeedConstraintFlttypeItem*)m_wndTaxiway.GetItemData(hTreeItem);

	HTREEITEM hParentItem = m_wndTaxiway.GetParentItem(hTreeItem);
	TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem =
		(TaxiwaySpeedConstraintTaxiwayItem*)m_wndTaxiway.GetItemData(hParentItem);


	m_wndConstraints.ResetListContent(pConstraintFlttypeItem, pConstraintTaxiwayItem->GetTaxiwayID());
	m_wndConstraints.EnableWindow(TRUE);
	UpdateConstraintsToolbar();
}

void CDlgTaxiSpeedConstraints::InitListCtrlHeader()
{
	m_wndConstraints.InitListHeader();
}

void CDlgTaxiSpeedConstraints::SetResizeItems()
{
	CXTResizeDialogEx::Init();

	SetResize(IDC_STATIC_TAXIWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(m_wndTaxiwayToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_CENTER);
	SetResize(IDC_TREE_TAXIWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(m_wndSplitterVer.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC_CONSTRAINTS, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(m_wndConstraintsToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_LIST_CONSTRAINTS, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
}

void CDlgTaxiSpeedConstraints::InitToolbar()
{
	// set the position of the flt toolbar
	CRect rectFltToolbar;
	m_wndTaxiway.GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	rectFltToolbar.top -= 26;
	rectFltToolbar.bottom = rectFltToolbar.top + 22;
	rectFltToolbar.left +=2;
	m_wndTaxiwayToolbar.MoveWindow(&rectFltToolbar);

	m_wndTaxiwayToolbar.GetToolBarCtrl().EnableButton(ID_TAXIWAY_NEW);
	m_wndTaxiwayToolbar.GetToolBarCtrl().EnableButton(ID_TAXIWAY_EDIT);
	m_wndTaxiwayToolbar.GetToolBarCtrl().EnableButton(ID_TAXIWAY_DEL);

	// set the position of the runway toolbar
	CRect rectRunwayToolbar;
	m_wndConstraints.GetWindowRect(&rectRunwayToolbar);
	ScreenToClient(&rectRunwayToolbar);
	rectRunwayToolbar.top -= 26;
	rectRunwayToolbar.bottom = rectRunwayToolbar.top + 22;
	rectRunwayToolbar.left += 4;
	m_wndConstraintsToolbar.MoveWindow(&rectRunwayToolbar);

	m_wndConstraintsToolbar.GetToolBarCtrl().EnableButton(ID_CONSTRAINTS_NEW);
	m_wndConstraintsToolbar.GetToolBarCtrl().EnableButton(ID_CONSTRAINTS_DEL);
}

void CDlgTaxiSpeedConstraints::ResetTreeCtrlContent()
{
	m_wndTaxiway.DeleteAllItems();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_STATIC;

	CString strTaxiwayItem;
	size_t nCount = m_TaxiwayConstraints.GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem = m_TaxiwayConstraints.GetItem(i);
		strTaxiwayItem.Format(_T("Taxiway: %s"), pConstraintTaxiwayItem->GetTaxiwayString());
		HTREEITEM hTaxiwayItem = m_wndTaxiway.InsertItem(strTaxiwayItem, cni, FALSE);
		m_wndTaxiway.SetItemData(hTaxiwayItem, (DWORD)pConstraintTaxiwayItem);

		TaxiwaySpeedConstraintFlttypeList& ConstraintFltTypeList = pConstraintTaxiwayItem->GetConstraintFltType();
		CString strFlttypeItem;
		size_t nCount = ConstraintFltTypeList.GetElementCount();
		for(size_t i=0;i<nCount;i++)
		{
			TaxiwaySpeedConstraintFlttypeItem* pConstraintFlttypeItem = ConstraintFltTypeList.GetItem(i);
			strFlttypeItem.Format(_T("Flight Type: %s"), pConstraintFlttypeItem->GetFltTypeName());
			HTREEITEM hFlttypeItem = m_wndTaxiway.InsertItem(strFlttypeItem, cni, FALSE, FALSE, hTaxiwayItem);
			m_wndTaxiway.SetItemData(hFlttypeItem, (DWORD)pConstraintFlttypeItem);
		}

		m_wndTaxiway.Expand(hTaxiwayItem, TVE_EXPAND);
	}

	m_hTaxiwayLastSelFltTypeItem = GetSelTreeItemByNodeType(FLTTYPE_NODE);
}

void CDlgTaxiSpeedConstraints::OnSelChangedTaxiwayTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	CString strError;
	if (   m_hTaxiwayLastSelFltTypeItem
		&& m_hTaxiwayLastSelFltTypeItem != m_wndTaxiway.GetSelectedItem()
		&& !m_wndConstraints.IsAllItemsValid(strError)
		)
	{
// 		m_wndTaxiway.SelectItem(m_hTaxiwayLastSelFltTypeItem);
		MessageBox(strError);
// 		*pResult = 1;
// 		return;
	}
	UpdateRightListCtrl();
	*pResult = 1;
}

void CDlgTaxiSpeedConstraints::UpdateConstraintsToolbar()
{
	m_wndConstraintsToolbar.GetToolBarCtrl().EnableButton(ID_CONSTRAINTS_NEW,
		m_wndConstraints.IsWindowEnabled());
	m_wndConstraintsToolbar.GetToolBarCtrl().EnableButton(ID_CONSTRAINTS_DEL,
		m_wndConstraints.IsWindowEnabled()/* && m_wndConstraints.GetCurSel() != -1*/);
}

LRESULT CDlgTaxiSpeedConstraints::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_VERTICAL_SPLITTER_BAR)
		{
			SPC_NMHDR* pHdr = (SPC_NMHDR*)lParam;
			int delta = pHdr->delta;

			CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_TAXIWAY), delta, CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_wndTaxiwayToolbar, delta, CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_wndTaxiway, delta,CW_LEFTALIGN);

			CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_CONSTRAINTS),-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndConstraintsToolbar,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndConstraints,-delta,CW_RIGHTALIGN);

			SetResizeItems();
		}
	}

	return CXTResizeDialogEx::DefWindowProc(message, wParam, lParam);
}

TaxiwaySpeedConstraintDataItem* CDlgTaxiSpeedConstraints::GetOneConstraintItemByTaxiwayID( int nTaxiwayID )
{
	TaxiwaySpeedConstraintDataItem* pConstraintDataItem = new TaxiwaySpeedConstraintDataItem;
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);
	IntersectionNodeList taxiwayNodeList = taxiway.GetIntersectNodes();
	if (taxiwayNodeList.size())
	{
		std::sort(taxiwayNodeList.begin(), taxiwayNodeList.end(), TaxiwayNodeSorter(nTaxiwayID));
		pConstraintDataItem->SetFromNodeID(taxiwayNodeList.begin()->GetID());
		pConstraintDataItem->SetToNodeID(taxiwayNodeList.back().GetID());
	}
	return pConstraintDataItem;
}

CDlgTaxiSpeedConstraints::TaxiwayChecker::TaxiwayChecker( TaxiSpeedConstraints* pTaxiwayConstraints, int nCurTaxiwayID /*= 0*/ )
	: m_pTaxiwayConstraints(pTaxiwayConstraints)
	, m_nCurTaxiwayID(nCurTaxiwayID)
{

}

BOOL CDlgTaxiSpeedConstraints::TaxiwayChecker::IsAllowed( int nTaxiwayID, CString& strError )
{
	if (0 == nTaxiwayID)
	{
		strError = _T("Invalid taxiway!");
		return FALSE;
	}
	if (nTaxiwayID == m_nCurTaxiwayID)
	{
		return TRUE;
	}

	size_t nCount = m_pTaxiwayConstraints->GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		if (m_pTaxiwayConstraints->GetItem(i)->GetTaxiwayID() == nTaxiwayID)
		{
			strError = _T("The selected taxiway already exists!");
			return FALSE;
		}
	}
	return TRUE;
}



CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::CTaxiSpeedConstraintsListCtrlEx()
	: m_pConstraintFlttypeItem(NULL)
	, m_nTaxiwayID(-1)
{

}

CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::~CTaxiSpeedConstraintsListCtrlEx()
{

}


BEGIN_MESSAGE_MAP(CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx, CListCtrlEx)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


void CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::InitListHeader()
{
	DWORD dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	SetExtendedStyle(dwStyle);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("From");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strList;
	InsertColumn(0, &lvc);

	lvc.pszText = _T("To");
	lvc.fmt = LVCFMT_DROPDOWN;
	InsertColumn(1, &lvc);

	lvc.pszText = _T("Max V(kts)");
	lvc.fmt = LVCFMT_NUMBER;
	InsertColumn(2, &lvc);
}

void CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::ResetListContent(
	TaxiwaySpeedConstraintFlttypeItem* pConstraintFlttyptItem, int nTaxiwayID )
{
	m_pConstraintFlttypeItem = pConstraintFlttyptItem;
	m_nTaxiwayID = nTaxiwayID;

	if (NULL == m_pConstraintFlttypeItem)
	{
		return;
	}

	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);
	m_taxiwayNodeList = taxiway.GetIntersectNodes();
	std::sort(m_taxiwayNodeList.begin(), m_taxiwayNodeList.end(), TaxiwayNodeSorter(nTaxiwayID));

	ResetDropDownList();

	DeleteAllItems();
	TaxiwaySpeedConstraintDataList& ConstraintDataList = pConstraintFlttyptItem->GetSpeedConstraints();
	size_t nCount = ConstraintDataList.GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		TaxiwaySpeedConstraintDataItem* pConstraintDataItem = ConstraintDataList.GetItem(i);
		int nItem = InsertItem(i, pConstraintDataItem->GetFromNodeName());
		SetItemText(nItem, 1, pConstraintDataItem->GetToNodeName());
		SetItemText(nItem, 2, pConstraintDataItem->GetMaxSpeedString());
		SetItemData(nItem, (DWORD)pConstraintDataItem);
	}
}

void CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::ResetDropDownList()
{
	//Taxiway
	LVCOLDROPLIST* pDroplistFrom =  GetColumnStyle(0);
	pDroplistFrom->List.RemoveAll();

	LVCOLDROPLIST* pDroplistTo =  GetColumnStyle(1);
	pDroplistTo->List.RemoveAll();

	IntersectionNodeList::iterator ite = m_taxiwayNodeList.begin();
	IntersectionNodeList::iterator iteEn = m_taxiwayNodeList.end();
	for (;ite!=iteEn;ite++)
	{
		pDroplistFrom->List.AddTail(ite->GetName());
		pDroplistTo->List.AddTail(ite->GetName());
	}
}

LRESULT CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_INPLACE_COMBO_KILLFOUCUS )
	{
		OnComboBoxChange( wParam, lParam );
		return TRUE;
	}
	else if (message == WM_INPLACE_EDIT)
	{
		int nSubItem;
		int nItem = GetLastDblClk(nSubItem);
		if (-1 != nItem)
		{
			TaxiwaySpeedConstraintDataItem* pConstraintDataItem = 
				(TaxiwaySpeedConstraintDataItem*)GetItemData(nItem);
	
			CString* pStr = (CString*)lParam;
			double dMaxSpeed = _tstof(*pStr);
			if (dMaxSpeed>0.0f)
			{
				pConstraintDataItem->SetMaxSpeed(dMaxSpeed);
			} 
			else
			{
				MessageBox(_T("Maximum speed can not be negtive or zero!"));
			}
			SetItemText(nItem, 2, pConstraintDataItem->GetMaxSpeedString());
		}

		return TRUE;
	}

	return CListCtrlEx::DefWindowProc(message, wParam, lParam);
}

LRESULT CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::OnComboBoxChange(WPARAM wParam, LPARAM lParam)
{
	int nSelIndex = (int)wParam;
	if(nSelIndex == -1)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nItem = dispinfo->item.iItem;
	int nSubItem = dispinfo->item.iSubItem;	

	TaxiwaySpeedConstraintDataItem* pConstraintDataItem = 
		(TaxiwaySpeedConstraintDataItem*)GetItemData(nItem);
	if (NULL == pConstraintDataItem)
	{
		return 0;
	}

	switch(nSubItem)
	{
	case 0: // From Node
		{
			pConstraintDataItem->SetFromNodeID(m_taxiwayNodeList[nSelIndex].GetID());
			SetItemText(nItem, nSubItem, pConstraintDataItem->GetFromNodeName());
		}
		break;
	case 1: // To Node
		{
			pConstraintDataItem->SetToNodeID(m_taxiwayNodeList[nSelIndex].GetID());
			SetItemText(nItem, nSubItem, pConstraintDataItem->GetToNodeName());
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}

	return 0;
}

void CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::AddOneNewItem()
{
	if (NULL == m_pConstraintFlttypeItem)
	{
		return;
	}
	TaxiwaySpeedConstraintDataList& ConstraintDataList = m_pConstraintFlttypeItem->GetSpeedConstraints();
	TaxiwaySpeedConstraintDataItem* pConstraintDataItem = CDlgTaxiSpeedConstraints::GetOneConstraintItemByTaxiwayID(m_nTaxiwayID);
	if (pConstraintDataItem)
	{
		ConstraintDataList.AddNewItem(pConstraintDataItem);

		int nItem = InsertItem(GetItemCount(), pConstraintDataItem->GetFromNodeName());
		SetItemText(nItem, 1, pConstraintDataItem->GetToNodeName());
		SetItemText(nItem, 2, pConstraintDataItem->GetMaxSpeedString());
		SetItemData(nItem, (DWORD)pConstraintDataItem);
	}
}

void CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::DeleteSelectedItem()
{
	if (NULL == m_pConstraintFlttypeItem)
	{
		return;
	}
	TaxiwaySpeedConstraintDataList& ConstraintDataList = m_pConstraintFlttypeItem->GetSpeedConstraints();

	int nCurSel = GetCurSel();
	if (-1 == nCurSel)
	{
		return;
	}
	TaxiwaySpeedConstraintDataItem* pConstraintDataItem = (TaxiwaySpeedConstraintDataItem*)GetItemData(nCurSel);
	ASSERT(pConstraintDataItem);
	DeleteItem(nCurSel);
	ConstraintDataList.DeleteItem(pConstraintDataItem);
}

BOOL CDlgTaxiSpeedConstraints::CTaxiSpeedConstraintsListCtrlEx::IsAllItemsValid(CString& strError)
{
	return m_pConstraintFlttypeItem
		? m_pConstraintFlttypeItem->CheckAllRoutesValid(m_taxiwayNodeList, strError)
		: TRUE;
}

void CDlgTaxiSpeedConstraints::UpdateSplitterRange()
{
	const int nLeftMargin = 120;
	const int nRightMargin = 120;
	CRect rc;
	GetClientRect(rc);
	m_wndSplitterVer.SetRange(nLeftMargin, rc.right - nRightMargin);
}

void CDlgTaxiSpeedConstraints::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	UpdateSplitterRange();
}
