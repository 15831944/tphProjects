// DlgPushBackAndTowOperationSpec.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgPushBackAndTowOperationSpec.h"
#include ".\dlgpushbackandtowoperationspec.h"
//#include "DlgSelectALTObject.h"
#include "../Database/DBElementCollection_Impl.h"
#include "DlgStandFamily.h"
#include "DlgSelectTowRouteItem.h"
#include "../InputAirside/Taxiway.h"
#include "DlgReleasePointSelection.h"
// CDlgPushBackAndTowOperationSpec dialog

IMPLEMENT_DYNAMIC(CDlgPushBackAndTowOperationSpec, CXTResizeDialog)
CDlgPushBackAndTowOperationSpec::CDlgPushBackAndTowOperationSpec(int nProjID ,PFuncSelectFlightType selFlightTy,CAirportDatabase* pAirportDB,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPushBackAndTowOperationSpec::IDD, pParent)
	,m_nProjID(nProjID)
	,m_towOperationSpec(nProjID,pAirportDB)
{
	m_FunSelectFlightType = selFlightTy;
	m_pAirportDatabase = pAirportDB;
}


CDlgPushBackAndTowOperationSpec::~CDlgPushBackAndTowOperationSpec()
{
}

void CDlgPushBackAndTowOperationSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgPushBackAndTowOperationSpec, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeData)
	ON_COMMAND(ID_TOOLBAR_ADD,OnToolBarAdd)
	ON_COMMAND(ID_TOOLBAR_DEL,OnToolBarDel)
	ON_COMMAND(ID_TOOLBAR_EDIT,OnToolBarEdit)
END_MESSAGE_MAP()


// CDlgPushBackAndTowOperationSpec message handlers

int CDlgPushBackAndTowOperationSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS ) ||
		!m_toolbar.LoadToolBar(IDR_TOOLBAR_PUSHBACKANDTOWOPERATION))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	// TODO:  Add your specialized creation code here

	return 0;
}

BOOL CDlgPushBackAndTowOperationSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	InitToolBar();
	// TODO:  Add extra initialization here

	SetResize(m_toolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_MAINFRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);


	m_towingRouteList.ReadData();
	m_towOperationSpec.ReadData();

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//runway
	{
		std::vector<ALTObject> vRunways;
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{
			ALTObject::GetObjectList(ALT_RUNWAY,*iterAirportID,vRunways);
		}

		std::vector<ALTObject>::iterator iter = vRunways.begin();
		for (;iter!= vRunways.end();++iter)
		{
			Runway *pRunway = new Runway;
			pRunway->ReadObject((*iter).getID());
			m_lstRunway.push_back(pRunway);
		}

	}
	//taxiway
	{
		std::vector<ALTObject> vtaxiways;
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{
			ALTObject::GetObjectList(ALT_TAXIWAY,*iterAirportID,vtaxiways);
		}

		std::vector<ALTObject>::iterator iter = vtaxiways.begin();
		for (;iter!= vtaxiways.end();++iter)
		{
			Taxiway *pTaxiway = new Taxiway;
			pTaxiway->ReadObject((*iter).getID());
			m_lstTaxiway.push_back(pTaxiway);


		}

	}
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPushBackAndTowOperationSpec::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{	
		CADODatabase::BeginTransaction();
		m_towOperationSpec.SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save data failed."));
		return;
		
	}
	

	CDialog::OnOK();
}

void CDlgPushBackAndTowOperationSpec::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}

void CDlgPushBackAndTowOperationSpec::InitToolBar()
{
	CRect rect;
	m_treeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 3;
	rect.top = rect.bottom - 19;
	rect.left = rect.left + 2;
	rect.right = rect.right-10;
	m_toolbar.MoveWindow(&rect,true);
	m_toolbar.ShowWindow(SW_SHOW);

	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);

}

void CDlgPushBackAndTowOperationSpec::UpdateToolBarState()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL)
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
		return;
	}

	enumNodeType nodeType = GetItemType(hSelItem);
	switch (nodeType)
	{
	case nodeType_Stand:
	case nodeType_FlightType:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);		
		}
		break;
	case nodeType_Operation:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);		
		}
		break;
	case nodeType_ReleasePoint:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);		
		}
		break;
	case nodeType_OutBoundRoute:
	case nodeType_ReturnRoute:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);		
		}
		break;
	case nodeType_OutBoundRouteType:
		{
			CReleasePoint *pData = GetItemReleasePointData(hSelItem);
			if(pData && pData->GetEnumOutRouteType() == TOWOPERATIONROUTETYPE_SHORTESTPATH)
				m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
			else
				m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);			
											
		}
		break;
	case nodeType_ReturnRouteType:
		{
			CReleasePoint *pData = GetItemReleasePointData(hSelItem);
			if(pData && pData->GetEnumRetRouteType() == TOWOPERATIONROUTETYPE_SHORTESTPATH)
				m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
			else
				m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);			
		}
		break;
	case nodeType_OutBoundItem:
	case nodeType_ReturnRouteItem:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);	
		}
		break;

	default:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);		
		}
	}
}
void CDlgPushBackAndTowOperationSpec::OnBnClickedButtonSave()
{
	try
	{	
		CADODatabase::BeginTransaction();
		m_towOperationSpec.SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save data failed."));
		return;

	}
}

void CDlgPushBackAndTowOperationSpec::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	UpdateToolBarState();
	
	*pResult = 0;
}

void CDlgPushBackAndTowOperationSpec::OnToolBarAdd()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL)
	{
		OnAddStand();
		return;
	}

	enumNodeType nodeType = GetItemType(hSelItem);
	switch (nodeType)
	{
	case nodeType_Stand:
		{
			OnAddFlightType(hSelItem);
		}
		break;
	case nodeType_FlightType:
		{

		}
		break;
	case nodeType_Operation:
		{
			OnAddReleasePoint(hSelItem);
		}
		break;
	case nodeType_OutBoundRouteType:
		{
			CReleasePoint *pData = GetItemReleasePointData(hSelItem);
			if(pData && pData->GetEnumOutRouteType() == TOWOPERATIONROUTETYPE_SHORTESTPATH)
				return;

			CDlgSelectTowRouteItem dlg(m_nProjID,NULL,this);
			if(dlg.DoModal() == IDOK)
			{	
				//add towing route item

				CTowOperationRouteItem *pRouteItem = new CTowOperationRouteItem;
				pRouteItem->SetEnumType(dlg.m_enumItemType);
				pRouteItem->SetObjectID(dlg.m_nSelALTObjID);
				pRouteItem->SetIntersectionID(dlg.m_nIntersectNodeID);
				pRouteItem->SetRouteType(0);

			//	CTowOperationSpecFltTypeData *pFlgihtType = GetItemFltTypeData(hSelItem);
				if(pData)
				{
					pData->GetOutBoundRoute()->AddNewItem(pRouteItem);

					AddTowRouteItem(pRouteItem,hSelItem);
				}

			}

		}
		break;
	case nodeType_OutBoundItem:
		{
			CTowOperationRouteItem *pItem = GetItemRouteItemData(hSelItem);
			if (pItem == NULL)
				return;

			CDlgSelectTowRouteItem dlg(m_nProjID,pItem,this);
			if(dlg.DoModal() == IDOK)
			{	
				//add towing route item

				CTowOperationRouteItem *pRouteItem = new CTowOperationRouteItem;
				pRouteItem->SetEnumType(dlg.m_enumItemType);
				pRouteItem->SetObjectID(dlg.m_nSelALTObjID);
				pRouteItem->SetIntersectionID(dlg.m_nIntersectNodeID);
				pRouteItem->SetRouteType(0);

				//CReleasePoint *pData = GetItemReleasePointData(hSelItem);
				//if(pData)
				{
					pItem->AddNewItem(pRouteItem);

					AddTowRouteItem(pRouteItem,hSelItem);
				}

			}
		}
		break;

	case nodeType_ReturnRouteType:
		{
			CReleasePoint *pData = GetItemReleasePointData(hSelItem);
			if(pData && pData->GetEnumRetRouteType() == TOWOPERATIONROUTETYPE_SHORTESTPATH)
				return;
		
			CDlgSelectTowRouteItem dlg(m_nProjID,NULL,this);
			if(dlg.DoModal() == IDOK)
			{	
				//add towing route item

				CTowOperationRouteItem *pRouteItem = new CTowOperationRouteItem;
				pRouteItem->SetEnumType(dlg.m_enumItemType);
				pRouteItem->SetObjectID(dlg.m_nSelALTObjID);
				pRouteItem->SetIntersectionID(dlg.m_nIntersectNodeID);
				pRouteItem->SetRouteType(1);

				//CTowOperationSpecFltTypeData *pFlgihtType = GetItemFltTypeData(hSelItem);
				if(pData)
				{
					pData->GetReturnRoute()->AddNewItem(pRouteItem);

					AddTowRouteItem(pRouteItem,hSelItem);
				}

			}

		}
		break;
	case nodeType_ReturnRouteItem:
		{
			CTowOperationRouteItem *pItem = GetItemRouteItemData(hSelItem);
			if (pItem == NULL)
				return;

			CDlgSelectTowRouteItem dlg(m_nProjID,pItem,this);
			if(dlg.DoModal() == IDOK)
			{	
				//add towing route item

				CTowOperationRouteItem *pRouteItem = new CTowOperationRouteItem;
				pRouteItem->SetEnumType(dlg.m_enumItemType);
				pRouteItem->SetObjectID(dlg.m_nSelALTObjID);
				pRouteItem->SetIntersectionID(dlg.m_nIntersectNodeID);
				pRouteItem->SetRouteType(1);

				//CReleasePoint *pData = GetItemReleasePointData(hSelItem);
				//if(pData)
				{
					pItem->AddNewItem(pRouteItem);

					AddTowRouteItem(pRouteItem,hSelItem);
				}

			}
		}
		break;

	default:
		{

		}
	}



	

}
void CDlgPushBackAndTowOperationSpec::AddTowRouteItem( CTowOperationRouteItem *pItem, HTREEITEM hParentItem )
{
	if(pItem == NULL || hParentItem == NULL)
		return;

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hParentItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hParentItem);
	//add item to tree
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	if (pItem->GetRouteType() == 0)
	{
		pItemDatEx->itemType = nodeType_OutBoundItem;
	}
	else
	{
		pItemDatEx->itemType = nodeType_ReturnRouteItem;
	}
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;
	pItemDatEx->pTowRouteItem = pItem;
	
	CString strRouteItemName = GetRouteItemName(pItem);

	HTREEITEM hRouteItem = m_treeCtrl.InsertItem(strRouteItemName, cni, FALSE, FALSE, hParentItem);
	m_treeCtrl.SetItemData(hRouteItem,(DWORD_PTR)pItemDatEx);


	int nChildCount = pItem->GetElementCount();
	for (int nChild = 0; nChild < nChildCount; ++nChild)
	{
		CTowOperationRouteItem *pChildItem = pItem->GetItem(nChild);
		AddTowRouteItem(pChildItem,hRouteItem);
		m_treeCtrl.Expand(hRouteItem,TVE_EXPAND);
	}

	m_treeCtrl.Expand(hParentItem,TVE_EXPAND);


}
CString CDlgPushBackAndTowOperationSpec::GetRouteItemName( CTowOperationRouteItem *pItem )
{
	if(pItem== NULL)
		return NULL;
	if (pItem->GetEnumType() == CTowOperationRouteItem::ItemType_Runway)
	{
		std::vector<Runway *>::iterator iter = m_lstRunway.begin();
		for (;iter != m_lstRunway.end(); ++iter)
		{
			if( (*iter) && (*iter)->getID() == pItem->GetObjectID())
			{
				CString strRunwayMark;
				strRunwayMark.Format(_T("%s(%s)"),(*iter)->GetMarking1().c_str(),(*iter)->GetMarking2().c_str());
				return _T("Runway:") + strRunwayMark;
			}
		}
	}
	else if(pItem->GetEnumType() == CTowOperationRouteItem::ItemType_Taxiway)
	{

		std::vector<Taxiway *>::iterator iter = m_lstTaxiway.begin();
		for (;iter != m_lstTaxiway.end(); ++iter)
		{
			if( (*iter) && (*iter)->getID() == pItem->GetObjectID())
			{
				CString strRunwayMark;
				strRunwayMark.Format(_T("%s"),(*iter)->GetMarking().c_str());
				return _T("Taxiway:") + strRunwayMark;
			}
		}


	}
	else if(pItem->GetEnumType() == CTowOperationRouteItem::ItemType_TowRoute)
	{
		int nCount = m_towingRouteList.GetTowingRouteCount();
		for (int nRoute = 0; nRoute < nCount; ++nRoute)
		{
			CTowingRoute *pRoute = m_towingRouteList.GetTowingRouteItem(nRoute);
			if(pRoute && pRoute->GetID() == pItem->GetObjectID())
			{
				CString strRouteName = pRoute->GetName();
				
				return _T("Tow Route:")+ strRouteName;
			}
		}
	}

	return _T("");
}

void CDlgPushBackAndTowOperationSpec::OnToolBarDel()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL)
	{
		return;
	}

	enumNodeType nodeType = GetItemType(hSelItem);
	switch (nodeType)
	{
	case nodeType_Stand:
		{
			OnDeleteStand();
		}
		break;
	case nodeType_FlightType:
		{

			OnDeleteFlightType();

		}
		break;
	case nodeType_ReleasePoint:
		{
			OnDeleteReleasePoint();
		}
		break;
	case nodeType_ReturnRouteItem:
	case nodeType_OutBoundItem:
		{
			CTowOperationRouteItem *pItem = GetItemRouteItemData(hSelItem);
			if (pItem == NULL)
				return;
			HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
			if(hParentItem == NULL)
				return;

			if(GetItemType(hParentItem) == nodeType_OutBoundRouteType )
			{
				CReleasePoint *pData = GetItemReleasePointData(hParentItem);
				if(pData)
				{
					pData->GetOutBoundRoute()->DeleteItem(pItem);
					m_treeCtrl.DeleteItem(hSelItem);
				}
			}
			else if(GetItemType(hParentItem) == nodeType_ReturnRouteType)
			{
				CReleasePoint *pData = GetItemReleasePointData(hParentItem);
				if(pData)
				{
					pData->GetReturnRoute()->DeleteItem(pItem);
					m_treeCtrl.DeleteItem(hSelItem);
				}
			}
			else if(GetItemType(hParentItem) == nodeType_ReturnRouteItem || GetItemType(hParentItem) == nodeType_OutBoundItem)
			{
				CTowOperationRouteItem *pParentItem = GetItemRouteItemData(hParentItem);
				if(pParentItem)
				{
					pParentItem->DeleteItem(pItem);
					m_treeCtrl.DeleteItem(hSelItem);
				}
			}
		}
		break;

	default:
		{

		}
	}
}

void CDlgPushBackAndTowOperationSpec::OnToolBarEdit()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL)
	{
		return;
	}

	enumNodeType nodeType = GetItemType(hSelItem);
	switch (nodeType)
	{
	case nodeType_Stand:
		{
			OnEditStand();
		}
		break;
	case nodeType_FlightType:
		{

			OnEditFlightType();

		}
		break;

	case nodeType_ReleasePoint:
		{
			OnEditReleasePoint();
		}
		break;
	default:
		{

		}
	}
}

void CDlgPushBackAndTowOperationSpec::LoadTree()
{
	int nCount = m_towOperationSpec.GetItemCount();
	for (int i =0; i < nCount; ++i)
	{
		CTowOperationSpecStand *pStandData = m_towOperationSpec.GetItem(i);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.net=NET_NORMAL;
		cni.nt=NT_NORMAL;
		cni.bAutoSetItemText = FALSE;

		//item data
		CTreeItemData *pItemDatEx = new CTreeItemData;
		pItemDatEx->itemType = nodeType_Stand;
		pItemDatEx->pStandData = pStandData;

		CString standGroupName = pStandData->GetStandGroup().getName().GetIDString();
		standGroupName.Trim();
		if(standGroupName.IsEmpty())
			standGroupName = _T("All");

		HTREEITEM hStandItem = m_treeCtrl.InsertItem(_T("Stands: ")+standGroupName, cni, FALSE, FALSE, TVI_ROOT);
		m_treeCtrl.SetItemData(hStandItem,(DWORD_PTR)pItemDatEx);

		
		{
			for (size_t nFlightType = 0;nFlightType < pStandData->GetElementCount();++nFlightType)
			{
				CTowOperationSpecFltTypeData *pFlightData = pStandData->GetItem(nFlightType);

				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this,cni);
				cni.net=NET_NORMAL;
				cni.nt=NT_NORMAL;
				cni.bAutoSetItemText = FALSE;


				//item data
				CTreeItemData *pItemDatEx = new CTreeItemData;
				pItemDatEx->itemType = nodeType_FlightType;
				pItemDatEx->pStandData = pStandData;
				pItemDatEx->pFlightTypeData = pFlightData;

				CString strConstraint;
				pFlightData->GetFltConstraint().screenPrint(strConstraint);	

				HTREEITEM hFlightTypeItem = m_treeCtrl.InsertItem(_T("Flight Type: ")+ strConstraint, cni, FALSE, FALSE, hStandItem);
				m_treeCtrl.SetItemData(hFlightTypeItem,(DWORD_PTR)pItemDatEx);

				//add operation node
				{
					AddOperationNode(hFlightTypeItem);
					m_treeCtrl.Expand(hFlightTypeItem,TVE_EXPAND);

				}

			}
		}

		m_treeCtrl.Expand(hStandItem,TVE_EXPAND);

	}
}

void CDlgPushBackAndTowOperationSpec::OnAddStand()
{
	CDlgStandFamily  dlgSelectStand(m_nProjID);
	if(dlgSelectStand.DoModal() == IDOK)
	{
		int nObjectGroupID = dlgSelectStand.GetSelStandFamilyID();
		ALTObjectGroup standGroup;
		standGroup.ReadData(nObjectGroupID);
		
		CTowOperationSpecStand *pTowStandData = new CTowOperationSpecStand;
		pTowStandData->SetProjID(m_nProjID);
		pTowStandData->SetAirportDatabase(m_pAirportDatabase);
		pTowStandData->SetStandGroup(standGroup);
		m_towOperationSpec.AddNewItem(pTowStandData);

		//add stand item
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.net=NET_NORMAL;
		cni.nt=NT_NORMAL;
		cni.bAutoSetItemText = FALSE;


		//item data
		CTreeItemData *pItemDatEx = new CTreeItemData;
		pItemDatEx->itemType = nodeType_Stand;
		pItemDatEx->pStandData = pTowStandData;

		CString standGroupName = standGroup.getName().GetIDString();
		standGroupName.Trim();
		if(standGroupName.IsEmpty())
			standGroupName = _T("All");

		HTREEITEM hStandItem = m_treeCtrl.InsertItem(_T("Stands: ")+standGroupName, cni, FALSE, FALSE, TVI_ROOT);
		m_treeCtrl.SetItemData(hStandItem,(DWORD_PTR)pItemDatEx);

		//insert flight type item
		
		{

			CTowOperationSpecFltTypeData *pFlightData = new CTowOperationSpecFltTypeData;
			pFlightData->SetAirportDatabase(m_pAirportDatabase);
			pFlightData->SetFltConstraint(FlightConstraint(m_pAirportDatabase));

			pTowStandData->AddNewItem(pFlightData);

			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.net=NET_NORMAL;
			cni.nt=NT_NORMAL;
			cni.bAutoSetItemText = FALSE;



			//item data
			CTreeItemData *pItemDatEx = new CTreeItemData;
			pItemDatEx->itemType = nodeType_FlightType;
			pItemDatEx->pStandData = pTowStandData;
			pItemDatEx->pFlightTypeData = pFlightData;

			CString strConstraint;
			pFlightData->GetFltConstraint().screenPrint(strConstraint);	

			HTREEITEM hFlightTypeItem = m_treeCtrl.InsertItem(_T("Flight Type: ")+ strConstraint, cni, FALSE, FALSE, hStandItem);
			m_treeCtrl.SetItemData(hFlightTypeItem,(DWORD_PTR)pItemDatEx);
			
			//add operation node
			{
				AddOperationNode(hFlightTypeItem);
				m_treeCtrl.Expand(hFlightTypeItem,TVE_EXPAND);

			}

		}

		m_treeCtrl.Expand(hStandItem,TVE_EXPAND);

	}


}

void CDlgPushBackAndTowOperationSpec::OnDeleteStand()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL || GetItemType(hSelItem) != nodeType_Stand)
		return;
	

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hSelItem);

	if(pTowStandData)
	{
		m_towOperationSpec.DeleteItem(pTowStandData);

		m_treeCtrl.DeleteItem(hSelItem);

		m_treeCtrl.SetImageList(m_treeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	}


}

void CDlgPushBackAndTowOperationSpec::OnEditStand()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL || GetItemType(hSelItem) != nodeType_Stand)
		return;

	CDlgStandFamily  dlgSelectStand(m_nProjID);
	if(dlgSelectStand.DoModal() == IDOK)
	{

		int nObjectGroupID = dlgSelectStand.GetSelStandFamilyID();
		ALTObjectGroup standGroup;
		standGroup.ReadData(nObjectGroupID);


		CTowOperationSpecStand *pTowStandData = GetItemStandData(hSelItem);

		if(pTowStandData)
		{

			pTowStandData->SetStandGroup(standGroup);
			CString standGroupName = standGroup.getName().GetIDString();
			standGroupName.Trim();
			if(standGroupName.IsEmpty())
				standGroupName = _T("All");

			m_treeCtrl.SetItemText(hSelItem,_T("Stands: ")+standGroupName);

		}
	}



}

void CDlgPushBackAndTowOperationSpec::OnAddFlightType(HTREEITEM hStandItem)
{
	if(GetItemType(hStandItem) != nodeType_Stand)
		return;

	if (m_FunSelectFlightType == NULL)
		return;
	FlightConstraint fltType ;
	if((*m_FunSelectFlightType)(NULL,fltType) == FALSE)
		return;
	

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hStandItem);

	CTowOperationSpecFltTypeData *pFlightData = new CTowOperationSpecFltTypeData;
	pFlightData->SetAirportDatabase(m_pAirportDatabase);
	pFlightData->SetFltConstraint(fltType);

	pTowStandData->AddNewItem(pFlightData);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_FlightType;
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;

	CString strConstraint;
	pFlightData->GetFltConstraint().screenPrint(strConstraint);	

	HTREEITEM hFlightTypeItem = m_treeCtrl.InsertItem(_T("Flight Type: ")+ strConstraint, cni, FALSE, FALSE, hStandItem);
	m_treeCtrl.SetItemData(hFlightTypeItem,(DWORD_PTR)pItemDatEx);
	
	//add operation
	{
		AddOperationNode(hFlightTypeItem);
		m_treeCtrl.Expand(hFlightTypeItem,TVE_EXPAND);

	}


}

void CDlgPushBackAndTowOperationSpec::OnAddReleasePoint(HTREEITEM hSel)
{
	if(GetItemType(hSel) != nodeType_Operation)
		return;

	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hSel);
	if (pFlightData->getOperationType() == TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)
		return;

	DlgReleasePointSelection dlg(m_nProjID);
	if (dlg.DoModal() != IDOK)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;

	CReleasePoint* pReleasepoint = new CReleasePoint;
	dlg.GetSelection(pReleasepoint);

	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_ReleasePoint;
	pItemDatEx->pStandData = GetItemStandData(hSel);
	pItemDatEx->pFlightTypeData = pFlightData;
	pItemDatEx->pReleasePoint = pReleasepoint;


	CString strReleasePointName = pReleasepoint->getName();
	HTREEITEM hReleaseItem = m_treeCtrl.InsertItem(_T("Release point: ")+ strReleasePointName, cni, FALSE, FALSE, hSel);
	m_treeCtrl.SetItemData(hReleaseItem,(DWORD_PTR)pItemDatEx);

	AddOutBoundRoute(hReleaseItem);
	AddReturnRoute(hReleaseItem);

	pFlightData->AddNewItem(pReleasepoint);
	m_treeCtrl.Expand(hReleaseItem,TVE_EXPAND);
	m_treeCtrl.Expand(hSel,TVE_EXPAND);

}

void CDlgPushBackAndTowOperationSpec::OnDeleteFlightType()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL || GetItemType(hSelItem) != nodeType_FlightType)
		return;
	
	CTowOperationSpecStand *pTowStandData = GetItemStandData(hSelItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hSelItem);
	if(pTowStandData)
		pTowStandData->DeleteItem(pFlightData);

	m_treeCtrl.DeleteItem(hSelItem);

}

void CDlgPushBackAndTowOperationSpec::OnDeleteReleasePoint()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL || GetItemType(hSelItem) != nodeType_ReleasePoint)
		return;

	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hSelItem);
	CReleasePoint* pReleasepoint = GetItemReleasePointData(hSelItem);

	if(pFlightData)
		pFlightData->DeleteItem(pReleasepoint);

	m_treeCtrl.DeleteItem(hSelItem);
}

void CDlgPushBackAndTowOperationSpec::OnEditFlightType()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL || GetItemType(hSelItem) != nodeType_FlightType)
		return;

	if(m_FunSelectFlightType == NULL)
		return;
	FlightConstraint fltType;
	if((*m_FunSelectFlightType)(NULL,fltType) == FALSE)
		return;

	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hSelItem);
	if(pFlightData)
	{
		pFlightData->SetFltConstraint(fltType);
		
		CString strConstraint;
		pFlightData->GetFltConstraint().screenPrint(strConstraint);	

		m_treeCtrl.SetItemText(hSelItem,_T("Flight Type: ")+ strConstraint);
	}

}

void CDlgPushBackAndTowOperationSpec::OnEditReleasePoint()
{
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	if(hSelItem == NULL || GetItemType(hSelItem) != nodeType_ReleasePoint)
		return;

	DlgReleasePointSelection dlg(m_nProjID);
	if (dlg.DoModal() != IDOK)
		return;

	CReleasePoint *pReleasePoint = GetItemReleasePointData(hSelItem);
	if(pReleasePoint)
	{
		dlg.GetSelection(pReleasePoint);
		m_treeCtrl.SetItemText(hSelItem,_T("Release point: ")+ pReleasePoint->getName());
	}

}

HTREEITEM CDlgPushBackAndTowOperationSpec::AddOperationNode( HTREEITEM hFlighttypeItem )
{
	if(GetItemType(hFlighttypeItem) != nodeType_FlightType)
		return NULL;

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hFlighttypeItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hFlighttypeItem);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_COMBOBOX;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_Operation;
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;

	CString strOperationName = TOWOPERATIONTYPENAME[pFlightData->GetEnumOperartionType()];

	HTREEITEM hOperationItem = m_treeCtrl.InsertItem(_T("Operations: ")+ strOperationName, cni, FALSE, FALSE, hFlighttypeItem);
	m_treeCtrl.SetItemData(hOperationItem,(DWORD_PTR)pItemDatEx);

	if(pFlightData->GetEnumOperartionType()  >  TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY &&
		pFlightData->GetEnumOperartionType() <= TOWOPERATIONTYPE_PUSHBACKTORELEASEPOINT )
	{
		AddReleaseNode(hOperationItem);
		m_treeCtrl.Expand(hOperationItem,TVE_EXPAND);
	}
	return hOperationItem;

}
HTREEITEM CDlgPushBackAndTowOperationSpec::AddReleaseNode( HTREEITEM hOperationNode )
{
	if(GetItemType(hOperationNode) != nodeType_Operation)
		return NULL;

	HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hOperationNode);
	while(hChildItem)
	{
		m_treeCtrl.DeleteItem(hChildItem);
		hChildItem = m_treeCtrl.GetChildItem(hOperationNode);
	}

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hOperationNode);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hOperationNode);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_SHOW_DIALOGBOX;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	int nCount = pFlightData->GetElementCount();
	for (int i = 0; i < nCount; i++)
	{
		//item data
		CReleasePoint* pReleaseData = pFlightData->GetItem(i);
		CTreeItemData *pItemDatEx = new CTreeItemData;
		pItemDatEx->itemType = nodeType_ReleasePoint;
		pItemDatEx->pStandData = pTowStandData;
		pItemDatEx->pFlightTypeData = pFlightData;
		pItemDatEx->pReleasePoint = pReleaseData;

		CString strReleasePointName = pReleaseData->getName();
		HTREEITEM hReleaseItem = m_treeCtrl.InsertItem(_T("Release point: ")+ strReleasePointName, cni, FALSE, FALSE, hOperationNode);
		m_treeCtrl.SetItemData(hReleaseItem,(DWORD_PTR)pItemDatEx);

		AddOutBoundRoute(hReleaseItem);
		AddReturnRoute(hReleaseItem);

		m_treeCtrl.Expand(hReleaseItem,TVE_EXPAND);
	}

	return NULL;

}

HTREEITEM CDlgPushBackAndTowOperationSpec::AddOutBoundRoute( HTREEITEM hReleasePointItem )
{

	if(GetItemType(hReleasePointItem) != nodeType_ReleasePoint)
		return NULL;

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hReleasePointItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hReleasePointItem);
	CReleasePoint* pReleasePointData = GetItemReleasePointData(hReleasePointItem); 

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_OutBoundRoute;
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;
	pItemDatEx->pReleasePoint = pReleasePointData;



	HTREEITEM hOutBoundRouteItem = m_treeCtrl.InsertItem(_T("Outbound route"), cni, FALSE, FALSE, hReleasePointItem);
	m_treeCtrl.SetItemData(hOutBoundRouteItem,(DWORD_PTR)pItemDatEx);

	AddOutBoundRouteType(hOutBoundRouteItem);

	m_treeCtrl.Expand(hOutBoundRouteItem,TVE_EXPAND);
	return hOutBoundRouteItem;
}


HTREEITEM CDlgPushBackAndTowOperationSpec::AddOutBoundRouteType( HTREEITEM hReturnRouteItem )
{

	if(GetItemType(hReturnRouteItem) != nodeType_OutBoundRoute)
		return NULL;

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hReturnRouteItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hReturnRouteItem);
	CReleasePoint* pReleasePointData = GetItemReleasePointData(hReturnRouteItem); 

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_COMBOBOX;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_OutBoundRouteType;
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;
	pItemDatEx->pReleasePoint = pReleasePointData;

	CString strRouteType = TOWOPERATIONROUTETYPENAME[pReleasePointData->GetEnumOutRouteType()];

	HTREEITEM hRouteTypeItem = m_treeCtrl.InsertItem(_T("Route type: ")+ strRouteType, cni, FALSE, FALSE, hReturnRouteItem);
	m_treeCtrl.SetItemData(hRouteTypeItem,(DWORD_PTR)pItemDatEx);

	if(pReleasePointData->GetEnumOutRouteType() == TOWOPERATIONROUTETYPE_USERDEFINE)
	{
		int nChildCount = pReleasePointData->GetOutBoundRoute()->GetElementCount();
		for (int nChild = 0; nChild < nChildCount; ++nChild)
		{
			CTowOperationRouteItem *pChildItem = pReleasePointData->GetOutBoundRoute()->GetItem(nChild);
			AddTowRouteItem(pChildItem,hRouteTypeItem);
		}
	}


	return hRouteTypeItem;

}
HTREEITEM CDlgPushBackAndTowOperationSpec::AddReturnRoute( HTREEITEM hReleasePointItem )
{
	if(GetItemType(hReleasePointItem) != nodeType_ReleasePoint)
		return NULL;

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hReleasePointItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hReleasePointItem);
	CReleasePoint* pReleasePointData = GetItemReleasePointData(hReleasePointItem); 

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;


	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_ReturnRoute;
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;
	pItemDatEx->pReleasePoint = pReleasePointData;


	HTREEITEM hReturnRouteItem = m_treeCtrl.InsertItem(_T("Return route"), cni, FALSE, FALSE, hReleasePointItem);
	m_treeCtrl.SetItemData(hReturnRouteItem,(DWORD_PTR)pItemDatEx);

	AddReturnRouteType(hReturnRouteItem);

	m_treeCtrl.Expand(hReturnRouteItem,TVE_EXPAND);
	return hReturnRouteItem;

}

HTREEITEM CDlgPushBackAndTowOperationSpec::AddReturnRouteType( HTREEITEM hReturnRouteItem )
{
	if(GetItemType(hReturnRouteItem) != nodeType_ReturnRoute)
		return NULL;

	CTowOperationSpecStand *pTowStandData = GetItemStandData(hReturnRouteItem);
	CTowOperationSpecFltTypeData *pFlightData = GetItemFltTypeData(hReturnRouteItem);
	CReleasePoint* pReleasePointData = GetItemReleasePointData(hReturnRouteItem); 

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_COMBOBOX;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;

	//item data
	CTreeItemData *pItemDatEx = new CTreeItemData;
	pItemDatEx->itemType = nodeType_ReturnRouteType;
	pItemDatEx->pStandData = pTowStandData;
	pItemDatEx->pFlightTypeData = pFlightData;
	pItemDatEx->pReleasePoint = pReleasePointData;

	CString strRouteType = TOWOPERATIONROUTETYPENAME[pReleasePointData->GetEnumRetRouteType()];

	HTREEITEM hRouteTypeItem = m_treeCtrl.InsertItem(_T("Route type: ")+ strRouteType, cni, FALSE, FALSE, hReturnRouteItem);
	m_treeCtrl.SetItemData(hRouteTypeItem,(DWORD_PTR)pItemDatEx);

	if(pReleasePointData->GetEnumRetRouteType() == TOWOPERATIONROUTETYPE_USERDEFINE)
	{
		int nChildCount = pReleasePointData->GetReturnRoute()->GetElementCount();
		for (int nChild = 0; nChild < nChildCount; ++nChild)
		{
			CTowOperationRouteItem *pChildItem = pReleasePointData->GetReturnRoute()->GetItem(nChild);
			AddTowRouteItem(pChildItem,hRouteTypeItem);
		}
	}
	return hRouteTypeItem;
}
void CDlgPushBackAndTowOperationSpec::OnAddTowRouteItem()
{

}

void CDlgPushBackAndTowOperationSpec::OnDelTowRouteItem()
{

}

CDlgPushBackAndTowOperationSpec::enumNodeType CDlgPushBackAndTowOperationSpec::GetItemType( HTREEITEM hItem )
{
	if (hItem == NULL)
	{
		return nodeType_None;
	}
	CTreeItemData *pItemData = (CTreeItemData *)m_treeCtrl.GetItemData(hItem);
	if(pItemData)
	{
		return pItemData->itemType;
	}

	return nodeType_None;
}

CTowOperationSpecStand * CDlgPushBackAndTowOperationSpec::GetItemStandData( HTREEITEM hItem )
{
	if (hItem == NULL)
	{
		return NULL;
	}
	CTreeItemData *pItemData = (CTreeItemData *)m_treeCtrl.GetItemData(hItem);
	if(pItemData)
	{
		return pItemData->pStandData;
	}

	return NULL;
}

CTowOperationSpecFltTypeData * CDlgPushBackAndTowOperationSpec::GetItemFltTypeData( HTREEITEM hItem )
{
	if (hItem == NULL)
	{
		return NULL;
	}
	CTreeItemData *pItemData = (CTreeItemData *)m_treeCtrl.GetItemData(hItem);
	if(pItemData)
	{
		return pItemData->pFlightTypeData;
	}

	return NULL;
}

CReleasePoint* CDlgPushBackAndTowOperationSpec::GetItemReleasePointData(HTREEITEM hItem)
{
	if (hItem == NULL)
	{
		return NULL;
	}
	CTreeItemData *pItemData = (CTreeItemData *)m_treeCtrl.GetItemData(hItem);
	if(pItemData)
	{
		return pItemData->pReleasePoint;
	}

	return NULL;
}

CTowOperationRouteItem * CDlgPushBackAndTowOperationSpec::GetItemRouteItemData( HTREEITEM hItem )
{
	if (hItem == NULL)
	{
		return NULL;
	}
	CTreeItemData *pItemData = (CTreeItemData *)m_treeCtrl.GetItemData(hItem);
	if(pItemData)
	{
		return pItemData->pTowRouteItem;
	}
	
	return NULL;
}

LRESULT CDlgPushBackAndTowOperationSpec::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_COMBOBOX_BEGIN)	
	{
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		if(hSelItem != NULL)
		{
			enumNodeType nodeType = GetItemType(hSelItem);

			if(nodeType == nodeType_Operation)
			{

				CWnd* pWnd = m_treeCtrl.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;

				m_treeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,150,rectWnd.Height(),SWP_SHOWWINDOW);
				CComboBox* pCB=(CComboBox*)pWnd;
				if (pCB->GetCount() != 0)
				{
					pCB->ResetContent();
				}
				if (pCB->GetCount() == 0)
				{
					for (int nType = TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY; nType < TOWOPERATIONTYPE_COUNT;++nType)
					{
						pCB->AddString(TOWOPERATIONTYPENAME[nType]);
					}

					pCB->SetCurSel(0);
				}

			}
			//else if(nodeType == nodeType_ReleasePoint)
			//{	
			//	CWnd* pWnd = m_treeCtrl.GetEditWnd((HTREEITEM)wParam);
			//	CRect rectWnd;
			//	HTREEITEM hItem = (HTREEITEM)wParam;

			//	m_treeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			//	pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			//	CComboBox* pCB=(CComboBox*)pWnd;
			//	if (pCB->GetCount() != 0)
			//	{
			//		pCB->ResetContent();
			//	}
			//	if (pCB->GetCount() == 0)
			//	{
			//		std::vector<CString> ::iterator iter = m_vStrReleasePoint.begin();
			//		for (;iter != m_vStrReleasePoint.end(); ++iter)
			//		{
			//			pCB->AddString(*iter);
			//		}

			//		pCB->SetCurSel(0);
			//	}

			//}
			else if(nodeType == nodeType_OutBoundRouteType || nodeType == nodeType_ReturnRouteType)
			{
				CWnd* pWnd = m_treeCtrl.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;

				m_treeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;
				if (pCB->GetCount() != 0)
				{
					pCB->ResetContent();
				}
				if (pCB->GetCount() == 0)
				{
					for (int nType = TOWOPERATIONROUTETYPE_SHORTESTPATH; nType < TOWOPERATIONROUTETYPE_COUNT;++nType)
					{
						pCB->AddString(TOWOPERATIONROUTETYPENAME[nType]);
					}

					pCB->SetCurSel(0);
				}
			}
		}
	}
	else if(message == UM_CEW_COMBOBOX_END)
	{
		UpdateToolBarState();
	}
	else if (message ==UM_CEW_COMBOBOX_SELCHANGE )
	{

		HTREEITEM hSelItem = (HTREEITEM)wParam;
		if(hSelItem != NULL)
		{
			CWnd* pWnd=m_treeCtrl.GetEditWnd(hSelItem);
			CComboBox* pTCB=(CComboBox*)pWnd;

			int nIndex = pTCB->GetCurSel();

			enumNodeType nodeType = GetItemType(hSelItem);

			if(nodeType == nodeType_Operation)
			{
				CTowOperationSpecFltTypeData* pFlightTypeData = GetItemFltTypeData(hSelItem);

				if(pFlightTypeData)
				{
					pFlightTypeData->SetEnumOperartionType((TOWOPERATIONTYPE)nIndex);

					CString strOperationName = TOWOPERATIONTYPENAME[pFlightTypeData->GetEnumOperartionType()];

					m_treeCtrl.SetItemText(hSelItem,_T("Operations: ")+ strOperationName);

					if(pFlightTypeData->GetEnumOperartionType()  >  TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY &&
						pFlightTypeData->GetEnumOperartionType() <= TOWOPERATIONTYPE_PUSHBACKTORELEASEPOINT )
					{

						AddReleaseNode(hSelItem);
						m_treeCtrl.Expand(hSelItem,TVE_EXPAND);
					}
					else
					{
						HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hSelItem);
						while(hChildItem)
						{
							m_treeCtrl.DeleteItem(hChildItem);
							hChildItem = m_treeCtrl.GetChildItem(hSelItem);
						}
					}

				}

			}
			//else if(nodeType == nodeType_ReleasePoint)
			//{	
			//	CWnd* pWnd=m_treeCtrl.GetEditWnd(hSelItem);
			//	CComboBox* pTCB=(CComboBox*)pWnd;

			//	int nIndex = pTCB->GetCurSel();

			//	CTowOperationSpecFltTypeData* pFlightTypeData = GetItemFltTypeData(hSelItem);
			//	if(pFlightTypeData != NULL)
			//	{
			//		if(nIndex < (int)m_vReleasePoint.size())
			//			pFlightTypeData->GetReleasePoint() = m_vReleasePoint.at(nIndex);
			//		if(nIndex < (int)m_vStrReleasePoint.size())
			//			m_treeCtrl.SetItemText(hSelItem, _T("Release point: ")+ m_vStrReleasePoint.at(nIndex));
			//	}
			//}
			else if(nodeType == nodeType_OutBoundRouteType) 
			{
				CReleasePoint* pReleasepoint = GetItemReleasePointData(hSelItem);

				if(pReleasepoint)
				{
					if(pReleasepoint->GetEnumOutRouteType() != (TOWOPERATIONROUTETYPE)nIndex)
					{
						pReleasepoint->SetEnumOutRouteType((TOWOPERATIONROUTETYPE)nIndex);

						CString strRouteType = TOWOPERATIONROUTETYPENAME[pReleasepoint->GetEnumOutRouteType()];

						m_treeCtrl.SetItemText(hSelItem, _T("Route type: ")+ strRouteType);
						if(pReleasepoint->GetEnumOutRouteType() == TOWOPERATIONROUTETYPE_USERDEFINE)
						{
							int nChildCount = pReleasepoint->GetOutBoundRoute()->GetElementCount();
							for (int nChild = 0; nChild < nChildCount; ++nChild)
							{
								CTowOperationRouteItem *pChildItem = pReleasepoint->GetOutBoundRoute()->GetItem(nChild);
								AddTowRouteItem(pChildItem,hSelItem);
							}
						}
						else
						{
							HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hSelItem);
							while(hChildItem)
							{
								m_treeCtrl.DeleteItem(hChildItem);
								hChildItem = m_treeCtrl.GetChildItem(hSelItem);
							}
						}

					}

				}


			}
			else if(nodeType == nodeType_ReturnRouteType)
			{
				CReleasePoint* pReleasepoint = GetItemReleasePointData(hSelItem);

				if(pReleasepoint)
				{
					if(pReleasepoint->GetEnumRetRouteType() != (TOWOPERATIONROUTETYPE)nIndex)
					{
						pReleasepoint->SetEnumRetRouteType((TOWOPERATIONROUTETYPE)nIndex);

						CString strRouteType = TOWOPERATIONROUTETYPENAME[pReleasepoint->GetEnumRetRouteType()];

						m_treeCtrl.SetItemText(hSelItem, _T("Route type: ")+ strRouteType);

						if(pReleasepoint->GetEnumRetRouteType() == TOWOPERATIONROUTETYPE_USERDEFINE)
						{
							int nChildCount = pReleasepoint->GetReturnRoute()->GetElementCount();
							for (int nChild = 0; nChild < nChildCount; ++nChild)
							{
								CTowOperationRouteItem *pChildItem = pReleasepoint->GetReturnRoute()->GetItem(nChild);
								AddTowRouteItem(pChildItem,hSelItem);
							}
						}
						else
						{
							HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hSelItem);
							while(hChildItem)
							{
								m_treeCtrl.DeleteItem(hChildItem);
								hChildItem = m_treeCtrl.GetChildItem(hSelItem);
							}
						}

					}


				}

			}


		}


		UpdateToolBarState();
	}
	else if(message == UM_CEW_COMBOBOX_SETWIDTH)
	{
		m_treeCtrl.SetWidth(200);
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

}






















