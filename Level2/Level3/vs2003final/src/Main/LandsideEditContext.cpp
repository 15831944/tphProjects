#include "StdAfx.h"
#include ".\landsideeditcontext.h"
#include "./TermPlanDoc.h"
#include "Resource.h"
#include "landside/LandsideQueryData.h"
#include "landside/InputLandside.h"
#include "Landside/LandsideEditCommand.h"
#include "common/ViewMsg.h"
#include "Landside/LandsideParkingLot.h"
#include "DlgAreaPortal.h"
#include <common/UnitsManager.h>
#include "Floor2.h"
#include "Landside/LandsideRoundabout.h"
#include "Landside/LandsideWalkway.h"

static const char* strParkingLotItemString[] = {_T("Parking space"),_T("Parking entry"),_T("Parking area"),_T("Drive pipe")};


InputLandside* CLandsideEditContext::getInput()const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	return pDoc->getARCport()->m_pInputLandside;
}

CNewMenu* CLandsideEditContext::GetContexMenu( CNewMenu& menu )
{

	CNewMenu *pCtxMenu = NULL;
	QueryData& itemdata = getRClickItemData();
	emLandsideItemType itemType;
	if(itemdata.GetData(KeyLandsideItemType,(int&)itemType))
	{
		switch(itemType)
		{
		case _LayoutObject:
			{
				emLandsideItemType parkLotItemType;
				itemdata.GetData(KeyParkLotItemType,(int&)parkLotItemType);
				if (parkLotItemType!=_ParkingLotSpace && parkLotItemType!=_ParkingLotEntry
					&& parkLotItemType!=_ParkingLotArea && parkLotItemType!=_ParkingLotDrivePipe)
				{
					menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
					//add floors to move to Menu
					CMenu* pSnapeMenu = pCtxMenu->GetSubMenu(9);
					CTermPlanDoc* pDoc = GetDoc();

					if(pDoc && pSnapeMenu)
					{
						CFloors& floors = pDoc->GetFloorByMode(EnvMode_LandSide);
						for(int i=0;i<floors.GetCount();i++){
							CRenderFloor* pfloor = floors.GetFloor(i);
							pSnapeMenu->InsertMenu(0,MF_ENABLED | MF_STRING,
								MENU_LEVEL_ID + i, pfloor->FloorName());
						}
					}
					pCtxMenu->SetMenuTitle(_T("Layout Object"), MFT_GRADIENT|MFT_TOP_TITLE);
					return pCtxMenu;
				}else
				{
					BOOL bInEdit=FALSE;
					itemdata.GetData(KeyIsInEditMode,bInEdit);
					if (!bInEdit)
					{
						menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
						pCtxMenu->SetMenuTitle(_T("ParkingLot Item"), MFT_GRADIENT|MFT_TOP_TITLE);
						return pCtxMenu;
					}else
					{
						menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(5));
						pCtxMenu->SetMenuTitle(strParkingLotItemString[parkLotItemType - _ParkingLotSpace], MFT_GRADIENT|MFT_TOP_TITLE);
						return pCtxMenu;
					}
					break;
				}
				
			}
			break;

		case _ObjectControlPoint:
		case _TaxiPoolAreaPoint:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));
				CMenu* pSnapeMenu = pCtxMenu->GetSubMenu(0);
				if(CTermPlanDoc* pDoc = GetDoc())
				{
					CFloors& floors = pDoc->GetFloorByMode(EnvMode_LandSide);
					for(int i=0;i<floors.GetCount();i++){
						CRenderFloor* pfloor = floors.GetFloor(i);
						pSnapeMenu->InsertMenu(0,MF_ENABLED | MF_STRING,
							MENU_LEVEL_ID + i, pfloor->FloorName());
					}
				}
				
				pCtxMenu->SetMenuTitle(_T("Control Point"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _ObjectControlPath:
		case _TaxiPoolAreaPath:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
				pCtxMenu->SetMenuTitle(_T("Control Path"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _LaneLinkage:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(4));
			
				return pCtxMenu;
			}
			break;
		case _CurbSideControlPath:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
				pCtxMenu->SetMenuTitle(_T("Curbside Control Path"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _CurbSideControlPoint:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));
				pCtxMenu->SetMenuTitle(_T("Curbside Control Point"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _ParkingLotAreaPath:
			{
				{
					menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
					pCtxMenu->SetMenuTitle(_T("Parkinglot Area Path"), MFT_GRADIENT|MFT_TOP_TITLE);
					return pCtxMenu;
				}
			}
			break;
		case _ParkingLotAreaPoint:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));

				return pCtxMenu;
			}
			break;
		case _ParkingLotSpacePath:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
				pCtxMenu->SetMenuTitle(_T("Parkinglot Drive Space"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _ParkingLotSpacePoint:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));

				return pCtxMenu;
			}
			break;
		case _ParkingLotDrivePipePath:
			{
				{
					menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
					pCtxMenu->SetMenuTitle(_T("Parkinglot Area Path"), MFT_GRADIENT|MFT_TOP_TITLE);
					return pCtxMenu;
				}
			}
			break;
		case _ParkingLotDrivePipePoint:
			{
				{
					menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));

					QueryData& itemdata = mRClickItemData;
					LandsideParkingLot* pLot=NULL;
					itemdata.GetData(KeyAltObjectPointer,(int&)pLot);

					CNewMenu* pNewMenu = new CNewMenu;
					pNewMenu->CreateMenu();

					if (pLot)
					{
						//pNewMenu->AppendMenu(MF_ENABLED | MF_STRING,
						//	MENU_PARKINGLOT_LEVEL , pLot->m_baselevel.m_name+_T("(0)"));

						CString strLevelNum;
						for(int i=0;i<(int)pLot->GetLevelCount();i++)
						{						
							strLevelNum.Format(_T("(%d)"),i+1);
							pNewMenu->AppendMenu(MF_ENABLED | MF_STRING,
								MENU_PARKINGLOT_LEVEL + i+1, pLot->GetLevel(i)->m_name+strLevelNum);
						}
					}
					
					pCtxMenu->ModifyMenu(0, MF_BYPOSITION|MF_POPUP, (UINT)pNewMenu->m_hMenu, "Snap Control Point To");
					pCtxMenu->SetMenuTitle(_T("Control Point"), MFT_GRADIENT|MFT_TOP_TITLE);

					return pCtxMenu;
				}
			}
			break;
		case _WalkwayControlPath:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
				pCtxMenu->SetMenuTitle(_T("Walkway Path"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _WalkwayControlPoint:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));
				CMenu* pSnapeMenu = pCtxMenu->GetSubMenu(0);
				if(CTermPlanDoc* pDoc = GetDoc())
				{
					CFloors& floors = pDoc->GetFloorByMode(EnvMode_LandSide);
					for(int i=0;i<floors.GetCount();i++){
						CRenderFloor* pfloor = floors.GetFloor(i);
						pSnapeMenu->InsertMenu(0,MF_ENABLED | MF_STRING,
							MENU_LEVEL_ID + i, pfloor->FloorName());
					}
				}
				return pCtxMenu;
			}
			break;
		case _BusStationWaitingPath:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(3));
				pCtxMenu->SetMenuTitle(_T("Waiting Path"), MFT_GRADIENT|MFT_TOP_TITLE);
				return pCtxMenu;
			}
			break;
		case _BusStationWaitingPoint:
			{
				menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(2));

				return pCtxMenu;
			}
			break;
		}

	}
	return NULL;
}

void CLandsideEditContext::OnCtxSnapPointToLevel( int iLevel )
{
	QueryData& itemdata = mRClickItemData;
	LandsideFacilityLayoutObject* pObj = NULL;	
	emLandsideItemType itemType;	

	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType)  )
	{
		if(itemType == _ObjectControlPoint )
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				if(ILandsideObjectControlPath* pCtrlPath = pObj->GetObjectControlPath())				
				{
					int nIdx = -1;
					itemdata.GetData(KeyControlPointIndex,nIdx);					
					if(nIdx>=0 && nIdx<pCtrlPath->getControlPath().getCount())
					{						
						LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,getInput());	
						StartEditOp(pModCmd); 
						ControlPoint& cp = pCtrlPath->getControlPath().getPoint(nIdx);
						cp.m_pt.z = iLevel;//pFloor->RealAltitude();
						pCtrlPath->getControlPath().invalidate();						
						NotifyViewCurentOperation();
						DoneCurEditOp(); 
					}				
				}
				
			}
		}
		else if(itemType ==  _LayoutObject)
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,getInput());	
				StartEditOp(pModCmd); 
				pObj->SetLevel(iLevel);		
				NotifyViewCurentOperation();
				DoneCurEditOp(); 
			}
		}
		else if(itemType == _WalkwayControlPoint)
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				if(pObj && pObj->GetType() == ALT_LWALKWAY)				
				{
					LandsideWalkway* pwalkway = (LandsideWalkway*)pObj;
					int nIdx = -1;
					itemdata.GetData(KeyControlPointIndex,nIdx);					
					if(nIdx>=0 && nIdx<pwalkway->getPath().getCount())
					{						
						LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,getInput());	
						StartEditOp(pModCmd); 
						CPoint2008& cp = pwalkway->getPath()[nIdx];
						cp.z = iLevel;//pFloor->RealAltitude();
								
						NotifyViewCurentOperation();
						DoneCurEditOp(); 
					}				
				}

			}
		}
	}


}
void CLandsideEditContext::OnSnapPointToParkLotLevel(int nSelLevel)
{
	nSelLevel--;
	QueryData& itemdata = mRClickItemData;

	emLandsideItemType itemType;
	if(!itemdata.GetData(KeyLandsideItemType,(int&)itemType) || itemType!=_ParkingLotDrivePipePoint)
	{
		return;
	}

	LandsideParkingLot* pLot=NULL;
	itemdata.GetData(KeyAltObjectPointer,(int&)pLot);
	if (pLot)
	{
		int nLevelIndex,nPipeIndex,nPointIndex;
		CPath2008 *pPath;
		itemdata.GetData(KeyLevelIndex,nLevelIndex);
		itemdata.GetData(KeyControlPathIndex,nPipeIndex);
		itemdata.GetData(KeyControlPointIndex,nPointIndex);
		itemdata.GetData(KeyParkingLotCtrlPath,(int&)pPath);

		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,getInput());	
		StartEditOp(pModCmd); 
		
		if (nSelLevel<0 || nSelLevel>=(int)pLot->GetLevelCount() || !pPath)
		{
			return;
		}
		if (nPointIndex<0||nPointIndex>=pPath->getCount())
		{
			return;
		}
		int offset=nSelLevel-nLevelIndex;//offset level num
		
		(*pPath)[nPointIndex].z=offset;


		NotifyViewCurentOperation();
		DoneCurEditOp();
		

	}
}
void CLandsideEditContext::OnCopyParkLotItem()
{
	QueryData& itemdata = mRClickItemData;
	emLandsideItemType itemType;	

	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType)  )
	{
		if(itemType == _LayoutObject)
		{
			emLandsideItemType parkLotItemType;
			itemdata.GetData(KeyParkLotItemType,(int&)parkLotItemType);

			if (parkLotItemType!=_ParkingLotSpace && parkLotItemType!=_ParkingLotEntry
				&& parkLotItemType!=_ParkingLotArea && parkLotItemType!=_ParkingLotDrivePipe)
			{
				return;
			}

			LandsideParkingLot* pLot=NULL;
			itemdata.GetData(KeyAltObjectPointer,(int&)pLot);
			if (!pLot)
			{
				return;
			}

			int nLevelIndex,nItemIndex;
			itemdata.GetData(KeyLevelIndex,nLevelIndex);
			itemdata.GetData(KeyControlPathIndex,nItemIndex);

			LandsideParkingLotLevel *curLevel=pLot->GetLevel(nLevelIndex);
			if(!curLevel)
				return;
			

			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,getInput());	
			StartEditOp(pModCmd); 

			switch(parkLotItemType)
			{
				case _ParkingLotSpace:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_parkspaces.getCount())
					{
						return;
					}
					curLevel->m_parkspaces.AddSpace(curLevel->m_parkspaces.getSpace(nItemIndex));
					break;
				}
				case _ParkingLotEntry:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_entryexits.getCount())
					{
						return;
					}
					curLevel->m_entryexits.AddEnt(curLevel->m_entryexits.getEnt(nItemIndex));
					break;
				}
				case _ParkingLotArea:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_areas.getCount())
					{
						return;
					}
					curLevel->m_areas.AddArea(curLevel->m_areas.getArea(nItemIndex));
					break;
				}
				case _ParkingLotDrivePipe:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_dirvepipes.getCount())
					{
						return;
					}
					curLevel->m_dirvepipes.AddPipe(curLevel->m_dirvepipes.getPipe(nItemIndex));
					break;
				}
			}

			NotifyViewCurentOperation();
			DoneCurEditOp(); 
		}
	}
}
void CLandsideEditContext::OnDelParkLotItem()
{
	QueryData& itemdata = mRClickItemData;
	emLandsideItemType itemType;	

	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType)  )
	{
		if(itemType == _LayoutObject)
		{
			emLandsideItemType parkLotItemType;
			itemdata.GetData(KeyParkLotItemType,(int&)parkLotItemType);

			if (parkLotItemType!=_ParkingLotSpace && parkLotItemType!=_ParkingLotEntry
				&& parkLotItemType!=_ParkingLotArea && parkLotItemType!=_ParkingLotDrivePipe)
			{
				return;
			}

			LandsideParkingLot* pLot=NULL;
			itemdata.GetData(KeyAltObjectPointer,(int&)pLot);
			if (!pLot)
			{
				return;
			}

			int nLevelIndex,nItemIndex;
			itemdata.GetData(KeyLevelIndex,nLevelIndex);
			itemdata.GetData(KeyControlPathIndex,nItemIndex);

			LandsideParkingLotLevel *curLevel=pLot->GetLevel(nLevelIndex);
			if(!curLevel)return;

			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,getInput());	
			StartEditOp(pModCmd); 

			switch(parkLotItemType)
			{
			case _ParkingLotSpace:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_parkspaces.getCount())
					{
						return;
					}
					curLevel->m_parkspaces.RemoveSpace(nItemIndex);
					break;
				}
			case _ParkingLotEntry:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_entryexits.getCount())
					{
						return;
					}
					curLevel->m_entryexits.RemoveEnt(nItemIndex);
					break;
				}
			case _ParkingLotArea:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_areas.getCount())
					{
						return;
					}
					curLevel->m_areas.RemoveArea(nItemIndex);
					break;
				}
			case _ParkingLotDrivePipe:
				{
					if (nItemIndex<0||nItemIndex>=curLevel->m_dirvepipes.getCount())
					{
						return;
					}
					curLevel->m_dirvepipes.RemovePipe(nItemIndex);
					break;
				}
			}

			NotifyViewCurentOperation();
			DoneCurEditOp(); 
		}
	}
}
void CLandsideEditContext::OnCtxDeleteRClickItem()
{
	QueryData& itemdata = mRClickItemData;	
	emLandsideItemType itemType;
	if(!itemdata.GetData(KeyLandsideItemType,(int&)itemType))
		return;

	if(itemType==_LaneLinkage)
	{
		LandsideIntersectionNode* pNode = NULL;
		int linkIndex = -1;
		itemdata.GetData(KeyAltObjectPointer,(int&)pNode);
		itemdata.GetData(KeyItemIndex, linkIndex);
		if(pNode && linkIndex>=0 && linkIndex<pNode->getLinageCount())
		{
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pNode,getInput());	
			StartEditOp(pModCmd); 
			pNode->RemoveLinkage(linkIndex);
			NotifyViewCurentOperation();
			DoneCurEditOp(); 
		}
	}

}
void CLandsideEditContext::OnCtxNewLandsidePortal() 
{
	CLandsidePortal *pNewPortal=new CLandsidePortal();
	CTermPlanDoc *pTermPlanDoc=(CTermPlanDoc *)GetDocument();
	if (!pTermPlanDoc)
	{
		return;
	}
	pNewPortal->color = pTermPlanDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::PortalsColor);

	CDlgAreaPortal dlg(pTermPlanDoc,CDlgAreaPortal::portal,pNewPortal,NULL);

	LandsideAddLayoutObjectCmd* pAddCmd = new LandsideAddLayoutObjectCmd(pNewPortal,getInput());
	pAddCmd->BeginOp();

	if(dlg.DoFakeModal() == IDOK)
	{
		CLandsidePortals &portals=getInput()->GetPortals();
		if( portals.hasName(pNewPortal->name) )
		{
			AfxMessageBox(_T("Name already exist!"));
			delete pAddCmd;
			return;
		}
		
		pNewPortal->floor = pTermPlanDoc->m_nActiveFloor;
		pNewPortal->m_path.clear();
		pNewPortal->m_path.init((int)pNewPortal->points.size());	
		for (int i=0;i<(int)pNewPortal->points.size();i++)
		{				
			pNewPortal->m_path[i].setPoint((DistanceUnit)pNewPortal->points.at(i)[VX],(DistanceUnit)pNewPortal->points.at(i)[VY],(DistanceUnit)pNewPortal->floor);
		}		
		
		getInput()->GetPortals().AddObject(pNewPortal);

		pAddCmd->EndOp();

		AddCommand(pAddCmd);

		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pAddCmd);

		
	}
	else 
	{
		delete pNewPortal;
	}
}

void CLandsideEditContext::OnCtxEditLandsidePortal(CLandsidePortal *pPortal)
{
	CTermPlanDoc *pTermPlanDoc=(CTermPlanDoc *)GetDocument();
	if (!pTermPlanDoc)
	{
		return;
	}
	LandsideModifyLayoutObjectCmd* pEditCmd = new LandsideModifyLayoutObjectCmd(pPortal,getInput());
	pEditCmd->m_strName=_T("Portal-")+pPortal->name;

	CString strOldName=pPortal->name;
	std::vector<ARCPoint2> oldPoint;
	oldPoint.assign(pPortal->points.begin(),pPortal->points.end());

	CDlgAreaPortal dlg(pTermPlanDoc,CDlgAreaPortal::portal, pPortal );


	pEditCmd->BeginOp();

	if(dlg.DoFakeModal() == IDOK) {

		CLandsidePortals &portals= getInput()->GetPortals();
		CLandsidePortal* namePortal = portals.getByName(pPortal->name);
		if( namePortal != NULL && namePortal!=pPortal )
		{
			AfxMessageBox(_T("Name already exist!"));				
			pPortal->name=strOldName;
			pPortal->points.clear();
			pPortal->points.assign(oldPoint.begin(),oldPoint.end());

			delete pEditCmd;
			return;
		}
		
		pPortal->floor = pTermPlanDoc->m_nActiveFloor;
		pPortal->m_path.clear();
		pPortal->m_path.init((int)pPortal->points.size());	
		for (int i=0;i<(int)pPortal->points.size();i++)
		{				
			pPortal->m_path[i].setPoint((DistanceUnit)pPortal->points.at(i)[VX],(DistanceUnit)pPortal->points.at(i)[VY],(DistanceUnit)pPortal->floor);
		}		
		getInput()->GetPortals().SaveData(-1);
		pEditCmd->EndOp();
		AddCommand(pEditCmd);

		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pEditCmd);
	}

}
void CLandsideEditContext::OnCtxDelLandsidePortal(CLandsidePortal *pPortal)
{
	LandsideDelLayoutObjectCmd* pDelCmd = new LandsideDelLayoutObjectCmd(pPortal,getInput());
	pDelCmd->BeginOp();
	pPortal->DeletePortal();
	pDelCmd->EndOp();
	AddCommand(pDelCmd);	
	GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pDelCmd);
}
void CLandsideEditContext::OnCtxLandsidePortalColor(CLandsidePortal *pPortal)
{
	LandsideModifyLayoutObjectCmd* pEditCmd = new LandsideModifyLayoutObjectCmd(pPortal,getInput());
	pEditCmd->BeginOp();

	CColorDialog colorDlg(pPortal->color, CC_ANYCOLOR | CC_FULLOPEN);
	if(colorDlg.DoModal() == IDOK) {
		pPortal->color = colorDlg.GetColor();
		getInput()->GetPortals().SaveData(-1);

		pEditCmd->EndOp();
		AddCommand(pEditCmd);

		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pEditCmd);
	}
}

//bool CLandsideEditContext::AddRemAlt( double dalt )
//{
//	if(!hasRemAlt(dalt))
//	{
//		mRemberAltitudes.push_back(dalt);
//		std::sort(mRemberAltitudes.begin(),mRemberAltitudes.end());
//		return true;
//	}
//	return false;
//}

//void CLandsideEditContext::OnSetPointToIndexAlt( int idx )
//{
//	if(idx<GetRemAltCount())
//	{
//		OnSetPointToAlt(GetRemAlt(idx));
//	}
//}

void CLandsideEditContext::OnSetPointToAlt( double d )
{
	QueryData& itemdata = mRClickItemData;
	LandsideFacilityLayoutObject* pObj = NULL;	
	emLandsideItemType itemType;
	
	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType)  )
	{
		if(itemType == _ObjectControlPoint )
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				if(pObj->GetType() == ALT_LSTRETCH)
				{
					int nIdx = -1;
					itemdata.GetData(KeyControlPointIndex,nIdx);
					LandsideStretch* pStretch = (LandsideStretch*)pObj;
					if(nIdx>=0 && nIdx<pStretch->getControlPath().getCount())
					{						
						LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,getInput());	
						StartEditOp(pModCmd); 
						ControlPoint& cp = pStretch->getControlPath().getPoint(nIdx);
						cp.m_pt.z =  d;
						pStretch->getControlPath().invalidate();						
						NotifyViewCurentOperation();
						DoneCurEditOp(); 
					}				
				}
			}
		}
	}
}
//
//BOOL CLandsideEditContext::IsEditMode() const
//{
//	CTermPlanDoc *pTermPlanDoc=(CTermPlanDoc *)GetDocument();
//	if (pTermPlanDoc)
//	{
//		return pTermPlanDoc->m_eAnimState == CTermPlanDoc::anim_none;
//	}
//	
//	return FALSE;
//}

//bool CLandsideEditContext::hasRemAlt( double dalt ) const
//{
//	return std::find(mRemberAltitudes.begin(),mRemberAltitudes.end(),dalt)!=mRemberAltitudes.end();
//}

void CLandsideEditContext::UpdateDrawing()
{
	GetDocument()->UpdateAllViews(NULL,VM_REDRAW);
}

double CLandsideEditContext::GetPointAlt()
{
	QueryData& itemdata = mRClickItemData;
	LandsideFacilityLayoutObject* pObj = NULL;	
	emLandsideItemType itemType;
	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType)  )
	{
		if(itemType == _ObjectControlPoint )
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				if(pObj->GetType() == ALT_LSTRETCH)
				{
					int nIdx = -1;
					itemdata.GetData(KeyControlPointIndex,nIdx);
					LandsideStretch* pStretch = (LandsideStretch*)pObj;
					if(nIdx>=0 && nIdx<pStretch->getControlPath().getCount())
					{						
					
						ControlPoint& cp = pStretch->getControlPath().getPoint(nIdx);
						return cp.m_pt.z;						
					}				
				}
			}
		}
	}
	return 0;
}

double CLandsideEditContext::GetActiveFloorHeight() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_LandSide);
	if( CRenderFloor* pfloor = floorlist.GetActiveFloor() )
	{
		return pfloor->Altitude();
	}
	return 0;
}

double CLandsideEditContext::GetVisualHeight( double dIndex ) const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_LandSide);
	return floorlist.GetVisualHeight(dIndex);
}

int CLandsideEditContext::GetActiveLevel() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_LandSide);
	return floorlist.GetActiveFloorLevel();
}

CRenderFloorList CLandsideEditContext::GetFloorList() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_LandSide);
	CRenderFloorList list;
	list.insert(list.end(),floorlist.m_vFloors.begin(),floorlist.m_vFloors.end());
	return list;
}

void CLandsideEditContext::OnCtxNewLandsideArea()
{
	CLandsideArea *pNewArea = new CLandsideArea();
	CTermPlanDoc *pTermPlanDoc=(CTermPlanDoc *)GetDocument();
	if (!pTermPlanDoc)
	{
		return;
	}
	pNewArea->color = pTermPlanDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::AreasColor);


	CDlgAreaPortal dlg(pTermPlanDoc,CDlgAreaPortal::area,pNewArea,NULL);
	LandsideAddLayoutObjectCmd* pAddCmd = new LandsideAddLayoutObjectCmd(pNewArea,getInput());
	pAddCmd->BeginOp();
	if(dlg.DoFakeModal() == IDOK) 
	{
		CLandsideAreas &areas=getInput()->GetLandsideAreas();
		if(areas.hasName(pNewArea->name))
		{
			AfxMessageBox(_T("Name already exist!"));
			delete pAddCmd;
			return;
		}
		
		pNewArea->floor = pTermPlanDoc->m_nActiveFloor;
		pNewArea->m_path.init((int)pNewArea->points.size());	
		for (int i=0;i<(int)pNewArea->points.size();i++)
		{				
			pNewArea->m_path[i].setPoint((DistanceUnit)pNewArea->points.at(i)[VX],(DistanceUnit)pNewArea->points.at(i)[VY],(DistanceUnit)pNewArea->floor);
		}		
		getInput()->GetLandsideAreas().AddObject(pNewArea);

		pAddCmd->EndOp();

		AddCommand(pAddCmd);

		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pAddCmd);

	}
	else {
		delete pNewArea;
	}
}

void CLandsideEditContext::OnCtxEditLandsideArea( CLandsideArea *pArea )
{
	CTermPlanDoc *pTermPlanDoc=(CTermPlanDoc *)GetDocument();
	if (!pTermPlanDoc)
	{
		return;
	}
	LandsideModifyLayoutObjectCmd* pEditCmd = new LandsideModifyLayoutObjectCmd(pArea,getInput());
	pEditCmd->m_strName=_T("Area-")+pArea->name;

	CString strOldName=pArea->name;
	std::vector<ARCPoint2> oldPoint;
	oldPoint.assign(pArea->points.begin(),pArea->points.end());

	CDlgAreaPortal dlg(pTermPlanDoc,CDlgAreaPortal::area, pArea );


	pEditCmd->BeginOp();

	if(dlg.DoFakeModal() == IDOK) 
	{
		CLandsideAreas &LandsideAreas = getInput()->GetLandsideAreas();
		CLandsideArea* pNameArea = LandsideAreas.getByName(pArea->name);
		if(pNameArea!=NULL && pNameArea!= pArea)
		{
			AfxMessageBox(_T("Name already exist!"));				
			pArea->name=strOldName;
			pArea->points.clear();
			pArea->points.assign(oldPoint.begin(),oldPoint.end());

			delete pEditCmd;
			return;
		}

		
		pArea->floor = pTermPlanDoc->m_nActiveFloor;
		pArea->m_path.clear();
		pArea->m_path.init((int)pArea->points.size());	
		for (int i=0;i<(int)pArea->points.size();i++)
		{				
			pArea->m_path[i].setPoint((DistanceUnit)pArea->points.at(i)[VX],(DistanceUnit)pArea->points.at(i)[VY],(DistanceUnit)pArea->floor);
		}		
		getInput()->GetLandsideAreas().SaveData(-1);
		pEditCmd->EndOp();
		AddCommand(pEditCmd);

		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pEditCmd);
	}
}

void CLandsideEditContext::OnCtxDelLandsideArea( CLandsideArea *pArea )
{
	LandsideDelLayoutObjectCmd* pDelCmd = new LandsideDelLayoutObjectCmd(pArea,getInput());
	pDelCmd->BeginOp();
	pArea->DeleteArea();
	pDelCmd->EndOp();
	AddCommand(pDelCmd);
	GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pDelCmd);
}

void CLandsideEditContext::OnCtxLandsideAreaColor( CLandsideArea *pArea )
{
	LandsideModifyLayoutObjectCmd* pEditCmd = new LandsideModifyLayoutObjectCmd(pArea,getInput());
	pEditCmd->BeginOp();

	CColorDialog colorDlg(pArea->color, CC_ANYCOLOR | CC_FULLOPEN);
	if(colorDlg.DoModal() == IDOK) {
		pArea->color = colorDlg.GetColor();
		getInput()->GetLandsideAreas().SaveData(-1);
		pEditCmd->EndOp();
		AddCommand(pEditCmd);

		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pEditCmd);
	}
}

bool CLandsideEditContext::isLeftDrive() const
{
	return !getInput()->IsLeftDrive(); //left hand drive , drive right
}


