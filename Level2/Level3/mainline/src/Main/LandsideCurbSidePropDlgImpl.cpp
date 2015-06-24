#include "StdAfx.h"
#include "resource.h"
#include ".\landsidecurbsidepropdlgimpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include ".\landsideparkinglotpropdlgimpl.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "landside/LandsideCurbSide.h"
#include "Floors.h"
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "Common/RenderFloor.h"

#define sItemFloor "Terminal Floor"
#define sItemPickArea "Pick Area"

//////////////////////////////////////////////////////////////////////////
LandsideCurbSidePropDlgImpl::LandsideCurbSidePropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
	m_hFloor = NULL;
}

CString LandsideCurbSidePropDlgImpl::getTitle()
{
	/*if(m_pObject->getID()<0)
		return "New Landside CurbSide";
	else
		return "Edit Landside CurbSide";*/
	return _T("Landside Curbside Specification");

}

void LandsideCurbSidePropDlgImpl::LoadTree()
{
	const LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	if(!pCurb)
		return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);
	CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_Terminal);
	

	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	//insert floor
	CString sFloorName;
	CRenderFloor* pFloor = termFloors.GetFloor(pCurb->m_nTernimalFloor);
	if(pFloor)
	{
		sFloorName = pFloor->FloorName();
	}
	m_hFloor = m_treeProp.InsertItem(sItemFloor);
	TreeCtrlItemInDlg floorItem(m_treeProp,m_hFloor);
	floorItem.SetStringText(sItemFloor, sFloorName);

	//pick area
	/*CString sPickArea;
	sPickArea.Format("Pickup Area(%s)",curLengthUnitString);
	m_hPickArea = m_treeProp.InsertItem(sPickArea);
	PropDlgTreeCtrlItem itempickArea(m_treeProp,m_hPickArea);
	itempickArea.AddChildPath(pCurb->m_PickArea,curLengthUnit,pCurb->m_nTernimalFloor);*/
	//link stretch
	{
		CString sStretch;
		if(pCurb->m_pStretch.valid())
			sStretch = pCurb->m_pStretch->getName().GetIDString();
		m_hLinkStretch = m_treeProp.InsertItem("Link Stretch");
		TreeCtrlItemInDlg stretchItem(m_treeProp,m_hLinkStretch);
		stretchItem.SetStringText("Link Stretch",sStretch);

	}
	//link stretch length
	{
		CString sLength = "Link Stretch Length";
		m_hLength = m_treeProp.InsertItem(sLength, m_hLinkStretch);
		TreeCtrlItemInDlg lengthItem(m_treeProp,m_hLength);
		lengthItem.SetLengthValueText(sLength,pCurb->m_dLength,curLengthUnit);
	}
	//land form
	{
		CString sLaneFrom = "Lane From";
		m_hLaneFrom = m_treeProp.InsertItem(sLaneFrom, m_hLinkStretch);
		TreeCtrlItemInDlg laneFromItem(m_treeProp,m_hLaneFrom);
		laneFromItem.SetIntValueText(sLaneFrom,pCurb->m_nLaneFrom);
	}
	//lane to
	{
		CString sLaneTo = "Lane To";
		m_hLaneTo = m_treeProp.InsertItem(sLaneTo, m_hLinkStretch);
		TreeCtrlItemInDlg laneToItem(m_treeProp,m_hLaneTo);
		laneToItem.SetIntValueText(sLaneTo,pCurb->m_nLaneTo);
	}
	m_treeProp.Expand(m_hLinkStretch,TVE_EXPAND);
	//spot length
	{
		CString sSpotlen= "Spot Length";
		m_hSpotLength = m_treeProp.InsertItem(sSpotlen);
		TreeCtrlItemInDlg spotlenItem(m_treeProp,m_hSpotLength);
		spotlenItem.SetLengthValueText(sSpotlen, pCurb->m_dSpotLength, GetCurLengthUnit() );

	}
	//pick area
	//add picking area item node to CAirsideObjectTreeCtrl control of  "add new curbside" setting dialog
	{
		CString sPickArea;
		if(pCurb->m_pStretch.valid())
			sPickArea = pCurb->getName().GetIDString();
		//m_hPickArea = m_treeProp.InsertItem("Pick Area");
		//TreeCtrlItemInDlg curbsideItem(m_treeProp,m_hPickArea);
		//curbsideItem.SetStringText("Pick Area",sPickArea);

		//show the coordinate of points
		CString strLabel = CString( "Pax Area(") + CARCLengthUnit::GetLengthUnitString(curLengthUnit) + CString( ")" );
		m_hPickArea = m_treeProp.InsertItem(strLabel);
		LoadTreeSubItemPickArea(m_hPickArea, pCurb->m_PickArea);	
		m_treeProp.Expand( m_hPickArea, TVE_EXPAND );
	}	
	//decision line
	//{
	//	CString sDecisionLine = "Decision Line";
 //       m_hDecisionLine = m_treeProp.InsertItem(sDecisionLine);
	//	TreeCtrlItemInDlg DecisionItem(m_treeProp,m_hDecisionLine);
	//	DecisionItem.SetLengthValueText(sDecisionLine,pCurb->m_DecisionLine,curLengthUnit);
	//}


	m_treeProp.SetRedraw(1);

}

//show the coordinate of picked area points on the tree
//x=***;y=***; floor z=***
//x=***;y=***; floor z=***
//x=***;y=***; floor z=***
void LandsideCurbSidePropDlgImpl::LoadTreeSubItemPickArea(HTREEITEM preItem,const CPath2008& path )
{
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	int pathCount = path.getCount();
	for (int i = 0; i < pathCount; i++)
	{
		CString csPoint;

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);

		double xUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(i).x);
		double yUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(i).y);
		double floorAt = path.getPoint(i).z;

		//set x, y, z text value and text colour
		strTemp.Format(_T("%.2f"),xUnit);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),yUnit);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),floorAt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",xUnit, yUnit, floorAt);

		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}
}

FallbackReason LandsideCurbSidePropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hLinkStretch||m_hRClickItem == m_hDecisionLine)
		return PICK_STRETCH_POS;
	if(m_hRClickItem == m_hPickArea)
		return PICK_MANYPOINTS;
		//return LINK_CURBSIDE_STRETCH_TERMINAL;
		//return PICK_STRETCH_POS;

	return PICK_MANYPOINTS;
}

void LandsideCurbSidePropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	
	if(m_hRClickItem == m_hPickArea
		|| m_hRClickItem == m_hLinkStretch
		|| m_hRClickItem == m_hDecisionLine)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}
	
}

void LandsideCurbSidePropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;	
	if(hTreeItem == m_hFloor)
	{
		OnComboFloors(hTreeItem);
	}
	else if(hTreeItem == m_hLength)
	{
		OnEditLength(hTreeItem);
	}
	else if(hTreeItem == m_hLaneFrom)
	{
		OnEditLaneFrom(hTreeItem);
	}
	else if(hTreeItem == m_hLaneTo)
	{				
		OnEditLaneTo(hTreeItem);
	}
	else if(hTreeItem == m_hSpotLength){
		OnEditSpotlength(hTreeItem);
	}
}

void LandsideCurbSidePropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	
	if(m_hRClickItem == m_hLinkStretch)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)
			&& pStretch )
		{
			pCurb->m_pStretch = pStretch;
			pCurb->m_dPosInStretch = dPos;
			//pCurb->m_DecisionLine = dPos;
			//CPoint2008 pt = pCurb->getStrech()->getControlPath().getPath().GetDistPoint(dPos);
			LoadTree();
			Update3D();
		}
	}
	if(m_hRClickItem == m_hPickArea)
	{
		GetFallBackAsPath(wParam,lParam,pCurb->m_PickArea);

		LoadTree();
		Update3D();
	}
	//if (m_hRClickItem == m_hDecisionLine)
	//{
	//	//GetFallBackAsPath(wParam,lParam,pCurb->m_DecisionLine);
	//	QueryData& qdata = *((QueryData*)wParam);
	//	LandsideStretch*pStretch = NULL;
	//	double DecisionLine = 0;
	//	if(qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
	//		&&	qdata.GetData(KeyPosInStretch,DecisionLine)
	//		&& pStretch)
	//	{
	//		pCurb->m_DecisionLine = DecisionLine;
	//		LoadTree();
	//		Update3D();
	//	}
	//}
}

BOOL LandsideCurbSidePropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(!m_hRClickItem)
		return TRUE;

	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;

	if (message == WM_INPLACE_COMBO2)
	{
		int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
		CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_Terminal);
		int nCount = termFloors.GetCount();
		selitem = MAX(0,selitem);
		selitem = MIN(nCount-1,selitem);
		pCurb->m_nTernimalFloor = selitem;
		Update3D();
		LoadTree();
	}
	if( message == WM_INPLACE_SPIN )
	{
		if(m_hLength == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			pCurb->m_dLength = dValue;
		}
		if(m_hLaneFrom == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nLaneIdx = pst->iPercent;
			pCurb->m_nLaneFrom = nLaneIdx;
		}
		if(m_hLaneTo == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nLaneIdx = pst->iPercent;
			pCurb->m_nLaneTo = nLaneIdx;
		}
		if(m_hSpotLength == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nNum = pst->iPercent;
			double curNum =  CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurLengthUnit(),pCurb->m_dSpotLength);
			if(nNum!= static_cast<int>(curNum+.5) ){
				pCurb->m_dSpotLength  = CARCLengthUnit::ConvertLength(GetCurLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,nNum);
			}
		}
		Update3D();
		LoadTree();	
	}
	return TRUE;
}



void LandsideCurbSidePropDlgImpl::OnComboFloors( HTREEITEM hItem )
{
	const LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_Terminal);
	
	std::vector<CString> vString;
	for(int i=0;i<termFloors.GetCount();i++)
	{
		vString.push_back(termFloors.GetFloor(i)->FloorName());
	}
	
	int nCurSel = pCurb->m_nTernimalFloor;
	nCurSel = MAX(0,nCurSel);
	nCurSel = MIN(termFloors.GetCount(),nCurSel);

	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(hItem,vString);
	GetTreeCtrl().m_comboBox.SetCurSel(pCurb->m_nTernimalFloor);

}
void LandsideCurbSidePropDlgImpl::OnEditLength( HTREEITEM hItem )
{
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;

	double dUnitNum = ConvertLength(pCurb->m_dLength);	
	double dMaxLen = 10000;
	if(pCurb->m_pStretch.valid())
	{
		LandsideStretch* pStretch = (LandsideStretch*)pCurb->m_pStretch.get();
		dMaxLen = ConvertLength(pStretch->getControlPath().getBuildPath().GetTotalLength());
	}

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,10000);
	GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	GetTreeCtrl().SpinEditLabel( hItem );

}

bool LandsideCurbSidePropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideCurbSide* pCurb = getCurbside())
	{
		CPath2008 &path = pCurb->m_PickArea;
		if (path.getCount() < 3)
		{
			errorMsg = _T("Pax Area Point can't less than three.");
			return false;
		}
		if( !pCurb->getStrech() )
		{
			errorMsg = _T("define curbside on a stretch");
			return false;
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

LandsideCurbSide* LandsideCurbSidePropDlgImpl::getCurbside()
{
	if(m_pObject->GetType() == ALT_LCURBSIDE)
	{
		return (LandsideCurbSide*)m_pObject;
	}
	return NULL;
}

void LandsideCurbSidePropDlgImpl::OnEditLaneFrom( HTREEITEM hItem )
{
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	int nLaneNum = 0;
	if(LandsideStretch* pStretch = pCurb->getStrech())
		nLaneNum = pStretch->getLaneNum();	
	else 
		return;

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,nLaneNum);
	GetTreeCtrl().SetDisplayNum(pCurb->m_nLaneFrom);	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideCurbSidePropDlgImpl::OnEditLaneTo( HTREEITEM hItem )
{
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	int nLaneNum = 0;
	if(LandsideStretch* pStretch = pCurb->getStrech())
		nLaneNum = pStretch->getLaneNum();	
	else 
		return;


	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,nLaneNum);
	GetTreeCtrl().SetDisplayNum(pCurb->m_nLaneTo);	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideCurbSidePropDlgImpl::GetFallBackReasonData( FallbackData& data )
{
	if(LandsideCurbSide* pCurb = getCurbside())
	{
		CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_Terminal);
		int nCurSel = pCurb->m_nTernimalFloor;
		nCurSel = MAX(0,nCurSel);
		nCurSel = MIN(termFloors.GetCount(),nCurSel);

		data.bCustomHeight = true;
		data.dWorkHeight = termFloors.GetFloor(nCurSel)->Altitude();
	}
}

void LandsideCurbSidePropDlgImpl::OnEditSpotlength( HTREEITEM hItem )
{
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	double curNum =CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurLengthUnit(),pCurb->m_dSpotLength) ;

	

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,INT_MAX);
	GetTreeCtrl().SetDisplayNum( static_cast<int>(curNum+0.5) );	

	GetTreeCtrl().SpinEditLabel( hItem );
}
