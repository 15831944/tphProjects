#include "StdAfx.h"
#include "Resource.h"
#include ".\dlgignitionspecification.h"
#include "../InputAirside/IgnitionSpeicification.h"
#include "../InputAirside/TowOperationSpecification.h"
#include "../Database/ADODatabase.h"
#include "DlgStandFamily.h"



IMPLEMENT_DYNAMIC(DlgIgnitionSpecification, CXTResizeDialog)
DlgIgnitionSpecification::DlgIgnitionSpecification(int nPrjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirportDatabase , CWnd* pParent/* = NULL*/)
: CXTResizeDialog(IDD_STANDBUFFER, pParent)
,m_nPrjID(nPrjID)
,m_pSelectFlightType(pSelectFlightType)
,m_pAirportDatabase(pAirportDatabase) 
{
	try
	{
		m_pIgnitionSpecification = new IgnitionSpeicification;
		m_pIgnitionSpecification->ReadData(nPrjID,pAirportDatabase);

		m_pTowOperationSpecification = new CTowOperationSpecification(nPrjID,pAirportDatabase);
		m_pTowOperationSpecification->ReadData();

	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pIgnitionSpecification)
		{
			delete m_pIgnitionSpecification;
			m_pIgnitionSpecification = NULL;
		}
		if(m_pTowOperationSpecification)
		{
			delete m_pTowOperationSpecification;
			m_pTowOperationSpecification = NULL;
		}
		return;
	}
}

DlgIgnitionSpecification::~DlgIgnitionSpecification(void)
{
	if (m_pIgnitionSpecification)
	{
		delete m_pIgnitionSpecification;
		m_pIgnitionSpecification = NULL;
	}
	if(m_pTowOperationSpecification)
	{
		delete m_pTowOperationSpecification;
		m_pTowOperationSpecification = NULL;
	}
}

void DlgIgnitionSpecification::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STANDBUFFER, m_wndTreeCtrl);
}

BEGIN_MESSAGE_MAP(DlgIgnitionSpecification, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_STANDBUFFER_ADD, OnNewItem)
	ON_COMMAND(ID_STANDBUFFER_DEL, OnDeleteItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_STANDBUFFER, OnNMDblclkTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDBUFFER, OnTvnSelchangedTree)
END_MESSAGE_MAP()

BOOL DlgIgnitionSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetWindowText("Ignition (engine start) Specification");

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndTreeCtrl.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitToolbar();
	SetTreeCtrl();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void DlgIgnitionSpecification::OnBnClickedOk()
{
	OnBnClickedSave();
	CXTResizeDialog::OnOK();
}

void DlgIgnitionSpecification::OnBnClickedSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pIgnitionSpecification->SaveData();
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

int DlgIgnitionSpecification::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS) ||	!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_STANDBUFFER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}


void DlgIgnitionSpecification::InitToolbar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolBar);
	ScreenToClient(rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_EDIT, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, FALSE);
}

void DlgIgnitionSpecification::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void DlgIgnitionSpecification::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)				//null selection
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, FALSE);
		return;
	}

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	if(hParItem == NULL)			//root selection
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, FALSE);
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if(hGrandItem == NULL)			//Stand group selection
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, TRUE);
		return;
	}

	HTREEITEM hGrandParItem = m_wndTreeCtrl.GetParentItem(hGrandItem);
	if(hGrandParItem == NULL)			//flight type selection
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, TRUE);
		return;
	}
	//flight type  sub item of flight item
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, FALSE);

	*pResult = 0;
}

void DlgIgnitionSpecification::SetTreeCtrl()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	HTREEITEM hStandItem;
	HTREEITEM hFlightItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hRoot = m_wndTreeCtrl.InsertItem("Engine start",cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);

	int nCount = m_pIgnitionSpecification->GetDataCount();
	for (int i = 0; i < nCount; i++)
	{
		IgnitionData* pData = m_pIgnitionSpecification->m_vDataList.at(i);
		CString strName = pData->m_StandGroup.getName().GetIDString();

		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		hStandItem = m_wndTreeCtrl.InsertItem("Stand: "+strName,cni,FALSE,FALSE,hRoot,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hStandItem,(DWORD_PTR)pData);

		int nFltCount = pData->GetDataCount();
		for (int j=0; j <nFltCount; j++)
		{
			FlightIgnitionData* pFltData = pData->m_vDataList.at(j);
			CString strFlt;
			pFltData->m_FltType.screenPrint(strFlt);

			cni.net = NET_SHOW_DIALOGBOX;
			hFlightItem = m_wndTreeCtrl.InsertItem("Flight: "+strFlt,cni,FALSE,FALSE,hStandItem,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hFlightItem,(DWORD_PTR)pFltData);

			CString strOP = "Push/Start and Taxi";
			int nType = -1;
			int nEngine = (int)pFltData->m_eEngineStartType;
			CTowOperationSpecFltTypeData* pTowData = m_pTowOperationSpecification->GetTowOperationSpecFltTypeData(pFltData->m_FltType, pData->m_StandGroup);
			if (pTowData)
			{
				nType = (int)pTowData->getOperationType();
				strOP = TOWOPERATIONTYPENAME[nType];
				if (nType == 0 && nEngine > (int) AfterUnhookofPushTractor)		//pushback only
					nEngine = 0;
				if (nType > 0 && nEngine <= (int) AfterUnhookofPushTractor)
				{
					nEngine = (int)AtReleasePoint;
				}
			}
			else
			{
				if (nEngine > (int)AfterUnhookofPushTractor)
				{
					nEngine = 0;
				}
			}

			cni.net = NET_NORMAL;
			HTREEITEM hChild = m_wndTreeCtrl.InsertItem("Operation type: "+strOP,cni, FALSE, FALSE, hFlightItem);
			m_wndTreeCtrl.SetItemData(hChild,nType);

			cni.net = NET_COMBOBOX;
			CString strInitiate = INITIATEENGINESTART[nEngine];

			hChild = m_wndTreeCtrl.InsertItem("Initiate engine start: "+strInitiate,cni, FALSE, FALSE, hFlightItem);
			m_wndTreeCtrl.SetItemData(hChild, nEngine);

			if (nEngine == (int)WithinADistanceOfReleasePoint)
			{
				CString strValue;
				strValue.Format("%d", pFltData->m_nDistance);
				cni.net = NET_EDITSPIN_WITH_VALUE;
				HTREEITEM hDistance = m_wndTreeCtrl.InsertItem("Distance(m): "+strValue,cni,FALSE, FALSE, hChild);
			}
			m_wndTreeCtrl.Expand(hFlightItem, TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hStandItem, TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hRoot, TVE_EXPAND);
}

void DlgIgnitionSpecification::OnNewItem()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;

	if (hParItem == NULL)		//Root select
	{
		CDlgStandFamily  dlg(m_nPrjID);
		if(IDOK == dlg.DoModal())
		{
			CString strStand = dlg.GetSelStandFamilyName();
			IgnitionData* pData = new IgnitionData(m_nPrjID);
			ALTObjectID standID(strStand);
			pData->m_StandGroup.setName(standID);
			m_pIgnitionSpecification->m_vDataList.push_back(pData);

			cni.net = NET_SHOW_DIALOGBOX;
			HTREEITEM hStand = m_wndTreeCtrl.InsertItem("Stand: "+strStand,cni, FALSE, FALSE, hSelItem);
			m_wndTreeCtrl.SetItemData(hStand,(DWORD_PTR)pData);


		}

	}
	else			//stand select
	{
		if (m_pSelectFlightType)
		{
			FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
			CString strFltType = "";
			fltType.screenPrint(strFltType);

			IgnitionData* pData = (IgnitionData*)m_wndTreeCtrl.GetItemData(hSelItem);
			FlightIgnitionData* pFlightData = new FlightIgnitionData;
			pFlightData->m_FltType = fltType;
			pData->m_vDataList.push_back(pFlightData);

			cni.net = NET_SHOW_DIALOGBOX;
			HTREEITEM hFlight = m_wndTreeCtrl.InsertItem("Flight: "+strFltType,cni, FALSE, FALSE, hSelItem);
			m_wndTreeCtrl.SetItemData(hFlight,(DWORD_PTR)pFlightData);

			CString strOP = "Push/Start and Taxi";
			int nType = -1;
			int nEngine = 0;
			CTowOperationSpecFltTypeData* pTowData = m_pTowOperationSpecification->GetTowOperationSpecFltTypeData(fltType, pData->m_StandGroup);
			if (pTowData)
			{
				nType = (int)pTowData->getOperationType();
				strOP = TOWOPERATIONTYPENAME[nType];
				if (nType != TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)
					nEngine += 4;				
			}

			cni.net = NET_NORMAL;
			HTREEITEM hChild = m_wndTreeCtrl.InsertItem("Operation type: "+strOP,cni, FALSE, FALSE, hFlight);
			m_wndTreeCtrl.SetItemData(hChild,nType);

			cni.net = NET_COMBOBOX;
			CString strInitiate = INITIATEENGINESTART[nEngine];
			hChild = m_wndTreeCtrl.InsertItem("Initiate engine start: "+strInitiate,cni, FALSE, FALSE, hFlight);
			pFlightData->m_eEngineStartType = (EngineStartType)nEngine;
			m_wndTreeCtrl.SetItemData(hChild, nEngine);
			m_wndTreeCtrl.Expand(hFlight, TVE_EXPAND);
		}
	}
	m_wndTreeCtrl.Expand(hSelItem, TVE_EXPAND);
}

void DlgIgnitionSpecification::OnDeleteItem()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)		//stand select
	{
		IgnitionData* pData = (IgnitionData*)m_wndTreeCtrl.GetItemData(hSelItem);
		m_pIgnitionSpecification->DelData(pData);
		m_wndTreeCtrl.DeleteItem(hSelItem);
	}
	else			//flight select
	{
		FlightIgnitionData* pFlightData = (FlightIgnitionData*)m_wndTreeCtrl.GetItemData(hSelItem);
		IgnitionData* pData = (IgnitionData*)m_wndTreeCtrl.GetItemData(hParItem);
		pData->DelData(pFlightData);
		m_wndTreeCtrl.DeleteItem(hSelItem);
	}
}

LRESULT DlgIgnitionSpecification::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_CEW_COMBOBOX_BEGIN)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		HTREEITEM hPreItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem) ;
		int nOpType = (int)m_wndTreeCtrl.GetItemData(hPreItem);

		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;
		if (pCB->GetCount() != 0)
			pCB->ResetContent();

		if (nOpType <0 || nOpType == (int)TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)
		{
			int nIdx = pCB->AddString("Commencing Push back");
			pCB->SetItemData(nIdx,(int)CommencingPushback);

			nIdx = pCB->AddString("Ready to power out");
			pCB->SetItemData(nIdx,(int)ReadyToPowerOut);

			nIdx = pCB->AddString("Completion of push back");
			pCB->SetItemData(nIdx,(int)CompletionOfPushback);

			nIdx = pCB->AddString("After unhook of push tractor");
			pCB->SetItemData(nIdx,(int)AfterUnhookofPushTractor);
		}
		else
		{
			int nIdx = pCB->AddString("At release point");
			pCB->SetItemData(nIdx,(int)AtReleasePoint);

			nIdx = pCB->AddString("After unhook by tow tractor");
			pCB->SetItemData(nIdx,(int)AfterUnhookByTowTractor);

			nIdx = pCB->AddString("Within a distance of release point");
			pCB->SetItemData(nIdx,(int)WithinADistanceOfReleasePoint);
		}
	}
	else if (message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
		FlightIgnitionData* pFlightData = (FlightIgnitionData*)m_wndTreeCtrl.GetItemData(hParItem);

		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB = (CComboBox*)pWnd;
		CString strText =  *(CString*)lParam;

		m_wndTreeCtrl.DeleteItem(hItem);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;

		CString strTitle = "Initiate engine start: ";
		if (strText == "Commencing Push back")
		{
			pFlightData->m_eEngineStartType = CommencingPushback;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);
		}
		else if (strText == "Ready to power out")
		{
			pFlightData->m_eEngineStartType = ReadyToPowerOut;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);
		}
		else if (strText == "Completion of push back")
		{
			pFlightData->m_eEngineStartType = CompletionOfPushback;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);
		}
		else if (strText == "After unhook of push tractor")
		{
			pFlightData->m_eEngineStartType = AfterUnhookofPushTractor;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);
		}
		else if (strText == "Within a distance of release point")
		{
			pFlightData->m_eEngineStartType = WithinADistanceOfReleasePoint;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);

			cni.net = NET_EDITSPIN_WITH_VALUE;
			HTREEITEM hChild = m_wndTreeCtrl.InsertItem("Distance(m): 0",cni,FALSE, FALSE, hItem);
		}
		else if (strText == "At release point")
		{
			pFlightData->m_eEngineStartType = AtReleasePoint;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);
		}
		else if (strText == "After unhook by tow tractor")
		{
			pFlightData->m_eEngineStartType = AfterUnhookByTowTractor;
			hItem = m_wndTreeCtrl.InsertItem(strTitle+strText,cni, FALSE, FALSE, hParItem);
		}

	}
	else if(message == UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
		HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
		FlightIgnitionData* pFlightData = (FlightIgnitionData*)m_wndTreeCtrl.GetItemData(hGrandItem);

		CString strValue = *((CString*)lParam);
		int nValue = atoi(strValue);
		pFlightData->m_nDistance = nValue;

		CString strText;
		strText.Format("Distance(m): %d", nValue);
		m_wndTreeCtrl.SetItemText(hItem,strText);

	}
	else if(message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
		HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);

		if (hGrandItem == NULL)		//Stand
		{
			CDlgStandFamily  dlg(m_nPrjID);
			if(IDOK == dlg.DoModal())
			{
				IgnitionData* pData = (IgnitionData*)m_wndTreeCtrl.GetItemData(hItem);
				CString strStand = dlg.GetSelStandFamilyName();
				ALTObjectID standID(strStand);
				pData->m_StandGroup.setName(standID);
				m_wndTreeCtrl.SetItemText(hItem,"Stand: "+strStand);

			}
		}
		else			//flight
		{
			if (m_pSelectFlightType)
			{
				FlightConstraint fltType = (*m_pSelectFlightType)(NULL);	
				FlightIgnitionData* pFlightData = (FlightIgnitionData*)m_wndTreeCtrl.GetItemData(hItem);
				IgnitionData* pData = (IgnitionData*)m_wndTreeCtrl.GetItemData(hParItem);

				CString strFltType = "";
				fltType.screenPrint(strFltType);
				pFlightData->m_FltType = fltType;
				m_wndTreeCtrl.DeleteItem(hItem);

				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this,cni);
				cni.nt = NT_NORMAL;
				cni.net = NET_SHOW_DIALOGBOX;
				HTREEITEM hFlight = m_wndTreeCtrl.InsertItem("Flight: "+strFltType,cni, FALSE, FALSE, hParItem);
				m_wndTreeCtrl.SetItemData(hFlight,(DWORD_PTR)pFlightData);

				CString strOP = "Push/Start and Taxi";
				int nType = -1;
				int nEngine = 0;
				CTowOperationSpecFltTypeData* pTowData = m_pTowOperationSpecification->GetTowOperationSpecFltTypeData(fltType, pData->m_StandGroup);
				if (pTowData)
				{
					nType = (int)pTowData->getOperationType();
					strOP = TOWOPERATIONTYPENAME[nType];
					if (nType != TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)
						nEngine += 4;				
				}

				cni.net = NET_NORMAL;
				HTREEITEM hChild = m_wndTreeCtrl.InsertItem("Operation type: "+strOP,cni, FALSE, FALSE, hFlight);
				m_wndTreeCtrl.SetItemData(hChild,nType);

				cni.net = NET_COMBOBOX;
				CString strInitiate = INITIATEENGINESTART[nEngine];
				hChild = m_wndTreeCtrl.InsertItem("Initiate engine start: "+strInitiate,cni, FALSE, FALSE, hFlight);
				m_wndTreeCtrl.SetItemData(hChild, nEngine);
			}
		}

	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}



