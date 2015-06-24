#include "stdafx.h"
#include "resource.h"
#include "LandsideWalkwayPropDlgImpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "landside/LandsideWalkway.h"
#include "Landside/ILandsideEditContext.h"

LandsideWalkwayPropDlgImpl::LandsideWalkwayPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{

}

CString LandsideWalkwayPropDlgImpl::getTitle()
{
	//if (m_pObject->getID()<0)
	//	return "New Landside Walkway";
	//else
	//	return "Edit Landside Walkway";
	return _T("Landside Walkway Specification");
}

void LandsideWalkwayPropDlgImpl::LoadTree()
{
	LandsideWalkway* pWalkway = getWalkway();
	if(!pWalkway) return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);

	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	
	{
		CString sPath = pWalkway->getName().GetIDString();;
		m_hPath = m_treeProp.InsertItem("Walkway Path");
		TreeCtrlItemInDlg pathItem(m_treeProp,m_hPath);
		pathItem.SetStringText("Walkway Path","   "+ sPath);
		LoadSubItemWalkwayPath(m_hPath);
		m_treeProp.Expand(m_hPath, TVE_EXPAND);
	}
	{	
		LandsideWalkway::WidthList& l = pWalkway->getWidthList();
		m_defaultWidth = m_treeProp.InsertItem("default width");

		double average = 0;
		for (size_t i = 0; i < l.size(); i++)
			average += l[i];
		
		if(m_widthItems.size() != 0 )
			average = average / l.size();
		else
			average = 300;

		CString sDefaultWidth;
		double dUnitValue = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,average);
		sDefaultWidth.Format("Set all ribs to the same width: %.2f (%s)", dUnitValue, curLengthUnitString);
		m_treeProp.SetItemText(m_defaultWidth, sDefaultWidth);
		m_treeProp.SetItemData(m_defaultWidth, (int)average*10000);
	}
	m_treeProp.SetRedraw(1);
}

void LandsideWalkwayPropDlgImpl::LoadSubItemWalkwayPath(HTREEITEM& parrent)
{
	LandsideWalkway* pWalkway = getWalkway();
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	CPath2008& path = pWalkway->getPath();
	LandsideWalkway::WidthList& l = pWalkway->getWidthList();

	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);

	CString str;
	m_widthItems.clear();
	HTREEITEM item = 0;

	for (size_t i = 0; i < l.size(); i++)
	{
		AirsideObjectTreeCtrlItemDataEx aoidDataEx;
		ItemStringSectionColor isscStringColor;
		
		str.Format("Rib %d:", i);
		HTREEITEM ribItem = m_treeProp.InsertItem(str,parrent);

		//coordinate
		CString csPoint;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);

		double xUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path[i].x);
		double yUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path[i].y);
		double floorAt = path[i].z;

		//set x, y, z text value and text colour
		str.Format(_T("%.2f"),xUnit);		
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		str.Format(_T("%.2f"),yUnit);		
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		str.Format(_T("%.2f"),floorAt);		
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		csPoint.Format("point %d:\tx=%.2f; y =%.2f ; floor z = %.2f",i, xUnit, yUnit, floorAt);

		item = m_treeProp.InsertItem( csPoint, ribItem);
		m_treeProp.SetItemDataEx(item,aoidDataEx);


		//width
		double dUnitValue = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,l[i]);
		str.Format("Width: %d (%s)",(int)dUnitValue, curLengthUnitString);
		item = m_treeProp.InsertItem(str,ribItem);
		m_treeProp.SetItemData(item, i);
		m_widthItems.push_back(item);

		//hight
		//floorAt =  CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,floorAt);
		//str.Format("Hight: %f (%s)", floorAt, curLengthUnitString);
		str.Format("Hight: %f Level", floorAt);
		item = m_treeProp.InsertItem(str, ribItem);
		m_treeProp.SetItemData(item, i);
		m_hightItems.push_back(item);
	}
}


FallbackReason LandsideWalkwayPropDlgImpl::GetFallBackReason()
{
	return PICK_MANYPOINTS;
}

void LandsideWalkwayPropDlgImpl::OnContextMenu(CMenu& menuPopup, CMenu *&pMenu)
{
	if(m_hRClickItem == m_hPath)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}

}

void LandsideWalkwayPropDlgImpl::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	m_hRClickItem = hTreeItem;
	if (std::find(m_widthItems.begin(), m_widthItems.end(), m_hRClickItem) != m_widthItems.end())
	{
		OnEditWdith(hTreeItem);
	}
	if (std::find(m_hightItems.begin(), m_hightItems.end(), m_hRClickItem) != m_hightItems.end())
	{
		OnEditHight(hTreeItem);
	}
	if (m_hRClickItem == m_defaultWidth)
	{
		OnEditDefaultWidth(hTreeItem);
	}
}

void LandsideWalkwayPropDlgImpl::DoFallBackFinished(WPARAM wParam, LPARAM lParam)
{
	LandsideWalkway* pWalkway = getWalkway();
	

	if(m_hRClickItem == m_hPath && pWalkway)
	{
		CPath2008& path = pWalkway->getPath();
		GetFallBackAsPath(wParam, lParam, path);

		ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
		ASSERT(plandsideCtx);
		if(!plandsideCtx)return;

		int nLevel = plandsideCtx->GetActiveLevel();
		pWalkway->SetLevel(nLevel);
		pWalkway->SetWidth(300);
		
		LoadTree();
		Update3D();
	}
}

LandsideWalkway* LandsideWalkwayPropDlgImpl::getWalkway()
{
	if (m_pObject->GetType() == ALT_LWALKWAY)
	{
		return (LandsideWalkway*)m_pObject;
	}
	return NULL;
}


void LandsideWalkwayPropDlgImpl::OnEditWdith(HTREEITEM hItem)
{
	LandsideWalkway* pWalkway = (LandsideWalkway*)m_pObject;
	if(!pWalkway) return;

	int dMaxWidth = 10000;

	LandsideWalkway::WidthList& l = pWalkway->getWidthList();
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	int index = (int)m_treeProp.GetItemData(hItem);
	double dUnitNum = ConvertLength( l[index] );	

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,dMaxWidth);
	GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideWalkwayPropDlgImpl::OnEditHight(HTREEITEM hItem)
{
	LandsideWalkway* pWalkway = getWalkway();
	if(!pWalkway) return;

	int dMaxHight = 1000000000;
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	int index = (int)m_treeProp.GetItemData(hItem);
	double hight = pWalkway->getPath()[index].z;//ConvertLength(pWalkway->getPath()[index].z);

	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(0,dMaxHight);
	m_treeProp.SetDisplayNum(static_cast<int>(hight));	
	m_treeProp.SpinEditLabel( hItem );
}

void LandsideWalkwayPropDlgImpl::OnEditDefaultWidth(HTREEITEM hItem)
{
	LandsideWalkway* pWalkway = (LandsideWalkway*)m_pObject;
	if(!pWalkway) return;

	int dMaxWidth = 10000;

	LandsideWalkway::WidthList& l = pWalkway->getWidthList();
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	int defaultWidth = (int)m_treeProp.GetItemData(hItem);
	double dUnitNum = ConvertLength( double(defaultWidth /10000) );
	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,dMaxWidth);
	GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	GetTreeCtrl().SpinEditLabel( hItem );
}

BOOL LandsideWalkwayPropDlgImpl::OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_hRClickItem)
		return TRUE;

	LandsideWalkway* pWalkway = getWalkway();
	LandsideWalkway::WidthList& l = pWalkway->getWidthList();

	if( message == WM_INPLACE_SPIN )
	{
		if (std::find(m_widthItems.begin(), m_widthItems.end(), m_hRClickItem) != m_widthItems.end())
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
			int index = m_treeProp.GetItemData(m_hRClickItem);
			l[index] = dValue;
		}
		if (std::find(m_hightItems.begin(), m_hightItems.end(), m_hRClickItem) != m_hightItems.end())
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = pst->iPercent;//UnConvertLength(pst->iPercent);
			CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
			int index = m_treeProp.GetItemData(m_hRClickItem);
			pWalkway->getPath()[index].z = dValue;
		}
		if (m_hRClickItem == m_defaultWidth)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
			int defaultWidth = m_treeProp.GetItemData(m_hRClickItem);

			if ((int)(dValue*10000) != defaultWidth)
			{
				std::fill(l.begin(), l.end(), (double)dValue);
			}
			
		}
		LoadTree();	
		Update3D();

	}
	return TRUE;
}


bool LandsideWalkwayPropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideWalkway* pWalkway = getWalkway())
	{

		return __super::OnOK(errorMsg);
	}	
	return false;
}