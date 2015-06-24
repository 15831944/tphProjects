#include "StdAfx.h"
#include ".\treectrlitem.h"
#include "../MFCExControl/ARCBaseTree.h"

CTreeCtrlItem::CTreeCtrlItem( CARCBaseTree* ctrl,HTREEITEM hItem ) 
:m_ctrl(ctrl)
{
	m_hItem = hItem;
}

void CTreeCtrlItem::RemoveAllChild()
{
	std::vector<HTREEITEM> vToDel;
	HTREEITEM hChild = GetCtrl().GetChildItem(m_hItem);
	while(hChild)
	{
		hChild = GetCtrl().GetNextSiblingItem(hChild);
	}
	for(size_t i=0;i<vToDel.size();++i)
	{
		GetCtrl().DeleteItem(vToDel[i]);
	}
}

CTreeCtrlItem CTreeCtrlItem::GetFirstChild()
{
	return CTreeCtrlItem(m_ctrl,GetCtrl().GetChildItem(m_hItem));
}

CTreeCtrlItem CTreeCtrlItem::GetNextSibling()
{
	return CTreeCtrlItem(m_ctrl,GetCtrl().GetNextSiblingItem(m_hItem));
}

void CTreeCtrlItem::Destroy()
{
	GetCtrl().DeleteItem(m_hItem);
	m_hItem = NULL;
}

void CTreeCtrlItem::SetImage( int nImage, int nSelImage )
{
	GetCtrl().SetItemImage(m_hItem,nImage,nSelImage);
}

CString CTreeCtrlItem::GetText() const
{
	return	GetCtrl().GetItemText(m_hItem);	
}

void CTreeCtrlItem::SetText( const CString& text )
{
	GetCtrl().SetItemText(m_hItem,(LPCTSTR)text);
}

void CTreeCtrlItem::SetData( DWORD data )
{
	GetCtrl().SetItemData(m_hItem,data);
}

DWORD CTreeCtrlItem::GetData()const
{
	return m_ctrl->GetItemData(m_hItem);
}

CTreeCtrlItem CTreeCtrlItem::AddChild( const CString& strNode,HTREEITEM HAfter ,int nImage ,int nSelectImage )
{
	COOLTREE_NODE_INFO cni;		
	CARCBaseTree::InitNodeInfo(GetCtrl().GetParent(),cni);
	cni.nImage = nImage + ID_IMAGE_COUNT;
	cni.nImageSeled = nSelectImage + ID_IMAGE_COUNT;
	HTREEITEM hChild =  GetCtrl().InsertItem(strNode,cni,FALSE,FALSE,m_hItem,HAfter);
	return CTreeCtrlItem(m_ctrl,hChild);
}

CTreeCtrlItem CTreeCtrlItem::AddChild( int StringID,HTREEITEM HAfter ,int nImage ,int nSelectImage)
{
	CString s;
	s.LoadString(StringID);
	return AddChild(s,HAfter);
}

void CTreeCtrlItem::Expand()
{
	GetCtrl().Expand(m_hItem,TVE_EXPAND);
}

CTreeCtrlItem CTreeCtrlItem::GetParent()const
{
	return CTreeCtrlItem(m_ctrl,m_ctrl->GetParentItem(m_hItem));
}