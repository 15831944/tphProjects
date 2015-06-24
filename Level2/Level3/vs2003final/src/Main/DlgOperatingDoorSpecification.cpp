// DlgOperatingDoorSpecification.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgOperatingDoorSpecification.h"
#include ".\dlgoperatingdoorspecification.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../Common/FLT_CNST.H"
#include "FlightDialog.h"
#include "TermPlanDoc.h"
#include "SelectALTObjectDialog.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/OperatingDoorSpec.h"
#include "../AirsideGUI/DlgStandFamily.h"

#include "DleACTypeDoorSelection.h"
// DlgOperatingDoorSpecification dialog

IMPLEMENT_DYNAMIC(DlgOperatingDoorSpecification, CXTResizeDialog)
DlgOperatingDoorSpecification::DlgOperatingDoorSpecification( CAirportDatabase *pAirPortdb, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgOperatingDoorSpecification::IDD, pParent)
	,m_pAirportDatabase(pAirPortdb)
	,m_pOperatingDoorSpec(NULL)
{
	try
	{
		m_pOperatingDoorSpec = new OperatingDoorSpec;
		m_pOperatingDoorSpec->SetAirportDatabase(pAirPortdb);
		m_pOperatingDoorSpec->ReadData();

	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pOperatingDoorSpec)
		{
			delete m_pOperatingDoorSpec;
			m_pOperatingDoorSpec = NULL;
		}
		return;
	}
}

DlgOperatingDoorSpecification::~DlgOperatingDoorSpecification()
{
	if (m_pOperatingDoorSpec)
	{
		delete m_pOperatingDoorSpec;
		m_pOperatingDoorSpec = NULL;
	}
}

void DlgOperatingDoorSpecification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OPERATINGDOOR, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(DlgOperatingDoorSpecification, CDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,AddNewItem)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,RemoveItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OPERATINGDOOR, OnTvnSelchangedTreeOperatingdoor)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgOperatingDoorSpecification message handlers

void DlgOperatingDoorSpecification::OnTvnSelchangedTreeOperatingdoor(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
		*pResult = 0;
		return;
	}

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);
		*pResult = 0;
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);
		*pResult = 0;
		return;
	}	
	HTREEITEM hGrandParItem = m_wndTreeCtrl.GetParentItem(hGrandItem);
	if (hGrandParItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
		*pResult = 0;
		return;
	}

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);

	*pResult = 0;
}

void DlgOperatingDoorSpecification::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pOperatingDoorSpec->SaveData();
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
	OnOK();
}

BOOL DlgOperatingDoorSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolbar.MoveWindow(&rect);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );

	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_OPERATINGDOOR,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);

	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	InitTree();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

int DlgOperatingDoorSpecification::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_TOOLBAR_MAIN_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_wndToolbar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_TOOLBARBUTTONADD);
	toolbar.SetCmdID(1, ID_TOOLBARBUTTONDEL);
	return 0;
}

void DlgOperatingDoorSpecification::InitTree()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	int nCount = m_pOperatingDoorSpec->GetFltDataCount();
	if (nCount <=0)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;

	for (int i =0; i < nCount; i++)
	{
		FltOperatingDoorData* pFltData = m_pOperatingDoorSpec->GetFltDoorData(i);
		FlightConstraint flightType = pFltData->GetFltType();

		char szBuffer[1024] = {0};
		flightType.screenPrint(szBuffer);
		CString flightNode;
		flightNode.Format("Flight Type : %s",szBuffer) ;

		flightType.getACType(szBuffer);
		ACTYPEDOORLIST* pAcDoors = m_pAirportDatabase->getAcDoorMan()->GetAcTypeDoorList(szBuffer);
		if (pAcDoors == NULL)
		{
			m_pOperatingDoorSpec->DelFltData(pFltData);
			i--;
			nCount--;
			continue;
		}

		ACTYPEDOORLIST::iterator iter;

		HTREEITEM hFltItem = m_wndTreeCtrl.InsertItem(flightNode,cni,TRUE,FALSE);
		m_wndTreeCtrl.SetItemData(hFltItem, (DWORD_PTR)pFltData);

		int nStandCount = pFltData->GetStandDoorCount();
		for (int j =0; j < nStandCount; j++)
		{
			StandOperatingDoorData* pStandData = pFltData->GetStandDoorData(j);

			CString strStand = pStandData->GetStandName().GetIDString();
			if (strStand == "")
				strStand = "All";

			CString strStandNode = "Stand Family: " + strStand;
			HTREEITEM hStandItem = m_wndTreeCtrl.InsertItem(strStandNode,cni,FALSE,FALSE,hFltItem,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hStandItem, (DWORD_PTR)pStandData);
			HTREEITEM hDoorItem = m_wndTreeCtrl.InsertItem("Doors used:",cni, FALSE, FALSE, hStandItem, TVI_LAST);

			int nDoorCount = pStandData->GetOpDoorCount();
			for(int idx = 0; idx <nDoorCount; idx++)
			{
				int nDoorID = pStandData->GetOpDoorID(idx);

				iter = pAcDoors->begin();
				while(iter != pAcDoors->end())
				{
					if ((*iter)->GetID() == nDoorID)
						break;

					iter++;
				}
				if (iter == pAcDoors->end())
					continue;

				CString strDoor = (*iter)->m_strName;
				m_wndTreeCtrl.InsertItem(strDoor,cni, FALSE, FALSE, hDoorItem, TVI_LAST);
				m_wndTreeCtrl.SetItemData(hDoorItem,nDoorID);
			}
			m_wndTreeCtrl.Expand(hDoorItem,TVE_EXPAND);
			m_wndTreeCtrl.Expand(hStandItem,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hFltItem,TVE_EXPAND);
	}
}

void DlgOperatingDoorSpecification::AddNewItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		AddNewFltItem();
		return;
	}

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParItem == NULL)
	{
		FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hItem);
		AddNewStandItem(hItem,pFltData);
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)
		return;

	HTREEITEM hGrandParItem = m_wndTreeCtrl.GetParentItem(hGrandItem);
	if (hGrandParItem == NULL)
	{
		FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hGrandItem);
		FlightConstraint flightType = pFltData->GetFltType();

		char szBuffer[1024] = {0};
		flightType.getACType(szBuffer);
		ACTYPEDOORLIST* pAcDoors = m_pAirportDatabase->getAcDoorMan()->GetAcTypeDoorList(szBuffer);

		flightType.screenPrint(szBuffer);

		StandOperatingDoorData* pStandData = (StandOperatingDoorData*)m_wndTreeCtrl.GetItemData(hParItem);
		AddNewDoorItem(hItem,pStandData,pAcDoors,szBuffer);
	}
}

void DlgOperatingDoorSpecification::RemoveItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParItem == NULL)
	{
		FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hItem);
		DelFltItem(hItem);
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)
	{
		FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hParItem);
		DelStandItem(hItem,pFltData);
		return;
	}
	
	if (m_wndTreeCtrl.GetItemText(hParItem) == "Doors used:")
	{
		StandOperatingDoorData* pStandData = (StandOperatingDoorData*)m_wndTreeCtrl.GetItemData(hGrandItem);
		DelDoorItem(hItem,pStandData);
	}
}

void DlgOperatingDoorSpecification::AddNewFltItem()
{
	CFlightDialog flightDlg(NULL);
	if(IDOK == flightDlg.DoModal())
	{		
		FlightConstraint fltType = flightDlg.GetFlightSelection();
		char szBuffer[1024] = {0};
		fltType.getACType(szBuffer);
		if(strlen(szBuffer) == 0)
		{
			MessageBox("The flight type must include aircraft type!",NULL,MB_OK) ;
			return;
		}

		ACTYPEDOORLIST* pAcDoors = m_pAirportDatabase->getAcDoorMan()->GetAcTypeDoorList(szBuffer);
		if (pAcDoors == NULL || (pAcDoors && pAcDoors->empty()))
		{
			MessageBox("There is no door data about the aircraft type in database!", NULL, MB_OK);
			return;
		}

		fltType.screenPrint(szBuffer);
		FltOperatingDoorData* pFltData= new FltOperatingDoorData(m_pAirportDatabase);
		pFltData->SetFltType(fltType);

		CString flightNode;
		flightNode.Format("Flight Type : %s",szBuffer) ;

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;

		HTREEITEM hFltItem = m_wndTreeCtrl.InsertItem(flightNode,cni,TRUE,FALSE);
		m_wndTreeCtrl.SetItemData(hFltItem, (DWORD_PTR)pFltData);

		StandOperatingDoorData* pStandData = new StandOperatingDoorData;

		CString strStandNode = "Stand Family: All";
		HTREEITEM hStandItem = m_wndTreeCtrl.InsertItem(strStandNode,cni,FALSE,FALSE,hFltItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hStandItem, (DWORD_PTR)pStandData);
		HTREEITEM hDoorItem = m_wndTreeCtrl.InsertItem("Doors used:",cni, FALSE, FALSE, hStandItem, TVI_LAST);

		if (pAcDoors == NULL)
			return;

		int nDoorCount = pAcDoors->size();
		for (int idx =0; idx < nDoorCount; idx++)
		{
			CString strName = pAcDoors->at(idx)->m_strName;
			int nDoorID = pAcDoors->at(idx)->GetID();

			pStandData->AddOpDoor(nDoorID);
			HTREEITEM hDetail = m_wndTreeCtrl.InsertItem(strName,cni, FALSE, FALSE, hDoorItem, TVI_LAST);
			m_wndTreeCtrl.SetItemData(hDetail,nDoorID);
		}
		pFltData->AddStandDoor(pStandData);
		m_pOperatingDoorSpec->AddFlightOpDoor(pFltData);

		m_wndTreeCtrl.Expand(hDoorItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hStandItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hFltItem,TVE_EXPAND);
	}
}

void DlgOperatingDoorSpecification::AddNewStandItem( HTREEITEM hSelItem,FltOperatingDoorData* pFltData )
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) 
		return ;

	//CSelectALTObjectDialog objDlg(0,vAirport.at(0));
	//objDlg.SetType( ALT_STAND );
	//if(objDlg.DoModal() != IDOK ) return;
	CDlgStandFamily objDlg(pDoc->GetProjectID());
	if(objDlg.DoModal()!=IDOK) return ;

	CString pnewIDstr;
//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return;
	pnewIDstr = objDlg.GetSelStandFamilyName();
	if (pnewIDstr.IsEmpty())
		return;

	StandOperatingDoorData* pStandData = new StandOperatingDoorData;
	pStandData->SetStandName( ALTObjectID((const char*)pnewIDstr));

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;

	CString strStandNode = "Stand Family: " + pnewIDstr;
	HTREEITEM hStandItem = m_wndTreeCtrl.InsertItem(strStandNode,cni,FALSE,FALSE,hSelItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hStandItem, (DWORD_PTR)pStandData);
	HTREEITEM hDoorItem = m_wndTreeCtrl.InsertItem("Doors used:",cni, FALSE, FALSE, hStandItem, TVI_LAST);

	char szBuffer[1024] = {0};
	pFltData->GetFltType().getACType(szBuffer);

	ACTYPEDOORLIST* pAcDoors = m_pAirportDatabase->getAcDoorMan()->GetAcTypeDoorList(szBuffer);
	if (pAcDoors == NULL)
		return;

	int nDoorCount = pAcDoors->size();
	for (int idx =0; idx < nDoorCount; idx++)
	{
		CString strName = pAcDoors->at(idx)->m_strName;
		int nDoorID = pAcDoors->at(idx)->GetID();

		pStandData->AddOpDoor(nDoorID);
		HTREEITEM hDetail = m_wndTreeCtrl.InsertItem(strName,cni, FALSE, FALSE, hDoorItem, TVI_LAST);
		m_wndTreeCtrl.SetItemData(hDetail,nDoorID);
	}
	pFltData->AddStandDoor(pStandData);
}

void DlgOperatingDoorSpecification::AddNewDoorItem( HTREEITEM hSelItem, StandOperatingDoorData* pStandData , ACTYPEDOORLIST* pACTypeDoors, const CString& strACType)
{

	DlgACTypeDoorSelection dlg(pACTypeDoors,strACType);
	if (dlg.DoModal()!= IDOK )
		return;

	CString strName = dlg.GetDoorName();
	int nDoorID  = dlg.GetDoorID();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;

	pStandData->AddOpDoor(nDoorID);
	HTREEITEM hDetail = m_wndTreeCtrl.InsertItem(strName,cni, FALSE, FALSE, hSelItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hDetail,nDoorID);

}

void DlgOperatingDoorSpecification::DelFltItem( HTREEITEM hSelItem )
{
	FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hSelItem);
	m_pOperatingDoorSpec->DelFltData(pFltData);
	m_wndTreeCtrl.DeleteItem(hSelItem);
}

void DlgOperatingDoorSpecification::DelStandItem( HTREEITEM hSelItem,FltOperatingDoorData* pFltData )
{
	StandOperatingDoorData* pStandData = (StandOperatingDoorData*)m_wndTreeCtrl.GetItemData(hSelItem);
	pFltData->DelStandData(pStandData);
	m_wndTreeCtrl.DeleteItem(hSelItem);
}

void DlgOperatingDoorSpecification::DelDoorItem( HTREEITEM hSelItem, StandOperatingDoorData* pStandData )
{
	int nDoorID = (int)m_wndTreeCtrl.GetItemData(hSelItem);
	pStandData->DelOpDoor(nDoorID);
	m_wndTreeCtrl.DeleteItem(hSelItem);
}

LRESULT DlgOperatingDoorSpecification::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		CString strData = _T("");
		HTREEITEM hItem = (HTREEITEM)wParam;
		if (hItem != NULL)
		{
			HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParItem == NULL)
			{
				FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hItem);
				EditFltItem(hItem);
			}
			else
			{
				HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
				if (hGrandItem == NULL)
				{
					FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hParItem);
					EditStandItem(hItem,pFltData);
				}
				else if (m_wndTreeCtrl.GetItemText(hParItem) == "Doors used:")
				{
					FltOperatingDoorData* pFltData = (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hParItem);
					FlightConstraint flightType = pFltData->GetFltType();

					char szBuffer[1024] = {0};
					flightType.getACType(szBuffer);
					ACTYPEDOORLIST* pAcDoors = m_pAirportDatabase->getAcDoorMan()->GetAcTypeDoorList(szBuffer);

					flightType.screenPrint(szBuffer);

					StandOperatingDoorData* pStandData = (StandOperatingDoorData*)m_wndTreeCtrl.GetItemData(hGrandItem);
					EditDoorItem(hItem,pStandData,pAcDoors,szBuffer);
				}
			}


		}




	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void DlgOperatingDoorSpecification::EditFltItem( HTREEITEM hSelItem )
{
	FltOperatingDoorData* pFltData= (FltOperatingDoorData*)m_wndTreeCtrl.GetItemData(hSelItem);

	CFlightDialog flightDlg(NULL);
	if(IDOK == flightDlg.DoModal())
	{		
		FlightConstraint fltType = flightDlg.GetFlightSelection();
		char szBuffer[1024] = {0};
		fltType.getACType(szBuffer);
		if(strlen(szBuffer) == 0)
		{
			MessageBox("The flight type must include aircraft type!",NULL,MB_OK) ;
			return;
		}

		ACTYPEDOORLIST* pAcDoors = m_pAirportDatabase->getAcDoorMan()->GetAcTypeDoorList(szBuffer);
		if (pAcDoors == NULL|| (pAcDoors && pAcDoors->empty()))
		{
			MessageBox("There is no door data about the aircraft type in database!", NULL, MB_OK);
			return;
		}

		fltType.screenPrint(szBuffer);

		pFltData->SetFltType(fltType);

		CString flightNode;
		flightNode.Format("Flight Type : %s",szBuffer) ;

		m_wndTreeCtrl.SetItemText(hSelItem,flightNode);
		m_wndTreeCtrl.Expand(hSelItem,TVE_EXPAND);
	}
}

void DlgOperatingDoorSpecification::EditStandItem( HTREEITEM hSelItem,FltOperatingDoorData* pFltData )
{
	StandOperatingDoorData* pStandData = (StandOperatingDoorData*)m_wndTreeCtrl.GetItemData(hSelItem);

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) 
		return ;

	//CSelectALTObjectDialog objDlg(0,vAirport.at(0));
	//objDlg.SetType( ALT_STAND );
	//if(objDlg.DoModal() != IDOK ) return;
	CDlgStandFamily objDlg(pDoc->GetProjectID());
	if(objDlg.DoModal()!=IDOK) return ;

	CString pnewIDstr;
//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return;
	pnewIDstr = objDlg.GetSelStandFamilyName();
	if (pnewIDstr.IsEmpty())
		return;


	pStandData->SetStandName( ALTObjectID((const char*)pnewIDstr));

	CString strStandNode = "Stand Family: " + pnewIDstr;
	m_wndTreeCtrl.SetItemText(hSelItem,strStandNode);
	m_wndTreeCtrl.Expand(hSelItem,TVE_EXPAND);
}

void DlgOperatingDoorSpecification::EditDoorItem( HTREEITEM hSelItem, StandOperatingDoorData* pStandData, ACTYPEDOORLIST* pACTypeDoors, const CString& strACType )
{
	int nDoorID = (int)m_wndTreeCtrl.GetItemData(hSelItem);
	pStandData->DelOpDoor(nDoorID);

	DlgACTypeDoorSelection dlg(pACTypeDoors,strACType);
	if (dlg.DoModal()!= IDOK )
		return;

	CString strName = dlg.GetDoorName();
	nDoorID  = dlg.GetDoorID();

	pStandData->AddOpDoor(nDoorID);
	m_wndTreeCtrl.SetItemText(hSelItem,strName);
	m_wndTreeCtrl.SetItemData(hSelItem,nDoorID);

}