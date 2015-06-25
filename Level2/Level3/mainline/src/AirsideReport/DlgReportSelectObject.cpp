// DlgReportSelectObject.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideReport.h"
#include "DlgReportSelectObject.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "resource.h"
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/IntersectionNodesInAirport.h"


// CDlgReportSelectObject dialog

IMPLEMENT_DYNAMIC(CDlgReportSelectObject, CDialog)
CDlgReportSelectObject::CDlgReportSelectObject(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_REPORT_SELECTALTOBJECT, pParent)
	, m_nProjectID(nProjID)
{
}

CDlgReportSelectObject::~CDlgReportSelectObject()
{
}

void CDlgReportSelectObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ALTOBJECT, m_wndALTObjectTree);
}


BEGIN_MESSAGE_MAP(CDlgReportSelectObject, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJECT, OnSelChangedTree)
END_MESSAGE_MAP()


// CDlgReportSelectObject message handlers
BOOL CDlgReportSelectObject::OnInitDialog()
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
void CDlgReportSelectObject::InitTreeContentsForAirspace()
{	
	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(ALT_WAYPOINT, m_nProjectID, vObject);
	
	HTREEITEM hAirspace = m_wndALTObjectTree.InsertItem(_T("Airspace"));
	m_wndALTObjectTree.SetItemData(hAirspace,(DWORD_PTR)new nodeData(objNodeType_Airspace));


	HTREEITEM hParentItem = m_wndALTObjectTree.InsertItem("Waypoint", hAirspace);
	m_wndALTObjectTree.SetItemData(hParentItem,(DWORD_PTR)new nodeData(objNodeType_WaypointRoot));

	int nObjectCount = vObject.size();
	for (int i = 0; i < nObjectCount; ++i)
	{
		ALTObject& altObject = vObject.at(i);
		ALTObjectID objName;
		altObject.getObjName(objName);

		HTREEITEM hParentItem = hAirspace;
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
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_Waypoint ,altObject.getID()));
				}
				else
					m_wndALTObjectTree.SetItemData(hNewItem,-1);

				//m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
			}

			hParentItem = hNewItem;
		}
	}
	m_wndALTObjectTree.Expand(hAirspace, TVE_EXPAND);
}

void CDlgReportSelectObject::InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID)
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
					m_wndALTObjectTree.SetItemData(hNewItem,(DWORD_PTR)new nodeData(objNodeType_Stand, altObject->getID()));
				}
				else
					m_wndALTObjectTree.SetItemData(hNewItem,-1);

				//m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
			}

			hParentItem = hNewItem;
		}
	}
	m_wndALTObjectTree.Expand(hAirport, TVE_EXPAND);
}

HTREEITEM CDlgReportSelectObject::FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem)
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

void CDlgReportSelectObject::OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	
	HTREEITEM hSelItem = m_wndALTObjectTree.GetSelectedItem();

	if (hSelItem == NULL)
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

		CString strObjName;
		strObjName.Empty();

		do
		{
			if (strObjName.IsEmpty())
				strObjName = m_wndALTObjectTree.GetItemText(hSelItem) + strObjName;
			else
				strObjName = m_wndALTObjectTree.GetItemText(hSelItem) + "-" + strObjName;

			hSelItem = m_wndALTObjectTree.GetParentItem(hSelItem);

		} while(reinterpret_cast<nodeData *>(m_wndALTObjectTree.GetItemData(hSelItem)) == NULL);

		m_selectedALTObjectID.m_val[0] = strObjName;

	}


	GetDlgItem(IDOK)->EnableWindow(bEnableOK);

}
void CDlgReportSelectObject::InitIntersectionNodeToAirport(HTREEITEM hAirport, int nAirportID)
{	
	HTREEITEM hParentItem = m_wndALTObjectTree.InsertItem("Intersection Node", hAirport);
	m_wndALTObjectTree.SetItemData(hParentItem,(DWORD_PTR)new nodeData(objNodeType_InterNodeRoot));
	std::vector<ALTObject> vTaxiway;
	ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vTaxiway);

	IntersectionNodesInAirport lstIntersecNode;
	lstIntersecNode.ReadData(nAirportID);

	for(size_t nTaxiway = 0; nTaxiway < vTaxiway.size(); ++nTaxiway)
	{
		CString strTaxiwayName = vTaxiway[nTaxiway].GetObjNameString();
		HTREEITEM hTaxiwayItem = m_wndALTObjectTree.InsertItem(strTaxiwayName,hParentItem);
		m_wndALTObjectTree.SetItemData(hTaxiwayItem,vTaxiway[nTaxiway].getID());
		for (int nNode =0; nNode < lstIntersecNode.GetCount(); ++nNode)
		{
			if (lstIntersecNode.NodeAt(nNode).HasObject(vTaxiway[nTaxiway].getID()) )
			{
				HTREEITEM hNodeItem = m_wndALTObjectTree.InsertItem(lstIntersecNode.NodeAt(nNode).GetName(),hTaxiwayItem);
				//m_wndALTObjectTree.SetItemData(hNodeItem,lstIntersecNode.NodeAt(nNode).GetID());
				m_wndALTObjectTree.SetItemData(hNodeItem,(DWORD_PTR)new nodeData(objNodeType_InterNode,lstIntersecNode.NodeAt(nNode).GetID()));
			}
		}
	}
}