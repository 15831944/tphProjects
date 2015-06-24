#include "StdAfx.h"
#include "resource.h"
#include ".\dlgcarryonstoragepriorities.h"
#include "../InputOnBoard/CInputOnboard.h"
#include "../InputOnBoard/CarryonStoragePriorities.h"
#include "../InputOnBoard/Storage.h"
#include "../inputs/IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "DlgNewPassengerType.h"
#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard/AircraftPlacements.h"

IMPLEMENT_DYNAMIC(DlgCarryonStoragePriorities, CXTResizeDialog)
DlgCarryonStoragePriorities::DlgCarryonStoragePriorities(InputTerminal * pInterm,InputOnboard* pInOnboard, CWnd* pParent /*=NULL*/)
: CXTResizeDialog(DlgCarryonStoragePriorities::IDD, pParent)
,m_pInOnboard(pInOnboard)
,m_pInterm(pInterm)
,m_pStorages(NULL)
,m_pSelFlightData(NULL)
{
	try
	{	
		m_pCarryonStoragePriorities = new CCarryonStoragePriorities;
		m_pCarryonStoragePriorities->ReadData(pInterm->inStrDict);
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pCarryonStoragePriorities)
		{
			delete m_pCarryonStoragePriorities;
			m_pCarryonStoragePriorities = NULL;
		}
		return;
	}

}

DlgCarryonStoragePriorities::~DlgCarryonStoragePriorities(void)
{
	delete m_pCarryonStoragePriorities;
	m_pCarryonStoragePriorities = NULL;
}

void DlgCarryonStoragePriorities::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_FLIGHT,m_listFlight);
	DDX_Control(pDX,IDC_LIST_PAXTYPE,m_lisPaxType);
	DDX_Control(pDX,IDC_LIST_STORAGEPRIORITY,m_wndListCtrl);

}


BEGIN_MESSAGE_MAP(DlgCarryonStoragePriorities, CXTResizeDialog)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT, OnSelchangeFlight)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPE, OnSelchangePaxType)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_STORAGEPRIORITY, OnLvnEndlabeleditListContents)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_STORAGEPRIORITY, OnItemchangedListData)
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
BOOL DlgCarryonStoragePriorities::OnInitDialog() 
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
	GetDlgItem(IDC_STATIC_PRIORITY)->GetWindowRect(&rectDataToolbar);
	GetDlgItem(IDC_STATIC_PRIORITY)->ShowWindow(FALSE);
	ScreenToClient(&rectDataToolbar);
	m_wndDataToolBar.MoveWindow(&rectDataToolbar, true);

	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, FALSE);
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);
	m_wndDataToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );

	SetResize(IDC_STATIC_PAXSET, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_PRIORITYSET, SZ_MIDDLE_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(m_wndPaxToolBar.GetDlgCtrlID(),SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(m_wndDataToolBar.GetDlgCtrlID(), SZ_MIDDLE_CENTER, SZ_MIDDLE_RIGHT );

	SetResize(IDC_LIST_FLIGHT, SZ_TOP_LEFT , SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_PAXTYPE, SZ_TOP_CENTER, SZ_MIDDLE_RIGHT);
	SetResize(IDC_LIST_STORAGEPRIORITY, SZ_MIDDLE_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	bool bFlight = InitFlightList();
	if (!bFlight)
	{
		MessageBox("Please define storages for flight first!");
		OnCancel();
		return FALSE;
	}


	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int DlgCarryonStoragePriorities::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_wndPaxToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR) ||
		!m_wndPaxToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndDataToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,CRect(0,0,0,0),IDR_PEOPLEMOVEBAR)||
		!m_wndDataToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

bool DlgCarryonStoragePriorities::InitFlightList()
{
	int nCount = m_pInOnboard->GetOnBoardFlightCandiates()->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CFlightTypeCandidate* pFltData = m_pInOnboard->GetOnBoardFlightCandiates()->getItem(i);
		int nFlightCount = pFltData->getCount();
		for (int j = 0; j < nFlightCount; j++)
		{
			COnboardFlight* pFlight = pFltData->getItem(j);
			CString strFlight = _T("");
			if (pFlight->getFlightType() == ArrFlight)
			{
				strFlight.Format(_T("ARR ID: %s ACTYPE: %s"),pFlight->getFlightID(),pFlight->getACType());
			}
			else
			{
				strFlight.Format(_T("DEP ID: %s ACTYPE: %s"),pFlight->getFlightID(),pFlight->getACType());
			} 
			int nFlightID = pFlight->getID();
			CAircaftLayOut* pLayout = m_pInOnboard->GetAircaftLayoutManager()->GetAircraftLayOutByFlightID(nFlightID);
			if (pLayout == NULL)
				continue;

			CStorageDataSet* pStorages = pLayout->GetPlacements()->GetStorageData();
			if (pStorages->GetItemCount() == 0)
				continue;

			CFlightCarryonStoragePriorities* pFlightData = m_pCarryonStoragePriorities->getFlightDataByID(nFlightID);
			int iIndex = m_listFlight.AddString(strFlight);
			ASSERT( iIndex!=LB_ERR );	//only judge error when insert string to listbox
			if (pFlightData == NULL)
			{
				pFlightData = new CFlightCarryonStoragePriorities;
				pFlightData->setFlightID(nFlightID);
				m_pCarryonStoragePriorities->AddData(pFlightData);
			}
			m_listFlight.SetItemData(iIndex, (DWORD_PTR)pFlightData); 

		}
	}
	if(m_listFlight.GetCount() == 0)
	{
		return false;
	}
	m_listFlight.SetCurSel(0);
	OnSelchangeFlight();
	return true;

}

void DlgCarryonStoragePriorities::InitPaxList()
{
	m_wndListCtrl.DeleteAllItems();

	if (m_pSelFlightData == NULL)
		return;

	int nCount = m_pSelFlightData->getDataCount();
	for (int i =0; i < nCount; i++)
	{
		CPaxCarryonStoragePriorities* pItem = m_pSelFlightData->getData(i);
		CString strPaxType = pItem->getNamedPaxType()->PrintStringForShow();

		int iIndex = m_lisPaxType.AddString(strPaxType);
		ASSERT( iIndex!=LB_ERR );	//only judge error when insert string to listbox
		m_lisPaxType.SetItemData(iIndex, (DWORD_PTR)pItem); 

	}

}

void DlgCarryonStoragePriorities::SetListCtrl()
{
	ASSERT(m_pStorages != NULL);		//the current storage data set cannot be null

	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	lvc.pszText = "Carry on item";
	lvc.cx  = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	CStringList strList;
	strList.RemoveAll();
	for (int i=0;i<CARRYON_NUM;i++)
	{

		strList.InsertAfter(strList.GetTailPosition(),CarryonItemName[i]);
	}
	lvc.csList = &strList;
	m_wndListCtrl.InsertColumn(0, &lvc);

	CString columnLabel; 
	int nCount = m_pStorages->GetItemCount();
	lvc.fmt = LVCFMT_DROPDOWN;

	strList.RemoveAll();
	for (int i = 0; i < STORAGEDEVICENUM; i++)
	{
		strList.AddTail(StorageDeviceName[i]);
	}


	for (int i=0; i < STORAGEDEVICENUM; i++)
	{
		columnLabel.Format("Priority%d",i);
		lvc.pszText = (char*)(LPCSTR)columnLabel;
		m_wndListCtrl.InsertColumn(i+1,&lvc);
	}
}

void DlgCarryonStoragePriorities::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pCarryonStoragePriorities->SaveData();
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

void DlgCarryonStoragePriorities::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	CXTResizeDialog::OnOK();
}

void DlgCarryonStoragePriorities::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CXTResizeDialog::OnCancel();
}

void DlgCarryonStoragePriorities::OnSelchangeFlight()
{
	m_lisPaxType.ResetContent();
	m_pSelFlightData = (CFlightCarryonStoragePriorities*)m_listFlight.GetItemData(m_listFlight.GetCurSel());
	InitPaxList();

	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, FALSE);
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);

	ASSERT(m_pSelFlightData != NULL);

	int nFlightID = m_pSelFlightData->getFlightID();
	CAircaftLayOut* pLayout = m_pInOnboard->GetAircaftLayoutManager()->GetAircraftLayOutByFlightID(nFlightID);
	if (pLayout != NULL)
	{
		m_pStorages =  pLayout->GetPlacements()->GetStorageData();
		int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();

		// Delete all of the columns.
		for (int i=0;i < nColumnCount;i++)
		{
			m_wndListCtrl.DeleteColumn(0);
		}

		SetListCtrl();
	}
		
	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);
	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);



	if (m_lisPaxType.GetCount()>0)
	{
		m_lisPaxType.SetCurSel(0);
		OnSelchangePaxType();
	}


}


void DlgCarryonStoragePriorities::OnSelchangePaxType()
{
	m_wndListCtrl.DeleteAllItems();
	InitListCtrl();
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, TRUE);
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);

	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, TRUE);
	m_wndPaxToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, TRUE);
	
}

void DlgCarryonStoragePriorities::InitListCtrl()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx >=0)
	{
		CPaxCarryonStoragePriorities* pItem = (CPaxCarryonStoragePriorities*)m_lisPaxType.GetItemData(nIdx);
		if (pItem == NULL)
			return;

 		int nCount = pItem->getDataCount();
		for (int i = 0; i < nCount; i++)
		{
			CPaxCarryonStoragePriorityItem* pData = pItem->getData(i);
			CString strName;
			m_wndListCtrl.InsertItem(i,_T(""));
			m_wndListCtrl.SetItemText(i,0,CarryonItemName[(int)pData->GetCarryonType()]);

			std::vector<StoragePriorityItem> vPriorities = pData->getPriorityList();
			int nDataCount = vPriorities.size();
			for (int j =0; j < STORAGEDEVICENUM; j++)
			{
				if (j>=nDataCount)
				{
					StoragePriorityItem item;
					vPriorities.push_back(item);
				}
				StoragePriorityItem state = vPriorities.at(j);
				m_wndListCtrl.SetItemText(i,state.m_nPriority+1,StorageDeviceName[(int)state.m_eStorageDevice]);
			}
			m_wndListCtrl.SetItemData(i,(DWORD_PTR)pData);
		}
	}
}

CString DlgCarryonStoragePriorities::getStorageName(int nStorageID)
{
	ASSERT(m_pStorages != NULL);		//the current storage data set cannot be null
	int nCount = m_pStorages->GetItemCount();
	for (int i =0; i < nCount; i++)
	{
		if (m_pStorages->GetItem(i)->GetID() == nStorageID)
		{
			return m_pStorages->GetItem(i)->GetIDName().GetIDString();
		}
	}
	ASSERT(FALSE);
	return "";
}

int DlgCarryonStoragePriorities::getStorageID(CString strStorageName)
{
	ASSERT(m_pStorages != NULL);		//the current storage data set cannot be null
	int nCount = m_pStorages->GetItemCount();
	for (int i =0; i < nCount; i++)
	{
		if (m_pStorages->GetItem(i)->GetIDName().GetIDString() == strStorageName)
		{
			return m_pStorages->GetItem(i)->GetID();
		}
	}
	ASSERT(FALSE);
	return -1;
}

void DlgCarryonStoragePriorities::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{

			m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
			LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
			LV_ITEM* plvItem = &plvDispInfo->item;
		
			int nItem = plvItem->iItem;
			int nSubItem = plvItem->iSubItem;
		
			CString strCurVal = plvDispInfo->item.pszText;
		
			CPaxCarryonStoragePriorityItem* pData = (CPaxCarryonStoragePriorityItem*)m_wndListCtrl.GetItemData(nItem);
			if (pData == NULL)
				return;
			if (nSubItem==0)
			{
				int nCarryonType;
				for (nCarryonType=0;nCarryonType<CARRYON_NUM;nCarryonType++)
				{
					if (CarryonItemName[nCarryonType] == strCurVal)
					{
						break;
					}
				}
				if (nCarryonType==CARRYON_NUM)
				{
					return;
				}
				pData->SetCarryonItem((EnumCarryonType)nCarryonType);
			}else
			{
				StoragePriorityItem _priority = pData->getPriorityItemByPriority(nSubItem-1);
		
				int nStorageDevice;
				for (nStorageDevice=0;nStorageDevice<STORAGEDEVICENUM;nStorageDevice++)
				{
                    if (StorageDeviceName[nStorageDevice]==strCurVal)
                    {
						break;
                    }
				}
				if (nStorageDevice==STORAGEDEVICENUM)
				{
					return;
				}
				pData->changePriorityData(nSubItem-1, (EnumStorageDevicePriority)nStorageDevice);
				
			}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
	//}
	*pResult = 0;
}

void DlgCarryonStoragePriorities::OnToolbarPaxTypeAdd()
{
	CPaxCarryonStoragePriorities* pItem = new CPaxCarryonStoragePriorities(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(pItem->getNamedPaxType(),m_pInterm,this);
	dlg.DoModal();

	CString strPaxType = pItem->getNamedPaxType()->PrintStringForShow();
	int iIndex = m_lisPaxType.AddString(strPaxType);
	ASSERT( iIndex!=LB_ERR );		//only judge error when insert string to listbox
	m_lisPaxType.SetItemData(iIndex, (DWORD_PTR)pItem); 
	m_pSelFlightData->AddData(pItem);

	m_lisPaxType.SetCurSel(iIndex);
	m_wndListCtrl.DeleteAllItems();
	m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, TRUE);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

}

void DlgCarryonStoragePriorities::OnToolbarPaxTypeDel()
{
	int nIdx = m_lisPaxType.GetCurSel();
	CPaxCarryonStoragePriorities* pDelItem = (CPaxCarryonStoragePriorities*)m_lisPaxType.GetItemData(nIdx);
	if (pDelItem)
	{
		m_lisPaxType.SetItemData(nIdx,NULL);
		m_pSelFlightData->DelData(pDelItem);
	}
	m_lisPaxType.DeleteString(nIdx);
	m_wndListCtrl.DeleteAllItems();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void DlgCarryonStoragePriorities::OnToolbarPaxTypeEdit()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx < 0)
		return;
	CPaxCarryonStoragePriorities* pItem = (CPaxCarryonStoragePriorities*)m_lisPaxType.GetItemData(nIdx);
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

void DlgCarryonStoragePriorities::OnDataAdd()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx <0)
		return;
	CPaxCarryonStoragePriorities* pItem = (CPaxCarryonStoragePriorities*)m_lisPaxType.GetItemData(nIdx);
	if (pItem)
	{
		int nCount = pItem->getDataCount();
		CPaxCarryonStoragePriorityItem* pData = new CPaxCarryonStoragePriorityItem;
		int nRowCount = m_wndListCtrl.GetItemCount();

		m_wndListCtrl.InsertItem(nRowCount, _T(""));

		m_wndListCtrl.SetItemText(nRowCount, 0, CarryonItemName[(int)pData->GetCarryonType()]);


		std::vector<StoragePriorityItem>& vPriorities = pData->getPriorityList();
		for (int i =0; i < STORAGEDEVICENUM; i++)
		{
			StoragePriorityItem item;
			item.m_nPriority = i;
			item.m_eStorageDevice=(EnumStorageDevicePriority)i;
			vPriorities.push_back(item);
			m_wndListCtrl.SetItemText(nRowCount, i+1, StorageDeviceName[i]);
		}
		m_wndListCtrl.SetItemData(nRowCount, (DWORD_PTR)pData);
		pItem->AddData(pData);
		m_wndListCtrl.SetCurSel(nRowCount);
		m_wndListCtrl.DblClickItem(nRowCount,0);

		m_wndListCtrl.SetItemState(nRowCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

}

void DlgCarryonStoragePriorities::OnDataDel()
{
	int nIdx = m_lisPaxType.GetCurSel();
	if (nIdx <0)
		return;

	CPaxCarryonStoragePriorities* pItem = (CPaxCarryonStoragePriorities*)m_lisPaxType.GetItemData(nIdx);
	if (pItem)
	{
		int nSel =  m_wndListCtrl.GetCurSel();
		if (nSel == -1)
			return;
		if(nSel >= m_wndListCtrl.GetItemCount())
			return;

		CPaxCarryonStoragePriorityItem* pData = (CPaxCarryonStoragePriorityItem*)m_wndListCtrl.GetItemData(nSel);
		pItem->DelData(pData);
		m_wndListCtrl.DeleteItemEx(nSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void DlgCarryonStoragePriorities::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( m_wndListCtrl.GetCurSel() > -1 )
		m_wndDataToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, TRUE);

	*pResult = 0;
}