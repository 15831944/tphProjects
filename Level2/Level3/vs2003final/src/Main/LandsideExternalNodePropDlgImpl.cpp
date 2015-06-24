#include "StdAfx.h"
#include ".\landsideexternalnodepropdlgimpl.h"
#include "resource.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include "Landside/InputLandside.h"
#include "Landside/LandsideRoundabout.h"

//////////////////////////////////////////////////////////////////////////
#include "TreeCtrlItemInDlg.h"
#include "Landside/LandsideExternalNode.h"
CString LandsideExternalNodePropDlgImpl::getTitle()
{
	/*if(m_pObject->getID()<0)
		return "New Landside Entry Exit Point";
	else
		return "Edit Landside Entry Exit Point";*/
	return _T("Landside EntryExit Point Specification ");
}

void LandsideExternalNodePropDlgImpl::LoadTree()
{
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);


	TreeCtrlItemInDlg rootItem = TreeCtrlItemInDlg::GetRootItem(m_treeProp);

	CString strLabel;
	strLabel  = _T("Position:");
	TreeCtrlItemInDlg posItem = rootItem.AddChild(strLabel);
	m_hPos = posItem.m_hItem;
	
	
	StretchSide side;
	side.m_pStretch = GetNodeData()->getStretch();
	side.ntype = GetNodeData()->IsLinkStretchPort1()?_stretch_entry:_stretch_exit;	
	if(side.IsValid())
	{
		StretchSideTreeItem SSItem = posItem.AddChild();
		SSItem.setStretchSide(side,0);
	}
	posItem.Expand();
	/*CString strLabel;	
	strLabel = _T( "Link Stretch: ");	
	m_hLinkStretch = m_treeProp.InsertItem(strLabel);
	LandsideStretch* pStretch = GetNodeData()->getStretch();
	if(pStretch)
	{
		TreeCtrlItemInDlg ctrlItem(m_treeProp,m_hLinkStretch);	
		ctrlItem.SetStringText(strLabel, pStretch->getName().GetIDString() );
	}

	strLabel = _T("Stretch End Point: ");
	m_hStertchPort = m_treeProp.InsertItem(strLabel);
	if(pStretch)
	{
		TreeCtrlItemInDlg cstrItem(m_treeProp,m_hStertchPort);
		cstrItem.SetStringText(strLabel, GetNodeData()->IsLinkStretchPort1()?"End Point1":"End Point2");
	}*/


	m_treeProp.SetRedraw(1);
}

//void LandsideExternalNodePropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
//{
//	/*if(hTreeItem == m_hLinkStretch)
//	{
//		OnNodeStretchCombo(hTreeItem);
//	}
//	if(hTreeItem == m_hStertchPort)
//	{
//		OnPortCombo(hTreeItem);
//	}*/
//	m_hRClickItem = hTreeItem;
//}

//BOOL LandsideExternalNodePropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
//{
//	LandsideExternalNode* pNode = GetNodeData();
//	/*if (message == WM_INPLACE_COMBO2)
//	{		
//		int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
//		if(m_hRClickItem == m_hLinkStretch)
//		{
//			if(selitem >=0 && selitem<(int)mStretchList.size())
//			{
//				pNode->setStretch(mStretchList.at(selitem));
//				LoadTree();
//				Update3D();
//			}
//		}
//		else if(m_hRClickItem==m_hStertchPort)
//		{
//			if(selitem>=0)
//			{
//				pNode->SetPort(selitem);
//				LoadTree();
//				Update3D();
//			}
//		}	
//	}*/
//	return TRUE;
//
//}
//
//void LandsideExternalNodePropDlgImpl::OnNodeStretchCombo(HTREEITEM hTreeItem)
//{
//	std::vector<CString> vString;
//
//	mStretchList.resize(0);
//
//	m_pParentDlg->getInputLandside()->getObjectList().GetObjectList(mStretchList,ALT_LSTRETCH);
//	for(size_t i=0;i<mStretchList.size();i++){
//		LandsideLayoutObject* pObj = mStretchList.at(i);
//		vString.push_back(pObj->getName().GetIDString());
//	}
//
//	GetTreeCtrl().SetComboWidth(300);
//	GetTreeCtrl().SetComboString(hTreeItem,vString);	
//}

//void LandsideExternalNodePropDlgImpl::OnPortCombo(HTREEITEM hTreeItem)
//{
//	std::vector<CString> vString;
//	vString.push_back("End Point1");
//	vString.push_back("End Point2");
//
//	GetTreeCtrl().SetComboWidth(300);
//	GetTreeCtrl().SetComboString(hTreeItem,vString);	
//	GetTreeCtrl().m_comboBox.SetCurSel(GetNodeData()->IsLinkStretchPort1()?0:1);
//}

LandsideExternalNodePropDlgImpl::LandsideExternalNodePropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{

}

bool LandsideExternalNodePropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideExternalNode* pNode = GetNodeData())
	{
		if( !pNode->getStretch() )
		{
			errorMsg = _T("Not Link To Stretch Yet!");
			return false;
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

int LandsideExternalNodePropDlgImpl::getBitmapResourceID()
{
	return IDB_BITMAP_LANDSIDE_CROSSWALK;
}

void LandsideExternalNodePropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hPos	)
	{
		pMenu = menuPopup.GetSubMenu(73);
	}	
}

void LandsideExternalNodePropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	if(lParam == PICK_STRETCHSIDE)
	{		
		std::vector<StretchSide>* vNewLinkags = (std::vector<StretchSide>*)wParam;
		if(vNewLinkags->size())
		{
			StretchSide& side = vNewLinkags->at(0);
			GetNodeData()->setStretch(side.m_pStretch.get());
			GetNodeData()->SetPort(side.ntype);
			LoadTree();
			Update3D();
		}		
	
	}
}

FallbackReason LandsideExternalNodePropDlgImpl::GetFallBackReason()
{	
	return PICK_STRETCHSIDE;	
}