// DlgSeatBlockDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatBlockDefine.h"
#include "../InputOnboard/CInputOnboard.h"
#include "../InputOnboard/AircraftPlacements.h"
#include "../InputOnboard/OnBoardSeatBlockData.h"
#include "../InputOnboard/AircaftLayOut.h"
#include "DlgSeatGroupSelection.h"
#include "../InputOnboard/OnBoardAnalysisCondidates.h"
#include "../Engine/terminal.h"
#include "../InputOnboard/OnBoardAnalysisCondidates.h"
#include "DlgOnboardFlightList.h"



// CDlgSeatBlockDefine dialog

IMPLEMENT_DYNAMIC(CDlgSeatBlockDefine, CXTResizeDialog)
CDlgSeatBlockDefine::CDlgSeatBlockDefine(InputOnboard* pInputOnboard,Terminal* pTerm,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSeatBlockDefine::IDD, pParent)
	,m_pOnboard(pInputOnboard)
	,m_pTerm(pTerm)
{
	m_pSeatBlockManager = new OnboardFlightSeatBlockContainr();
	m_pSeatBlockManager->ReadData(-1);
}

CDlgSeatBlockDefine::~CDlgSeatBlockDefine()
{
	if (m_pSeatBlockManager)
	{
		delete m_pSeatBlockManager;
		m_pSeatBlockManager = NULL;
	}
}

void CDlgSeatBlockDefine::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_FLIGHT,m_wndListBox);
	DDX_Control(pDX,IDC_TREE_SEATBLOCK,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSeatBlockDefine, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_MODEL_NEW,OnAddSeatBlock)
	ON_COMMAND(ID_MODEL_DELETE,OnRemoveSeatBlock)
	ON_COMMAND(ID_MODEL_EDIT,OnEditSeatBlock)
	ON_BN_CLICKED(ID_FLIGHT_ADD,OnNewFlight)
	ON_BN_CLICKED(ID_FLIGHT_DEL,OnDelFlight)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT,OnSelChangeFlight)
END_MESSAGE_MAP()

BOOL CDlgSeatBlockDefine::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();


	OnInitToolbar();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	
	SetResize(IDC_LIST_FLIGHT,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_SEATBLOCK,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);

	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);


	SetResize(m_toolBarFlight.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_CENTER);
	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_TOP_CENTER);

	OnInitFlightList();
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

CAircraftPlacements* CDlgSeatBlockDefine::GetPlacement(OnboardFlightSeatBlock* pFlightSeatBlock)
{
	if(m_pOnboard == NULL)
		return NULL;

	COnBoardAnalysisCandidates *pOnBoardList = m_pOnboard->GetOnBoardFlightCandiates();
	COnboardFlight* pFlightData=pOnBoardList->GetExistOnboardFlight(pFlightSeatBlock->GetFlightID(),pFlightSeatBlock->GetActype(),FlightType(pFlightSeatBlock->GetFlightType()),pFlightSeatBlock->GetDay());
	if (pFlightData == NULL)
		return NULL;

	CAircaftLayOut* playout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID());
	if (playout == NULL)
		return NULL;
	
	return playout->GetPlacements();
}
// CDlgSeatBlockDefine message handlers
void CDlgSeatBlockDefine::OnAddSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	OnboardFlightSeatBlock* pFlightSeatBlock = (OnboardFlightSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
	if (pFlightSeatBlock == NULL)
		return;
	
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	if (hSelItem == hItem || hSelItem == NULL)
	{
		OnboardSeatBloceData* pSeatBlockItem = new OnboardSeatBloceData();

		HTREEITEM hBlockItem = m_wndTreeCtrl.InsertItem(pSeatBlockItem->GetBlockName(),cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hBlockItem,(DWORD_PTR)pSeatBlockItem);
		pFlightSeatBlock->AddItem(pSeatBlockItem);
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		m_wndTreeCtrl.DoEdit(hBlockItem);
		m_vBlockItem.push_back(hBlockItem);
	}
	else if(std::find(m_vBlockItem.begin(),m_vBlockItem.end(),hSelItem) != m_vBlockItem.end())
	{
		CAircraftPlacements* pPlacement = GetPlacement(pFlightSeatBlock);
		if (pPlacement == NULL)
			return;
		
		CDlgSeatGroupSelection dlg(pPlacement->GetSeatData());
		if(dlg.DoModal() == IDOK)
		{
			OnboardSeatBloceData* pSeatBlock = (OnboardSeatBloceData*)m_wndTreeCtrl.GetItemData(hSelItem);
			ALTObjectID grupID = dlg.getSeatGroup();
			pSeatBlock->AddItem(grupID);
			int idx = pSeatBlock->GetCount() - 1;
			HTREEITEM hSeatItem = m_wndTreeCtrl.InsertItem(grupID.GetIDString(),cni,FALSE,FALSE,hSelItem);
			m_wndTreeCtrl.SetItemData(hSeatItem,(DWORD_PTR)idx);
			m_vSeatItem.push_back(hSeatItem);
		}
		m_wndTreeCtrl.Expand(hSelItem,TVE_EXPAND);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSeatBlockDefine::OnRemoveSeatBlock()
{
	HTREEITEM hRootItem = m_wndTreeCtrl.GetRootItem();
	OnboardFlightSeatBlock* pFlightSeatBlock = (OnboardFlightSeatBlock*)m_wndTreeCtrl.GetItemData(hRootItem);
	if (pFlightSeatBlock == NULL)
		return;

	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (std::find(m_vBlockItem.begin(),m_vBlockItem.end(),hItem) != m_vBlockItem.end())
	{
		OnboardSeatBloceData* pItem = (OnboardSeatBloceData*)m_wndTreeCtrl.GetItemData(hItem);
		pFlightSeatBlock->DeleteItem(pItem);
	}
	else if(std::find(m_vSeatItem.begin(),m_vSeatItem.end(),hItem) != m_vSeatItem.end())
	{
		HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
		OnboardSeatBloceData* pItem = (OnboardSeatBloceData*)m_wndTreeCtrl.GetItemData(hParentItem);
		int idx = (int)m_wndTreeCtrl.GetItemData(hItem);
		pItem->DeleteItem(idx);
	}
	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSeatBlockDefine::OnEditSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (std::find(m_vBlockItem.begin(),m_vBlockItem.end(),hItem) != m_vBlockItem.end())
	{
		m_wndTreeCtrl.DoEdit(hItem);
	}
	else if(std::find(m_vSeatItem.begin(),m_vSeatItem.end(),hItem) != m_vSeatItem.end())
	{
		HTREEITEM hRootItem = m_wndTreeCtrl.GetRootItem();
		OnboardFlightSeatBlock* pFlightSeatBlock = (OnboardFlightSeatBlock*)m_wndTreeCtrl.GetItemData(hRootItem);
		if (pFlightSeatBlock == NULL)
			return;
		
		CAircraftPlacements* pPlacement = GetPlacement(pFlightSeatBlock);
		if (pPlacement == NULL)
			return;
		CDlgSeatGroupSelection dlg(pPlacement->GetSeatData());
		if(dlg.DoModal() == IDOK)
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			OnboardSeatBloceData* pSeatBlock = (OnboardSeatBloceData*)m_wndTreeCtrl.GetItemData(hParentItem);
			int idx = (int)m_wndTreeCtrl.GetItemData(hItem);

			ALTObjectID grupID = dlg.getSeatGroup();
			ALTObjectID& dataID = pSeatBlock->GetItem(idx);
			dataID = grupID;
			m_wndTreeCtrl.SetItemText(hItem,grupID.GetIDString());
		}
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

LRESULT CDlgSeatBlockDefine::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		{
			OnLButtonDblClk(wParam,lParam);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return 0;
		}
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			OnboardSeatBloceData* pItem = (OnboardSeatBloceData*)m_wndTreeCtrl.GetItemData(hItem);
			if (strValue.IsEmpty())
			{
				if (pItem->GetBlockName().IsEmpty())
				{
					m_wndTreeCtrl.DoEdit(hItem);
				}
			}
			else
			{
				pItem->SetBlockName(strValue);
				m_wndTreeCtrl.SetItemText(hItem,strValue);
			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case SELCHANGED:
		OnSelChanged(wParam,lParam);
		break;
	default:
		break;
	}

	return CXTResizeDialog::WindowProc(message,wParam,lParam);
}

void CDlgSeatBlockDefine::OnLButtonDblClk( WPARAM wParam, LPARAM lParam )
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(point, &uFlags);
	if(std::find(m_vSeatItem.begin(),m_vSeatItem.end(),hItem) != m_vSeatItem.end())
	{
		HTREEITEM hRootItem = m_wndTreeCtrl.GetRootItem();
		OnboardFlightSeatBlock* pFlightSeatBlock = (OnboardFlightSeatBlock*)m_wndTreeCtrl.GetItemData(hRootItem);
		if (pFlightSeatBlock == NULL)
			return;

		CAircraftPlacements* pPlacement = GetPlacement(pFlightSeatBlock);
		if (pPlacement == NULL)
			return;
		CDlgSeatGroupSelection dlg(pPlacement->GetSeatData());
		if(dlg.DoModal() == IDOK)
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			OnboardSeatBloceData* pSeatBlock = (OnboardSeatBloceData*)m_wndTreeCtrl.GetItemData(hParentItem);
			int idx = (int)m_wndTreeCtrl.GetItemData(hItem);

			ALTObjectID grupID = dlg.getSeatGroup();
			ALTObjectID& dataID = pSeatBlock->GetItem(idx);
			dataID = grupID;
			m_wndTreeCtrl.SetItemText(hItem,grupID.GetIDString());
		}
	}
}

void CDlgSeatBlockDefine::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pSeatBlockManager->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

LRESULT CDlgSeatBlockDefine::OnSelChanged( WPARAM wParam, LPARAM lParam )
{
	UpdateToolbarStatus();
	return 0;
}

void CDlgSeatBlockDefine::UpdateToolbarStatus()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hRootItem = m_wndTreeCtrl.GetRootItem();
	
	if (m_wndListBox.GetCurSel() == LB_ERR)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
	}
	else
	{
		if (hItem == NULL || hItem == hRootItem)
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		}
		else if(hRootItem == m_wndTreeCtrl.GetParentItem(hItem))
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
		}
		else
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
		}
	}
	
	
}

void CDlgSeatBlockDefine::OnSelChangeFlight()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,TRUE);
	}
	else
	{
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,FALSE);
	}
	OnInitTreeCtrl();
}

void CDlgSeatBlockDefine::OnInitToolbar()
{
	CRect rectFlight;
	m_wndListBox.GetWindowRect(&rectFlight);
	ScreenToClient(&rectFlight);
	rectFlight.bottom = rectFlight.top - 6;
	rectFlight.top = rectFlight.bottom - 22;
	rectFlight.left = rectFlight.left + 2;
	m_toolBarFlight.MoveWindow(&rectFlight,TRUE);
	m_toolBarFlight.ShowWindow(SW_SHOW);

	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,FALSE);

	CRect rectBlock;
	m_wndTreeCtrl.GetWindowRect(&rectBlock);
	ScreenToClient(&rectBlock);
	rectBlock.bottom = rectBlock.top - 6;
	rectBlock.top = rectBlock.bottom - 22;
	rectBlock.left = rectBlock.left + 2;
	m_wndToolBar.MoveWindow(&rectBlock,TRUE);
	m_wndToolBar.ShowWindow(SW_SHOW);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
}

void CDlgSeatBlockDefine::OnInitTreeCtrl()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	OnboardFlightSeatBlock* pFlightSeatBlock = (OnboardFlightSeatBlock*)m_wndListBox.GetItemData(nSel);
	if (pFlightSeatBlock == NULL)
		return;
	
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(_T("Seat Blocks"),cni,FALSE);
	m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pFlightSeatBlock);
	for(int i = 0; i < pFlightSeatBlock->GetItemCount(); i++)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_LABLE;
		OnboardSeatBloceData* pBlockData = pFlightSeatBlock->GetItem(i);
		CString strBlockName;
		HTREEITEM hBlockItem = m_wndTreeCtrl.InsertItem(pBlockData->GetBlockName(),cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hBlockItem,(DWORD_PTR)pBlockData);
		m_vBlockItem.push_back(hBlockItem);
		
		for (int j = 0; j < pBlockData->GetCount(); j++)
		{
			const ALTObjectID& objID = pBlockData->GetItem(j);
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = 0;

			HTREEITEM hSeatItem = m_wndTreeCtrl.InsertItem(objID.GetIDString(),cni,FALSE,FALSE,hBlockItem);
			m_wndTreeCtrl.SetItemData(hSeatItem,(DWORD_PTR)j);
			m_vSeatItem.push_back(hSeatItem);
		}
		m_wndTreeCtrl.Expand(hBlockItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgSeatBlockDefine::OnInitFlightList()
{
	for (int i = 0; i < m_pSeatBlockManager->GetItemCount(); i++)
	{
		OnboardFlightSeatBlock* pFlightSeatBlock = m_pSeatBlockManager->GetItem(i);
		CString strFlight;
		if (pFlightSeatBlock->GetFlightType() == ArrFlight)
		{
			strFlight.Format(_T("Flight %s - ACTYPE %s Arr Day%d"),pFlightSeatBlock->GetFlightID(),pFlightSeatBlock->GetActype(), pFlightSeatBlock->GetDay());
		}
		else
		{
			strFlight.Format(_T("Flight %s - ACTYPE %s Dep Day%d"),pFlightSeatBlock->GetFlightID(),pFlightSeatBlock->GetActype(), pFlightSeatBlock->GetDay());
		}

		int idx = m_wndListBox.AddString(strFlight);
		m_wndListBox.SetItemData(idx,(DWORD_PTR)pFlightSeatBlock);
	}

	if (m_wndListBox.GetCount() > 0)
	{
		m_wndListBox.SetCurSel(0);
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,TRUE);
		UpdateToolbarStatus();
		OnInitTreeCtrl();
	}
}

void CDlgSeatBlockDefine::OnNewFlight()
{
	CDlgOnboardFlightList dlg(m_pTerm);
	if(dlg.DoModal()==IDOK)
	{
		COnboardFlight *pOnboardFlight=dlg.getOnboardFlight();
		if(m_pSeatBlockManager->Exist(pOnboardFlight))
		{			
			MessageBox("Flight already exists in list.");
			return;
		}		
		OnboardFlightSeatBlock *pFlightSeatBlock = new OnboardFlightSeatBlock();
		pFlightSeatBlock->SetFlightID(pOnboardFlight->getFlightID());
		pFlightSeatBlock->SetFlightType(pOnboardFlight->getFlightType());
		pFlightSeatBlock->SetActype(pOnboardFlight->getACType());
		pFlightSeatBlock->SetDay(pOnboardFlight->getDay());
		m_pSeatBlockManager->AddItem(pFlightSeatBlock);
		CString strFlight;
		if (pFlightSeatBlock->GetFlightType() == ArrFlight)
		{
			strFlight.Format(_T("Flight %s - ACTYPE %s Arr Day%d"),pFlightSeatBlock->GetFlightID(),pFlightSeatBlock->GetActype(), pFlightSeatBlock->GetDay());
		}
		else
		{
			strFlight.Format(_T("Flight %s - ACTYPE %s Dep Day%d"),pFlightSeatBlock->GetFlightID(),pFlightSeatBlock->GetActype(), pFlightSeatBlock->GetDay());
		}
		int idx = m_wndListBox.AddString(strFlight);
		if (idx != LB_ERR)
		{
			m_wndListBox.SetItemData(idx,(DWORD)pFlightSeatBlock);
			m_wndListBox.SetCurSel(idx);
			UpdateToolbarStatus();
			OnInitTreeCtrl();
		}
	}	
}	

void CDlgSeatBlockDefine::OnDelFlight()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	OnboardFlightSeatBlock* pFlightSeatBlock = (OnboardFlightSeatBlock*)m_wndListBox.GetItemData(nSel);
	m_pSeatBlockManager->DeleteItem(pFlightSeatBlock);

	m_wndListBox.DeleteString(nSel);

	if (nSel > 0)
	{
		m_wndListBox.SetCurSel(nSel - 1);
	}

	UpdateToolbarStatus();
	OnInitTreeCtrl();
}

int CDlgSeatBlockDefine::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolBarFlight.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))
		|| !m_toolBarFlight.LoadToolBar(IDR_ADDDEL_FLIGHT))
	{
		TRACE0("Can't create toolbar!\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,CRect(0,0,0,0))
		|| !m_wndToolBar.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetDlgCtrlID(m_toolBarFlight.GetDlgCtrlID()+1);
	return 0;
}

void CDlgSeatBlockDefine::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pSeatBlockManager->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	CXTResizeDialog::OnOK();
}