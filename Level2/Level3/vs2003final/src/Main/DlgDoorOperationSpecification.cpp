// DlgDoorOperationSpecification.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgDoorOperationSpecification.h"
#include ".\dlgdooroperationspecification.h"
#include "../InputOnBoard/EntryDoorOperationalSpecification.h"
#include "../Database/ADODatabase.h"
#include "../Common/NewPassengerType.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/probab.h"
#include "../Common/ProbDistManager.h"
#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/CInputOnboard.h"
#include "DlgNewPassengerType.h"
#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard/AircraftPlacements.h"
#include "../InputOnBoard/DeckPlacement.h"
#include "DlgProbDist.h"
#include "Common\ProDistrubutionData.h"
#include "InputOnBoard\OnBoardAnalysisCondidates.h"
#include "DlgOnboardFlightList.h"
#include "InputOnBoard\OnBoardSeatBlockData.h"


// DlgDoorOperationSpecification dialog

IMPLEMENT_DYNAMIC(DlgDoorOperationSpecification, CXTResizeDialog)
DlgDoorOperationSpecification::DlgDoorOperationSpecification(InputTerminal* pTerminal,InputOnboard* pInputOnboard, bool bEntryDoor, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgDoorOperationSpecification::IDD, pParent)
	,m_pInputTerminal(pTerminal)
	,m_pInputOnboard(pInputOnboard)
	,m_bEntryDoor(bEntryDoor)
{
	try
	{
		if(bEntryDoor)
			m_pDoorOperations = new DoorOperationalSpecification(DoorOperationalSpecification::EntryDoor);
		else
			m_pDoorOperations = new DoorOperationalSpecification(DoorOperationalSpecification::ExitDoor);
		m_pDoorOperations->ReadData(pTerminal->inStrDict);


		m_pSeatBlocks = new OnboardFlightSeatBlockContainr;
		m_pSeatBlocks->ReadData(-1);

	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pDoorOperations)
		{
			delete m_pDoorOperations;
			m_pDoorOperations = NULL;
		}
		return;
	}
}

DlgDoorOperationSpecification::~DlgDoorOperationSpecification()
{
	if (m_pDoorOperations)
	{
		delete m_pDoorOperations;
		m_pDoorOperations = NULL;
	}
}

void DlgDoorOperationSpecification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DOOROPERATIONSETTING,m_wndTreeCtrl);
	DDX_Control(pDX,IDC_LIST_FLT,m_listFlight);

}


BEGIN_MESSAGE_MAP(DlgDoorOperationSpecification, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,AddNewItem)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,RemoveItem)

	ON_COMMAND(ID_TOOLBAR_ADD,AddFlight)
	ON_COMMAND(ID_TOOLBAR_DEL,DelFlight)

	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DOOROPERATIONSETTING, OnTvnSelchangedTreeDooroperationsetting)
	ON_LBN_SELCHANGE(IDC_LIST_FLT,OnSelChangeFlightList)
END_MESSAGE_MAP()


// DlgDoorOperationSpecification message handlers

void DlgDoorOperationSpecification::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pDoorOperations->SaveData();
		CADODatabase::CommitTransaction() ;
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
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

void DlgDoorOperationSpecification::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	OnOK();
}

void DlgDoorOperationSpecification::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL DlgDoorOperationSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	if (m_bEntryDoor)
	{
		SetWindowText("Entry Door Operation Specification");
	}
	else
	{
		SetWindowText("Exit Door Operation Specification");
	}

	CRect rect;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);

	GetDlgItem(IDC_STATIC_FLIGHT_TOOLBAR)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_FLIGHT_TOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_flightToolBar.MoveWindow(&rect,true);
	m_flightToolBar.ShowWindow(SW_SHOW);

	m_flightToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_flightToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);


	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetResize(m_flightToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_FLT,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_DOORFRAME,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_TOP_RIGHT);
	SetResize(IDC_TREE_DOOROPERATIONSETTING,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	//OnInitTreeCtrl();
	InitFlightList();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

int DlgDoorOperationSpecification::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR)||
		!m_wndToolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;// fail to create
	}

	if (!m_flightToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR)||
		!m_flightToolBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;// fail to create
	}

	return 0;
}

void DlgDoorOperationSpecification::AddNewItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if (hSel == NULL)
		return;

	SelTreeItemType eSelItem = GetItemType(hSel);


	if (eSelItem == TextSeatBlock)		//Add flight item
	{
		AddBlock(hSel);
	}
	else if(eSelItem == TextPaxType)
	{
		AddPaxType(hSel);
	}
	UpdateToolBarState();
}

void DlgDoorOperationSpecification::RemoveItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	SelTreeItemType eSelItem = GetItemType(hSel);

	if (eSelItem == SeatBlockItem)
	{
		RemoveBlock(hSel);
	}
	else if(eSelItem == PaxTypeItem)
	{
		RemovePaxType(hSel);
	}
	UpdateToolBarState();
}

void DlgDoorOperationSpecification::AddPaxType(HTREEITEM hSel)
{
	if (GetItemType(hSel) == TextPaxType )
	{
		CPassengerType* pPaxType = new CPassengerType(m_pInputTerminal->inStrDict);
		CDlgNewPassengerType dlg(pPaxType,m_pInputTerminal);
		dlg.DoModal();

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt=NT_NORMAL;
		cni.net=NET_SHOW_DIALOGBOX;

		CString strObjName = pPaxType->PrintStringForShow();

		DoorOperationalData* pDoorData = (DoorOperationalData*)GetItemData(hSel);
		if(pDoorData)
            pDoorData->AddPaxType(pPaxType);

		HTREEITEM hPaxItem = m_wndTreeCtrl.InsertItem(strObjName,cni,FALSE,FALSE,hSel);
		m_wndTreeCtrl.SetItemData(hPaxItem,NewItemData(PaxTypeItem, (DWORD_PTR)pPaxType));
		m_wndTreeCtrl.Expand(hSel,TVE_EXPAND);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}


}

void DlgDoorOperationSpecification::RemovePaxType(HTREEITEM hSel)
{
	if(hSel == NULL)
		return;

	HTREEITEM hParent = m_wndTreeCtrl.GetParentItem(hSel);
	if (GetItemType(hParent) == TextPaxType)
	{
		DoorOperationalData* pDoorData = (DoorOperationalData*)GetItemData(hParent);

		CPassengerType* pPaxItem = (CPassengerType*)GetItemData(hSel);

		pDoorData->DelPaxType(pPaxItem);
		m_wndTreeCtrl.DeleteItem(hSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

		//add default item
		if(m_wndTreeCtrl.GetChildItem(hParent) == NULL)
		{
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.nt=NT_NORMAL;
			cni.net=NET_SHOW_DIALOGBOX;
			HTREEITEM hPaxType = m_wndTreeCtrl.InsertItem("DEFAULT",cni, FALSE, FALSE, hParent, TVI_LAST);
			CPassengerType* pPaxType = new CPassengerType(m_pInputTerminal->inStrDict);
			m_wndTreeCtrl.SetItemData(hPaxType,(DWORD_PTR)NewItemData(PaxTypeItem,(DWORD_PTR)pPaxType) );
			pDoorData->AddPaxType(pPaxType);

		}
	}		
}
DlgDoorOperationSpecification::SelTreeItemType DlgDoorOperationSpecification::GetItemType(HTREEITEM hSel)
{
	if (hSel == NULL)
	{
		return NoneSel;
	}
	ItemData *pData = (ItemData *)m_wndTreeCtrl.GetItemData(hSel);
	if(pData)
		return pData->enumType;

	return NoneSel;
}

void DlgDoorOperationSpecification::OnTvnSelchangedTreeDooroperationsetting(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateToolBarState();
}
void DlgDoorOperationSpecification::UpdateToolBarState()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	SelTreeItemType selType =	GetItemType(hSel);
	if (selType == TextSeatBlock || selType == TextPaxType)			//root
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
		return;
	}
	if (selType == SeatBlockItem ||selType ==  PaxTypeItem)			//root
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		return;
	}	

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
}
void DlgDoorOperationSpecification::InitDoorDataAndTreeItems(HTREEITEM hDoorItem, DoorOperationalData*pDoorData)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;

	CString strStatus = "CLOSED";
	if (pDoorData->getStatus() == DoorOperationalData::OPEN)
		strStatus = "OPEN";
	HTREEITEM hSubItem = m_wndTreeCtrl.InsertItem("Status: "+ strStatus,cni,FALSE,FALSE,hDoorItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hSubItem,NewItemData(Status_item,(DWORD_PTR)pDoorData));


	if (pDoorData->getStatus() == DoorOperationalData::OPEN)
	{

		CString strContent = "Initiate closure(secs < STD): ";
		if (!m_bEntryDoor)
            strContent = "Initiate opening(secs > checks on): ";

		strContent += pDoorData->getInitateClosure()->getPrintDist();
		hSubItem = m_wndTreeCtrl.InsertItem(strContent,cni,FALSE,FALSE,hDoorItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hSubItem,NewItemData(InitialTime,(DWORD_PTR)pDoorData));	



		strContent = "Closure time(secs): ";
		if (!m_bEntryDoor)
			strContent = "Opening time(secs): ";
		strContent += pDoorData->getClosureTime()->getPrintDist();
		hSubItem = m_wndTreeCtrl.InsertItem(strContent,cni,FALSE,FALSE,hDoorItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hSubItem,NewItemData(ClosureTime,(DWORD_PTR)pDoorData));	


		cni.net = NET_NORMAL;
		hSubItem = m_wndTreeCtrl.InsertItem("Seat Block & Pax Type",cni,FALSE,FALSE,hDoorItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hSubItem,NewItemData(TextSeatBlockRoot,(DWORD_PTR)pDoorData));	


		cni.net = NET_NORMAL;
		strContent = "";
		strContent.Format("Seat Block");
		HTREEITEM hSeatBlockRoot = m_wndTreeCtrl.InsertItem(strContent,cni,FALSE,FALSE,hSubItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hSeatBlockRoot,NewItemData(TextSeatBlock,(DWORD_PTR)pDoorData));
		
		{
			cni.net = NET_COMBOBOX;
			std::vector<int> vSeatBlock = pDoorData->getSeatBlock();
			if (vSeatBlock.size() == 0)
			{
				strContent.Format("ALL SEAT BLOCKS");
				HTREEITEM hBlock = m_wndTreeCtrl.InsertItem(strContent,cni,FALSE,FALSE,hSeatBlockRoot,TVI_LAST);
				m_wndTreeCtrl.SetItemData(hBlock,(DWORD_PTR)NewItemData(SeatBlockItem,-1));
			}
			else
			{	
				OnboardFlightSeatBlock *pSeatBlock = GetFlightSeatBlock(getCurFlightDoorSpec());
				if(pSeatBlock)
				{
					for (int nBlock = 0; nBlock <(int)vSeatBlock.size(); ++ nBlock)
					{
						int nBlockID = vSeatBlock[nBlock];
						OnboardSeatBloceData* pBlock = pSeatBlock->GetByID(nBlockID);
						if(pBlock)
						{
							HTREEITEM hBlock = m_wndTreeCtrl.InsertItem(pBlock->GetBlockName(),cni,FALSE,FALSE,hSeatBlockRoot,TVI_LAST);
							m_wndTreeCtrl.SetItemData(hBlock,(DWORD_PTR)NewItemData(SeatBlockItem,nBlockID));
						}
						else
						{
							HTREEITEM hBlock = m_wndTreeCtrl.InsertItem("BLOCK NOT EXIST",cni,FALSE,FALSE,hSeatBlockRoot,TVI_LAST);
							m_wndTreeCtrl.SetItemData(hBlock,(DWORD_PTR)NewItemData(SeatBlockItem,nBlockID));
						}
					}
				}
			}
		}

		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		HTREEITEM hPaxRoot = m_wndTreeCtrl.InsertItem("Pax Type",cni,FALSE,FALSE,hSubItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hPaxRoot,(DWORD_PTR)NewItemData(TextPaxType,(DWORD_PTR)pDoorData) );

		{
			cni.net = NET_SHOW_DIALOGBOX;
			
			int nCount = pDoorData->getPaxTypeCount();
			if(nCount == 0)
			{
				HTREEITEM hPaxType = m_wndTreeCtrl.InsertItem("DEFAULT",cni, FALSE, FALSE, hPaxRoot, TVI_LAST);

				CPassengerType* pPaxType = new CPassengerType(m_pInputTerminal->inStrDict);
				m_wndTreeCtrl.SetItemData(hPaxType,(DWORD_PTR)NewItemData(PaxTypeItem,(DWORD_PTR)pPaxType) );
				pDoorData->AddPaxType(pPaxType);
			}
			else
			{
				for (int nItem = 0; nItem < nCount; ++ nItem)
				{	
					const CPassengerType* pPaxType = pDoorData->getPaxTypeByIdx(nItem);	

					HTREEITEM hPaxType = m_wndTreeCtrl.InsertItem(pPaxType->PrintStringForShow(),cni, FALSE, FALSE, hPaxRoot, TVI_LAST);
					m_wndTreeCtrl.SetItemData(hPaxType,(DWORD_PTR)NewItemData(PaxTypeItem,(DWORD_PTR)pPaxType));
				}
			}
		}
		m_wndTreeCtrl.Expand(hSeatBlockRoot,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hPaxRoot,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hSubItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hDoorItem, TVE_EXPAND);


}

LRESULT DlgDoorOperationSpecification::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	if(message == UM_CEW_COMBOBOX_BEGIN)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		SelTreeItemType eSelItem = GetItemType(hItem);

		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;
		if (pCB->GetCount() != 0)
			pCB->ResetContent();

		if (eSelItem == Status_item)	// Door item sons
		{
			pCB->AddString("CLOSED");
			pCB->AddString("OPEN");
	
		}
		else if(eSelItem == InitialTime || eSelItem == ClosureTime)//time distribution
		{
			int nCount = _g_GetActiveProbMan( m_pInputTerminal )->getCount();
			for( int idx =0; idx<nCount; idx++ )
			{
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInputTerminal )->getItem(idx);			
				pCB->AddString(pPBEntry->m_csName);
			}
			CString s;
			s.LoadString( IDS_STRING_NEWDIST );
			pCB->AddString(s);

		}
		else if(eSelItem == SeatBlockItem)
		{
			HTREEITEM hBlockRoot = m_wndTreeCtrl.GetParentItem(hItem);
			DoorOperationalData *pDoorData = (DoorOperationalData *)GetItemData(hBlockRoot);
			std::vector<int> vSeatBlock = pDoorData->getSeatBlock();
			OnboardFlightSeatBlock *pSeatBlock = GetFlightSeatBlock(getCurFlightDoorSpec());
			if(pSeatBlock)
			{
				int nCount = pSeatBlock->GetItemCount();
				for (int nItem = 0; nItem < nCount; ++ nItem)
				{
					OnboardSeatBloceData* pData = pSeatBlock->GetItem(nItem);
					if(std::find(vSeatBlock.begin(), vSeatBlock.end(), pData->GetID()) != vSeatBlock.end())
						continue;

					int nIndex = pCB->AddString(pData->GetBlockName());
					pCB->SetItemData(nIndex,pData->GetID());
				}
			}
		}
		pCB->SetCurSel(0);
		
	}
	if (message == UM_CEW_COMBOBOX_SELCHANGE || message == UM_CEW_COMBOBOX_END)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		SelTreeItemType eSelItem = GetItemType(hItem);

		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB = (CComboBox*)pWnd;

		if (eSelItem == Status_item)	// Door item sons
		{
			DoorOperationalData *pDoorData = (DoorOperationalData *)GetItemData(hItem);

			//Status
			CString strText =  *(CString*)lParam;
			ASSERT(pDoorData != NULL);			//the cur select door data cannot be null

			if (strText == "CLOSED")
			{
				pDoorData->setStatus(DoorOperationalData::CLOSED);
				pDoorData->RemoveOpenSetting();
				HTREEITEM hDoorItem = m_wndTreeCtrl.GetParentItem(hItem);
				HTREEITEM hDelItem = m_wndTreeCtrl.GetChildItem(hDoorItem);
				while (hDelItem != NULL)
				{
					m_wndTreeCtrl.DeleteItem(hDelItem);
					hDelItem = m_wndTreeCtrl.GetChildItem(hDoorItem);
				}
				InitDoorDataAndTreeItems(hDoorItem, pDoorData);
			}
			else
			{
				pDoorData->setStatus(DoorOperationalData::OPEN);

				HTREEITEM hDoorItem = m_wndTreeCtrl.GetParentItem(hItem);
				HTREEITEM hDelItem = m_wndTreeCtrl.GetChildItem(hDoorItem);
				while (hDelItem != NULL)
				{
					m_wndTreeCtrl.DeleteItem(hDelItem);
					hDelItem = m_wndTreeCtrl.GetChildItem(hDoorItem);
				}
				InitDoorDataAndTreeItems(hDoorItem, pDoorData);
				m_wndTreeCtrl.m_hPreItem = hItem;
			}
		}
		else if (eSelItem == InitialTime || eSelItem == ClosureTime)
		{
			DoorOperationalData *pDoorData = (DoorOperationalData *)GetItemData(hItem);

			CString strText =  *(CString*)lParam;
			CProbDistEntry* pPDEntry = NULL;
			CProbDistManager* pProbDistMan = m_pInputTerminal->m_pAirportDB->getProbDistMan();
			if(strText == "NEW PROBABILITY DISTRIBUTION" )
			{
				CDlgProbDist dlg(m_pInputTerminal->m_pAirportDB,true,this);
				if (dlg.DoModal() != IDOK && pProbDistMan->getCount() >0)
				{
					m_wndTreeCtrl.SetItemText(hItem,pProbDistMan->getItem(0)->m_csName);
					return 0;
				}

				CProbDistEntry* pPDEntry = dlg.GetSelectedPD();
				if(pPDEntry == NULL)
					return 0;
				strText = pPDEntry->m_csName;

				if (eSelItem == InitialTime)			//initiate time setting
				{
					pDoorData->getInitateClosure()->SetProDistrubution(pPDEntry);
					CString strContent = "Initiate closure(secs < STD): ";
					if (!m_bEntryDoor)
						strContent = "Initiate opening(secs > checks on): ";
					m_wndTreeCtrl.SetItemText(hItem, strContent+strText);
				}
				else				// open/close time
				{
					pDoorData->getClosureTime()->SetProDistrubution(pPDEntry);
					CString strContent = "Closure time(secs): ";
					if (!m_bEntryDoor)
						strContent = "Opening time(secs): ";
					m_wndTreeCtrl.SetItemText(hItem, strContent+strText);
				}
			}
			else
			{
				int nCount = pProbDistMan->getCount();
				for( int i=0; i<nCount; i++ )
				{
					pPDEntry = pProbDistMan->getItem( i );
					if(pPDEntry->m_csName == strText)
						break;
				}
				strText = pPDEntry->m_csName;	

				if (eSelItem == InitialTime)			//initiate time setting
				{
					pDoorData->getInitateClosure()->SetProDistrubution(pPDEntry);
					CString strContent = "Initiate closure(secs < STD): ";
					if (!m_bEntryDoor)
						strContent = "Initiate opening(secs > checks on): ";
					m_wndTreeCtrl.SetItemText(hItem, strContent+strText);
				}
				else				// open/close time
				{
					pDoorData->getClosureTime()->SetProDistrubution(pPDEntry);
					CString strContent = "Closure time(secs): ";
					if (!m_bEntryDoor)
						strContent = "Opening time(secs): ";
					m_wndTreeCtrl.SetItemText(hItem, strContent+strText);
				}


			} 

		}
		else if(eSelItem == SeatBlockItem)
		{
			CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB = (CComboBox*)pWnd;
			int nCurSel = pCB->GetCurSel();
			int nBlockID = (int)pCB->GetItemData(nCurSel);
			
			HTREEITEM hBlockRoot = m_wndTreeCtrl.GetParentItem(hItem);
			DoorOperationalData *pDoorData = (DoorOperationalData *)GetItemData(hBlockRoot);

			int nOldBlockID = GetItemData(hItem);
			if(pDoorData)
			{
				pDoorData->DelSeatBlock(nOldBlockID);
				pDoorData->AddSeatBlock(nBlockID);
			}
			m_wndTreeCtrl.SetItemData(hItem,NewItemData(SeatBlockItem, nBlockID));
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	if (message == UM_CEW_EDITSPIN_END )
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		SelTreeItemType eSelItem = GetItemType(hItem);

		//if (eSelItem != SeatBlockItem)		// not seat block set
		//	return 0;

		//COOLTREE_NODE_INFO* pInfo = m_wndTreeCtrl.GetItemNodeInfo( hItem );
		//CString strValue = *((CString*)lParam);
		//HTREEITEM hPrevious = m_wndTreeCtrl.GetNextItem(hItem, TVGN_PREVIOUS);	


		//GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	if (message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		SelTreeItemType eSelItem = GetItemType(hItem);

		CPassengerType* pPaxType = (CPassengerType*)GetItemData(hItem);
		CDlgNewPassengerType dlg(pPaxType,m_pInputTerminal);
		dlg.DoModal();
		m_wndTreeCtrl.SetItemText(hItem, pPaxType->PrintStringForShow());
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);		
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void DlgDoorOperationSpecification::AddFlight()
{
	//show dialog
	CDlgOnboardFlightList::FlightListType enumType = CDlgOnboardFlightList::ALL_FLIGHT_LIST;
	if(m_bEntryDoor)
		enumType = CDlgOnboardFlightList::DEP_FLIGHT_LIST;
	else
		enumType = CDlgOnboardFlightList::ARR_FLIGHT_LIST;


	CDlgOnboardFlightList dlg((Terminal *)m_pInputTerminal, enumType);
	if(dlg.DoModal()==IDOK)
	{
		COnboardFlight *onboardFlight=dlg.getOnboardFlight();

		if (FlightExist(onboardFlight))
		{
			MessageBox("Flight already exist.");
			return;
		}
		FlightDoorOperationalSpec *newFlightSpec = new FlightDoorOperationalSpec();
		newFlightSpec->setAirlineNumber(onboardFlight->getFlightID());
		newFlightSpec->setACType(onboardFlight->getACType());
		newFlightSpec->setFlightType((int)onboardFlight->getFlightType());
		newFlightSpec->setDay(onboardFlight->getDay());


		m_pDoorOperations->AddData(newFlightSpec);
		
		int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.getFlightString());
		m_listFlight.SetCurSel(tmpIndex);
		m_flightToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,true);
		m_listFlight.SetItemData(tmpIndex,(DWORD)newFlightSpec);

		RefreshTreeCtrl();
	}	

}
bool DlgDoorOperationSpecification::FlightExist(COnboardFlight *flight)
{

	for (int i=0;i<m_listFlight.GetCount();i++)
	{
		FlightDoorOperationalSpec *curFlight=((FlightDoorOperationalSpec *)m_listFlight.GetItemData(i));
		if (curFlight->getACType()==flight->getACType()
			&& curFlight->getAirlineNumber()==flight->getFlightID()
			&& curFlight->getFlightType()==flight->getFlightType()
			&& curFlight->getDay()==flight->getDay())
		{			
			return true;
		}
	}
	return false;
}
void DlgDoorOperationSpecification::DelFlight()
{

	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		return;
	}	
	m_pDoorOperations->DelData(getCurFlightDoorSpec());
	m_listFlight.DeleteString(curFlightIndex);	 
	if (curFlightIndex<m_listFlight.GetCount())
	{
		m_listFlight.SetCurSel(curFlightIndex);

	}else if(m_listFlight.GetCount()>0)
	{
		m_listFlight.SetCurSel(m_listFlight.GetCount()-1);
	}

	//initialize the door settings
	RefreshTreeCtrl();
}

FlightDoorOperationalSpec *DlgDoorOperationSpecification::getCurFlightDoorSpec()
{
	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		return NULL;
	}
	return (FlightDoorOperationalSpec *)m_listFlight.GetItemData(curFlightIndex);
}

void DlgDoorOperationSpecification::InitFlightList()
{
	int nCount = m_pDoorOperations->getFlightCount();
	for (int nFlight = 0; nFlight < nCount; ++nFlight)
	{
		 const FlightDoorOperationalSpec* pFlight = m_pDoorOperations->getFlightDataByIdx(nFlight);
		 ASSERT(pFlight != NULL);
		 if(pFlight)
		 {
			 int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),pFlight->GetFlightString());
			 m_listFlight.SetItemData(tmpIndex,(DWORD)pFlight);
		 }
	}

	if(nCount > 0)
	{
		m_listFlight.SetCurSel(0);
		m_flightToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,true);
		RefreshTreeCtrl();
	}

}

void DlgDoorOperationSpecification::RefreshTreeCtrl()
{
	m_wndTreeCtrl.DeleteAllItems();

	FlightDoorOperationalSpec* pSelFlight = getCurFlightDoorSpec();
	if(pSelFlight == NULL)
	{
		return;
	}


	//find the flight
	COnboardFlight* pOBFlight = m_pInputOnboard->GetOnBoardFlightCandiates()->GetExistOnboardFlight(pSelFlight->getAirlineNumber(),
																		pSelFlight->getACType(),
																		(FlightType)pSelFlight->getFlightType(),
																		pSelFlight->getDay());
	//flight does not exist	
	if(pOBFlight == NULL)
	{
		return;
	}

	//no layout defined
	CAircaftLayOut* pLayout = m_pInputOnboard->GetAircaftLayoutManager()->GetAircraftLayOutByFlightID(pOBFlight->getID());
	if(pLayout == NULL)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;

	CDoorDataSet* pDoorSet = pLayout->GetPlacements()->GetDoorData();

	std::vector<int > vExistDoor;
	int nDoorCount = pDoorSet->GetItemCount();
	for (int nDoorIdx = 0; nDoorIdx < nDoorCount; nDoorIdx++)
	{
		CDoor* pDoor = pDoorSet->GetItem(nDoorIdx);
		if (pDoor)
		{
			DoorOperationalData* pDoorData = pSelFlight->GetDoorOperationData(pDoor);
			if(pDoorData == NULL)
			{
				pDoorData = new DoorOperationalData;
				pSelFlight->AddDoorOperation(pDoorData);
			}

			CString strDoorName = pDoor->GetIDName().GetIDString();
			HTREEITEM hDoorItem = m_wndTreeCtrl.InsertItem(strDoorName,cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
			
			pDoorData->setDoorID(pDoor->GetID());
			pDoorData->setDeckID(pDoor->GetDeck()->GetID());
			vExistDoor.push_back(pDoor->GetID());

			m_wndTreeCtrl.SetItemData(hDoorItem,(DWORD_PTR)NewItemData(DoorSel,(DWORD_PTR)pDoorData));
			InitDoorDataAndTreeItems(hDoorItem, pDoorData);
		}
	}
	pSelFlight->RemoveInvalidDoor(vExistDoor);

}

DWORD_PTR DlgDoorOperationSpecification::GetItemData( HTREEITEM hSel )
{
	if (hSel == NULL)
	{
		return 0;
	}
	ItemData *pData = (ItemData *)m_wndTreeCtrl.GetItemData(hSel);
	if(pData)
		return pData->pData;

	return 0;
}

void DlgDoorOperationSpecification::AddBlock(HTREEITEM hSel)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;

	CString strContent = "  ";
	HTREEITEM hBlock = m_wndTreeCtrl.InsertItem(strContent,cni,FALSE,FALSE,hSel,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hBlock,(DWORD_PTR)NewItemData(SeatBlockItem,-1) );

	m_wndTreeCtrl.DoEdit(hBlock);

}

void DlgDoorOperationSpecification::RemoveBlock(HTREEITEM hSel)
{
	if(hSel == NULL)
		return;

	HTREEITEM hParent = m_wndTreeCtrl.GetParentItem(hSel);
	if (GetItemType(hSel) == SeatBlockItem)
	{
		DoorOperationalData* pDoorData = (DoorOperationalData*)GetItemData(hParent);

		int nBlockID = (int)GetItemData(hSel);

		pDoorData->DelSeatBlock(nBlockID);
		m_wndTreeCtrl.DeleteItem(hSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

		//add default item
		if(m_wndTreeCtrl.GetChildItem(hParent) == NULL)
		{
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.nt=NT_NORMAL;
			cni.net=NET_COMBOBOX;
			HTREEITEM hPaxType = m_wndTreeCtrl.InsertItem("ALL SEAT BLOCKS",cni, FALSE, FALSE, hParent, TVI_LAST);
			m_wndTreeCtrl.SetItemData(hPaxType, NewItemData(SeatBlockItem,-1));
		}
	}
}

OnboardFlightSeatBlock * DlgDoorOperationSpecification::GetFlightSeatBlock( FlightDoorOperationalSpec* pFlight )
{
	std::map<FlightDoorOperationalSpec *,OnboardFlightSeatBlock *>::iterator iter = m_mapFlightSeatBlock.find(pFlight);
	if(iter != m_mapFlightSeatBlock.end())
		return iter->second;

	OnboardFlightSeatBlock *pSeatBlock = m_pSeatBlocks->GetSeatBlock(pFlight->getAirlineNumber(), pFlight->getACType(), pFlight->getFlightType(), pFlight->getDay());

	m_mapFlightSeatBlock[pFlight] = pSeatBlock;
	ASSERT(pSeatBlock != NULL);

	return pSeatBlock;
}

void DlgDoorOperationSpecification::OnSelChangeFlightList()
{
	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		m_flightToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,false);
		return;
	}

	RefreshTreeCtrl();
}































