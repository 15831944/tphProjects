// DlgWaveCrossRunwaySpecification.cpp : implementation file
//

#include "stdafx.h"
#include "DlgWaveCrossRunwaySpecification.h"
#include ".\dlgwavecrossrunwayspecification.h"
#include "../Common/WinMsg.h"
#include "TermPlanDoc.h"
#include "TermPlan.h"
#include "MainFrm.h"
#include "3DView.h"
#include "ViewMsg.h"
#include <CommonData/Fallback.h>
#include "InputAirside/HoldPosition.h"
// CDlgWaveCrossRunwaySpecification dialog

IMPLEMENT_DYNAMIC(CDlgWaveCrossRunwaySpecification, CXTResizeDialog)
CDlgWaveCrossRunwaySpecification::CDlgWaveCrossRunwaySpecification(int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgWaveCrossRunwaySpecification::IDD, pParent)
{
	m_nProjID = nProjID;
}

CDlgWaveCrossRunwaySpecification::~CDlgWaveCrossRunwaySpecification()
{
}

void CDlgWaveCrossRunwaySpecification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgWaveCrossRunwaySpecification, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_WM_CREATE()
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnTreeDoubleClick)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_COMMAND(ID_TOOLBAR_ADD,OnToolBarAdd)
	ON_COMMAND(ID_TOOLBAR_DEL,OnToolBarDel)
	ON_COMMAND(ID_TOOLBAR_EDIT,OnToolBarEdit)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeData)
END_MESSAGE_MAP()


// CDlgWaveCrossRunwaySpecification message handlers
BOOL CDlgWaveCrossRunwaySpecification::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::Create(lpszTemplateName, pParentWnd);
}
int CDlgWaveCrossRunwaySpecification::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS ) ||
		!m_toolbar.LoadToolBar(IDR_TOOLBAR_WAVECROSSRUNWAY))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

BOOL CDlgWaveCrossRunwaySpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolBar();

	SetResize(m_toolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_MAINFRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_waveCrossSpecification.ReadData();
	LoadTree();

	SetWindowText(_T("Wave Crossing Specification"));
	UpdateToolBarState();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWaveCrossRunwaySpecification::InitToolBar()
{
	CRect rect;
	m_treeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	rect.right = rect.right-10;
	m_toolbar.MoveWindow(&rect,true);
	m_toolbar.ShowWindow(SW_SHOW);

	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
}

void CDlgWaveCrossRunwaySpecification::LoadTree()
{
	size_t nCount = m_waveCrossSpecification.GetElementCount();
	for (size_t i =0; i < nCount; ++i)
	{
		 LoadWaveCrossItem(m_waveCrossSpecification.GetItem(i));
	}

}

void CDlgWaveCrossRunwaySpecification::UpdateToolBarState()
{
	HTREEITEM hTreeItem = m_treeCtrl.GetSelectedItem();
	if(hTreeItem == NULL )
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);		
		return;
	}
	enumItemType itemType = GetItemType(hTreeItem);

	if(itemType == itemType_WaveCrossName)
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);	
	}
	else if(itemType == itemType_HoldRoot)
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
	}
	else if(itemType == itemType_Hold)
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
	}
	else
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
	}

}

void CDlgWaveCrossRunwaySpecification::LoadWaveCrossItem( CWaveCrossingSpecificationItem *pItem )
{
	//Load name

	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);

	HTREEITEM hItemName = NULL;
	{
		CTreeItemData *pTreeItemData = new CTreeItemData;
		pTreeItemData->itemType = itemType_WaveCrossName;
		pTreeItemData->pItem = pItem;
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		colorStringSection.strSection = pItem->GetName();
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("Wave crossing points: %s"),pItem->GetName());

		hItemName = m_treeCtrl.InsertItem(strNodeText,TVI_ROOT);
		m_treeCtrl.SetItemDataEx(hItemName,colorItemData);
	}
	// holding points
	{

		CTreeItemData *pTreeItemData = new CTreeItemData;
		pTreeItemData->itemType = itemType_HoldRoot;
		pTreeItemData->pItem = pItem;
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString strNodeText;
		strNodeText.Format(_T("Holding points"));

		HTREEITEM hItemHoldRoot = m_treeCtrl.InsertItem(strNodeText,hItemName);
		m_treeCtrl.SetItemDataEx(hItemHoldRoot,colorItemData);
		
		//holds
		{
			int nCount = pItem->GetHoldCount();
			for (int nHold = 0; nHold < nCount; ++nHold)
			{
				HoldPosition *pHoldPosition = pItem->GetHoldPosition(nHold);
				CTreeItemData *pTreeItemData = new CTreeItemData;
				pTreeItemData->itemType = itemType_Hold;
				pTreeItemData->pItem = pItem;
				pTreeItemData->pHoldPosition = pHoldPosition;

				colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
				colorItemData.vrItemStringSectionColorShow.clear();
				colorStringSection.strSection = pHoldPosition->m_strSign;
				colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
				CString strNodeText;
				strNodeText.Format(_T("%s"),pHoldPosition->m_strSign);

				HTREEITEM hItemHold = m_treeCtrl.InsertItem(strNodeText,hItemHoldRoot);
				m_treeCtrl.SetItemDataEx(hItemHold,colorItemData);
			}
			m_treeCtrl.Expand(hItemHoldRoot,TVE_EXPAND);


		}


	}

	//active runway for take off


	{
		CTreeItemData *pTreeItemData = new CTreeItemData;
		pTreeItemData->itemType = itemType_None;
		pTreeItemData->pItem = pItem;
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString strNodeText;
		strNodeText.Format(_T("Case: Active runway for take off"));

		HTREEITEM hItemTakeOffRoot = m_treeCtrl.InsertItem(strNodeText,hItemName);
		m_treeCtrl.SetItemDataEx(hItemTakeOffRoot,colorItemData);
		LoadTakeOffItems(hItemTakeOffRoot,pItem);
		//take off minimum
	}


	
	{
		CTreeItemData *pTreeItemData = new CTreeItemData;
		pTreeItemData->itemType = itemType_None;
		pTreeItemData->pItem = pItem;
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString strNodeText;
		strNodeText.Format(_T("Case: Active runway for landing"));

		HTREEITEM hItemLandingRoot = m_treeCtrl.InsertItem(strNodeText,hItemName);
		m_treeCtrl.SetItemDataEx(hItemLandingRoot,colorItemData);

		//take off minimum
		LoadLandingItems(hItemLandingRoot,pItem);
	}
	m_treeCtrl.Expand(hItemName,TVE_EXPAND);
	m_treeCtrl.SelectItem(hItemName);


}

void CDlgWaveCrossRunwaySpecification::OnAddWaveCross()
{

}

void CDlgWaveCrossRunwaySpecification::OnDelWaveCross()
{

}

void CDlgWaveCrossRunwaySpecification::OnAddHoldPosition()
{

}

void CDlgWaveCrossRunwaySpecification::OnDeleteHoldPosition()
{

}

void CDlgWaveCrossRunwaySpecification::OnEditWaveCrossName()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_WaveCrossName)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		CString strName =pItem ->GetName();
		if(strName.IsEmpty())
			strName = _T("NULL");

		m_treeCtrl.StringEditLabel(hSelItem, strName);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditTakeOffMinimumType()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_TakeOffMinState)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;
		
		std::vector<CString> vOption;
		vOption.push_back(_T("Yes"));
		vOption.push_back(_T("No"));
		m_treeCtrl.SetComboString(hSelItem,vOption);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditTakeOffWaves()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_TakeOffWaves)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		int nWaves =pItem ->GetTakeoffSettings().GetAllowWaves();
		CString strWaves;
		strWaves.Format(_T("%d"),nWaves);
		m_treeCtrl.SetDisplayNum(nWaves);
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hSelItem,strWaves);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditLandingMinimumType()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_LandingMinState)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		std::vector<CString> vOption;
		vOption.push_back(_T("Yes"));
		vOption.push_back(_T("No"));
		m_treeCtrl.SetComboString(hSelItem,vOption);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditLandingMinMiles()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_LandingMinMiles)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		int nMiles =pItem ->GetLandingSettings().GetMinMiles();
		CString strMiles;
		strMiles.Format(_T("%d"),nMiles);
		m_treeCtrl.SetDisplayNum(nMiles);
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hSelItem,strMiles);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditLandingMinSeconds()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_LandingMinSeconds)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		int nSeconds =pItem ->GetLandingSettings().GetMinSecons();
		CString strSeconds;
		strSeconds.Format(_T("%d"),nSeconds);
		m_treeCtrl.SetDisplayNum(nSeconds);
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hSelItem,strSeconds);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditLandingAllowWaves()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_LandingAllowWaves)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		int nWaves =pItem ->GetLandingSettings().GetAllowWaves();
		CString strWaves;
		strWaves.Format(_T("%d"),nWaves);
		m_treeCtrl.SetDisplayNum(nWaves);
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hSelItem,strWaves);
	}
}

void CDlgWaveCrossRunwaySpecification::OnEditLandingAllowSeconds()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(GetItemType(hSelItem) == itemType_LandingAllowSeconds)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return;

		int nSeconds =pItem ->GetLandingSettings().GetAllowSeconds();
		CString strSeconds;
		strSeconds.Format(_T("%d"),nSeconds);
		m_treeCtrl.SetDisplayNum(nSeconds);
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hSelItem,strSeconds);
	}
}
CDlgWaveCrossRunwaySpecification::enumItemType CDlgWaveCrossRunwaySpecification::GetItemType( HTREEITEM hItem )
{
	if (hItem == NULL)
	{
		return itemType_None;
	}
	CARCTreeCtrlExWithColor::CItemData *pItemData = m_treeCtrl.GetItemDataEx(hItem);
	if(pItemData)
	{
		CTreeItemData *pWaveCrossData  = (CTreeItemData *)pItemData->dwptrItemData;
		if(pWaveCrossData)
			return pWaveCrossData->itemType;
	}

	return itemType_None;
}

CWaveCrossingSpecificationItem * CDlgWaveCrossRunwaySpecification::GetItemWaveCross( HTREEITEM hItem )
{
	if (hItem == NULL)
	{
		return NULL;
	}
	CARCTreeCtrlExWithColor::CItemData *pItemData = m_treeCtrl.GetItemDataEx(hItem);
	if(pItemData)
	{
		CTreeItemData *pWaveCrossData  = (CTreeItemData *)pItemData->dwptrItemData;
		if(pWaveCrossData)
			return pWaveCrossData->pItem;
	}
	return NULL;
}

HoldPosition* CDlgWaveCrossRunwaySpecification::GetHoldPosition( HTREEITEM hItem )
{	
	if (hItem == NULL)
	{
		return NULL;
	}
	CARCTreeCtrlExWithColor::CItemData *pItemData = m_treeCtrl.GetItemDataEx(hItem);
	if(pItemData)
	{
		CTreeItemData *pWaveCrossData  = (CTreeItemData *)pItemData->dwptrItemData;
		if(pWaveCrossData)
			return pWaveCrossData->pHoldPosition;
	}
	return NULL;
}
void CDlgWaveCrossRunwaySpecification::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	try
	{	
		CADODatabase::BeginTransaction();
		m_waveCrossSpecification.SaveData(0);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		AfxMessageBox(_T("Save data failed."));
		return;
	}

	CXTResizeDialog::OnOK();
}

void CDlgWaveCrossRunwaySpecification::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}
void CDlgWaveCrossRunwaySpecification::OnBnClickedButtonSave()
{	
	try
	{	
		CADODatabase::BeginTransaction();
		m_waveCrossSpecification.SaveData(0);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		AfxMessageBox(_T("Save data failed."));
	}
	// TODO: Add your control notification handler code here
}

void CDlgWaveCrossRunwaySpecification::OnPickfrommap()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();


	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	enum FallbackReason enumReason = PICK_HOLDLINE;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

LRESULT CDlgWaveCrossRunwaySpecification::OnTempFallbackFinished( WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	HoldPosition* pData = reinterpret_cast< HoldPosition* >(wParam);
	
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem)
	{
		if(GetItemType(hSelItem) == itemType_HoldRoot)
		{
			CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
			HoldPosition *pHoldPosition = new HoldPosition;
			*pHoldPosition = *pData;
			pItem->AddHoldPosition(pHoldPosition);


			CARCTreeCtrlExWithColor::CItemData colorItemData;
			CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
			colorItemData.lSize = sizeof(colorItemData);
			colorStringSection.colorSection = RGB(0,0,255);

			CTreeItemData *pTreeItemData = new CTreeItemData;
			pTreeItemData->itemType = itemType_Hold;
			pTreeItemData->pItem = pItem;
			pTreeItemData->pHoldPosition = pHoldPosition;

			colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
			colorItemData.vrItemStringSectionColorShow.clear();
			colorStringSection.strSection = pHoldPosition->m_strSign;
			colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
			CString strNodeText;
			strNodeText.Format(_T("%s"),pHoldPosition->m_strSign);

			HTREEITEM hItemHold = m_treeCtrl.InsertItem(strNodeText,hSelItem);
			m_treeCtrl.SetItemDataEx(hItemHold,colorItemData);
			
			m_treeCtrl.Expand(hSelItem,TVE_EXPAND);
			
		}
	}


	

	return 1;
}

LRESULT CDlgWaveCrossRunwaySpecification::OnTreeDoubleClick( WPARAM, LPARAM lParam)
{
	HTREEITEM hTreeItem = (HTREEITEM)lParam;
	 m_treeCtrl.SelectItem(hTreeItem);


	enumItemType itemType = GetItemType(hTreeItem);
	switch (itemType)
	{
	case itemType_None:
		{

		}
		break;
	case itemType_WaveCrossName:
		{
			OnEditWaveCrossName();
		}
		break;
	//case itemType_HoldRoot:
	//	{

	//	}
	//	break;
	//itemType_Hold,
	//		itemType_ActiveTakeOff,
	case itemType_TakeOffMinState:
		{
			OnEditTakeOffMinimumType();
		}
		break;
	case itemType_TakeOffWaves:
		{
		}
		OnEditTakeOffWaves();
		break;
	case itemType_LandingMinState:
		{
			OnEditLandingMinimumType();
		}
		break;
	case itemType_LandingMinMiles:
		{
			OnEditLandingMinMiles();
		}
		break;

	case itemType_LandingMinSeconds:
		{
			OnEditLandingMinSeconds();
		}
		break;

	case itemType_LandingAllowWaves:
		{
			OnEditLandingAllowWaves();
		}
		break;

	case itemType_LandingAllowSeconds:
		{
			OnEditLandingAllowSeconds();
		}
		break;
	default:
		{

			break;
		}

	}


	return 1;
}

void CDlgWaveCrossRunwaySpecification::HideDialog( CWnd* parentWnd )
{

	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
	{
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgWaveCrossRunwaySpecification::ShowDialog( CWnd* parentWnd )
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);
	EnableWindow();
}

void CDlgWaveCrossRunwaySpecification::OnToolBarAdd()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	enumItemType itemType = GetItemType(hSelItem);

	if(itemType == itemType_HoldRoot)
	{
		OnPickfrommap();
	}
	else
	{
		CWaveCrossingSpecificationItem *pNewItem = new CWaveCrossingSpecificationItem;
		m_waveCrossSpecification.AddNewItem(pNewItem);
		LoadWaveCrossItem(pNewItem);
		OnEditWaveCrossName();
	}


}

void CDlgWaveCrossRunwaySpecification::OnToolBarDel()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	enumItemType itemType = GetItemType(hSelItem);

	if(itemType == itemType_Hold)
	{
		HoldPosition *pHoldPosition = GetHoldPosition(hSelItem);
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem != NULL)
		{
			pItem->DeleteHoldPosition(pHoldPosition);
			m_treeCtrl.DeleteItem(hSelItem);
		}
	}
	else if(itemType == itemType_WaveCrossName)
	{
		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem != NULL)
		{
			m_waveCrossSpecification.DeleteItem(pItem);
			DeleteItem(hSelItem);
			m_treeCtrl.SetImageList(m_treeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
		}
		
	}
}

void CDlgWaveCrossRunwaySpecification::OnToolBarEdit()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	enumItemType itemType = GetItemType(hSelItem);

	if(itemType == itemType_Hold)
	{

	}
	else if(itemType == itemType_WaveCrossName)
	{
		OnEditWaveCrossName();
	}
}

LRESULT CDlgWaveCrossRunwaySpecification::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		int nValue = pst->iPercent;

		HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
		
		enumItemType itemType = GetItemType(hSelItem);

		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

		if(itemType == itemType_TakeOffWaves)
		{
			pItem->GetTakeoffSettings().SetAllowWaves(nValue);

			CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hSelItem);
			if(pItemDataEx)
			{
				pItemDataEx->vrItemStringSectionColorShow.clear();

				CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
				colorStringSection.colorSection = RGB(0,0,255);

				pItemDataEx->vrItemStringSectionColorShow.clear();

				CString strWaves;
				strWaves.Format(_T("%d"),pItem->GetTakeoffSettings().GetAllowWaves());

				colorStringSection.strSection = strWaves;
				pItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);
				CString strNodeText;
				strNodeText.Format(_T("Allow %s waves, holding take off aircraft till crossing completed"),strWaves);
				m_treeCtrl.SetItemText(hSelItem,strNodeText);
			}
		}
		if(itemType == 	itemType_LandingMinMiles)
		{
			pItem->GetLandingSettings().SetMinMiles(nValue);
			CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hSelItem);
			if(pItemDataEx)
			{
				pItemDataEx->vrItemStringSectionColorShow.clear();

				CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
				colorStringSection.colorSection = RGB(0,0,255);
				CString strMiles;
				strMiles.Format(_T("%d"),pItem->GetLandingSettings().GetMinMiles());
				colorStringSection.strSection = strMiles;
				pItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);

				CString strNodeText;
				strNodeText.Format(_T("%s nm"),strMiles);
				m_treeCtrl.SetItemText(hSelItem,strNodeText);
				
				//update parent item
				{
					HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
					if(hParentItem == NULL)
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

					CARCTreeCtrlExWithColor::CItemData *pParentItemDataEx = m_treeCtrl.GetItemDataEx(hParentItem);
					if(pParentItemDataEx)
					{
						pParentItemDataEx->vrItemStringSectionColorShow.clear();

						CString strMiles;
						strMiles.Format(_T("%d"),pItem->GetLandingSettings().GetMinMiles());
						colorStringSection.strSection = strMiles;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);


						CString strSeconds;
						strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetMinSecons());
						colorStringSection.strSection = strSeconds;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);


						CString strNodeText;
						strNodeText.Format(_T("Allow one wave or fewer wave to cross when aircraft landing at least %s nm or %s seconds from threshold"),strMiles,strSeconds);
						m_treeCtrl.SetItemText(hParentItem,strNodeText);
					}
				}

			}


		}
		if(itemType == 	itemType_LandingMinSeconds)
		{
			pItem->GetLandingSettings().SetMinSecons(nValue);

			CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hSelItem);
			if(pItemDataEx)
			{
				pItemDataEx->vrItemStringSectionColorShow.clear();

				CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
				colorStringSection.colorSection = RGB(0,0,255);

				CString strSeconds;
				strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetMinSecons());
				colorStringSection.strSection = strSeconds;
				pItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);
				CString strNodeText;
				strNodeText.Format(_T("%s seconds"),strSeconds);
				m_treeCtrl.SetItemText(hSelItem,strNodeText);

				//update parent item
				{
					HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
					if(hParentItem == NULL)
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

					CARCTreeCtrlExWithColor::CItemData *pParentItemDataEx = m_treeCtrl.GetItemDataEx(hParentItem);
					if(pParentItemDataEx)
					{
						pParentItemDataEx->vrItemStringSectionColorShow.clear();

						CString strMiles;
						strMiles.Format(_T("%d"),pItem->GetLandingSettings().GetMinMiles());
						colorStringSection.strSection = strMiles;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);


						CString strSeconds;
						strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetMinSecons());
						colorStringSection.strSection = strSeconds;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);


						CString strNodeText;
						strNodeText.Format(_T("Allow one wave or fewer wave to cross when aircraft landing at least %s nm or %s seconds from threshold"),strMiles,strSeconds);
	
						m_treeCtrl.SetItemText(hParentItem,strNodeText);
					}
				}

			}


		}
		if(itemType == itemType_LandingAllowWaves)
		{
			pItem->GetLandingSettings().SetAllowWaves(nValue);
			CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hSelItem);
			if(pItemDataEx)
			{
				pItemDataEx->vrItemStringSectionColorShow.clear();

				CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
				colorStringSection.colorSection = RGB(0,0,255);

				CString strWaves;
				strWaves.Format(_T("%d"),pItem->GetLandingSettings().GetAllowWaves());
				colorStringSection.strSection = strWaves;
				pItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);

				CString strNodeText;
				strNodeText.Format(_T("%s waves"),strWaves);
				m_treeCtrl.SetItemText(hSelItem,strNodeText);
				
				//update parent items
				{
					HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
					if(hParentItem == NULL)
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

					CARCTreeCtrlExWithColor::CItemData *pParentItemDataEx = m_treeCtrl.GetItemDataEx(hParentItem);
					if(pParentItemDataEx)
					{
						pParentItemDataEx->vrItemStringSectionColorShow.clear();

						CString strWaves;
						strWaves.Format(_T("%d"),pItem->GetLandingSettings().GetAllowWaves());
						colorStringSection.strSection = strWaves;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);


						CString strSeconds;
						strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetAllowSeconds());
						colorStringSection.strSection = strSeconds;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);

						CString strNodeText;
						strNodeText.Format(_T("Allow %s waves to cross and delay arrivals by %s seconds per wave"),strWaves,strSeconds);
						
						m_treeCtrl.SetItemText(hParentItem,strNodeText);						
					}
				}
			}
		}
		if(itemType == itemType_LandingAllowSeconds)
		{
			pItem->GetLandingSettings().SetAllowSeconds(nValue);

			CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hSelItem);
			if(pItemDataEx)
			{
				pItemDataEx->vrItemStringSectionColorShow.clear();

				CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
				colorStringSection.colorSection = RGB(0,0,255);

				CString strSeconds;
				strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetAllowSeconds());
				colorStringSection.strSection = strSeconds;
				pItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);
				CString strNodeText;
				strNodeText.Format(_T("%s seconds"),strSeconds);
				m_treeCtrl.SetItemText(hSelItem,strNodeText);


				//update parent items
				{
					HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
					if(hParentItem == NULL)
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

					CARCTreeCtrlExWithColor::CItemData *pParentItemDataEx = m_treeCtrl.GetItemDataEx(hParentItem);
					if(pParentItemDataEx)
					{
						pParentItemDataEx->vrItemStringSectionColorShow.clear();

						CString strWaves;
						strWaves.Format(_T("%d"),pItem->GetLandingSettings().GetAllowWaves());
						colorStringSection.strSection = strWaves;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);


						CString strSeconds;
						strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetAllowSeconds());
						colorStringSection.strSection = strSeconds;
						pParentItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);

						CString strNodeText;
						strNodeText.Format(_T("Allow %s waves to cross and delay arrivals by %s seconds per wave"),strWaves,strSeconds);

						m_treeCtrl.SetItemText(hParentItem,strNodeText);						
					}
				}

			}

		}


	}
	if (message == WM_INPLACE_EDIT)
	{
		CString strValue = *(CString*)lParam;
		
		HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();

		enumItemType itemType = GetItemType(hSelItem);

		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
		
		if(itemType == itemType_WaveCrossName)
		{
			pItem->SetName(strValue);
			
			CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hSelItem);
			if(pItemDataEx)
			{
				pItemDataEx->vrItemStringSectionColorShow.clear();
				
				CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
				colorStringSection.colorSection = RGB(0,0,255);
				colorStringSection.strSection = pItem->GetName();
				pItemDataEx->vrItemStringSectionColorShow.push_back(colorStringSection);
				CString strItemText;
				strItemText.Format(_T("Wave crossing points: %s"),pItem->GetName());
				m_treeCtrl.SetItemText(hSelItem,strItemText);
			}
		}
	}

	if (message == WM_INPLACE_COMBO2)
	{	
		int nSelIndex = m_treeCtrl.m_comboBox.GetCurSel();
		bool bMinimum = false;
		if(nSelIndex == 0)
			bMinimum = true;

		HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();

		enumItemType itemType = GetItemType(hSelItem);

		CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
		if(pItem == NULL)
			return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

		if(itemType == itemType_TakeOffMinState)
		{
			if(pItem->GetTakeoffSettings().GetMinimum() != bMinimum)
			{
				pItem->GetTakeoffSettings().SetMinimum(bMinimum);


				HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
				DeleteItem(hSelItem);

				LoadTakeOffItems(hParentItem,pItem);

			}
		}
		if (itemType == itemType_LandingMinState)
		{
			if(pItem->GetLandingSettings().GetMinimum() != bMinimum)
			{
				pItem->GetLandingSettings().SetMinimum(bMinimum);
				HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
				DeleteItem(hSelItem);
				LoadLandingItems(hParentItem,pItem);
			}
		}

	}


	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgWaveCrossRunwaySpecification::LoadTakeOffItems( HTREEITEM hItemTakeOffRoot, CWaveCrossingSpecificationItem *pItem )
{
	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);

	{
		CTreeItemData *pMinItemData = new CTreeItemData;
		pMinItemData->itemType = itemType_TakeOffMinState;
		pMinItemData->pItem = pItem;
		colorItemData.dwptrItemData = (DWORD_PTR)pMinItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString stropition = _T("No");
		if(pItem->GetTakeoffSettings().GetMinimum())
			stropition = _T("Yes");

		colorStringSection.strSection = stropition;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("Minimum wave per take off: %s"),stropition);
		HTREEITEM hItemMinimum = m_treeCtrl.InsertItem(strNodeText,hItemTakeOffRoot);
		m_treeCtrl.SetItemDataEx(hItemMinimum,colorItemData);

		if(pItem->GetTakeoffSettings().GetMinimum())//yes
		{
			CTreeItemData *pYesItemData = new CTreeItemData;
			pYesItemData->itemType = itemType_None;
			pYesItemData->pItem = pItem;
			colorItemData.dwptrItemData = (DWORD_PTR)pYesItemData;
			colorItemData.vrItemStringSectionColorShow.clear();
			CString strNodeText;
			strNodeText.Format(_T("Allow one wave to cross, holding take off till crossing completed"));
			HTREEITEM hItemYes = m_treeCtrl.InsertItem(strNodeText,hItemMinimum);
			m_treeCtrl.SetItemDataEx(hItemYes,colorItemData);


		}
		else//no
		{
			CTreeItemData *pNoItemData = new CTreeItemData;
			pNoItemData->itemType = itemType_TakeOffWaves;
			pNoItemData->pItem = pItem;
			colorItemData.dwptrItemData = (DWORD_PTR)pNoItemData;
			colorItemData.vrItemStringSectionColorShow.clear();

			CString strWaves;
			strWaves.Format(_T("%d"),pItem->GetTakeoffSettings().GetAllowWaves());

			colorStringSection.strSection = strWaves;
			colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
			CString strNodeText;
			strNodeText.Format(_T("Allow %s waves, holding take off aircraft till crossing completed"),strWaves);
			HTREEITEM hItemNo = m_treeCtrl.InsertItem(strNodeText,hItemMinimum);
			m_treeCtrl.SetItemDataEx(hItemNo,colorItemData);

		}
		m_treeCtrl.Expand(hItemMinimum,TVE_EXPAND);

	}
	m_treeCtrl.Expand(hItemTakeOffRoot,TVE_EXPAND);

}

void CDlgWaveCrossRunwaySpecification::LoadLandingItems( HTREEITEM hItemLandingRoot, CWaveCrossingSpecificationItem *pItem )
{
	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	{
		CTreeItemData *pMinItemData = new CTreeItemData;
		pMinItemData->itemType = itemType_LandingMinState;
		pMinItemData->pItem = pItem;
		colorItemData.dwptrItemData = (DWORD_PTR)pMinItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString stropition = _T("No");
		if(pItem->GetLandingSettings().GetMinimum())
			stropition = _T("Yes");

		colorStringSection.strSection = stropition;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("Minimum wave per Landing: %s"),stropition);
		HTREEITEM hItemMinimum = m_treeCtrl.InsertItem(strNodeText,hItemLandingRoot);
		m_treeCtrl.SetItemDataEx(hItemMinimum,colorItemData);

		if(pItem->GetLandingSettings().GetMinimum())//yes
		{
			CTreeItemData *pYesItemData = new CTreeItemData;
			pYesItemData->itemType = itemType_None;
			pYesItemData->pItem = pItem;
			colorItemData.dwptrItemData = (DWORD_PTR)pYesItemData;
			colorItemData.vrItemStringSectionColorShow.clear();
			CString strMiles;
			strMiles.Format(_T("%d"),pItem->GetLandingSettings().GetMinMiles());
			colorStringSection.strSection = strMiles;
			colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);


			CString strSeconds;
			strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetMinSecons());
			colorStringSection.strSection = strSeconds;
			colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);


			CString strNodeText;
			strNodeText.Format(_T("Allow one wave or fewer wave to cross when aircraft landing at least %s nm or %s seconds from threshold"),strMiles,strSeconds);
			HTREEITEM hItemYes = m_treeCtrl.InsertItem(strNodeText,hItemMinimum);
			m_treeCtrl.SetItemDataEx(hItemYes,colorItemData);

			//miles
			{

				CTreeItemData *pYesItemData = new CTreeItemData;
				pYesItemData->itemType = itemType_LandingMinMiles;
				pYesItemData->pItem = pItem;
				colorItemData.dwptrItemData = (DWORD_PTR)pYesItemData;
				colorItemData.vrItemStringSectionColorShow.clear();
				CString strMiles;
				strMiles.Format(_T("%d"),pItem->GetLandingSettings().GetMinMiles());
				colorStringSection.strSection = strMiles;
				colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

				CString strNodeText;
				strNodeText.Format(_T("%s nm"),strMiles);
				HTREEITEM hItemMiles = m_treeCtrl.InsertItem(strNodeText,hItemYes);
				m_treeCtrl.SetItemDataEx(hItemMiles,colorItemData);

			}
			//seconds
			{

				CTreeItemData *pYesItemData = new CTreeItemData;
				pYesItemData->itemType = itemType_LandingMinSeconds;
				pYesItemData->pItem = pItem;
				colorItemData.dwptrItemData = (DWORD_PTR)pYesItemData;
				colorItemData.vrItemStringSectionColorShow.clear();
				CString strSeconds;
				strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetMinSecons());
				colorStringSection.strSection = strSeconds;
				colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

				CString strNodeText;
				strNodeText.Format(_T("%s seconds"),strSeconds);
				HTREEITEM hItemMiles = m_treeCtrl.InsertItem(strNodeText,hItemYes);
				m_treeCtrl.SetItemDataEx(hItemMiles,colorItemData);

			}
			m_treeCtrl.Expand(hItemYes,TVE_EXPAND);
			m_treeCtrl.Expand(hItemMinimum,TVE_EXPAND);
		}
		else//no
		{
			CTreeItemData *pNoItemData = new CTreeItemData;
			pNoItemData->itemType = itemType_None;
			pNoItemData->pItem = pItem;
			colorItemData.dwptrItemData = (DWORD_PTR)pNoItemData;
			colorItemData.vrItemStringSectionColorShow.clear();

			CString strWaves;
			strWaves.Format(_T("%d"),pItem->GetLandingSettings().GetAllowWaves());
			colorStringSection.strSection = strWaves;
			colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);


			CString strSeconds;
			strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetAllowSeconds());
			colorStringSection.strSection = strSeconds;
			colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);


			CString strNodeText;
			strNodeText.Format(_T("Allow %s waves to cross and delay arrivals by %s seconds per wave"),strWaves,strSeconds);
			HTREEITEM hItemNo = m_treeCtrl.InsertItem(strNodeText,hItemMinimum);
			m_treeCtrl.SetItemDataEx(hItemNo,colorItemData);

			//waves
			{

				CTreeItemData *pWavesItemData = new CTreeItemData;
				pWavesItemData->itemType = itemType_LandingAllowWaves;
				pWavesItemData->pItem = pItem;
				colorItemData.dwptrItemData = (DWORD_PTR)pWavesItemData;
				colorItemData.vrItemStringSectionColorShow.clear();
				CString strWaves;
				strWaves.Format(_T("%d"),pItem->GetLandingSettings().GetAllowWaves());
				colorStringSection.strSection = strWaves;
				colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

				CString strNodeText;
				strNodeText.Format(_T("%s waves"),strWaves);
				HTREEITEM hItemMiles = m_treeCtrl.InsertItem(strNodeText,hItemNo);
				m_treeCtrl.SetItemDataEx(hItemMiles,colorItemData);

			}
			//seconds
			{

				CTreeItemData *psecondsItemData = new CTreeItemData;
				psecondsItemData->itemType = itemType_LandingAllowSeconds;
				psecondsItemData->pItem = pItem;
				colorItemData.dwptrItemData = (DWORD_PTR)psecondsItemData;
				colorItemData.vrItemStringSectionColorShow.clear();
				CString strSeconds;
				strSeconds.Format(_T("%d"),pItem->GetLandingSettings().GetAllowSeconds());
				colorStringSection.strSection = strSeconds;
				colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

				CString strNodeText;
				strNodeText.Format(_T("%s seconds"),strSeconds);
				HTREEITEM hItemSeconds = m_treeCtrl.InsertItem(strNodeText,hItemNo);
				m_treeCtrl.SetItemDataEx(hItemSeconds,colorItemData);

			}

			m_treeCtrl.Expand(hItemNo,TVE_EXPAND);
			m_treeCtrl.Expand(hItemMinimum,TVE_EXPAND);
		}

		m_treeCtrl.Expand(hItemLandingRoot,TVE_EXPAND);


	}

}

void CDlgWaveCrossRunwaySpecification::DeleteItem( HTREEITEM hItem )
{
	CString strText = m_treeCtrl.GetItemText(hItem);
	HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hItem);
	CString strText1 = m_treeCtrl.GetItemText(hChildItem);
	while(hChildItem != NULL)
	{
		DeleteChildItem(hChildItem);
		hChildItem = m_treeCtrl.GetChildItem(hItem);
	}

	m_treeCtrl.DeleteItem(hItem);

}

void CDlgWaveCrossRunwaySpecification::DeleteChildItem( HTREEITEM hItem )
{
	if(m_treeCtrl.GetChildItem(hItem) == NULL)
	{
		m_treeCtrl.DeleteItem(hItem);	
	}
	else
	{
		DeleteItem(hItem);
	}

}


void CDlgWaveCrossRunwaySpecification::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	UpdateToolBarState();
	*pResult = 0;
}






























