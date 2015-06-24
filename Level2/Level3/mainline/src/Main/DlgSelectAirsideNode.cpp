// DLgSelectAirsideNode.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectAirsideNode.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "resource.h"
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/IntersectionNodesInAirport.h"
#include ".\dlgselectairsidenode.h"
// CDLgSelectAirsideNode dialog
#include "../InputAirside/Taxiway.h"
IMPLEMENT_DYNAMIC(CDlgSelectAirsideNode, CDialog)
CDlgSelectAirsideNode::CDlgSelectAirsideNode(int nProjID, CWnd* pParent /*=NULL*/)
: CDialog(CDlgSelectAirsideNode::IDD, pParent)
, m_nProjectID(nProjID)
{
}

CDlgSelectAirsideNode::~CDlgSelectAirsideNode()
{
}


// CDLgSelectAirsideNode message handlers
// DlgReportSelectObject.cpp : implementation file
//

void CDlgSelectAirsideNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ALTOBJECT, m_wndALTObjectTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectAirsideNode, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJECT, OnSelChangedTree)
END_MESSAGE_MAP()


// CDlgReportSelectObject message handlers
BOOL CDlgSelectAirsideNode::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Select Report Object"));

	m_wndALTObjectTree.ModifyStyle(NULL,WS_VSCROLL);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjectID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_wndALTObjectTree.InsertItem(airport.getName());
		m_wndALTObjectTree.SetItemData(hAirport,(DWORD_PTR)new nodeData(objNodeType_Airport));
		InitTreeContentsForAirport(hAirport, *iterAirportID);
		InitIntersectionNodeToAirport(hAirport, *iterAirportID);
	}

	InitTreeContentsForAirspace();
	return TRUE;
}
void CDlgSelectAirsideNode::InitTreeContentsForAirspace()
{	
	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(ALT_WAYPOINT, m_nProjectID, vObject);

	HTREEITEM hAirspace = m_wndALTObjectTree.InsertItem(_T("Airspace"));
	m_wndALTObjectTree.SetItemData(hAirspace,(DWORD_PTR)new nodeData(objNodeType_Airspace));


	HTREEITEM hWaypointRootItem = m_wndALTObjectTree.InsertItem("Waypoint", hAirspace);
	m_wndALTObjectTree.SetItemData(hWaypointRootItem,(DWORD_PTR)new nodeData(objNodeType_WaypointRoot));
	
	//HTREEITEM hParentItem = hWaypointRootItem;
	int nObjectCount = vObject.size();
	for (int i = 0; i < nObjectCount; ++i)
	{
		ALTObject& altObject = vObject.at(i);
		ALTObjectID objName;
		altObject.getObjName(objName);

		HTREEITEM hParentItem = hWaypointRootItem;
		for (int nLevel = 0; nLevel < OBJECT_STRING_LEVEL; ++nLevel)
		{
			if (objName.m_val[nLevel].empty())
				break;

			HTREEITEM hNewItem = FindTreeChildItemByName(objName.m_val[nLevel].c_str(), hParentItem);
			if (hNewItem == NULL)
			{
				hNewItem = m_wndALTObjectTree.InsertItem(objName.m_val[nLevel].c_str(), hParentItem);
				if ((nLevel + 1 == OBJECT_STRING_LEVEL) ||(nLevel + 1 != OBJECT_STRING_LEVEL && objName.m_val[nLevel +1].empty() ))
				{
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_Waypoint ,altObject.GetObjNameString(nLevel),altObject.getID()));
				}
				else
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_WaypointLeaf ,altObject.GetObjNameString(nLevel)));

				//m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
			}

			hParentItem = hNewItem;
		}
	}
	m_wndALTObjectTree.Expand(hAirspace, TVE_EXPAND);
}

void CDlgSelectAirsideNode::InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID)
{

	HTREEITEM hStandRootItem = m_wndALTObjectTree.InsertItem("Stand", hAirport);
	m_wndALTObjectTree.SetItemData(hStandRootItem,(DWORD_PTR)new nodeData(objNodeType_StandRoot));
	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_STAND, nAirportID, vObject);
	ALTObjectList vObject;
	ALTAirport::GetStandList(nAirportID,vObject);
	
	int nObjectCount = vObject.size();
	for (int i = 0; i < nObjectCount; ++i)
	{
		ALTObject* altObject = vObject.at(i).get();
		ALTObjectID objName;
		altObject->getObjName(objName);

		HTREEITEM hParentItem = hStandRootItem;
		for (int nLevel = 0; nLevel < OBJECT_STRING_LEVEL; ++nLevel)
		{
			if (objName.m_val[nLevel].empty())
				break;

			HTREEITEM hNewItem = FindTreeChildItemByName(objName.m_val[nLevel].c_str(), hParentItem);
			if (hNewItem == NULL)
			{
				hNewItem = m_wndALTObjectTree.InsertItem(objName.m_val[nLevel].c_str(), hParentItem);

				if ((nLevel + 1 == OBJECT_STRING_LEVEL) ||(nLevel + 1 != OBJECT_STRING_LEVEL && objName.m_val[nLevel +1].empty() ))
				{
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_Stand, altObject->GetObjNameString(nLevel),altObject->getID()));
				}
				else
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_StandLeaf, altObject->GetObjNameString(nLevel)));

				//m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
			}

			hParentItem = hNewItem;
		}
	}
	m_wndALTObjectTree.Expand(hAirport, TVE_EXPAND);
}

HTREEITEM CDlgSelectAirsideNode::FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem)
{
	HTREEITEM hChildItem = m_wndALTObjectTree.GetChildItem(hParentItem);
	while (hChildItem != NULL)
	{
		if (m_wndALTObjectTree.GetItemText(hChildItem).CompareNoCase(lpszName) == 0)
			return hChildItem;

		hChildItem = m_wndALTObjectTree.GetNextSiblingItem(hChildItem);
	}

	return NULL;
}

//int CDlgReportSelectObject::GetTreeItemDepthIndex(HTREEITEM hTreeItem)
//{
//	int nDepthIndex = 0;
//	while (hTreeItem = m_wndALTObjectTree.GetParentItem(hTreeItem))
//		++nDepthIndex;
//
//	return nDepthIndex;
//}

void CDlgSelectAirsideNode::OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{	
	*pResult = 0;

	//not use again, 


	HTREEITEM hSelItem = m_wndALTObjectTree.GetSelectedItem();

	if (hSelItem == NULL)
		return;
	//select one node
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	return;


	BOOL bEnableOK = FALSE;

	nodeData* pNodeData = reinterpret_cast<nodeData *>(m_wndALTObjectTree.GetItemData(hSelItem));
	if (pNodeData != NULL)
	{
		if (pNodeData->nodeType == objNodeType_Stand || pNodeData->nodeType == objNodeType_Waypoint )
		{
			m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
			m_selNode.SetNodeID(pNodeData->nNodeID);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			bEnableOK = TRUE;
		}
		else if (pNodeData->nodeType ==objNodeType_InterNode )
		{
			m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_Intersection);
			m_selNode.SetNodeID(pNodeData->nNodeID);	
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			bEnableOK = TRUE;
		}

	}


	if (bEnableOK == TRUE)
	{
		for (int i  = 0; i < OBJECT_STRING_LEVEL; ++i)
			m_selectedALTObjectID.m_val[i].clear();

		//CString strObjName;
		//strObjName.Empty();
		//nodeData* pTempNodeData = NULL;
		//do
		//{
		//	if (strObjName.IsEmpty())
		//		strObjName = m_wndALTObjectTree.GetItemText(hSelItem) + strObjName;
		//	else
		//		strObjName = m_wndALTObjectTree.GetItemText(hSelItem) + "-" + strObjName;

		//	hSelItem = m_wndALTObjectTree.GetParentItem(hSelItem);
		//	pTempNodeData = reinterpret_cast<nodeData *>(m_wndALTObjectTree.GetItemData(hSelItem));

		//} while(pTempNodeData->nodeType != objNodeType_StandRoot || 
		//	pTempNodeData->nodeType != objNodeType_WaypointRoot || 
		//	pTempNodeData->nodeType != objNodeType_InterNodeRoot);

		m_selectedALTObjectID.m_val[0] = pNodeData->strName;

	}


	GetDlgItem(IDOK)->EnableWindow(bEnableOK);


	//while (m_wndALTObjectTree.GetItemData(m_wndALTObjectTree.GetParentItem()))
	//{

	//};
	//if (hSelItem != NULL && nItemDepthIndex != 0)
	//{
	//	bEnableOK = TRUE;

	//	while (nItemDepthIndex > 0)
	//	{
	//		m_selectedALTObjectID.m_val[nItemDepthIndex - 1] = m_wndALTObjectTree.GetItemText(hSelItem);
	//		hSelItem = m_wndALTObjectTree.GetParentItem(hSelItem);
	//		--nItemDepthIndex;
	//	}
	//}

	//GetDlgItem(IDOK)->EnableWindow(bEnableOK);

}
void CDlgSelectAirsideNode::InitIntersectionNodeToAirport(HTREEITEM hAirport, int nAirportID)
{	
	HTREEITEM hTaxiwayRoot = m_wndALTObjectTree.InsertItem("Taxiway intersection node", hAirport);
	m_wndALTObjectTree.SetItemData(hTaxiwayRoot,(DWORD_PTR)new nodeData(objNodeType_InterNodeRoot));
	std::vector<ALTObject> vTaxiway;
	ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vTaxiway);

	IntersectionNodesInAirport lstIntersecNode;
	lstIntersecNode.ReadData(nAirportID);


	for(size_t nTaxiway = 0; nTaxiway < vTaxiway.size(); ++nTaxiway)
	{
		HTREEITEM hParentItem = NULL;
		//add taxiway node to tree
		{
			ALTObjectID objName;
			vTaxiway[nTaxiway].getObjName(objName);

			hParentItem = hTaxiwayRoot;

			for (int nLevel = 0; nLevel < OBJECT_STRING_LEVEL; ++nLevel)
			{
				if (objName.m_val[nLevel].empty())
					break;

				HTREEITEM hNewItem = FindTreeChildItemByName(objName.m_val[nLevel].c_str(), hParentItem);
				if (hNewItem == NULL)
				{
					hNewItem = m_wndALTObjectTree.InsertItem(objName.m_val[nLevel].c_str(), hParentItem);

					if ((nLevel + 1 == OBJECT_STRING_LEVEL) ||(nLevel + 1 != OBJECT_STRING_LEVEL && objName.m_val[nLevel +1].empty() ))
					{
						m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_Taxiway, vTaxiway[nTaxiway].GetObjNameString(nLevel),vTaxiway[nTaxiway].getID()));
					}
					else
						m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_TaxiwayLeaf, vTaxiway[nTaxiway].GetObjNameString(nLevel)));

					//m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
				}

				hParentItem = hNewItem;
			}

		}

		HTREEITEM hTaxiwayItem = hParentItem;
		//CString strTaxiwayName = vTaxiway[nTaxiway].GetObjNameString();
		//HTREEITEM hTaxiwayItem = m_wndALTObjectTree.InsertItem(strTaxiwayName,hParentItem);
		//m_wndALTObjectTree.SetItemData(hTaxiwayItem,(DWORD_PTR)new nodeData(objNodeType_Taxiway,strTaxiwayName,vTaxiway[nTaxiway].getID()));
		for (int nNode =0; nNode < lstIntersecNode.GetCount(); ++nNode)
		{
			if ( lstIntersecNode.NodeAt(nNode).HasObject(vTaxiway[nTaxiway].getID()) )
			{
				HTREEITEM hNodeItem = m_wndALTObjectTree.InsertItem(lstIntersecNode.NodeAt(nNode).GetName(),hTaxiwayItem);
				//m_wndALTObjectTree.SetItemData(hNodeItem,lstIntersecNode.NodeAt(nNode).GetID());
				m_wndALTObjectTree.SetItemData(hNodeItem,(DWORD_PTR)new nodeData(objNodeType_InterNode,lstIntersecNode.NodeAt(nNode).GetName(),lstIntersecNode.NodeAt(nNode).GetID()));
			}
		}
	}
}
//  objNodeType_Unkown = 0,
	//	objNodeType_StandRoot,
	//	objNodeType_StandLeaf,
	//	objNodeType_Stand ,
	//	objNodeType_WaypointRoot,
	//	objNodeType_WaypointLeaf,
	//	objNodeType_Waypoint,
	//	objNodeType_InterNodeRoot,
	//	objNodeType_InterNode,
	//	objNodeType_Airport,
	//	objNodeType_Airspace,
	//	objNodeType_TaxiwayLeaf,
	//	objNodeType_Taxiway
void CDlgSelectAirsideNode::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSelItem = m_wndALTObjectTree.GetSelectedItem();

	if (hSelItem == NULL)
		return;
	//check the selected item
	nodeData *pNodeData = (nodeData *)m_wndALTObjectTree.GetItemData(hSelItem);
	if(pNodeData == NULL)
		return;
	//check the nodes type
	 switch (pNodeData->nodeType)
	 {
	 case objNodeType_StandRoot:
		 {
			 m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
			 m_selNode.SetNodeName(_T("Stand:All"));
		 }
		 break;
	 case objNodeType_WaypointRoot:
		 {
			 m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
			 m_selNode.SetNodeName(_T("Waypoint:All"));
		 }
		 break;
	 case objNodeType_StandLeaf:
	 case objNodeType_Stand:
		 {
			 m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
			 m_selNode.SetNodeName(_T("Stand:") + pNodeData->strName);
			
			 ALTObjectGroup altGroup;
			 altGroup.setName(ALTObjectID(pNodeData->strName));
			 m_selNode.SetObjGroup(altGroup);
		 }
		 break;
	 case objNodeType_WaypointLeaf:
	 case objNodeType_Waypoint:
		 {
			 m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
			 m_selNode.SetNodeName(_T("Waypoint:") + pNodeData->strName);
			 ALTObjectGroup altGroup;
			 altGroup.setName(ALTObjectID(pNodeData->strName));
			 //altGroup.setType()
			 m_selNode.SetObjGroup(altGroup);
		 }
		 break;
	 case objNodeType_InterNodeRoot:
		 {
			 m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_Taxiway);
			 m_selNode.SetNodeName(_T("IntNode:All"));
		 }
		 break;
	case objNodeType_TaxiwayLeaf:
	case objNodeType_Taxiway:
		{
			m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_Taxiway);
			m_selNode.SetNodeName(_T("IntNode:") + pNodeData->strName);
			ALTObjectGroup altGroup;
			altGroup.setName(ALTObjectID(pNodeData->strName));
			altGroup.SetProjID(m_nProjectID) ;
			altGroup.setType(ALT_TAXIWAY) ;
			m_selNode.SetObjGroup(altGroup);
		}
		break;
	case objNodeType_InterNode:
		{
			m_selNode.SetNodeType(CAirsideReportNode::ReportNodeType_Intersection);
			m_selNode.SetNodeName( pNodeData->strName);
			m_selNode.SetNodeID(pNodeData->nNodeID) ;
		}
		break;
	 default://not valid
		 {
			 return;
		 }
		 break;
	 }
	 std::vector<int > vObjID;
	 GetObjNodes(vObjID, hSelItem);
	 m_selNode.SetObjIDList(vObjID);
	

	CDialog::OnOK();
}

void CDlgSelectAirsideNode::GetObjNodes( std::vector<int>& vObjID,HTREEITEM hCurItem )
{
	HTREEITEM hChildItem = m_wndALTObjectTree.GetChildItem(hCurItem);
	if(hChildItem == NULL)//node
	{
		nodeData *pNodeData = (nodeData *)m_wndALTObjectTree.GetItemData(hCurItem);
		if(pNodeData != NULL)
		{
			vObjID.push_back(pNodeData->nNodeID);
		}
	}

	while (hChildItem != NULL)
	{
		SearchChildItem( vObjID,hChildItem);
		
		//next child
		hChildItem = m_wndALTObjectTree.GetNextSiblingItem(hChildItem);
	}
}

void CDlgSelectAirsideNode::SearchChildItem( std::vector<int>& vObjID, HTREEITEM hSibingItem)
{
	HTREEITEM hChildItem = m_wndALTObjectTree.GetChildItem(hSibingItem);
	if(hChildItem == NULL)//node
	{
		nodeData *pNodeData = (nodeData *)m_wndALTObjectTree.GetItemData(hSibingItem);
		if(pNodeData != NULL)
		{
			vObjID.push_back(pNodeData->nNodeID);
		}
	}
	
	while(hChildItem != NULL)
	{
		SearchChildItem(vObjID,hChildItem);
		
		hChildItem = m_wndALTObjectTree.GetNextSiblingItem(hChildItem);
	}

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CDlgSelectAirsideNodeByType::CDlgSelectAirsideNodeByType(int nProjID,Airsdie_Resource_ID _ResID , CWnd* pParent /* = NULL */)
:m_ResType(_ResID),CDlgSelectAirsideNode(nProjID,pParent)
{

}
CDlgSelectAirsideNodeByType::~CDlgSelectAirsideNodeByType()
{

}
BOOL CDlgSelectAirsideNodeByType::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	SetWindowText(_T("Select Airside Resource"));

	m_wndALTObjectTree.ModifyStyle(NULL,WS_VSCROLL);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjectID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_wndALTObjectTree.InsertItem(airport.getName());
		m_wndALTObjectTree.SetItemData(hAirport,(DWORD_PTR)new nodeData(objNodeType_Airport));
		InitTreeContentsForAirport(hAirport, *iterAirportID);
		m_wndALTObjectTree.Expand(hAirport,TVE_EXPAND) ;
	}
	return TRUE;
}
void CDlgSelectAirsideNodeByType::InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID)
{
	HTREEITEM hStandRootItem = hAirport;
	m_wndALTObjectTree.SetItemData(hStandRootItem,(DWORD_PTR)new nodeData(objNodeType_StandRoot));
	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_STAND, nAirportID, vObject);
	ALTObjectList vObject;
	ALTAirport::GetAirsideResourceObject(nAirportID,vObject,m_ResType);

	int nObjectCount = vObject.size();
	for (int i = 0; i < nObjectCount; ++i)
	{
		ALTObject* altObject = vObject.at(i).get();
		ALTObjectID objName;
		altObject->getObjName(objName);

		HTREEITEM hParentItem = hStandRootItem;
		for (int nLevel = 0; nLevel < OBJECT_STRING_LEVEL; ++nLevel)
		{
			if (objName.m_val[nLevel].empty())
				break;

			HTREEITEM hNewItem = FindTreeChildItemByName(objName.m_val[nLevel].c_str(), hParentItem);
			if (hNewItem == NULL)
			{
				hNewItem = m_wndALTObjectTree.InsertItem(objName.m_val[nLevel].c_str(), hParentItem);

				if ((nLevel + 1 == OBJECT_STRING_LEVEL) ||(nLevel + 1 != OBJECT_STRING_LEVEL && objName.m_val[nLevel +1].empty() ))
				{
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_Stand, altObject->GetObjNameString(nLevel),altObject->getID()));
				}
				else
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_StandLeaf, altObject->GetObjNameString(nLevel)));

				//m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
				m_wndALTObjectTree.Expand(hNewItem,TVE_EXPAND) ;
			}

			hParentItem = hNewItem;
		}
	}
	m_wndALTObjectTree.Expand(hAirport, TVE_EXPAND);
}
void CDlgSelectAirsideNodeByType::OnOK()
{
	HTREEITEM hSelItem = m_wndALTObjectTree.GetSelectedItem();

	if (hSelItem == NULL)
		return;
	//check the selected item
	nodeData *pNodeData = (nodeData *)m_wndALTObjectTree.GetItemData(hSelItem);
	if(pNodeData == NULL)
		return;
	m_selNode.SetNodeName( pNodeData->strName);
	CDialog::OnOK() ;
}

CDlgTaxiWaySelect::CDlgTaxiWaySelect( int nProjID, CWnd* pParent /*= NULL*/ ):CDlgSelectAirsideNode(nProjID,pParent)
{

}

BOOL CDlgTaxiWaySelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Select Taxiway Object"));

	m_wndALTObjectTree.ModifyStyle(NULL,WS_VSCROLL);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjectID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_wndALTObjectTree.InsertItem(airport.getName());
		m_wndALTObjectTree.SetItemData(hAirport,(DWORD_PTR)new nodeData(objNodeType_Airport));
		InitIntersectionNodeToAirport(hAirport, *iterAirportID);
		m_wndALTObjectTree.Expand(hAirport,TVE_EXPAND) ;
	}
	return TRUE;
}