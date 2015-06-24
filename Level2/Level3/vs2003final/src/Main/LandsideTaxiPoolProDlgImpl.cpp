#include "StdAfx.h"
#include "Resource.h"
#include ".\landsidetaxipoolprodlgimpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "Landside/LandsideTaxiPool.h"
#include <limits>
#include "Landside/ILandsideEditContext.h"


LandsideTaxiPoolProDlgImpl::LandsideTaxiPoolProDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg)
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
}

LandsideTaxiPoolProDlgImpl::~LandsideTaxiPoolProDlgImpl(void)
{
}

CString LandsideTaxiPoolProDlgImpl::getTitle()
{
	return _T("Taxi Pool Specification");
}

void LandsideTaxiPoolProDlgImpl::LoadTree()
{
	LandsideTaxiPool* pTaxiPool = getTaxiPool();
	if(!pTaxiPool) return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);

	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);
	//queue path
	{
		//CString sPath = "Queue Points";
		//m_hQueue = m_treeProp.InsertItem(sPath);
		//TreeCtrlItemInDlg waitItem(m_treeProp, m_hQueue);
		//m_treeProp.SetItemText(m_hQueue, sPath);

		CString strLabel = CString( "Queue Point(") + CARCLengthUnit::GetLengthUnitString(curLengthUnit) + CString( ")" );

		m_hQueue = m_treeProp.InsertItem(strLabel);
		LoadTreeSubItemCtrlPoints(m_hQueue,pTaxiPool->getControlPath());	
		m_treeProp.Expand( m_hQueue, TVE_EXPAND );


		//CPath2008& path = pTaxiPool->getPath();
		//for (int i = 0; i < path.getCount(); i++)
		//{
		//	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
		//	ItemStringSectionColor isscStringColor;

		//	//coordinate
		//	CString str, csPoint;
		//	aoidDataEx.lSize = sizeof(aoidDataEx);
		//	aoidDataEx.dwptrItemData = 0;
		//	aoidDataEx.vrItemStringSectionColorShow.clear();
		//	isscStringColor.colorSection = RGB(0,0,255);

		//	double xUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path[i].x);
		//	double yUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path[i].y);

		//	//set x, y, z text value and text colour
		//	str.Format(_T("%.2f"),xUnit);		
		//	isscStringColor.strSection = str;
		//	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		//	str.Format(_T("%.2f"),yUnit);		
		//	isscStringColor.strSection = str;
		//	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		//	csPoint.Format("point %d: x=%.2f; y =%.2f",i, xUnit, yUnit);

		//	HTREEITEM item = m_treeProp.InsertItem( csPoint, m_hQueue);
		//	m_treeProp.SetItemDataEx(item,aoidDataEx);
		//}
		//waitItem.Expand();
	}
	
	//area
	{
		CString sArea = "Region Area";
		m_hArea = m_treeProp.InsertItem(sArea);
		TreeCtrlItemInDlg AreaItem(m_treeProp,m_hArea);
		AreaItem.SetStringText(sArea,"");
		{
			if (pTaxiPool->getAreaPath().getCount()>0)
			{

				CString sRegion = "Region";
				HTREEITEM m_hRegion = m_treeProp.InsertItem(sRegion,m_hArea);
				{
					CString sAreaPath = CString( "AreaPath(") + CARCLengthUnit::GetLengthUnitString(curLengthUnit) + CString( ")" );;
					HTREEITEM m_hAreaPath = m_treeProp.InsertItem(sAreaPath,m_hRegion);
					LoadTreeSubItemCtrlPoints(m_hAreaPath,pTaxiPool->getAreaPath());
					m_treeProp.Expand( m_hAreaPath, TVE_EXPAND );
				}
				m_treeProp.Expand( m_hRegion, TVE_EXPAND );

				CString sVisible = "visible";
				HTREEITEM m_hVisible = m_treeProp.InsertItem(sVisible,m_hArea);
				TreeCtrlItemInDlg VisibleItem(m_treeProp,m_hVisible);
				VisibleItem.AddTokenItem(sVisible);
				m_treeProp.Expand( m_hVisible, TVE_EXPAND );
			}
		}
		m_treeProp.Expand( m_hArea, TVE_EXPAND );
	}

	//queue width
	{
		CString sWidth = "Queue width";
		m_hWidth = m_treeProp.InsertItem(sWidth);
		TreeCtrlItemInDlg widthItem(m_treeProp,m_hWidth);
		widthItem.SetLengthValueText(sWidth, pTaxiPool->GetWidth(),curLengthUnit);

	}

	//enter stretch
	{
		CString strEnterStretch;
		StretchSide side = pTaxiPool->GetEntryStretch();
		if (side.IsValid())
			strEnterStretch = side.m_pStretch->getName().GetIDString();
		CString sEnter = "Enter stretch";
		m_hEnterStretch = m_treeProp.InsertItem(sEnter);
		TreeCtrlItemInDlg enterItem(m_treeProp,m_hEnterStretch);
		{
			CString sName = strEnterStretch;
			CString sSide = (side.ntype == _stretch_entry)?"Entry":"Exit";

			CString strT;
			strT.Format("Enter Stretch:%s Side:%s",sName,sSide);

			m_treeProp.SetItemText(m_hEnterStretch,strT);
			enterItem.AddTokenItem(sName);
			enterItem.AddTokenItem(sSide);
			AirsideObjectTreeCtrlItemDataEx* pData= enterItem.GetInitData();
			if(pData)
			{	
				pData->dwptrItemData = 0;
			}
		}
	}

	//exit stretch
	{
		CString strExitStretch;
		StretchSide side = pTaxiPool->GetExitStretch();
		if (side.IsValid())
			strExitStretch = side.m_pStretch->getName().GetIDString();
		CString sExit = "Exit stretch";
		m_hExitStretch = m_treeProp.InsertItem(sExit);
		TreeCtrlItemInDlg exitItem(m_treeProp,m_hExitStretch);
		{
			CString sName = strExitStretch;
			CString sSide = (side.ntype == _stretch_entry)?"Entry":"Exit";

			CString strT;
			strT.Format("Exit Stretch:%s Side:%s",sName,sSide);

			m_treeProp.SetItemText(m_hExitStretch,strT);
			exitItem.AddTokenItem(sName);
			exitItem.AddTokenItem(sSide);
			AirsideObjectTreeCtrlItemDataEx* pData= exitItem.GetInitData();
			if(pData)
			{	
				pData->dwptrItemData = 0;
			}
		}
	}

	m_treeProp.SetRedraw(1);
}

FallbackReason LandsideTaxiPoolProDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hEnterStretch || m_hRClickItem == m_hExitStretch) 
		return PICK_STRETCHSIDE;

	if(m_hRClickItem == m_hQueue || m_hRClickItem == m_hArea)
		return PICK_MANYPOINTS;

	return PICK_TWOPOINTS;
}

void LandsideTaxiPoolProDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hEnterStretch || m_hRClickItem == m_hExitStretch || m_hRClickItem == m_hQueue 
		||m_hRClickItem == m_hArea)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}
}

void LandsideTaxiPoolProDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;
	if (hTreeItem == m_hWidth)
		OnEditWidth(hTreeItem);
}

void LandsideTaxiPoolProDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lPara )
{
	LandsideTaxiPool* pTaxiPool = getTaxiPool();

	if(!pTaxiPool) return;

	if(lPara == PICK_STRETCHSIDE)
	{
		if(m_hRClickItem == m_hEnterStretch)
		{
			std::vector<StretchSide>* vNewLinkags = (std::vector<StretchSide>*)wParam;
			if(vNewLinkags->size())
			{
				StretchSide& side = vNewLinkags->at(0);
				pTaxiPool->SetEntryStretch(side);
				LoadTree();
				Update3D();
			}		
		}

		if (m_hRClickItem == m_hExitStretch)
		{
			std::vector<StretchSide>* vNewLinkags = (std::vector<StretchSide>*)wParam;
			if(vNewLinkags->size())
			{
				StretchSide& side = vNewLinkags->at(0);
				pTaxiPool->SetExitStretch(side);
				LoadTree();
				Update3D();
			}		
		}
	}
	if (lPara == PICK_MANYPOINTS)
	{
		if (m_hRClickItem == m_hQueue)
		{
			ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
			ASSERT(plandsideCtx);
			if(!plandsideCtx)return;

			int nLevel = plandsideCtx->GetActiveLevel();
			LandsideTaxiPool* pPool = (LandsideTaxiPool*)m_pObject;

			CPath2008 path;
			GetFallBackAsPath(wParam,lPara,path);

			//set the level
			CPath2008 pathret= path;
			for(int i=0;i<path.getCount();i++){
				pathret[i].z = nLevel;
			}
			//
			pPool->getControlPath().setPath(pathret);
			LoadTree();
			Update3D();
		}

		if (m_hRClickItem == m_hArea)
		{
			ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
			ASSERT(plandsideCtx);
			if(!plandsideCtx)return;

			int nLevel = plandsideCtx->GetActiveLevel();
			LandsideTaxiPool* pPool = (LandsideTaxiPool*)m_pObject;

			CPath2008 path;
			GetFallBackAsPath(wParam,lPara,path);

			//set the level
			CPath2008 pathret= path;
			for(int i=0;i<path.getCount();i++){
				pathret[i].z = nLevel;
			}
			//
			pPool->getAreaPath().setPath(pathret);
			LoadTree();
			Update3D();
		}
	}
}

BOOL LandsideTaxiPoolProDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (!m_hRClickItem)
		return TRUE;

	LandsideTaxiPool* pTaxiPool = getTaxiPool();

	if( message == WM_INPLACE_SPIN )
	{
		if(m_hWidth == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			pTaxiPool->SetWidth(dValue);
		}

		LoadTree();	
		Update3D();

	}
	return TRUE;
}

bool LandsideTaxiPoolProDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideTaxiPool* pTaxiPool = getTaxiPool())
	{
		if(!pTaxiPool->GetEntryStretch().IsValid())
		{
			errorMsg = _T("define a enter stretch for Taxi Pool");
			return false;
		}
		
		if (pTaxiPool->GetEntryStretch().ntype != _stretch_exit)
		{
			errorMsg = _T("Enter stretch side type should be tail");
			return false;
		}

		if (!pTaxiPool->GetExitStretch().IsValid())
		{
			errorMsg = _T("define a exit stretch for Taxi Pool");
			return false;
		}

		if (pTaxiPool->GetExitStretch().ntype != _stretch_entry)
		{
			errorMsg = _T("Exit stretch side type should be head");
			return false;
		}

		return __super::OnOK(errorMsg);
	}	
	return false;
}

LandsideTaxiPool* LandsideTaxiPoolProDlgImpl::getTaxiPool()
{
	if (m_pObject->GetType() == ALT_LTAXIPOOL)
	{
		return (LandsideTaxiPool*)m_pObject;
	}
	return NULL;
}

void LandsideTaxiPoolProDlgImpl::OnEditWidth( HTREEITEM hItem )
{
	LandsideTaxiPool* pTaxiPool = (LandsideTaxiPool*)m_pObject;

	int dMaxLen = (std::numeric_limits<int>::max)();
	double dUnitNum = ConvertLength(pTaxiPool->GetWidth());	

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,dMaxLen);
	GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	GetTreeCtrl().SpinEditLabel( hItem );
}
