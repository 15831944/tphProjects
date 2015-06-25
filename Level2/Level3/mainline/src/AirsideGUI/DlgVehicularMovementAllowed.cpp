// DlgVehicularMovementAllowed.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgVehicularMovementAllowed.h"
#include ".\dlgvehicularmovementallowed.h"
#include "../InputAirside/VehicleSpecifications.h"
#include "../InputAirside//InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/taxiway.h"
#include "../InputAirside/VehicleDefine.h"
#include "../Common/WinMsg.h"
#include "../Database/DBElementCollection_Impl.h"

// CDlgVehicularMovementAllowed dialog

IMPLEMENT_DYNAMIC(CDlgVehicularMovementAllowed, CDialogResize)
CDlgVehicularMovementAllowed::CDlgVehicularMovementAllowed(int nProjID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(IDD_DIALOG_VEHICULARMOVEMENT/*CDlgVehicularMovementAllowed::IDD*/, pParent)
	,m_nProjID(nProjID)
	,m_VehicularData(nProjID)
	,m_pCurTypeItemData(NULL)
{
}

CDlgVehicularMovementAllowed::~CDlgVehicularMovementAllowed()
{

}

void CDlgVehicularMovementAllowed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_TYPETREE, m_treeVehicle);
	DDX_Control(pDX, IDC_LIST_TAXIWAYLIST, m_lstTaxiway);
}
BEGIN_MESSAGE_MAP(CDlgVehicularMovementAllowed, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADDTAXIWAY, OnNewTaxiwayItem)
	ON_COMMAND(ID_TOOLBAR_DELTAXIWAY, OnDelTaxiwayItem)
	ON_BN_CLICKED(IDC_CHECK_DIABLESETTINGS, OnBnClickedCheckDiablesettings)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TYPETREE, OnTvnSelchangedTreeTypetree)
	//ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_TYPETREE, OnTvnSelchangingTreeTypetree)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	//ON_LBN_SELCHANGE(IDC_LIST_TAXIWAYLIST, OnLbnSelchangeListTaxiway)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TAXIWAYLIST, OnLbnSelchangeListTaxiway)
END_MESSAGE_MAP()

// CDlgVehicularMovementAllowed message handlers

BOOL CDlgVehicularMovementAllowed::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	m_VehicleSpecifications.ReadData(m_nProjID);

	InitTreeCtrl();
	m_VehicularData.ReadData(m_nProjID);

	InitListCtrl();

	SetResize(IDC_CHECK_DIABLESETTINGS, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_toolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_VEHICLEFRAME, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_TYPETREE, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_LIST_TAXIWAYLIST, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);	



	//select the first type
	HTREEITEM hRoot = m_treeVehicle.GetRootItem();
	HTREEITEM hItemSel = m_treeVehicle.GetChildItem(hRoot);
	m_treeVehicle.SelectItem(hItemSel);
	UpdateToolbar();

	bool bDisable = m_VehicularData.GetDiable();
	if(bDisable)
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_DIABLESETTINGS);
		CButton *pCheckButton = (CButton *)pWnd;
		if(pCheckButton)
			pCheckButton->SetCheck(TRUE);

		DisableDialog();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVehicularMovementAllowed::InitTreeCtrl()
{
	HTREEITEM hRoot = m_treeVehicle.InsertItem(_T("Vehicles"));
	
	int nItemCount =  (int)m_VehicleSpecifications.GetElementCount() ;
	for (int i =0; i < nItemCount; ++ i)
	{
		/*enumVehicleBaseType nTypeID = m_VehicularData.GetItemType(i);*/
		CString strName = m_VehicleSpecifications.GetItem(i)->getName();
		int nNameID = m_VehicleSpecifications.GetItem(i)->GetID();

		CVehicularMovementAllowedTypeItem* pVehicleItem = new CVehicularMovementAllowedTypeItem(nNameID);
		m_VehicularData.m_vTypeData.push_back(pVehicleItem);

		HTREEITEM hTypeItem = m_treeVehicle.InsertItem(strName,hRoot);
		m_treeVehicle.SetItemData(hTypeItem,nNameID);
	}

	m_treeVehicle.Expand(hRoot, TVE_EXPAND);



}
void CDlgVehicularMovementAllowed::GetAllTaxiway()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	std::vector<int>::iterator iterAirportID = vAirportIds.begin();
	for (; iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		ALTAirport::GetTaxiwaysIDs(*iterAirportID, m_vTaxiwaysIDs);

		std::vector<int>::iterator iterTaxiwayID = m_vTaxiwaysIDs.begin();
		for (; iterTaxiwayID != m_vTaxiwaysIDs.end(); ++iterTaxiwayID)
		{
			Taxiway taxiway;
			taxiway.ReadObject(*iterTaxiwayID);

			CString strTmpTaxiway;
			strTmpTaxiway.Format("%s", taxiway.GetObjNameString());
			m_strListTaxiway.InsertAfter(m_strListTaxiway.GetTailPosition(), strTmpTaxiway);
			m_mapTaxiwayName[*iterTaxiwayID] = strTmpTaxiway;

		}
	}

	//m_nTaxiwayCount = m_vTaxiwaysIDs.size();
}
void CDlgVehicularMovementAllowed::InitListCtrl()
{
	DWORD dwStyle = m_lstTaxiway.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstTaxiway.SetExtendedStyle( dwStyle );

	int nColCount = 3;
	char* columnLabel[] = {"Taxiway ", "From", "To" };


	int DefaultColumnWidth[] = { 120, 120, 120 };
	int nColFormat[] = { LVCFMT_DROPDOWN, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN };


	GetAllTaxiway();

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.csList = &m_strListTaxiway;;	
	lvc.fmt = nColFormat[0];
	lvc.pszText = columnLabel[0];
	lvc.cx = DefaultColumnWidth[0];
	m_lstTaxiway.InsertColumn(0, &lvc);
	
	//
	CStringList strEmptyList;
	lvc.csList = &strEmptyList;
	for (int i = 1; i < nColCount; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_lstTaxiway.InsertColumn(i, &lvc);
	}
}

void CDlgVehicularMovementAllowed::InitToolBar()
{
	CRect rcToolbar;
	GetDlgItem(IDC_STATIC_TAXIWAYTOOLABR)->GetWindowRect(&rcToolbar);
	
	ScreenToClient(&rcToolbar);
	m_toolBar.MoveWindow(&rcToolbar,TRUE);

}
int CDlgVehicularMovementAllowed::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_toolBar.CreateEx(this) || !m_toolBar.LoadToolBar(IDR_TOOLBAR_VEHICULARMOVEMENTALLOWED))
	{
		TRACE("Error create toolbar");
	}

	return 0;
}
void CDlgVehicularMovementAllowed::GetNodeListByTaxiwayID(int nTaxiwayID)
{
	m_vNodeList.clear();
	m_strListNode.RemoveAll();
	
	if (nTaxiwayID == -1)	// have no taxiway
		return;
	
	std::map<int, std::vector<IntersectionNode> >::iterator iter = m_mapTaxiwayNode.find(nTaxiwayID);
	if( iter != m_mapTaxiwayNode.end())
	{
		m_vNodeList = iter->second;
	}
	else
	{
		// get taxiway name
		Taxiway taxiway;
		taxiway.ReadObject(nTaxiwayID);

		CString strTaxiway;
		strTaxiway.Format("%s", taxiway.GetObjNameString());

		// get the list of intersect node

		m_vNodeList = taxiway.GetIntersectNodes();
		//int nCurNodeCount = m_vNodeList.size();
		//if (nCurNodeCount <= 0)
		//	return;

		m_mapTaxiwayNode[nTaxiwayID] = m_vNodeList;
	}

	m_strListNode.RemoveAll();
	m_mapNodeName.clear();
	std::vector<IntersectionNode>::iterator iterNode = m_vNodeList.begin();
	for (; iterNode != m_vNodeList.end(); ++iterNode)
	{
		/*CString strObjName;
		ALTObject * pObj = (*iter).GetOtherObject(nTaxiwayID);
		if(pObj)
		strObjName = pObj->GetObjNameString();

		CString strNode;
		strNode.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);*/
		CString strNodeName = iterNode->GetName();
		m_strListNode.InsertAfter( m_strListNode.GetTailPosition(),  strNodeName);
		m_mapNodeName[(*iterNode).GetID()] = strNodeName;
	}
}
void CDlgVehicularMovementAllowed::OnNewTaxiwayItem()
{
	int nNewIndex = m_lstTaxiway.GetItemCount();
	if(m_strListTaxiway.GetCount() > 0 && m_pCurTypeItemData != NULL)
	{	
		CVehicularMovementAllowedTypeItemData TypeItemData(m_pCurTypeItemData->getVehicleNameID());
		m_pCurTypeItemData->AddItem(TypeItemData);
	
		//int nIndex = m_lstTaxiway.InsertItem(nNewIndex,m_strListTaxiway.GetAt(0));
		m_lstTaxiway.EditNew();
		
        return ;
	}
	CString errorMes ;
    if(m_strListTaxiway.GetCount() == 0)
		errorMes.Format("%s","No TaxiWay have been defined yet!") ;
	if(m_pCurTypeItemData == NULL)
		errorMes.Format("%s","Please Select Vehicular type Firstly!") ;
    MessageBox(errorMes,"Error",MB_OK) ;
}

void CDlgVehicularMovementAllowed::OnDelTaxiwayItem()
{
	POSITION pos =  m_lstTaxiway.GetFirstSelectedItemPosition();
	if (pos)
	{
		int nSelectItem = m_lstTaxiway.GetNextSelectedItem(pos);
		m_lstTaxiway.DeleteItem(nSelectItem);

		HTREEITEM hSelectedItem = m_treeVehicle.GetSelectedItem();
		long nItemData = m_treeVehicle.GetItemData(hSelectedItem);

		//enumVehicleBaseType enumSelType = VehicleType_General;
		if (nItemData >= 0)
		{
			//enumSelType = (enumVehicleBaseType)nItemData;
			CVehicularMovementAllowedTypeItem* TypeItemData = m_VehicularData.GetItem(nItemData);
			if(TypeItemData)
				TypeItemData->DelItem(nSelectItem);
		}

		//if(m_lstTaxiway.GetItemCount() > 0 && m_lstTaxiway.GetItemCount() > nSelectItem)
		//{
		//	m_lstTaxiway.Selec
		//}

	}
	//m_lstTaxiway

}

void CDlgVehicularMovementAllowed::UpdateToolbar()
{
	if(m_treeVehicle.GetSelectedItem() && m_treeVehicle.GetParentItem(m_treeVehicle.GetSelectedItem()))
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADDTAXIWAY, TRUE);
	else
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADDTAXIWAY, FALSE);

	if(m_lstTaxiway.GetItemCount() > 0)
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELTAXIWAY, TRUE);
	else
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELTAXIWAY, FALSE);
}

void CDlgVehicularMovementAllowed::DisableDialog()
{
	m_treeVehicle.EnableWindow(FALSE);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADDTAXIWAY, FALSE);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELTAXIWAY, FALSE);
	m_lstTaxiway.EnableWindow(FALSE);

}
void CDlgVehicularMovementAllowed::EnableDialog()
{
	m_treeVehicle.EnableWindow(TRUE);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADDTAXIWAY, TRUE);
	if(m_lstTaxiway.GetItemCount() > 0)
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELTAXIWAY, TRUE);
	else
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELTAXIWAY, FALSE);

	m_lstTaxiway.EnableWindow(TRUE);
}
// CDlgVehicularMovementAllowedEx message handlers

void CDlgVehicularMovementAllowed::OnBnClickedCheckDiablesettings()
{
	CWnd *pWnd =  GetDlgItem(IDC_CHECK_DIABLESETTINGS);
	CButton *pCheckButton = (CButton *)pWnd;
	int nCheck = pCheckButton->GetCheck();
	if (nCheck > 0)
	{
		DisableDialog();
	}
	else
	{
		EnableDialog();
	}

	m_VehicularData.SetDisable(nCheck>0?true:false);
}
void CDlgVehicularMovementAllowed::OnTvnSelchangedTreeTypetree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if(pNMTreeView->itemNew.hItem == pNMTreeView->itemOld.hItem)
		return ;

	HTREEITEM hSelectedItem = m_treeVehicle.GetSelectedItem();
	if(m_treeVehicle.GetParentItem(hSelectedItem) != NULL)
	{	
		m_lstTaxiway.EnableWindow(TRUE);
		
		int nItemData = m_treeVehicle.GetItemData(hSelectedItem);

		//enumVehicleBaseType enumSelType = VehicleType_General;
		if (nItemData >= 0)
		{
			//enumSelType = (enumVehicleBaseType)nItemData;
			m_pCurTypeItemData = m_VehicularData.GetItem(nItemData);
			if (m_pCurTypeItemData == NULL)
			{
				return;
			}
			AddTaxiwayIntoList();

			if(m_lstTaxiway.GetItemCount() > 0)
			{
				m_lstTaxiway.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				//OnLbnSelchangeListTaxiway();
			}
		}

		UpdateToolbar();

	}
	else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADDTAXIWAY, FALSE);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELTAXIWAY, FALSE);
		m_lstTaxiway.EnableWindow(FALSE);
		m_lstTaxiway.DeleteAllItems();
	}





}
void CDlgVehicularMovementAllowed::AddTaxiwayIntoList()
{
	m_lstTaxiway.DeleteAllItems();

	//CVehicularMovementAllowedTypeItem* TypeItemData = m_VehicularData.GetItem(enumType);
	if(m_pCurTypeItemData == NULL)
		return;

	for (int i = 0; i< m_pCurTypeItemData->GetItemCount(); ++i)
	{
		CVehicularMovementAllowedTypeItemData itemData = m_pCurTypeItemData->GetItem(i);
		int nTaxiwayID = itemData.GetTaxiwayID();

		CString strTaxiwayName;
		std::map<int, CString >::iterator iterName = m_mapTaxiwayName.find(nTaxiwayID);
		if(iterName != m_mapTaxiwayName.end())
		{

			strTaxiwayName = iterName->second;
		}
		CString strNodeFrom;
		CString strNodeTo;

		GetNodeListByTaxiwayID(nTaxiwayID);	
		int nNodeFrom = itemData.GetNodeFrom();
		std::map<int, CString >::iterator iterNodeFrom = m_mapNodeName.find(nNodeFrom);
		if(iterNodeFrom != m_mapNodeName.end())
			strNodeFrom = iterNodeFrom->second;

		int nNodeTo = itemData.GetNodeTo();
		std::map<int, CString >::iterator iterNodeTo = m_mapNodeName.find(nNodeTo);
		if(iterNodeTo != m_mapNodeName.end())
			strNodeTo = iterNodeTo->second;

		m_lstTaxiway.InsertItem(i,strTaxiwayName);
		m_lstTaxiway.SetItemText(i,1,strNodeFrom);
		m_lstTaxiway.SetItemText(i,2,strNodeTo);
	}

}
void CDlgVehicularMovementAllowed::OnBnClickedButtonSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_VehicularData.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
}

LRESULT CDlgVehicularMovementAllowed::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nCurSubSel = dispinfo->item.iSubItem;	

	//CTaxiwayConstraintData* pCurData = (CTaxiwayConstraintData*)m_lstTaxiway.GetItemData(nCurSel);
	if(m_pCurTypeItemData == NULL)
		return 0;

	CVehicularMovementAllowedTypeItemData& itemData = m_pCurTypeItemData->GetItem(nCurSel);
	switch (nCurSubSel)
	{
	case 0:
		{
			int nTaxiwayID = m_vTaxiwaysIDs.at(nComboxSel);
			if(itemData.GetTaxiwayID() != nTaxiwayID)
			{
				itemData.SetNodeFrom(-1);
				itemData.SetNodeTo(-1);

				m_lstTaxiway.SetItemText(nCurSel,1,_T(""));
				m_lstTaxiway.SetItemText(nCurSel,2,_T(""));

				GetNodeListByTaxiwayID(nTaxiwayID);

				LVCOLDROPLIST* pDroplistFrom =  m_lstTaxiway.GetColumnStyle(1);
				LVCOLDROPLIST* pDroplistTo =  m_lstTaxiway.GetColumnStyle(2);
				pDroplistFrom->List.RemoveAll();
				pDroplistTo->List.RemoveAll();
				POSITION pos;
				for( pos = m_strListNode.GetHeadPosition(); pos;)
				{
					pDroplistFrom->List.AddTail(m_strListNode.GetAt(pos));
					pDroplistTo->List.AddTail(m_strListNode.GetAt(pos));
					m_strListNode.GetNext( pos );
				}	

			}
			itemData.SetTaxiwayID(nTaxiwayID);

		}
		break;
	case 1: // From
		{
			int nCurNodeFromID = (m_vNodeList.at(nComboxSel)).GetID();
			itemData.SetNodeFrom(nCurNodeFromID);
		}
		break;
	case 2: // To
		{
			int nCurNodeToID = (m_vNodeList.at(nComboxSel)).GetID();
			itemData.SetNodeTo(nCurNodeToID);
		}
		break;
	default:
		break;
	}	

	return 0;
}
void CDlgVehicularMovementAllowed::OnLbnSelchangeListTaxiway(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION pos =  m_lstTaxiway.GetFirstSelectedItemPosition();
	if (pos)
	{
		int nSelectItem = m_lstTaxiway.GetNextSelectedItem(pos);
		if(m_pCurTypeItemData)
		{
			CVehicularMovementAllowedTypeItemData itemData = m_pCurTypeItemData->GetItem(nSelectItem);
			GetNodeListByTaxiwayID(itemData.GetTaxiwayID());

			LVCOLDROPLIST* pDroplistFrom =  m_lstTaxiway.GetColumnStyle(1);
			LVCOLDROPLIST* pDroplistTo =  m_lstTaxiway.GetColumnStyle(2);
			if(pDroplistFrom && pDroplistTo)
			{
				pDroplistFrom->List.RemoveAll();
				pDroplistTo->List.RemoveAll();
				POSITION pos;
				for( pos = m_strListNode.GetHeadPosition(); pos;)
				{
					pDroplistFrom->List.AddTail(m_strListNode.GetAt(pos));
					pDroplistTo->List.AddTail(m_strListNode.GetAt(pos));
					m_strListNode.GetNext( pos );
				}	
			}

		}

	}


	UpdateToolbar();

	*pResult = 0;
}

void CDlgVehicularMovementAllowed::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		CADODatabase::BeginTransaction() ;
		m_VehicularData.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CDialog::OnOK();
}

void CDlgVehicularMovementAllowed::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

//void CDlgVehicularMovementAllowed::OnTvnSelchangingTreeTypetree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	*pResult = 0;
//
//	if (m_pCurTypeItemData == NULL)
//		return;
//
//	int nInvalid = m_pCurTypeItemData->IsDataValid();
//	if(nInvalid >= 0)
//	{
//		CVehicularMovementAllowedTypeItemData itemData = m_pCurTypeItemData->GetItem(nInvalid);
//		CString strMessage = _T("");
//		
//		CString strObject;
//		if(itemData.GetTaxiwayID() < 0)
//			strObject += "Taxiway";
//		if(itemData.GetNodeFrom() < 0)
//		{
//			if(strObject.IsEmpty())
//				strObject += "From Node";
//			else
//				strObject +=",From Node";
//		}
//
//		if (itemData.GetNodeTo() < 0)
//		{
//			if(strObject.IsEmpty())
//				strObject += "To Node";		
//			else
//				strObject += ",To Node";
//		}
//
//		strMessage.Format(_T("The data of line %d is not valid, please set the data: \n %s is not valid"),nInvalid + 1, strObject);
//		MessageBox(strMessage,_T("warning"));
//
//		m_lstTaxiway.SetItemState(nInvalid,LVIS_SELECTED|LVIS_FOCUSED,LVIS_FOCUSED|LVIS_SELECTED);
//		//m_lstTaxiway.edit
//		*pResult = 1;
//	}
//
//
//	
//}











