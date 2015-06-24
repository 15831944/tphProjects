// DlgSeatBlock.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatBlock.h"
#include "../InputOnboard/AircaftLayOut.h"
#include "../InputOnboard/Seat.h"
#include "../InputOnboard/AircraftPlacements.h"
#include ".\dlgseatblock.h"
#include "DlgSeatSelection.h"


// CDlgSeatBlock dialog

IMPLEMENT_DYNAMIC(CDlgSeatBlock, CXTResizeDialog)
CDlgSeatBlock::CDlgSeatBlock(COnBoardSeatBlockList* pSeatBlockList,int nFlightID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSeatBlock::IDD, pParent)
	,m_nFlightID(nFlightID)
	,m_pSeatBlockList(pSeatBlockList)
	,m_pCopySeatBlockList(NULL)
	,m_pPlacement(NULL)
{
	
}

CDlgSeatBlock::~CDlgSeatBlock()
{
	if (m_pCopySeatBlockList)
	{
		delete m_pCopySeatBlockList;
		m_pCopySeatBlockList = NULL;
	}
}

void CDlgSeatBlock::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSeatBlock, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_MODEL_NEW,OnAddSeatBlock)
	ON_COMMAND(ID_MODEL_DELETE,OnRemoveSeatBlock)
	ON_COMMAND(ID_MODEL_EDIT,OnEditSeatBlock)
	ON_COMMAND(ID_ADD_SEAT,OnAddSeats)
	ON_COMMAND(ID_DELETE_NAME,OnDeleteName)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
END_MESSAGE_MAP()


// CDlgSeatBlock message handlers

BOOL CDlgSeatBlock::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_pCopySeatBlockList = new COnBoardSeatBlockList();
	*m_pCopySeatBlockList = *m_pSeatBlockList;
	CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(m_nFlightID);
	if (_layout)
	{
		m_pPlacement = _layout->GetPlacements();
	}
	OnInitToolbar();
	OnInitTreeCtrl();
	UpdateToolbarStatus();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgSeatBlock::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CDlgSeatBlock::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect(&rect);
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
void CDlgSeatBlock::OnAddSeats()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardSeatBlockItem* pItem = (COnBoardSeatBlockItem*)m_wndTreeCtrl.GetItemData(hItem);
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	cni.unMenuID = 0;
	COnBoardSeatBlock* pSeatBlock = NULL;
	if (m_pPlacement == NULL)
	{
		MessageBox(_T("Please define flight layout"));
		return;
	}

	if (m_pPlacement->GetSeatData()->GetItemCount() == 0)
	{
		MessageBox(_T("Please define seat in layout"));
		return;
	}
	else
	{
		pSeatBlock = new COnBoardSeatBlock();
		pSeatBlock->setStartSeat(m_pPlacement->GetSeatData()->GetItem(0)->GetID());
		pSeatBlock->setEndSeat(m_pPlacement->GetSeatData()->GetItem(0)->GetID());
		pItem->addItem(pSeatBlock);
	}
	
	CString strValue = _T("");
	CSeat* pStartSeat = NULL;
	pStartSeat = m_pPlacement->GetSeatData()->GetSeatByID(pSeatBlock->getStartSeat());
	strValue.Format(_T("Start at seat: %s"),pStartSeat->GetIDName().GetIDString());
	HTREEITEM hStartItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hStartItem,(DWORD_PTR)pSeatBlock);
	m_vStartItem.push_back(hStartItem);

	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	strValue.Format(_T("Increment by: %s"),getStringByIncreaseType(pSeatBlock->getIncreaseType()));
	HTREEITEM hTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hStartItem);
	m_wndTreeCtrl.SetItemData(hTypeItem,(DWORD_PTR)pSeatBlock);
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pSeatBlock->getIncreaseCount());
	strValue.Format(_T("Number of %s: %d"),getStringByIncreaseType(pSeatBlock->getIncreaseType()),pSeatBlock->getIncreaseCount());
	HTREEITEM hNumItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hTypeItem);
	m_wndTreeCtrl.SetItemData(hNumItem,(DWORD_PTR)pSeatBlock);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CSeat* pEndSeat = NULL;
	pEndSeat = m_pPlacement->GetSeatData()->GetSeatByID(pSeatBlock->getEndSeat());
	strValue.Format(_T("End at seat: %s"),pEndSeat->GetIDName().GetIDString());
	HTREEITEM hEndItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hEndItem,(DWORD_PTR)pSeatBlock);
	m_vEndItem.push_back(hEndItem);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	m_wndTreeCtrl.Expand(hTypeItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hStartItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

LRESULT CDlgSeatBlock::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		{
			OnLButtonDblClk(wParam,lParam);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return 0;
		}
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;
			m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			if(pCB->GetCount()!=0)
			{
				pCB->ResetContent();
			}
			pCB->AddString(_T("Row"));
			pCB->AddString(_T("Column"));
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			COnBoardSeatBlock* pSeatBlock = (COnBoardSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
			pSeatBlock->setIncreaseType(getSeatInCreaseTypeByString(strValue));
			strValue.Format(_T("Increment by: %s"),getStringByIncreaseType(pSeatBlock->getIncreaseType()));
			m_wndTreeCtrl.SetItemText(hItem,strValue);
			HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
			SeatInCreaseType emInCreaseType;
			if (pSeatBlock->getIncreaseType() == ROW_SORT)
			{
				emInCreaseType = CLOUMNS_SORT;
			}
			else
			{
				emInCreaseType = ROW_SORT;
			}
			strValue.Format(_T("Number of %s: %d"),getStringByIncreaseType(emInCreaseType),pSeatBlock->getIncreaseCount());
			m_wndTreeCtrl.SetItemText(hChildItem,strValue);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			COnBoardSeatBlock* pSeatBlock = (COnBoardSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			CString strData = _T("");
			pSeatBlock->setIncreaseCount(atoi(strValue));
			cni->fMinValue = static_cast<float>(atoi(strValue));
			SeatInCreaseType emInCreaseType;
			if (pSeatBlock->getIncreaseType() == ROW_SORT)
			{
				emInCreaseType = CLOUMNS_SORT;
			}
			else
			{
				emInCreaseType = ROW_SORT;	
			}
			strData.Format(_T("Number of %s: %s"),getStringByIncreaseType(emInCreaseType),strValue);
			m_wndTreeCtrl.SetItemText(hItem,strData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			COnBoardSeatBlockItem* pItem = (COnBoardSeatBlockItem*)m_wndTreeCtrl.GetItemData(hItem);
			if (strValue.IsEmpty())
			{
				if (pItem->getBlockName().IsEmpty())
				{
					m_wndTreeCtrl.DoEdit(hItem);
				}
			}
			else
			{
				pItem->setBlockName(strValue);
				CString strData = _T("");
				strData.Format(_T("Name: %s"),strValue);
				m_wndTreeCtrl.SetItemText(hItem,strData);
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
void CDlgSeatBlock::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(point, &uFlags);
	COnBoardSeatBlock* pSeatBlock = NULL;
	HTREEITEM hParentItem = NULL;
	CString strValue = _T("");
	if(std::find(m_vStartItem.begin(),m_vStartItem.end(),hItem) != m_vStartItem.end())
	{
		CDlgSeatSelection dlg(m_pPlacement->GetSeatData());
		if(dlg.DoModal() == IDOK)
		{
			pSeatBlock = (COnBoardSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
			pSeatBlock->setStartSeat(dlg.getSeat()->GetID());
			strValue.Format(_T("Start at seat: %s"),dlg.getSeat()->GetIDName().GetIDString());
			m_wndTreeCtrl.SetItemText(hItem,strValue);
		}
	}

	if (std::find(m_vEndItem.begin(),m_vEndItem.end(),hItem) != m_vEndItem.end())
	{
		CDlgSeatSelection dlg(m_pPlacement->GetSeatData());
		if(dlg.DoModal() == IDOK)
		{
			pSeatBlock = (COnBoardSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
			pSeatBlock->setEndSeat(dlg.getSeat()->GetID());
			strValue.Format(_T("End at seat: %s"),dlg.getSeat()->GetIDName().GetIDString());
			m_wndTreeCtrl.SetItemText(hItem,strValue);
		}
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSeatBlock::OnDeleteName()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	if (hItem == NULL || hChildItem == NULL)
	{
		return;
	}
	COnBoardSeatBlockItem* pItem = (COnBoardSeatBlockItem*)m_wndTreeCtrl.GetItemData(hItem);
	COnBoardSeatBlock* pSeatBlock = (COnBoardSeatBlock*)m_wndTreeCtrl.GetItemData(hChildItem);
	pItem->removeItem(pSeatBlock);
	m_wndTreeCtrl.DeleteItem(hChildItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSeatBlock::OnRemoveSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardSeatBlockItem* pItem = (COnBoardSeatBlockItem*)m_wndTreeCtrl.GetItemData(hItem);
	m_pCopySeatBlockList->removeItem(pItem);
	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSeatBlock::OnAddSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	cni.unMenuID = IDR_MENU_ONBOARD_SEATS;
	COnBoardSeatBlockItem* pSeatBlockItem = new COnBoardSeatBlockItem();
	CString strValue = _T("");
	strValue.Format(_T("Name: %s"),pSeatBlockItem->getBlockName());
	HTREEITEM hBlockItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hBlockItem,(DWORD_PTR)pSeatBlockItem);
	m_pCopySeatBlockList->addItem(pSeatBlockItem);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	m_wndTreeCtrl.DoEdit(hBlockItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSeatBlock::OnEditSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	m_wndTreeCtrl.DoEdit(hItem);
}

void CDlgSeatBlock::OnInitTreeCtrl()
{
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(_T("Seat Blocks"),cni,FALSE);
	for (int i = 0; i < m_pCopySeatBlockList->getCount(); i++)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_LABLE;
		cni.unMenuID = IDR_MENU_ONBOARD_SEATS;
		CString strValue = _T("");
		COnBoardSeatBlockItem* pSeatBlockItem = m_pCopySeatBlockList->getItem(i);
		strValue.Format(_T("Name: %s"),pSeatBlockItem->getBlockName());
		HTREEITEM hBlockItem = m_wndTreeCtrl.InsertItem(pSeatBlockItem->getBlockName(),cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hBlockItem,(DWORD_PTR)pSeatBlockItem);
		for (int j = 0; j < pSeatBlockItem->getCount(); j++)
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = 0;
			COnBoardSeatBlock* pSeatBlock = pSeatBlockItem->getItem(j);
			CSeat* pStartSeat = NULL;
			pStartSeat = m_pPlacement->GetSeatData()->GetSeatByID(pSeatBlock->getStartSeat());
			strValue.Format(_T("Start at seat: %s"),pStartSeat->GetIDName().GetIDString());
			HTREEITEM hStartItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hBlockItem);
			m_wndTreeCtrl.SetItemData(hStartItem,(DWORD_PTR)pSeatBlock);
			m_vStartItem.push_back(hStartItem);

			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			strValue.Format(_T("Increment by: %s"),getStringByIncreaseType(pSeatBlock->getIncreaseType()));
			HTREEITEM hTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hStartItem);
			m_wndTreeCtrl.SetItemData(hTypeItem,(DWORD_PTR)pSeatBlock);
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			cni.fMinValue = static_cast<float>(pSeatBlock->getIncreaseCount());
			SeatInCreaseType emInCreaseType;
			if (pSeatBlock->getIncreaseType() == ROW_SORT)
			{
				emInCreaseType = CLOUMNS_SORT;
			}
			else
			{
				emInCreaseType = ROW_SORT;	
			}
			strValue.Format(_T("Number of %s: %d"),getStringByIncreaseType(emInCreaseType),pSeatBlock->getIncreaseCount());
			HTREEITEM hNumItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hTypeItem);
			m_wndTreeCtrl.SetItemData(hNumItem,(DWORD_PTR)pSeatBlock);
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			CSeat* pEndSeat = NULL;
			pEndSeat = m_pPlacement->GetSeatData()->GetSeatByID(pSeatBlock->getEndSeat());
			strValue.Format(_T("End at seat: %s"),pEndSeat->GetIDName().GetIDString());
			HTREEITEM hEndItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hBlockItem);
			m_vEndItem.push_back(hEndItem);
			m_wndTreeCtrl.SetItemData(hEndItem,(DWORD_PTR)pSeatBlock);
			m_wndTreeCtrl.Expand(hTypeItem,TVE_EXPAND);
			m_wndTreeCtrl.Expand(hStartItem,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hBlockItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

CString CDlgSeatBlock::getStringByIncreaseType(SeatInCreaseType emInCreaseType)
{
	CString strValue = _T("");
	switch(emInCreaseType)
	{
	case ROW_SORT:
		strValue = _T("Row");
		break;
	case CLOUMNS_SORT:
		strValue = _T("Column");
	    break;
	default:
	    break;
	}
	return strValue;
}

SeatInCreaseType CDlgSeatBlock::getSeatInCreaseTypeByString(CString strValue)
{
	if (!strValue.CompareNoCase(_T("Row")))
	{
		return ROW_SORT;
	}
	else
	{
		return CLOUMNS_SORT;
	}
}

void CDlgSeatBlock::OnOK()
{
	CXTResizeDialog::OnOK();
}

void CDlgSeatBlock::OnBtnSave()
{
	m_pSeatBlockList->Clear();
	*m_pSeatBlockList = *m_pCopySeatBlockList;
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pSeatBlockList->SaveData(m_nFlightID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgSeatBlock::UpdateToolbarStatus()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();

	if (hItem == m_wndTreeCtrl.GetRootItem() || hItem == NULL)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
	}
	else if (m_wndTreeCtrl.GetParentItem(hItem) == m_wndTreeCtrl.GetRootItem())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
	}
}

LRESULT CDlgSeatBlock::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	UpdateToolbarStatus();
	return 0;
}