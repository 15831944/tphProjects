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

//#define sItemFloor "Terminal Floor"
#define S_LINKSTRETCH "Link Stretch"

#define S_PICKAREA "Pick Area"
#define S_INSTETCH_PARK   "In Stretch Parking"
#define S_OUTSTRETCH_PARK "Out Stretch Parking"
#define S_INSTRETCH_LENGTH "Link Stretch Length"
#define S_INSTRETCH_LANEFROM "Lane From"
#define S_INSTRETCH_LANETO "Lane To"
#define S_INSTRETCH_SPOTLEN "Spot Length"
#define S_YES "Yes"
#define S_NO "NO"


//////////////////////////////////////////////////////////////////////////
LandsideCurbSidePropDlgImpl::LandsideCurbSidePropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
	//m_hFloor = NULL;
}

CString LandsideCurbSidePropDlgImpl::getTitle()
{
	/*if(m_pObject->getID()<0)
		return "New Landside CurbSide";
	else
		return "Edit Landside CurbSide";*/
	return _T("Landside Curbside Specification");

}



void LandsideCurbSidePropDlgImpl::UpdateInStretchItem(TreeCtrlItemInDlg&useParkItem)
{
	LandsideCurbSide*pCurb= getCurbside();

	useParkItem.SetStringText(S_INSTETCH_PARK,  pCurb->m_bUseInStretchParking?S_YES:S_NO);

	useParkItem.DeleteAllChild();
	if(pCurb->m_bUseInStretchParking)
	{
		//link stretch length
		{
			TreeCtrlItemInDlg lengthItem  = useParkItem.AddChild();//m_treeProp,m_hLength);
			lengthItem.SetLengthValueText(S_INSTRETCH_LENGTH,pCurb->m_dLength,GetCurLengthUnit());
			m_hLength = lengthItem.m_hItem;
		}
		//land form
		{
			TreeCtrlItemInDlg lanefromItem = useParkItem.AddChild();
			lanefromItem.SetIntValueText(S_INSTRETCH_LANEFROM, pCurb->m_nLaneFrom);			
			m_hLaneFrom = lanefromItem.m_hItem;
		}
		//lane to
		{
			TreeCtrlItemInDlg lanetoItem = useParkItem.AddChild();
			lanetoItem.SetIntValueText(S_INSTRETCH_LANETO, pCurb->m_nLaneTo);			
			m_hLaneTo = lanetoItem.m_hItem;
		}		
		//spot length
		{
			TreeCtrlItemInDlg spotLenItem = useParkItem.AddChild();
			spotLenItem.SetLengthValueText(S_INSTRETCH_SPOTLEN, pCurb->m_dSpotLength, GetCurLengthUnit() );
			m_hSpotLength = spotLenItem.m_hItem;
		}

		useParkItem.Expand();
	}


};

void UpdateLinkStretchItem(const LandsideCurbSide* pCurb, TreeCtrlItemInDlg& item)
{
	CString sStretch;
	if(pCurb->m_pStretch.valid())
		sStretch = pCurb->m_pStretch->getName().GetIDString();
	item.SetStringText(S_LINKSTRETCH,sStretch);
}

void LandsideCurbSidePropDlgImpl::LoadOutParkingSpacees(TreeCtrlItemInDlg& parentItem, BOOL bExpandLast)
{			
	parentItem.DeleteAllChild();

	LandsideCurbSide* pCurb = getCurbside();

	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(GetCurLengthUnit());

	//add sub items
	int nCount =  pCurb->m_outStretchParkingspace.getCount();
	for(int i=0;i<nCount;i++)
	{
		const ParkingSpace& parkspace= pCurb->m_outStretchParkingspace.getSpace(i);
		//SPACE i
		CString s;
		s.Format(_T("ParkingSpace(%d)"), i+1);
		TreeCtrlItemInDlg spaceItem = parentItem.AddChild(s);
		spaceItem.SetUserType(_ParkSpace);
		spaceItem.SetUserData(i);

		CString sPath = "Path (" + curLengthUnitString + ")";
		TreeCtrlItemInDlg pathItem = spaceItem.AddChild(sPath);
		pathItem.AddChildPath(parkspace.m_ctrlPath,GetCurLengthUnit(),0);
		pathItem.SetUserType(_PSPath);

		{			
			TreeCtrlItemInDlg typeItem = spaceItem.AddChild(PARK_SPOT_TYPE);
			typeItem.SetStringText(PARK_SPOT_TYPE,  parkspace.GetTypeString());		
			typeItem.SetUserType(_PSType);				
		}
		{			
			TreeCtrlItemInDlg withItem = spaceItem.AddChild(PARK_SPOT_WIDTH);
			withItem.SetLengthValueText(PARK_SPOT_WIDTH,parkspace.m_dWidth,GetCurLengthUnit() );	
			withItem.SetUserType(_PSWidth);
		}
		{				
			TreeCtrlItemInDlg lenItem = spaceItem.AddChild(PARK_SPOT_LENGTH);
			lenItem.SetLengthValueText(PARK_SPOT_LENGTH,parkspace.m_dLength,GetCurLengthUnit() );
			lenItem.SetUserType(_PSLength);
		}
		{
			TreeCtrlItemInDlg angleItem = spaceItem.AddChild(PARK_SPOT_ANGLE);
			angleItem.SetDegreeValueText(PARK_SPOT_ANGLE,parkspace.m_dAngle);	
			angleItem.SetUserType(_PSAngle);
		}
		{
			TreeCtrlItemInDlg opereationItem =  spaceItem.AddChild(PARK_SPOT_OPERRATION);
			opereationItem.SetStringText(PARK_SPOT_OPERRATION,parkspace.GetOpTypeString());
			opereationItem.SetUserType(_PSOperation);
		}

		if(bExpandLast && i==nCount-1 )
		{
			spaceItem.Expand();
		}
		
	}
	

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

	
	TreeCtrlItemInDlg RootItem = TreeCtrlItemInDlg::GetRootItem(GetTreeCtrl());
	//link stretch
	{
		TreeCtrlItemInDlg item = RootItem.AddChild();
		m_hLinkStretch = item.m_hItem;
		UpdateLinkStretchItem(pCurb, item);
	}

	//use in stretch parking
	//CString sUseInPark = "In Stretch Parking";

	TreeCtrlItemInDlg useParkItem = RootItem.AddChild();
	m_hUseInPark = useParkItem.m_hItem;
	UpdateInStretchItem(useParkItem);	
	

	//use out stretch parking
	TreeCtrlItemInDlg outParkItem = RootItem.AddChild();
	m_hUseOutPark = outParkItem.m_hItem;
	UpdateOutStetchItem(outParkItem);
	
	//pick area
	//add picking area item node to CAirsideObjectTreeCtrl control of  "add new curbside" setting dialog
	{
		CString sPickArea;
		if(pCurb->m_pStretch.valid())
			sPickArea = pCurb->getName().GetIDString();
		
		//show the coordinate of points
		CString strLabel = CString( "Pax Area(") + CARCLengthUnit::GetLengthUnitString(curLengthUnit) + CString( ")" );
		m_hPickArea = m_treeProp.InsertItem(strLabel);
		TreeCtrlItemInDlg paxareaItem(m_treeProp, m_hPickArea);

		paxareaItem.AddChildPath(pCurb->m_PickArea, curLengthUnit);

		//LoadTreeSubItemPickArea(m_hPickArea, pCurb->m_PickArea);	
		//m_treeProp.Expand( m_hPickArea, TVE_EXPAND );
	}	


	m_treeProp.SetRedraw(1);

}

//show the coordinate of picked area points on the tree
//x=***;y=***; floor z=***
//x=***;y=***; floor z=***
//x=***;y=***; floor z=***
//void LandsideCurbSidePropDlgImpl::LoadTreeSubItemPickArea(HTREEITEM preItem,const CPath2008& path )
//{
//	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
//	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
//
//	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
//	ItemStringSectionColor isscStringColor;
//	CString strTemp = _T("");
//	HTREEITEM hItemTemp = 0;
//
//	int pathCount = path.getCount();
//	for (int i = 0; i < pathCount; i++)
//	{
//		CString csPoint;
//
//		aoidDataEx.lSize = sizeof(aoidDataEx);
//		aoidDataEx.dwptrItemData = 0;
//		aoidDataEx.vrItemStringSectionColorShow.clear();
//		isscStringColor.colorSection = RGB(0,0,255);
//
//		double xUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(i).x);
//		double yUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(i).y);
//		double floorAt = path.getPoint(i).z;
//
//		//set x, y, z text value and text colour
//		strTemp.Format(_T("%.2f"),xUnit);		
//		isscStringColor.strSection = strTemp;
//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
//
//		strTemp.Format(_T("%.2f"),yUnit);		
//		isscStringColor.strSection = strTemp;
//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
//
//		strTemp.Format(_T("%.2f"),floorAt);		
//		isscStringColor.strSection = strTemp;
//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
//
//		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",xUnit, yUnit, floorAt);
//
//		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
//		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
//	}
//}

FallbackReason LandsideCurbSidePropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hLinkStretch)
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
		/*|| m_hRClickItem == m_hDecisionLine*/)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}

	TreeCtrlItemInDlg RCItem(GetTreeCtrl(), m_hRClickItem);
	if(!RCItem.isNull())
	{
		int nUserType = RCItem.GetUserType() ;
		if(nUserType == _ParkSpaceRoot)
		{
			pMenu = menuPopup.GetSubMenu(73);
		}
		if(nUserType == _ParkSpace)
		{
			pMenu = menuPopup.GetSubMenu(100);
		}
		if(nUserType == _PSPath)
		{
			pMenu = menuPopup.GetSubMenu(73);
		}
	}
	
}

void LandsideCurbSidePropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;	
	if(hTreeItem == m_hUseInPark )
	{
		//OnComboFloors(hTreeItem);
		std::vector<CString> vString;
		vString.push_back(S_YES);
		vString.push_back(S_NO);
		GetTreeCtrl().SetComboWidth(300);
		GetTreeCtrl().SetComboString(m_hRClickItem,vString);
		GetTreeCtrl().m_comboBox.SetCurSel(getCurbside()->m_bUseInStretchParking?0:1);

	}
	if(hTreeItem == m_hUseOutPark )
	{
		//OnComboFloors(hTreeItem);
		std::vector<CString> vString;
		vString.push_back(S_YES);
		vString.push_back(S_NO);
		GetTreeCtrl().SetComboWidth(300);
		GetTreeCtrl().SetComboString(m_hRClickItem,vString);
		GetTreeCtrl().m_comboBox.SetCurSel(getCurbside()->m_bUseOutStretchParking?0:1);

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
	else if(hTreeItem == m_hSpotLength)
	{
		OnEditSpotlength(hTreeItem);
		return;
	}
	else
	{
		TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);
		if(!rcItem.IsValid())
		{
			return;
		}
		int nUserType = rcItem.GetUserType();
		LandsideCurbSide* pCurb = getCurbside();
		
		if(nUserType == _PSAngle) //edit angle
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				double dUnitNum = space->m_dAngle;
				GetTreeCtrl().SetDisplayType( 2 );
				GetTreeCtrl().SetSpinRange(-90,90);
				GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
				GetTreeCtrl().SpinEditLabel( m_hRClickItem );
			}
		}
		if(nUserType == _PSLength)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				double dUnitNum = ConvertLength(space->m_dLength);	

				GetTreeCtrl().SetDisplayType( 2 );
				GetTreeCtrl().SetSpinRange(0,10000);
				GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
				GetTreeCtrl().SpinEditLabel( m_hRClickItem );

			}

		}
		if(nUserType == _PSOperation)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				std::vector<CString> vString;
				vString.push_back(ParkingSpace::OpTypeString(ParkingSpace::_backup));
				vString.push_back(ParkingSpace::OpTypeString(ParkingSpace::_through));


				GetTreeCtrl().SetComboWidth(300);
				GetTreeCtrl().SetComboString(m_hRClickItem,vString);
				GetTreeCtrl().m_comboBox.SetCurSel(space->m_type);
			}

		}
		if(nUserType == _PSType)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				std::vector<CString> vString;
				vString.push_back(ParkingSpace::TypeString(ParkingSpace::_perpendicular));
				vString.push_back(ParkingSpace::TypeString(ParkingSpace::_parallel));
				vString.push_back(ParkingSpace::TypeString(ParkingSpace::_angle));

				GetTreeCtrl().SetComboWidth(300);
				GetTreeCtrl().SetComboString(m_hRClickItem,vString);
				GetTreeCtrl().m_comboBox.SetCurSel(space->m_type);
			}

		}

		if(nUserType == _PSWidth)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				double dUnitNum = ConvertLength(space->m_dWidth);	

				GetTreeCtrl().SetDisplayType( 2 );
				GetTreeCtrl().SetSpinRange(0,10000);
				GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
				GetTreeCtrl().SpinEditLabel( m_hRClickItem );
			}

		}
	}
}

void LandsideCurbSidePropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);
	if(!rcItem.IsValid())
	{
		return;
	}
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	
	if(rcItem == m_hLinkStretch)
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
			UpdateLinkStretchItem(pCurb, TreeCtrlItemInDlg(GetTreeCtrl(), m_hLinkStretch) );			
			Update3D();		
		}
		return;
	}
	if(rcItem == m_hPickArea)
	{
		GetFallBackAsPath(wParam,lParam,pCurb->m_PickArea);
		rcItem.AddChildPath(pCurb->m_PickArea, GetCurLengthUnit() );
		rcItem.Expand();
		Update3D();
		return;
	}
	if(rcItem == m_hUseOutPark)
	{
		CPath2008 path;
		GetFallBackAsPath(wParam,lParam,path);
		if(path.getCount()>1)
		{
			//new a Parking space;
			ParkingSpace space;
			space.m_ctrlPath = path;
			pCurb->m_outStretchParkingspace.AddSpace(space);
			LoadOutParkingSpacees(rcItem, TRUE );
			Update3D();			
		}	
		return;
	}


	int itemType = rcItem.GetUserType();
	if(itemType == _PSPath)
	{	
		if(rcItem.GetParent().GetUserType()== _ParkSpace)
		{
			int idx = rcItem.GetParent().GetUserData();
			if(ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(idx) )
			{
				GetFallBackAsPath(wParam,lParam,space->m_ctrlPath);
				rcItem.AddChildPath(space->m_ctrlPath,GetCurLengthUnit());
				rcItem.Expand();
				Update3D();
			}
		};
		return;
	}

}

BOOL LandsideCurbSidePropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(!m_hRClickItem)
		return TRUE;

	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;

	
	if( message == WM_INPLACE_SPIN )
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);
		if(!rcItem.IsValid())
			return TRUE;
	
		if(m_hLength == m_hRClickItem)
		{
			double dValue = UnConvertLength(pst->iPercent);
			pCurb->m_dLength = dValue;
			
			rcItem.SetLengthValueText(S_INSTRETCH_LENGTH, dValue, GetCurLengthUnit() );
			Update3D();
			return TRUE;
		}
		if(m_hLaneFrom == m_hRClickItem)
		{
			int nLaneIdx = pst->iPercent;
			pCurb->m_nLaneFrom = nLaneIdx;
			
			rcItem.SetIntValueText(S_INSTRETCH_LANEFROM, nLaneIdx);
			Update3D();
			return TRUE;
		}
		if(m_hLaneTo == m_hRClickItem)
		{
			int nLaneIdx = pst->iPercent;
			pCurb->m_nLaneTo = nLaneIdx;
			rcItem.SetIntValueText(S_INSTRETCH_LANETO, nLaneIdx);
			Update3D();
			return TRUE;
		}
		if(m_hSpotLength == m_hRClickItem)
		{
			int nNum = pst->iPercent;
			double curNum =  CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurLengthUnit(),pCurb->m_dSpotLength);
			if(nNum!= static_cast<int>(curNum+.5) )
			{
				pCurb->m_dSpotLength  = CARCLengthUnit::ConvertLength(GetCurLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,nNum);
			}
			
			rcItem.SetLengthValueText(S_INSTRETCH_SPOTLEN, pCurb->m_dSpotLength, GetCurLengthUnit() );
			Update3D();
			return TRUE;
		}


		int itemType = rcItem.GetUserType();

		if(itemType == _PSWidth)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				double dValue = UnConvertLength(pst->iPercent);
				
				space->m_dWidth = dValue;
				rcItem.SetLengthValueText(PARK_SPOT_WIDTH,dValue,GetCurLengthUnit());
				Update3D();
			}

		}
		if(itemType == _PSLength)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				double dValue = UnConvertLength(pst->iPercent);
				
				space->m_dLength = dValue;
				rcItem.SetLengthValueText(PARK_SPOT_LENGTH,dValue,GetCurLengthUnit());
				Update3D();
			}
		}
		if(itemType == _PSAngle)
		{
			double dValue = pst->iPercent;
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				space->m_dAngle = dValue;
				rcItem.SetDegreeValueText(PARK_SPOT_ANGLE,dValue);
				Update3D();
			}
		}
	
	}
	if(message == WM_INPLACE_COMBO2)
	{
		TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);
		if(!rcItem.IsValid())
			return TRUE;
		int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	

		if(m_hRClickItem == m_hUseInPark)
		{
			pCurb->m_bUseInStretchParking = (selitem==0?TRUE:FALSE);
			UpdateInStretchItem(rcItem);
			return TRUE;
		}

		if(m_hRClickItem == m_hUseOutPark)
		{
			pCurb->m_bUseOutStretchParking = (selitem==0?TRUE:FALSE);
			UpdateOutStetchItem(rcItem);
			return TRUE;
		}


		int itemType = rcItem.GetUserType();
		if(itemType == _PSType)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				selitem = MAX(ParkingSpace::_perpendicular,selitem);
				selitem = MIN(ParkingSpace::_angle,selitem);					
				space->m_type = (ParkingSpace::ParkType)selitem;
				rcItem.SetStringText(PARK_SPOT_TYPE,space->GetTypeString());	
				Update3D();
			}						
		}
		if(itemType == _PSOperation)
		{
			int spaceIndex = rcItem.GetParent().GetUserData();
			ParkingSpace* space = pCurb->m_outStretchParkingspace.getSpacePtr(spaceIndex);
			if(space)
			{
				space->m_opType = (ParkingSpace::OperationType)selitem;
				rcItem.SetStringText(PARK_SPOT_OPERRATION,space->GetOpTypeString());
				Update3D();
			}			
		}

		
	}
	return TRUE;
}



//void LandsideCurbSidePropDlgImpl::OnComboFloors( HTREEITEM hItem )
//{
//	const LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
//	CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_Terminal);
//	
//	std::vector<CString> vString;
//	for(int i=0;i<termFloors.GetCount();i++)
//	{
//		vString.push_back(termFloors.GetFloor(i)->FloorName());
//	}
//	
//	int nCurSel = pCurb->m_nTernimalFloor;
//	nCurSel = MAX(0,nCurSel);
//	nCurSel = MIN(termFloors.GetCount(),nCurSel);
//
//	GetTreeCtrl().SetComboWidth(300);
//	GetTreeCtrl().SetComboString(hItem,vString);
//	GetTreeCtrl().m_comboBox.SetCurSel(pCurb->m_nTernimalFloor);
//
//}
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
		//CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_Terminal);
		//int nCurSel = pCurb->m_nTernimalFloor;
		//nCurSel = MAX(0,nCurSel);
		//nCurSel = MIN(termFloors.GetCount(),nCurSel);

		//data.bCustomHeight = true;
		//data.dWorkHeight = termFloors.GetFloor(nCurSel)->Altitude();
	}
}

void LandsideCurbSidePropDlgImpl::OnEditSpotlength( HTREEITEM hItem )
{
	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	double curNum = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurLengthUnit(),pCurb->m_dSpotLength) ;	

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,100);
	GetTreeCtrl().SetDisplayNum( static_cast<int>(curNum+0.5) );	

	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideCurbSidePropDlgImpl::OnPropDelete()
{
	TreeCtrlItemInDlg rcItem(GetTreeCtrl(),m_hRClickItem);

	if(rcItem.isNull())
		return;

	LandsideCurbSide* pCurb = (LandsideCurbSide*)m_pObject;
	int nUserType =rcItem.GetUserType();
	if(nUserType == _ParkSpace)
	{
		int nIndex = rcItem.GetUserData();
		pCurb->m_outStretchParkingspace.RemoveSpace(nIndex);
		
		LoadTree();
		Update3D();
	}

}

void LandsideCurbSidePropDlgImpl::UpdateOutStetchItem( TreeCtrlItemInDlg& outparkspaceRoot)
{

	outparkspaceRoot.SetStringText(S_OUTSTRETCH_PARK,  getCurbside()->m_bUseOutStretchParking?S_YES:S_NO);
	
	outparkspaceRoot.SetUserType(_ParkSpaceRoot);
	LoadOutParkingSpacees(outparkspaceRoot, FALSE);
	outparkspaceRoot.Expand();
}
