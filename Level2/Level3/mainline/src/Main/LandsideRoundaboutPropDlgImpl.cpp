#include "StdAfx.h"
#include ".\landsideroundaboutpropdlgimpl.h"
#include "../Landside/LandsideRoundabout.h"
#include "Resource.h"
#include "ARCLayoutObjectDlg.h"
#include "TreeCtrlItemInDlg.h"
#include "common/WinMsg.h"
#include "Common/StringTypeConvert.h"
#include "Landside/ILandsideEditContext.h"

LandsideRoundabout* LandsideRoundaboutPropDlgImpl::getRoundabout()
{
	if(m_pObject && m_pObject->GetType() == ALT_LROUNDABOUT)
	{
		return (LandsideRoundabout*)m_pObject;
	}
	return NULL;
}

int LandsideRoundaboutPropDlgImpl::getBitmapResourceID()
{
	return IDB_BITMAP_LANDSIDE_ROUNDABOUT;
}

CString LandsideRoundaboutPropDlgImpl::getTitle()
{
	/*if(m_pObject && m_pObject->getID()>=0)
	{
		return "Edit Landside Roundabout";
	}
	else
	{
		return "New Landside Roundabout";
	}*/
	return _T("Landside Roundabout Specification");
}




static CString ClockwiseboolText(BOOL b)
{
	return b?"Clockwise":"Counterclockwise";
}

void LandsideRoundaboutPropDlgImpl::LoadTree()
{
	LandsideRoundabout* pAbout = getRoundabout();
	if(!pAbout)
		return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);

	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(FALSE);

	CString strLable;
	strLable.Format("Position(%s)",curLengthUnitString.GetString());

	
	TreeCtrlItemInDlg rootItem(m_treeProp,m_treeProp.GetRootItem());
	TreeCtrlItemInDlg positem = rootItem.AddChild(strLable);
	positem.AddChildPath(pAbout->m_position,curLengthUnit,0);
	m_hPosition=  positem.m_hItem;
	positem.Expand();


	
	TreeCtrlItemInDlg inradiusItem = rootItem.AddChild();
	inradiusItem.SetLengthValueText("Inner Radius",pAbout->m_dInRadius,curLengthUnit);
	m_hInRadius = inradiusItem.m_hItem;

	TreeCtrlItemInDlg outraiusItem = rootItem.AddChild(); 
	outraiusItem.SetLengthValueText("Outer Radius",pAbout->m_dOutRadius,curLengthUnit);
	m_hOutRadius = outraiusItem.m_hItem;

	TreeCtrlItemInDlg landnumberItem = rootItem.AddChild();
	landnumberItem.SetIntValueText("Lane number",pAbout->m_nLaneNum);
	m_hLaneNum = landnumberItem.m_hItem;

	TreeCtrlItemInDlg clockwiseItem = rootItem.AddChild();
	clockwiseItem.SetStringText("Rotation",ClockwiseboolText(pAbout->m_bClockwise));
	m_hClockwise = clockwiseItem.m_hItem;

    
	TreeCtrlItemInDlg linkstretchItem =  rootItem.AddChild("Pick Entries and Exits from map");
	loadLinkStretch(linkstretchItem);
	linkstretchItem.Expand();
	
	m_treeProp.SetRedraw(TRUE);
	
	
}

LandsideRoundaboutPropDlgImpl::LandsideRoundaboutPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{

}

FallbackReason LandsideRoundaboutPropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hPosition)
	{
		return PICK_ONEPOINT;
	}
	if(m_hRClickItem == m_hlinkStetches)
	{
		return PICK_STRETCHSIDE_LIST;
	}
	return PICK_ONEPOINT;
}

void LandsideRoundaboutPropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hPosition
		|| m_hRClickItem == m_hlinkStetches)
	{
		pMenu = menuPopup.GetSubMenu(73);
	}

	TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);
	if(!rcItem.isNull() )
	{
		if(rcItem.GetParent() == m_hlinkStetches)
		{
			pMenu = menuPopup.GetSubMenu(100);
		}

	}
	
}


void LandsideRoundaboutPropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	LandsideRoundabout* pAbout= getRoundabout();
	if(!pAbout)
		return;

	m_hRClickItem = hTreeItem;
	if(hTreeItem == m_hInRadius)
	{
		//GetTreeCtrl().EditLabel(hTreeItem);
		GetTreeCtrl().StringEditLabel(hTreeItem,TreeCtrlItemInDlg::GetValueText(pAbout->getInnerRadius(),GetCurLengthUnit()));
	}
	if(hTreeItem == m_hOutRadius)
	{
		GetTreeCtrl().StringEditLabel(hTreeItem,
									TreeCtrlItemInDlg::GetValueText(pAbout->getOutRadius(),GetCurLengthUnit())
									);

	}
	if(hTreeItem == m_hClockwise)
	{
		BOOL b = pAbout->isClockwise();
		pAbout->setClockwise(!b);
		LoadTree();
		Update3D();
	}
	if(hTreeItem == m_hLaneNum)
	{
		int laneNum = pAbout->getLaneNumber();
		GetTreeCtrl().SetDisplayType(2);
		GetTreeCtrl().SetDisplayNum(laneNum);
		GetTreeCtrl().SetSpinRange(1,40);
		GetTreeCtrl().SpinEditLabel(hTreeItem);

	}
}

void LandsideRoundaboutPropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	LandsideRoundabout* pAbout = getRoundabout();
	if(!pAbout)
		return;

	if(lParam == PICK_ONEPOINT)
	{
		ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
		ASSERT(plandsideCtx);
		if(!plandsideCtx)return;

		CPath2008 path;
		GetFallBackAsPath(wParam,lParam,path);
		int nLevel = plandsideCtx->GetActiveLevel();
		LandsideStretch* pStretch = (LandsideStretch*)m_pObject;
		//set the level
		for(int i=0;i<path.getCount();i++){
			path[i].z = nLevel;
		}
		if(path.getCount()>0)
		{
			pAbout->setPosition( path);
			LoadTree();
			Update3D();
		}
	}
	if(lParam == PICK_STRETCHSIDE_LIST)
	{
		int nPreCout = pAbout->getLinkStretchCount();
		std::vector<StretchSide>* vNewLinkags = (std::vector<StretchSide>*)wParam;

		for(size_t i=0;i<vNewLinkags->size();i++)
			pAbout->AddLinkStretch(vNewLinkags->at(i));

		int nnewCount = pAbout->getLinkStretchCount();
		if(nnewCount!=nPreCout)
		{
			LoadTree();
			Update3D();
		}
	}
}

BOOL LandsideRoundaboutPropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	LandsideRoundabout* pabout = getRoundabout();
	if(message == WM_INPLACE_SPIN )
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if( m_hRClickItem == m_hLaneNum)	
		{
			int iLaneNum = (pst->iPercent);		
			if(iLaneNum <= 0)
				iLaneNum = 1;
			if(iLaneNum!=pabout->getLaneNumber())
			{
				pabout->setLaneNumber(iLaneNum);
				LoadTree();
				Update3D();
			}			
		}
	}
	if(message == WM_INPLACE_EDIT)
	{
		CString strValue = *(CString*)lParam;
		double d=0;
		if(toType(strValue,d))
		{
			d = UnConvertLength(d);
			if(m_hRClickItem==m_hInRadius)
			{					
				pabout->setInnerRadius(d);
			}
			if(m_hRClickItem==m_hOutRadius)
			{
				pabout->setOutRadius(d);
			}
			LoadTree();
			Update3D();
		}		
		
	}
	return TRUE;
}

void LandsideRoundaboutPropDlgImpl::OnPropDelete()
{
	if(LandsideRoundabout* pabout = getRoundabout())
	{
		TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);
		if(rcItem.GetParent()==m_hlinkStetches)
		{
			StretchSideTreeItem item = rcItem;
			int idx = item.getItemIdx();
			pabout->removeLinkStretch(idx);
			//rcItem.Destroy();
		}

		loadLinkStretch(rcItem.GetParent());
		Update3D();

	}
}

void LandsideRoundaboutPropDlgImpl::loadLinkStretch( TreeCtrlItemInDlg& linkstretchItem )
{
	m_hlinkStetches = linkstretchItem.m_hItem;
	linkstretchItem.DeleteAllChild();
	LandsideRoundabout* pAbout = getRoundabout();
	for(size_t i=0;i<pAbout->m_vLinkStretches.size();i++)
	{
		StretchSideTreeItem item  = linkstretchItem.AddChild();
		item.setStretchSide(pAbout->m_vLinkStretches[i],i);
	}
	
}

int StretchSideTreeItem::getItemIdx() const
{
	if(AirsideObjectTreeCtrlItemDataEx* pData = GetData())
	{
		return (int)pData->dwptrItemData;
	}
	return -1;
}

void StretchSideTreeItem::setStretchSide( const StretchSide& s,int idx )
{
	CString sName = s.m_pStretch->getName().GetIDString();
	CString sSide = (s.ntype == _stretch_entry)?"Exit":"Entry";

	CString strT;
	strT.Format("%s: %s",sSide,sName);

	GetCtrl().SetItemText(m_hItem,strT);
	//AddTokenItem(sSide);
	AddTokenItem(sName);

	AirsideObjectTreeCtrlItemDataEx* pData= GetInitData();
	if(pData)
	{	
		pData->dwptrItemData = idx;
	}
}