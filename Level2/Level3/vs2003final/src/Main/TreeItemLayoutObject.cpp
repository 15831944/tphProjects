#include "StdAfx.h"
#include ".\treeitemlayoutobject.h"
#include "common/ILayoutObject.h"
#include "landside\LandsideLayoutObject.h"
#include "resource.h"
#include "tvnode.h"
#include "../MFCExControl/ARCBaseTree.h"
#include "Floor2.h"
#include "Common/ALT_TypeString.h"
using namespace MSV;
bool CTreeItemLayoutObjectRoot::AddFcObject( LandsideFacilityLayoutObject* pObj )
{
	const ALTObjectID& objID = pObj->getName();	
	int idLength = objID.idLength();

	CTreeItemLayoutObjectGroup paretItem = *this;
	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{		
		CString strLevel = objID.m_val[nLevel].c_str();		
		strLevel.MakeUpper();
		if(strLevel.IsEmpty())
			break;

		bool bIsEndLevel = (nLevel>=idLength-1);

		if( bIsEndLevel )
		{
			CTreeItemMSVNode nodeItem = paretItem.AddObjectItem(strLevel);
			if(nodeItem.getNodeType()!=NodeType_Object)
				return false;
			else
			{
				CTreeItemLayoutObject objItem = nodeItem;		
				objItem.setObject(pObj);
				return true;
			}
		}
		else
		{
			CTreeItemMSVNode nodeItem = paretItem.AddGroupItem(strLevel);
			if(nodeItem.getNodeType()!=NodeType_ObjectGroup)
				return false;
			else
			{
				paretItem = nodeItem;
				continue;
			}
		}		
	}
	return false;
}

bool CTreeItemLayoutObjectRoot::AddObject( LandsideLayoutObject* pObj )
{
	if(LandsideFacilityLayoutObject* fcObj = pObj->toFacilityObject() )
	{
		return AddFcObject(fcObj);
	}
	else if(pObj->GetType() == ALT_LPORTAL )//add object in list
	{
		if(CTreeItemLayoutObject item = AddPortalItem(pObj->GetNameString()) )
		{
			item.setObject(pObj);
			return true;
		}
	}
	else if(pObj->GetType() == ALT_LAREA )
	{
		if(CTreeItemLayoutObject item = AddAreaItem(pObj->GetNameString()) )
		{
			item.setObject(pObj);
			return true;
		}
	}

	return false;
}

MSV::enumNodeType CTreeItemMSVNode::getNodeType()const
{
	CNodeData* pdata = (CNodeData*)GetData();
	if(pdata)
		return pdata->nodeType;
	return NodeType_Normal;
}

MSV::CNodeData* CTreeItemMSVNode::InitNodeData( MSV::enumNodeType eType ,CNodeDataAllocator* pallocator)
{
	CNodeData* pData = pallocator->allocateNodeData(eType);	
	SetData((DWORD)pData);	
	return pData;
}

void CTreeItemMSVNode::SetResourceID( UINT idRes )
{
	if(CNodeData* pData = getNodeData()){
		pData->nIDResource = idRes;
	}
	else{
		ASSERT(FALSE);
	}	
}

bool CTreeItemMSVNode::GetLayoutObjectList( LandsideFacilityLayoutObjectList& layoutList ) 
{
	return false;
}

MSV::CNodeData* CTreeItemMSVNode::createNodeData(MSV::enumNodeType eType)
{
	if(CNodeData* pNodeData = getNodeData()){
		return pNodeData->mpCreator->allocateNodeData(eType);
	}
	return NULL;
}



CTreeItemMSVNode CTreeItemLayoutObjectGroup::AddGroupItem( const CString& str )
{
	ASSERT(getNodeData());
	bool bInsert = false;
	HTREEITEM insertPos = FindInsertPos(str, bInsert);
	if(bInsert)
	{
		CTreeItemMSVNode childNode = AddChild(str,insertPos);
		childNode.InitNodeData(NodeType_ObjectGroup,getNodeData()->mpCreator);	
		childNode.SetResourceID(IDR_CTX_LANDPROCESSORGROUP);
		return childNode;
	}	
	return CTreeItemMSVNode(m_ctrl,insertPos);
}

CTreeItemMSVNode CTreeItemLayoutObjectGroup::AddObjectItem( const CString& str )
{
	ASSERT(getNodeData());
	bool bInsert = false;
	HTREEITEM insertPos = FindInsertPos(str, bInsert);	
	if(bInsert)
	{
		CTreeItemMSVNode childNode = AddChild(str,insertPos);
		childNode.InitNodeData(NodeType_Object,getNodeData()->mpCreator);
		childNode.SetResourceID(IDR_CTX_LANDPROCESSOR);
		return childNode;
	}
	return CTreeItemMSVNode(m_ctrl,insertPos);
}

CTreeItemLayoutObject CTreeItemLayoutObjectGroup::FindObjectItem( LandsideLayoutObject* pObj )
{
	CTreeItemMSVNode child = GetFirstChild();
	while(child)
	{
		if(child.getNodeType() == NodeType_ObjectGroup)
		{
			CTreeItemLayoutObjectGroup groupNode= child;
			if( CTreeItemLayoutObject ret = groupNode.FindObjectItem(pObj) )
				return ret;
		}
		else if(child.getNodeType() == NodeType_Object)
		{
			CTreeItemLayoutObject objNode= child;
			if(objNode.getObject() == pObj)
				return objNode;
		}
		child = child.GetNextSibling();
	}
	return child;
}

HTREEITEM CTreeItemLayoutObjectGroup::FindInsertPos( const CString& str,bool& bInsert )
{	
	CTreeItemMSVNode child = GetFirstChild();	
	HTREEITEM hPre = TVI_FIRST;
	while(child)
	{
		if( child.GetText() == str )
		{
			bInsert = false;
			return child.GeHItem();
		}
		if(child.GetText()>str)
		{
			break;
		}
		hPre = child.GeHItem();
		child = child.GetNextSibling();
	}	
	bInsert = true;
	return hPre;
}

bool CTreeItemLayoutObjectGroup::TravelTreeNode(CTreeItemLayoutObject hItem,LandsideFacilityLayoutObjectList& layoutList)
{
	CTreeItemLayoutObject child = hItem.GetFirstChild();
	if (child)
	{
		TravelTreeNode(child,layoutList);
	}
	else
	{
		LandsideFacilityLayoutObjectList curLayoutList;
		if(hItem.GetLayoutObjectList(curLayoutList))
		{
			for (int i = 0; i < curLayoutList.getCount(); i++)
			{
				LandsideFacilityLayoutObject* pLayoutObject = curLayoutList.getObject(i);
				layoutList.AddObject(pLayoutObject);
			}
		}
	}
	hItem = hItem.GetNextSibling();
	if (hItem)
	{
		TravelTreeNode(hItem,layoutList);
	}
	return true;
}
bool CTreeItemLayoutObjectGroup::GetLayoutObjectList( LandsideFacilityLayoutObjectList& layoutList )
{
	CTreeItemLayoutObject child = GetFirstChild();	
	HTREEITEM hPre = TVI_FIRST;
	while(child)
	{
		TravelTreeNode(child,layoutList);
		child = child.GetNextSibling();
	}

	if (layoutList.getCount() == 0)
	{
		return false;
	}

	return true;
}



bool CTreeItemLayoutObjectRoot::RemoveObject( LandsideLayoutObject* pObj )
{
	CTreeItemMSVNode item = FindObjectItem(pObj);
	if(!item)
		return false;
	_RemoveObjItem(item);
	return true;
}



void CTreeItemLayoutObjectRoot::_RemoveObjItem( CTreeItemMSVNode& objItem )
{
	if(objItem.GeHItem() == m_hItem )
		return;

	CTreeItemMSVNode parent = objItem.GetParent();
	if(!objItem.GetFirstChild())
		objItem.Destroy();
	
	_RemoveObjItem(parent);	
}

void CTreeItemLayoutObjectRoot::AddObjectList( ILandsideLayoutObjectList & objlist )
{
	for(int i=0;i<objlist.getCount();i++)
	{
		LandsideLayoutObject* pObj = objlist.getLayoutObject(i);
		if(pObj->GetType() == getObjectType())
		{
			AddObject(pObj);
		}
	}
}

ALTOBJECT_TYPE CTreeItemLayoutObjectRoot::getObjectType()
{
	CNodeData* pNodeData = (CNodeData*)GetData();
	return	(ALTOBJECT_TYPE)pNodeData->nSubType;
}

void CTreeItemLayoutObjectRoot::setObjectType( ALTOBJECT_TYPE type )
{
	CNodeData* pNodeData = (CNodeData*)GetData();
	pNodeData->nSubType = type;
}

//landside_type


CString CTreeItemLayoutObjectRoot::GetMenuAddCaption()
{
	ALTOBJECT_TYPE objtype = getObjectType();
	if(objtype<ALT_LTYPE_END && objtype>=ALT_LTYPE_START)
	{
		return _T("Add New ") + ALT_TypeString::Get(objtype);
	}
	return "";
}

CTreeItemMSVNode CTreeItemLayoutObjectRoot::AddPortalItem( const CString& str )
{
	ASSERT(getNodeData());
	bool bInsert = false;
	HTREEITEM insertPos = FindInsertPos(str, bInsert);	
	if(bInsert)
	{
		CTreeItemMSVNode childNode = AddChild(str,insertPos);
		childNode.InitNodeData(NodeType_Object,getNodeData()->mpCreator);
		childNode.SetResourceID(IDR_CTX_PORTAL);
		return childNode;
	}
	return CTreeItemMSVNode(m_ctrl,insertPos);
}

CTreeItemMSVNode CTreeItemLayoutObjectRoot::AddAreaItem( const CString& str )
{
	ASSERT(getNodeData());
	bool bInsert = false;
	HTREEITEM insertPos = FindInsertPos(str, bInsert);	
	if(bInsert)
	{
		CTreeItemMSVNode childNode = AddChild(str,insertPos);
		childNode.InitNodeData(NodeType_Object,getNodeData()->mpCreator);
		childNode.SetResourceID(IDR_CTX_LANDSIDE_AREA);
		return childNode;
	}
	return CTreeItemMSVNode(m_ctrl,insertPos);
}


CTreeItemLayoutObjectRoot CTreeItemLayout::FindObjectRoot( ALTOBJECT_TYPE type )
{
	CTreeItemMSVNode child = GetFirstChild();
	while(child)
	{
		if( child.getNodeType() == NodeType_ObjectRoot )
		{
			CTreeItemLayoutObjectRoot objRoot = child;
			if(objRoot.getObjectType() == type)
				return objRoot;
		}
		else
		{
			CTreeItemMSVNode childchild = child.GetFirstChild();
			while(childchild)
			{
				if(childchild.getNodeType() == NodeType_ObjectRoot)
				{
					CTreeItemLayoutObjectRoot objRoot = childchild;
					if(objRoot.getObjectType() == type)
						return objRoot;
				}
				childchild = childchild.GetNextSibling();
			}
		}

		child = child.GetNextSibling();
	}
	return child;
}



bool CTreeItemLayout::RemoveLayoutObject( LandsideLayoutObject* pObj )
{
	if( CTreeItemLayoutObjectRoot objRoot = FindObjectRoot(pObj->GetType()) )
	{	
		return objRoot.RemoveObject(pObj);	
	}
	return false;
}

bool CTreeItemLayout::AddLayoutObject( LandsideLayoutObject* pObj )
{
	if( CTreeItemLayoutObjectRoot objRoot = FindObjectRoot(pObj->GetType()) )
	{
		return objRoot.AddObject(pObj);
	}
	return false;
}

bool CTreeItemLayout::ReNameLayoutObject( LandsideLayoutObject* pObj )
{
	return RemoveLayoutObject(pObj) && AddLayoutObject(pObj);
}




//////////////////////////////////////////////////////////////////////////
void CLevelTreeItem::SetFloorIndex( int idx )
{
	CNodeData* pLevelNode = (CNodeData*) GetData();
	CTVNode* pTVNode = (CTVNode*)pLevelNode->nOtherData;
	pTVNode->m_dwData = (DWORD)idx;		
	pLevelNode->dwData = (DWORD) idx;
}

int CLevelTreeItem::GetFloorIndex()
{
	CNodeData* pLevelNode = (CNodeData*) GetData();
	return (int)pLevelNode->dwData;
}

void CLevelTreeItem::SetActive( BOOL b )
{
	int iActive  = ID_NODEIMG_FLOORACTIVE+ID_IMAGE_COUNT;
	int inActive = ID_NODEIMG_FLOORNOTACTIVE + ID_IMAGE_COUNT;
	if(b){
		SetImage(iActive,iActive);
	}
	else
		SetImage(inActive,inActive);
}

bool CLevelTreeItem::IsValid() const
{
	if(!__super::IsValid())
		return false;
	return getNodeType()==NodeType_Level;
}

CLevelTreeItem CLevelRootTreeItem::AddLevelItem( CFloor2* pLevel )
{
	int idx = pLevel->Level();
		
	CNodeData* pLevelNode = createNodeData(NodeType_Level);
	CTVNode* pTVNode = new CTVNode(pLevel->FloorName(), IDR_CTX_LEVEL);
	pTVNode->m_dwData = (DWORD)idx;

	pLevelNode->nIDResource = IDR_CTX_LEVEL;
	pLevelNode->dwData = (DWORD) idx;		
	pLevelNode->nOtherData = (int)pTVNode;

	CLevelTreeItem child = AddChild(pLevel->FloorName(),TVI_FIRST);
	child.SetData((DWORD)pLevelNode);
	child.SetActive(pLevel->IsActive());

	return child;
}

void CLevelRootTreeItem::RemoveLevelItem( int nFloor )
{
	CLevelTreeItem childItem = GetFirstChild();
	while(childItem)
	{
		if( nFloor == childItem.GetFloorIndex() )
		{
			childItem.Destroy();
			return;
		}			
		childItem = childItem.GetNextSibling();
	}
}



bool CLevelRootTreeItem::IsValid() const
{
	if(!__super::IsValid())
		return false;
	return getNodeType() == NodeType_LevelRoot;
}
#include "floors.h"
void CLevelRootTreeItem::AddLevelList( CFloors& levelList )
{
	int nCount = levelList.GetCount();
	for (int i=0;i<nCount;i++ )
	{
		AddLevelItem(levelList.GetFloor2(i));
	}
}


void CLevelRootTreeItem::Init( MSV::CNodeDataAllocator* pNodedataAlc )
{
	CNodeData* pData  = InitNodeData(NodeType_LevelRoot,pNodedataAlc);
	CTVNode* pTVNode = new CTVNode( GetText(), IDR_CTX_LEVEL);
	pTVNode->m_dwData = (DWORD)pTVNode;
	pData->nOtherData = (int)pTVNode;
	SetResourceID(IDR_CTX_LEVELS);
}

LandsideLayoutObject* CTreeItemLayoutObject::getObject()
{
	if(getNodeType()==NodeType_Object)
	{
		if(CNodeData* pNodeData = getNodeData() )
		{
			return (LandsideLayoutObject*)pNodeData->nOtherData;
		}
	}
	return NULL;	
}

void CTreeItemLayoutObject::setObject( LandsideLayoutObject* pobj )
{
	CNodeData* pNodeData = getNodeData();
	if(pNodeData)
		pNodeData->nOtherData = (int)pobj;
}

bool CTreeItemLayoutObject::GetLayoutObjectList( LandsideFacilityLayoutObjectList& layoutList ) 
{
	LandsideFacilityLayoutObject* pLandsideObject = (LandsideFacilityLayoutObject*)getObject();
	if (pLandsideObject)
	{
		layoutList.AddObject(pLandsideObject);
	}
	
	if (layoutList.getCount() == 0)
	{
		return false;
	}

	return true;
}

bool CTreeItemLayoutObject::IsValid() const
{
	return __super::IsValid() && getNodeType()== NodeType_Object;
}
