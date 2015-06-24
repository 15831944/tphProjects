#include "StdAfx.h"
#include ".\landsideintersectionpropdlgimpl.h"
#include "resource.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include "Landside/InputLandside.h"

//////////////////////////////////////////////////////////////////////////
#include "Landside/LandsideIntersectionNode.h"


CString LandsideIntersectionPropDlgImpl::getTitle()
{
	/*if(m_pObject->getID()<0)
		return "New Landside Intersection";
	else
		return "Edit Landside Intersection";*/
	return _T("Landside Intersection Specification");
}

void LandsideIntersectionPropDlgImpl::LoadTree()
{
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	isInInitTree = true;
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);


	CString strLabel = _T("");

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)m_pObject;

	//type of node.
	CString nodetype = LandsideIntersectionNode::GetNodeTypeString(pNode->getNodeType());
	strLabel.Format(_T("Type of Lanes : %s"), nodetype.GetString() );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 

	isscStringColor.strSection = nodetype;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hNodeType = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hNodeType,aoidDataEx);	

	// control points.
	strLabel = CString( "Lane Linkages: ");
	
	m_hLaneLinkages = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemLaneLinkages(m_hLaneLinkages, pNode);	
	m_treeProp.Expand( m_hLaneLinkages, TVE_EXPAND );
	

	m_treeProp.SetRedraw(1);
	isInInitTree = false;
}

FallbackReason LandsideIntersectionPropDlgImpl::GetFallBackReason()
{
	return LINK_STRETCH;
}


#define LINKAGE_TAG _T("Linkage")
#define GROUP_ID _T("Concurrent flow group name:")
class LinkStretchListItem  : TreeCtrlItemInDlg //load tree
{
public:

};


void LandsideIntersectionPropDlgImpl::LoadTreeSubItemLaneLinkages( HTREEITEM preItem, LandsideIntersectionNode* pNode )
{
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;	
	HTREEITEM hItemTemp = 0;
	m_hLinkageItems.clear();
	m_hCrossOverItems.clear();

	int iPathCount = pNode->getLinageCount();
	for(int i = 0;i < iPathCount; i++)
	{
		LaneLinkage& linkage = pNode->getLinkage(i);
		CString strTmp;
		strTmp.Format("%s, %d",LINKAGE_TAG, i+1);
		HTREEITEM hLink = m_treeProp.InsertItem(strTmp,preItem);
		m_hLinkageItems.push_back(hLink);

		//add from

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		isscStringColor.colorSection = RGB(0,0,255);
	
		aoidDataEx.vrItemStringSectionColorShow.clear(); 
		CString strFromStretch = linkage.mLaneExit.m_pStretch->getName().GetIDString();
		CString strLaneIdx; strLaneIdx.Format("%d",linkage.mLaneExit.nLaneIdx + 1);
		CString strFrom;
		strFrom.Format("From:%s (Lane Index %d)",strFromStretch.GetString(),linkage.mLaneExit.nLaneIdx+1);

		isscStringColor.strSection = strFromStretch;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		isscStringColor.strSection = strLaneIdx;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		HTREEITEM hFromItem = m_treeProp.InsertItem(strFrom,hLink);
		m_treeProp.SetItemDataEx(hFromItem,aoidDataEx);

	

		aoidDataEx.vrItemStringSectionColorShow.clear();
		CString strToStretch = linkage.mLaneEntry.m_pStretch->getName().GetIDString();
		strLaneIdx.Format("%d",linkage.mLaneEntry.nLaneIdx + 1);
		CString strTo;
		strTo.Format("To:%s (Lane Index %d)",strToStretch.GetString(),linkage.mLaneEntry.nLaneIdx+1);

		isscStringColor.strSection = strToStretch;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		isscStringColor.strSection = strTo;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		HTREEITEM hToItem = m_treeProp.InsertItem(strTo,hLink);
		m_treeProp.SetItemDataEx(hToItem,aoidDataEx);
		LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)m_pObject;
		if(pNode->getNodeType() == LandsideIntersectionNode::NodeType::_Signalized)
		{
			AirsideObjectTreeCtrlItemDataEx aoidDataEx;

			CString strID;
			//strGroupID.Format(_T("%d"),linkage.getGroupID());
			strID=GROUP_ID+linkage.getGroupName();
			HTREEITEM item = m_treeProp.InsertItem(strID,hLink);
			m_treeProp.SetItemText(item, strID);

			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = (DWORD)&linkage;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255); 

			isscStringColor.strSection = linkage.getGroupName();
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			m_treeProp.SetItemDataEx(item, aoidDataEx);
			//unsigned int v = (unsigned int)i;
			//m_treeProp.SetItemData(hLink, (v << 16) + (unsigned int)linkage.getGroupID());

			CString strCrossOver = "Crossover light:";
			if(linkage.isCrossOver)
				strCrossOver += "Yes";
			else
				strCrossOver += "No";
			HTREEITEM hCrossOver = m_treeProp.InsertItem(strCrossOver,hLink);
			m_hCrossOverItems.push_back(hCrossOver);
			m_treeProp.SetItemText(hCrossOver, strCrossOver);

			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = (DWORD)&linkage;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255); 
			if(linkage.isCrossOver)
				isscStringColor.strSection= "Yes";
			else
				isscStringColor.strSection= "No";
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			m_treeProp.SetItemDataEx(hCrossOver, aoidDataEx);
		}
	}
}

LandsideIntersectionPropDlgImpl::LandsideIntersectionPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)

{
	isInInitTree = true;
}

BOOL LandsideIntersectionPropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)m_pObject;
	if (message == WM_INPLACE_COMBO2)
	{	
		CString strText;
		CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
		m_treeProp.m_comboBox.GetWindowText(strText);
		int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
		if (m_hLClickItem == m_hNodeType)
		{
			if(selitem >=0)
			{
				pNode->setNodeType((LandsideIntersectionNode::NodeType)selitem);
				LoadTree();
				Update3D();
			}
		}
		std::vector<HTREEITEM>::iterator it;
		it = std::find(m_hCrossOverItems.begin(),m_hCrossOverItems.end(),m_hRClickItem);
		if (it != m_hCrossOverItems.end()&&!strText.IsEmpty())
		{
			LandsideIntersectionNode* pIntersection = getIntersection();
			int index = it - m_hCrossOverItems.begin();
			if(!strText.Compare("No"))
				pIntersection->getLinkage(index).isCrossOver = false;
			else
				pIntersection->getLinkage(index).isCrossOver = true;
			//pIntersection->setNewGroupName(index,strText);
			AirsideObjectTreeCtrlItemDataEx *dataEx=GetTreeCtrl().GetItemDataEx(m_hRClickItem);

			CString strCrossover;
			strCrossover="Crossover light:"+strText;
			ItemStringSectionColor isscStringColor;	
			dataEx->vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255); 
			isscStringColor.strSection = strText;
			dataEx->vrItemStringSectionColorShow.push_back(isscStringColor);

			GetTreeCtrl().SetItemText(m_hRClickItem,strCrossover);
			Update3D();

			return TRUE;
		}

		it = std::find(m_hLinkageItems.begin(), m_hLinkageItems.end(), GetTreeCtrl().GetParentItem(m_hRClickItem));
		if (it != m_hLinkageItems.end()&&!strText.IsEmpty())
		{
			LandsideIntersectionNode* pIntersection = getIntersection();
			int index = it - m_hLinkageItems.begin();
			pIntersection->setNewGroupName(index,strText);
			AirsideObjectTreeCtrlItemDataEx *dataEx=GetTreeCtrl().GetItemDataEx(m_hRClickItem);

			CString strID;
			strID=GROUP_ID+strText;
			ItemStringSectionColor isscStringColor;	
			dataEx->vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255); 
			isscStringColor.strSection = strText;
			dataEx->vrItemStringSectionColorShow.push_back(isscStringColor);

			GetTreeCtrl().SetItemText(m_hRClickItem,strID);
			Update3D();
		}
			
	}

	return TRUE;
}

void LandsideIntersectionPropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;	
	if(hTreeItem == m_hNodeType)
	{
		OnNodeTypeCombo();
// 		LoadTree();
// 		Update3D();
		return;
	}
	AirsideObjectTreeCtrlItemDataEx *dataEx=GetTreeCtrl().GetItemDataEx(m_hRClickItem);
	if (dataEx==NULL)
	{
		return;
	}
	std::vector<HTREEITEM>::iterator it;
	it = std::find(m_hCrossOverItems.begin(),m_hCrossOverItems.end(),m_hRClickItem);
	if (it != m_hCrossOverItems.end())
	{
		OnEditCrossOver(hTreeItem);
		return;
	}

	it = std::find(m_hLinkageItems.begin(), m_hLinkageItems.end(), GetTreeCtrl().GetParentItem(m_hRClickItem));
	if (it != m_hLinkageItems.end())
	{
		OnEditGroupID(hTreeItem);
	}
}

//void LandsideIntersectionPropDlgImpl::OnLButtonDownPropTree(HTREEITEM hTreeItem)
//{
//	m_hLClickItem = hTreeItem;
//	GetTreeCtrl().GetItemData(m_hLClickItem);
//
//		CString strLink = GetTreeCtrl().GetItemText(m_hLClickItem);
//		int taglen = strlen(LINKAGE_TAG) + 1;
//		std::string temp(strLink.GetString());
//		int idx = atoi(temp.substr(taglen, temp.length()-taglen).c_str());
//		
//		LandsideIntersectionNode* pIntersection = getIntersection();
//		pIntersection->getLinkage(idx).isSelected = true;
//}

void LandsideIntersectionPropDlgImpl::OnSelChangedPropTree(HTREEITEM hTreeItem)
{
	m_hLClickItem = hTreeItem;
	if(isInInitTree) return;
	LandsideIntersectionNode* pIntersection = getIntersection();
	if( std::find(m_hLinkageItems.begin(),m_hLinkageItems.end(),m_hLClickItem)==m_hLinkageItems.end()) {
		for (int i = 0; i < pIntersection->getLinageCount(); i++)
			pIntersection->getLinkage(i).isSelected = false;

		Update3D();
		return;
	}

	CString strLink = GetTreeCtrl().GetItemText(m_hLClickItem);
	int taglen = strlen(LINKAGE_TAG) + 1;
	std::string temp(strLink.GetString());
	int idx = atoi(temp.substr(taglen, temp.length()-taglen).c_str()) - 1;
	
	for (int i = 0; i < pIntersection->getLinageCount(); i++)
		pIntersection->getLinkage(i).isSelected = false;
	pIntersection->getLinkage(idx).isSelected = true;

	Update3D();
}

void LandsideIntersectionPropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hLaneLinkages)
	{
		pMenu = menuPopup.GetSubMenu(73);
	}
	else if(std::find(m_hLinkageItems.begin(),m_hLinkageItems.end(),m_hRClickItem) != m_hLinkageItems.end())
	{
		pMenu = menuPopup.GetSubMenu(100);
		
	}
	
}

void LandsideIntersectionPropDlgImpl::OnNodeTypeCombo()
{
	std::vector<CString> vString;
	vString.push_back(LandsideIntersectionNode::GetNodeTypeString(LandsideIntersectionNode::_Signalized));
	vString.push_back(LandsideIntersectionNode::GetNodeTypeString(LandsideIntersectionNode::_Unsignalized));
	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(m_hNodeType,vString);	
}

void LandsideIntersectionPropDlgImpl::OnEditGroupID(HTREEITEM& item)
{
	LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)m_pObject;
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	if(!pNode) return;

	int dMaxID = 9;

//	AirsideObjectTreeCtrlItemDataEx *dataEx=m_treeProp.GetItemDataEx(item);
//	LaneLinkage *linkage = (LaneLinkage *)dataEx->dwptrItemData;
////	int groupID = linkage->getGroupID();
//
//	CString strGroupName = linkage->getGroupName();
//	std::vector<CString> vPhase;
//
//	pNode->GetGroupNameList(vPhase);
//
//	int nCurSel = -1;
//	std::vector<CString>::iterator iter = std::find(vPhase.begin(),vPhase.end(),strGroupName);
//	if (iter != vPhase.end())
//	{
//		nCurSel = iter - vPhase.begin();
//	}
	
	//retrieve all intersection node
	std::vector<LandsideFacilityLayoutObject*> vList;
	std::vector<CString> vGroupName;
	m_pParentDlg->getInputLandside()->getObjectList().GetObjectList(vList,ALT_LINTERSECTION);
	for (int i = 0; i < (int)vList.size(); i++)
	{
		LandsideFacilityLayoutObject* pObject = vList.at(i);
		LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)pObject;
		std::vector<CString> vNodeName;
		if(pNode->GetGroupNameList(vNodeName))
		{
			for (int j = 0; j < (int)vNodeName.size(); j++)
			{
				CString strGroupName = vNodeName.at(j);
				if (std::find(vGroupName.begin(),vGroupName.end(),strGroupName) == vGroupName.end())
				{
					vGroupName.push_back(strGroupName);
				}
			}
		}
	}
	m_treeProp.SetComboWidth(200);
	m_treeProp.SetEditableComboString(item,vGroupName);
//	GetTreeCtrl().m_comboBox.SetCurSel(nCurSel);
}

void LandsideIntersectionPropDlgImpl::OnEditCrossOver(HTREEITEM& item)
{
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	std::vector<CString> vCrossStyle;
	vCrossStyle.insert(vCrossStyle.begin(),"No");
	vCrossStyle.insert(vCrossStyle.begin()+1,"Yes");
	m_treeProp.SetComboWidth(200);
	m_treeProp.SetEditableComboString(item,vCrossStyle);
}

void LandsideIntersectionPropDlgImpl::OnPropDelete()
{
	LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)m_pObject;
	std::vector<HTREEITEM>::iterator itrFind = std::find(m_hLinkageItems.begin(),m_hLinkageItems.end(),m_hRClickItem);
	if(itrFind !=m_hLinkageItems.end())
	{
		int idx = itrFind - m_hLinkageItems.begin();
		pNode->RemoveLinkage(idx);
	}
	LoadTree();
	Update3D();
}

void LandsideIntersectionPropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	LandsideIntersectionNode* pNode = (LandsideIntersectionNode*)m_pObject;
	if(m_hRClickItem==m_hLaneLinkages)
	{
		std::vector<LaneLinkage>* vNewLinkags = (std::vector<LaneLinkage>*)wParam;
		for(size_t i=0;i<vNewLinkags->size();i++)
			pNode->AddLinkage(vNewLinkags->at(i));
		LoadTree();
		pNode->is1stEdit = true;
		Update3D();
	}	
}

int LandsideIntersectionPropDlgImpl::getBitmapResourceID()
{
	return IDB_BITMAP_LANDSIDE_INTERSECTION;
}
