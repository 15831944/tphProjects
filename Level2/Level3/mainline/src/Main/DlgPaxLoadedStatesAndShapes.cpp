// DlgPaxLoadedStatesAndShapes.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgPaxLoadedStatesAndShapes.h"
#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/PaxLoadedStatesAndShapes.h"
#include "DlgCarryonOwnership.h"
#include "../InputOnBoard/CarryonOwnership.h"
#include "../InputOnBoard/LoadedStateAndShapData.h"
#include "../Database/ADODatabase.h"
#include "DlgNewPassengerType.h"

// DlgPaxLoadedStatesAndShapes dialog

IMPLEMENT_DYNAMIC(DlgPaxLoadedStatesAndShapes, CXTResizeDialog)
DlgPaxLoadedStatesAndShapes::DlgPaxLoadedStatesAndShapes(InputTerminal * pInterm,CCarryonOwnership* pCarryonOwnership,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgPaxLoadedStatesAndShapes::IDD, pParent)
	,m_pInterm(pInterm)
	,m_pCarryonOwnership(pCarryonOwnership)
{
	try
	{	
		m_pPaxLoadedStatesAndShapes = new PaxLoadedStatesAndShapesList;
		m_pPaxLoadedStatesAndShapes->ReadData(pInterm->inStrDict);
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pPaxLoadedStatesAndShapes)
		{
			delete m_pPaxLoadedStatesAndShapes;
			m_pPaxLoadedStatesAndShapes = NULL;
		}
		return;
	}

}

DlgPaxLoadedStatesAndShapes::~DlgPaxLoadedStatesAndShapes()
{
	delete m_pPaxLoadedStatesAndShapes;
	m_pPaxLoadedStatesAndShapes = NULL;
}

void DlgPaxLoadedStatesAndShapes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_PAXTYPE,m_lisPaxType);
	DDX_Control(pDX,IDC_LIST_STATESANDSHAPE,m_wndListCtrl);

}


BEGIN_MESSAGE_MAP(DlgPaxLoadedStatesAndShapes, CXTResizeDialog)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPE, OnSelchangePaxType)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_STATESANDSHAPE, OnLvnEndlabeleditListContents)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_STATESANDSHAPE, OnItemchangedListData)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarPaxTypeAdd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarPaxTypeDel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarPaxTypeEdit)

	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnDataAdd)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnDataDel)

	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgPaxLoadedStatesAndShapes message handlers
BOOL DlgPaxLoadedStatesAndShapes::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();

	// TODO: Add extra initialization here

	CRect rectPaxToolbar;
	GetDlgItem(IDC_STATIC_PAXTYPE)->GetWindowRect(&rectPaxToolbar);
	GetDlgItem(IDC_STATIC_PAXTYPE)->ShowWindow(FALSE);
	ScreenToClient(&rectPaxToolbar);
	m_wndPaxToolBar.MoveWindow(&rectPaxToolbar, true);
	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE);
	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);
	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);

	CRect rectDataToolbar;
	GetDlgItem(IDC_STATIC_STATEANDSHAPE)->GetWindowRect(&rectDataToolbar);
	GetDlgItem(IDC_STATIC_STATEANDSHAPE)->ShowWindow(FALSE);
	ScreenToClient(&rectDataToolbar);
	m_wndDataToolBar.MoveWindow(&rectDataToolbar, true);
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, FALSE);
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);
	m_wndDataToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );

	SetResize(IDC_STATIC_TYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_LINK, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(m_wndPaxToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndDataToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_RIGHT);

	SetResize(IDC_LIST_PAXTYPE, SZ_TOP_LEFT , SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_STATESANDSHAPE, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitListBox();
	SetListCtrl();
	InitListCtrl();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int DlgPaxLoadedStatesAndShapes::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_wndPaxToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR) ||
		!m_wndPaxToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndDataToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),IDR_PEOPLEMOVEBAR) ||
		!m_wndDataToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void DlgPaxLoadedStatesAndShapes::InitListBox()
{
	int nCount = m_pPaxLoadedStatesAndShapes->getDataCount();
	for (int i =0; i < nCount; i++)
	{
		PaxLoadedStatesAndShapes* pItem = m_pPaxLoadedStatesAndShapes->getData(i);
		CString strPaxType = pItem->getNamedPaxType()->PrintStringForShow();

		int iIndex = m_lisPaxType.AddString(strPaxType);
		ASSERT( iIndex!=LB_ERR );	//only judge error when insert string to listbox
		m_lisPaxType.SetItemData(iIndex, (DWORD_PTR)pItem); 

	}

	m_lisPaxType.SetCurSel(0);
}

void DlgPaxLoadedStatesAndShapes::SetListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	
		"PACKS",			
		"PURSE",
		"BRIEFCASE",
		"COAT",
		"ROLLERBOARD",
		"BACKPACK",
		"DUTYFREE",
		"USERDEFINE1",
		"USERDEFINE2",
		"USERDEFINE3",
		"SHAPE"
	};
	int DefaultColumnWidth[] = { 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,100};

	int nColFormat[] = 
	{	
			LVCFMT_NOEDIT, 
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_BMP
	};

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	CStringList strList;
	strList.RemoveAll();
	lvc.csList = &strList;

	for (int i = 0; i < 11; i++)
	{
		lvc.fmt = nColFormat[i];	
		lvc.cx = DefaultColumnWidth[i];
		if (i ==0 || i ==10)
		{
			lvc.pszText = columnLabel[i];
		}
		else
		{
			CString strName = m_pCarryonOwnership->GetItemName((EnumCarryonType)(i-1));
			if (strName.IsEmpty())
			{
				lvc.pszText = columnLabel[i];
			}
			else
			{
				lvc.pszText = (char*)(LPCSTR)strName;
			}
		}
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void DlgPaxLoadedStatesAndShapes::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pPaxLoadedStatesAndShapes->SaveData();
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
		CADODatabase::CommitTransaction() ;
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

void DlgPaxLoadedStatesAndShapes::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	CXTResizeDialog::OnOK();
}

void DlgPaxLoadedStatesAndShapes::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CXTResizeDialog::OnCancel();
}

void DlgPaxLoadedStatesAndShapes::OnSelchangePaxType()
{
	m_wndListCtrl.DeleteAllItems();
	InitListCtrl();
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, TRUE);
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);

	if (m_lisPaxType.GetCurSel() == LB_ERR)
	{
		m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);
		m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);
	}
	else
	{
		m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, TRUE);
		m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, TRUE);
	}
	
}

void DlgPaxLoadedStatesAndShapes::InitListCtrl()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx >=0)
	{
		PaxLoadedStatesAndShapes* pItem = (PaxLoadedStatesAndShapes*)m_lisPaxType.GetItemData(nIdx);
		if (pItem == NULL)
			return;

		int nCount = pItem->getDataCount();
		for (int i = 0; i < nCount; i++)
		{
			LoadedStateAndShapData* pData = pItem->getData(i);
			CString strName;
			strName.Format("PACK%d", pData->getPackID());
			m_wndListCtrl.InsertItem(i,strName);

			std::vector<CarryonItemState> vStates = pData->getStateList();
			int nDataCount = vStates.size();
			for (int j =0; j < nDataCount; j++)
			{
				CarryonItemState state = vStates.at(j);
				CString strCount;
				strCount.Format("%d",state.m_nItemCount);
				m_wndListCtrl.SetItemText(i,(int)state.m_eItemType+1,strCount);
			}
			m_wndListCtrl.SetItemData(i,(DWORD_PTR)pData);
		}
	}
}

void DlgPaxLoadedStatesAndShapes::OnListItemChanged(int nItem, int nSubItem)
{
	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	int dCurVal = atoi(strCurVal.GetBuffer(0));	
	LoadedStateAndShapData* pData = (LoadedStateAndShapData*)m_wndListCtrl.GetItemData(nItem);;
	std::vector<CarryonItemState>& vStates = pData->getStateList();
	int nCount = vStates.size();
	int i =0;
	for (i =0; i < nCount; i++)
	{
		CarryonItemState _state = vStates.at(i);
		if ((int)_state.m_eItemType == nSubItem-1)
		{
			_state.m_nItemCount = dCurVal;
			vStates.erase(vStates.begin()+i);
			vStates.push_back(_state);
			break;
		}
	}
	if (i>= nCount)
	{
		CarryonItemState _state;
		_state.m_eItemType = (EnumCarryonType)(nSubItem-1);
		_state.m_nItemCount = dCurVal;
		vStates.push_back(_state);
	}


}

void DlgPaxLoadedStatesAndShapes::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if ( nItem < 0|| nSubItem >=10)
		return;

	OnListItemChanged(nItem, nSubItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

void DlgPaxLoadedStatesAndShapes::OnToolbarPaxTypeAdd()
{
	PaxLoadedStatesAndShapes* pItem = new PaxLoadedStatesAndShapes(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(pItem->getNamedPaxType(),m_pInterm,this);

	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = pItem->getNamedPaxType()->PrintStringForShow();

		if (!m_pPaxLoadedStatesAndShapes->findExistName(strPaxType))
		{
			int iIndex = m_lisPaxType.AddString(strPaxType);
			ASSERT( iIndex!=LB_ERR );		//only judge error when insert string to listbox
			m_lisPaxType.SetItemData(iIndex, (DWORD_PTR)pItem); 
			m_pPaxLoadedStatesAndShapes->AddData(pItem);

			m_lisPaxType.SetCurSel(iIndex);
			m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, TRUE);

			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return;
		}
		else
		{
			AfxMessageBox(_T("The passenger type already exists!"));
		}
	}

	if (pItem)
	{
		delete pItem;
		pItem = NULL;
	}
}

void DlgPaxLoadedStatesAndShapes::OnToolbarPaxTypeDel()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx == LB_ERR)
	{
		return;
	}

	PaxLoadedStatesAndShapes* pDelItem = (PaxLoadedStatesAndShapes*)m_lisPaxType.GetItemData(nIdx);
	if (pDelItem)
	{
		m_lisPaxType.SetItemData(nIdx,NULL);
		m_pPaxLoadedStatesAndShapes->DelData(pDelItem);
	}
	m_lisPaxType.DeleteString(nIdx);
	if (nIdx == 0)
	{
		m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);
		m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);
		m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE);
		m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE,FALSE);
	}
	else
	{
		m_lisPaxType.SetCurSel(nIdx-1);
	}
	

	m_wndListCtrl.DeleteAllItems();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void DlgPaxLoadedStatesAndShapes::OnToolbarPaxTypeEdit()
{
	int nIdx = m_lisPaxType.GetCurSel();
	PaxLoadedStatesAndShapes* pItem = (PaxLoadedStatesAndShapes*)m_lisPaxType.GetItemData(nIdx);
	if (pItem)
	{
		CDlgNewPassengerType dlg(pItem->getNamedPaxType(),m_pInterm,this);
		if(dlg.DoModal() == IDOK)
		{
			CString strPaxType = pItem->getNamedPaxType()->PrintStringForShow();
			m_lisPaxType.SetItemData(nIdx,NULL);
			m_lisPaxType.DeleteString(nIdx);

			m_lisPaxType.InsertString(nIdx, strPaxType);
			m_lisPaxType.SetItemData(nIdx,(DWORD_PTR)pItem);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}



	}
}

void DlgPaxLoadedStatesAndShapes::OnDataAdd()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx <0)
		return;

	PaxLoadedStatesAndShapes* pItem = (PaxLoadedStatesAndShapes*)m_lisPaxType.GetItemData(nIdx);
	if (pItem)
	{
		int nCount = pItem->getDataCount();
		int nMaxID = pItem->GetMaxPACKID();
		LoadedStateAndShapData* pData = new LoadedStateAndShapData;
		pData->setPackID(nMaxID+1);
		CString strPackID;
		strPackID.Format("PACK%d",nMaxID+1);
		m_wndListCtrl.InsertDataItem(nCount,pData);
		pItem->AddData(pData);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}


}

void DlgPaxLoadedStatesAndShapes::OnDataDel()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx <0)
		return;

	PaxLoadedStatesAndShapes* pItem = (PaxLoadedStatesAndShapes*)m_lisPaxType.GetItemData(nIdx);
	if (pItem)
	{
		int nSel =  m_wndListCtrl.GetCurSel();
		if (nSel == -1)
			return;

		LoadedStateAndShapData* pData = (LoadedStateAndShapData*)m_wndListCtrl.GetItemData(nSel);
		pItem->DelData(pData);
		m_wndListCtrl.DeleteItemEx(nSel);
		if (nSel == 0)
		{
			m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE,FALSE);
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void DlgPaxLoadedStatesAndShapes::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( m_wndListCtrl.GetCurSel() > -1 )
	{
		m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, TRUE);
	}
	else
	{
		m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);
	}

	*pResult = 0;
}