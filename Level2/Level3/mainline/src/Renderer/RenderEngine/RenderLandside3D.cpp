#include "StdAfx.h"
#include ".\renderlandside3d.h"
#include "landside/Landside.h"
#include "OgreUtil.h"
#include "Render3DScene.h"
#include "LandsideStretch3D.h"
#include "landside/InputLandside.h"
#include "common/IARCportLayoutEditContext.h"
#include "LandIntersection3D.h"
#include "LandsideParkingLot3D.h"
#include "landside/LandsideQueryData.h"
#include "landsidecurbside3d.h"
#include "LandsideExternalNode3D.h"
#include "Render3DSceneTags.h"
#include "LandsideCrosswalk3D.h"
#include "LandsideWalkway3D.h"
#include "LandsideBusStation3D.h"
#include "LandsideRoundAboout3D.h"
#include "Landside/LandsidePortal.h"
#include "LandsidePortal3D.h"
#include "LandsideArea3D.h"
#include "LandsideStretchSegment3D.h"
#include "LandsideControlDevice3D.h"
#include "3DFloorNode.h"
#include <common/ViewMsg.h>
#include <common/ViewMsgInfo.h>
#include "LandsideTaxiPool3D.h"
#include "LandsideTaxiQueue3D.h"
#include "LandsideDecisionPoint3D.h"

using namespace Ogre;

void CRenderLandside3D::OnUpdateLayoutObject(LandsideFacilityLayoutObject* pObj, bool bUpdateRelate)
{
	CLayoutObject3DNode objNode =  OgreUtil::createOrRetrieveSceneNode(pObj->getGuid(),getScene());
	

	switch(pObj->GetType())
	{
	case ALT_LSTRETCH:
		{
			LandsideStretch3D stretch3d = objNode;
			stretch3d.Setup3D(pObj);
		}
		break;
	case ALT_LINTERSECTION:
		{
			LandIntersection3D node3d = objNode;
			node3d.Setup3D(pObj);
		}
		break;
	case ALT_LPARKINGLOT:
		{
			LandsideParkingLot3D lot3d = objNode;
			lot3d.Setup3D(pObj);
		}
		break;
	case ALT_LCURBSIDE:
		{
			LandsideCurbside3D cub3d = objNode;
			cub3d.Setup3D(pObj);
		}
		break;
	case ALT_LSTRETCHSEGMENT:
		{
			LandsideStretchSegment3D seg3d = objNode;
			seg3d.Setup3D(pObj);
		}
		break;
	case ALT_LEXT_NODE:
		{
			LandsideExternalNode3D exNode3D = objNode;
			exNode3D.Setup3D(pObj);
		}
		break;
	case ALT_LCROSSWALK:
		{
			LandsideCrosswalk3D crosswalk3D = objNode;
			crosswalk3D.Setup3D(pObj);
		}
		break;

	case ALT_LWALKWAY:
		{
			LandsideWalkway3D walkway3D = objNode;
			walkway3D.Setup3D(pObj);
		}
		break;
	case ALT_LBUSSTATION:
		{
			LandsideBusStation3D busStation3D = objNode;
			busStation3D.Setup3D(pObj);
		}
		break;
	case ALT_LROUNDABOUT:
		{
			LandsideRoundAboout3D round3D = objNode;
			round3D.Setup3D(pObj);
			break;
		}
	case ALT_LSTOPSIGN:
	case ALT_LTRAFFICLIGHT:
	case ALT_LYIELDSIGN:
	case ALT_LTOLLGATE:
	case ALT_LSPEEDSIGN:
		{
			LandsideControlDevice3D ctrl3D = objNode;
			ctrl3D.Setup3D(pObj);
			break;
		}
	case ALT_LDECISIONPOINT:
		{
			LandsideDecisionPoint3D dec3D = objNode;
			dec3D.Setup3D(pObj);
			break;
		}
	case ALT_LTAXIQUEUE:
		{
			LandsideTaxiQueue3D q3d = objNode;
			q3d.Setup3D(pObj);
		}
		break;
	case ALT_LTAXIPOOL:
		{
			LandsideTaxiPool3D pool3D = objNode;
			pool3D.Setup3D(pObj);
		}
		break;

	}
    
	C3DRootSceneNode root  = GetRoot();
	CRender3DSceneTags* pTagMan = root.GetTagManager();

	ILayoutObjectDisplay::DspProp& dsp = pObj->getDisplayProp().GetProp(ILayoutObjectDisplay::_Name);

	if (dsp.bOn)
	{
		pTagMan->m_layoutObjectTags.SetStaticLayoutTag(pObj->getID(),objNode.GetSceneNode(),pObj->getName().GetIDString(),ARCColor3(dsp.cColor));
		pTagMan->m_layoutObjectTags.Show(pObj->getID(),true);
	}
	else
	{
		pTagMan->m_layoutObjectTags.Show(pObj->getID(),false);
	}

	AddChild( C3DSceneNode(objNode.GetSceneNode()) );
	
	//update relate 
	if(pObj->GetType() == ALT_LSTRETCH && bUpdateRelate)
		UpdateStretchRelateObjects((LandsideFacilityLayoutObject*)pObj);

}

void CRenderLandside3D::OnUpdateLayoutObject( LandsideLayoutObject* pObj, bool bUpdateRelate /*= true*/ )
{
	if(LandsideFacilityLayoutObject* pfcObj = pObj->toFacilityObject())
	{
		OnUpdateLayoutObject(pfcObj,bUpdateRelate);
	}
	else if(pObj->GetType() == ALT_LPORTAL)
	{
		UpdatePortal((CLandsidePortal*)pObj);
	}
	else if(pObj->GetType()==ALT_LAREA)
	{
		UpdateArea((CLandsideArea*)pObj);
	}
}

void CRenderLandside3D::UpdatePortal(CLandsidePortal *pPortal)
{
	//CString strPortal3DName =_T("Portal-")+pPortal->name;
	LandsidePortal3D portalNode = OgreUtil::createOrRetrieveSceneNode(pPortal->getGuid(),getScene());
	portalNode.RemoveAllObject();
	portalNode.Update3D(pPortal);
	
	AddChild(C3DSceneNode(portalNode.GetSceneNode()));

}

void CRenderLandside3D::DeleteLayoutObject(LandsideFacilityLayoutObject* pObj)
{
	RemoveChild(pObj->getGuid());
}

void CRenderLandside3D::DeleteLayoutObject( LandsideLayoutObject* pObj )
{
	RemoveChild(pObj->getGuid());
}



bool CRenderLandside3D::OnCommandUpdate( ILandsideEditCommand* pCmd )
{
	if(!pCmd)
		return false;

	if(LandsideCopyLayoutObjectCmd* pCopyObjCmd = pCmd->toCopyObjCmd())
	{
		LandsideLayoutObject* pObj  = pCopyObjCmd->getObject();
		if(pCopyObjCmd->mLastOp==Command::_undo)
		{
			DeleteLayoutObject(pObj);
		}
		else
		{
			LandsideLayoutObject* pOldObj = pCopyObjCmd->getOldObject();
			OnUpdateLayoutObject(pOldObj);
			OnUpdateLayoutObject(pObj);
		}
		return true;
	}
	if(LandsideAddLayoutObjectCmd* pAddObjCmd = pCmd->toAddObjCmd())
	{	
		LandsideLayoutObject* pObj  = pAddObjCmd->getObject();
		if(pAddObjCmd->mLastOp==Command::_undo || pAddObjCmd->mLastOp == Command::_canceled )
		{
			DeleteLayoutObject(pObj);
		}
		else
		{
			OnUpdateLayoutObject(pObj);
		}	
 		return true;
	}
	if(LandsideDelLayoutObjectCmd*pDelObjCmd = pCmd->toDelObjCmd())
	{	
		LandsideLayoutObject* pobj = pDelObjCmd->getObject();
		if(pDelObjCmd->mLastOp == Command::_undo)
		{
			OnUpdateLayoutObject(pobj);
		}
		else
		{
			DeleteLayoutObject(pobj);
		}		
		return true;
	}
	if(LandsideModifyLayoutObjectCmd* pModObjCmd = pCmd->toModObjCmd())
	{
		LandsideLayoutObject* pobj = pModObjCmd->getObject();
		OnUpdateLayoutObject(pobj);				
	}
	return false;
}

void CRenderLandside3D::Setup( IARCportLayoutEditContext* pEditContext )
{
	ILandsideEditContext* landside = pEditContext->GetLandsideContext();

	//update floors
	_updateFloors(landside,true);

	//setup object
	UpdateAllObject(landside);
}

void CRenderLandside3D::OnUpdatePickMode(IRenderSceneMouseState* pMouse)
{
	//NS3DViewCommon::EMouseState mouestate = scene.GetMouseState();
	if(ILandsideEditContext* pLandsideCtx = GetRoot().GetModel()->GetLandsideContext() )
	{
		InputLandside* pInput = pLandsideCtx->getInput();
		LandsideFacilityLayoutObjectList& objlist = pInput->getObjectList();
		for(int i=0;i<objlist.getCount();i++)
		{
			LandsideFacilityLayoutObject* pObj = objlist.getObject(i);
			CLayoutObject3DNode objNode = OgreUtil::createOrRetrieveSceneNode(pObj->getGuid(),getScene());
			objNode.ChangeToPicMode(pMouse);
		}

	}
}


void CRenderLandside3D::UpdateStretchRelateObjects( LandsideFacilityLayoutObject* pStretch )
{
	if(ILandsideEditContext* pLandsideCtx =  GetRoot().GetModel()->GetLandsideContext() )
	{
		LandsideFacilityLayoutObjectList& objlIst = pLandsideCtx->getInput()->getObjectList();
		for(int i=0;i<objlIst.getCount();++i)
		{
			LandsideFacilityLayoutObject* pObj = objlIst.getObject(i);
			if(pObj==pStretch)
				continue;	
			if(pObj->IsLinkToStretch(pStretch))
			{
				OnUpdateLayoutObject(pObj);
			}			
		}
	}
}


void CRenderLandside3D::OnFloorAltChange( CBaseFloor* pFloor )
{
	ILandsideEditContext* pLandsideCtx =  GetRoot().GetModel()->GetLandsideContext();
	if(!pLandsideCtx)
		return;

	C3DFloorNode floorNode = GetSceneNode(pFloor->GetUID());
	floorNode.UpdateAltitude(pFloor->Altitude());
	UpdateAllObject(pLandsideCtx);
}

void CRenderLandside3D::UpdateAllObject( ILandsideEditContext* pLandsideCtx )
{
	InputLandside* pInput = pLandsideCtx->getInput();
	LandsideFacilityLayoutObjectList& objlist = pInput->getObjectList();
	for(int i=0;i<objlist.getCount();i++)
	{
		OnUpdateLayoutObject(objlist.getObject(i),false);
	}
	
	CLandsidePortals* portals = &(pInput->GetPortals());
	for (int i=0;i<portals->getCount();i++)
	{
		CLandsidePortal* portal = (CLandsidePortal*)portals->getLayoutObject(i);
		UpdatePortal(portal);
	}
	CLandsideAreas* areas = &(pInput->GetLandsideAreas());
	for (int i = 0; i < areas->getCount();i++)
	{
		CLandsideArea* area = (CLandsideArea*)areas->getLayoutObject(i);
		UpdateArea(area);
	}
}

bool CRenderLandside3D::OnUpdate( LPARAM lHint, CObject* pHint )
{
	ILandsideEditContext* pLandsideCtx =  GetRoot().GetModel()->GetLandsideContext();

	switch(lHint)
	{
	case VM_NEW_ONE_FLOOE:
	case VM_DELETE_ONE_FLOOR:
		{
			_updateFloors(pLandsideCtx,false);
			UpdateAllObject(pLandsideCtx);
			return true;
		}
		break;
	case VM_UPDATE_ONE_FLOOR:
		{
			CBaseFloor* pFloor = (CBaseFloor*)pHint;
			C3DSceneNode floorlist3D = GetCreateChildNode("landside_levellist");
			C3DFloorNode floor3D =  floorlist3D.GetCreateChildNode(pFloor->GetUID());			
			floor3D.UpdateAll(pFloor,true);
			UpdateAllObject(pLandsideCtx);
			return true;
		}
		break;
	case VM_ACTIVE_ONE_FLOOR:
		{
			_updateFloorGrids(pLandsideCtx);
			return true;
		}
		break;
	case VM_UPDATE_ALL_FLOORS:
		{
			_updateFloors(pLandsideCtx,false);
			UpdateAllObject(pLandsideCtx);
			return true;		
		}
		break;
	case VM_UPDATE_FLOOR_ALTITUDE:
		{
			FloorAltChangeInfo* pInfo =(FloorAltChangeInfo*)pHint;
			OnFloorAltChange(pInfo->pFloor);
			return true;				
		}
		break;	
	case VM_UPDATE_COMMAND:
		{
			return OnCommandUpdate((ILandsideEditCommand*)pHint);
		}		
		break;		
	case VM_UPDATE_ALTOBJECT3D:
		{
			OnUpdateLayoutObject((LandsideFacilityLayoutObject*)pHint);
			return true;
		}
		break;
	case VM_UPDATE_TRAFFICLIGHT:
	case VM_UPDATE_LANDSIDE_LAYOUTOPTION:
		{
			UpdateAllObject(pLandsideCtx);
			return true;
		}
		break;
	default:;
	}
	return false;
}

void CRenderLandside3D::_updateFloors( ILandsideEditContext* landside, bool updateALL )
{
	CRenderFloorList floorlist = landside->GetFloorList();
	C3DSceneNode floorlist3D = GetCreateChildNode("landside_levellist");
	floorlist3D.RemoveAllChildNodes();

	for(size_t i=0;i<floorlist.size();i++){
		CRenderFloor* pFloor = floorlist.at(i);
		if(!pFloor->IsVisible())
			continue;

		C3DFloorNode floor3D =  floorlist3D.GetSceneNode(pFloor->GetUID());
		if(floor3D && !updateALL)
		{
			floorlist3D.AddChild(floor3D);
		}
		else
		{
			floor3D = floorlist3D.GetCreateChildNode(pFloor->GetUID());
			floor3D.UpdateAll(pFloor,true);
		}
	}
}

void CRenderLandside3D::_updateFloorGrids( ILandsideEditContext* landside )
{
	CRenderFloorList floorlist = landside->GetFloorList();
	C3DSceneNode floorlist3D = GetCreateChildNode("landside_levellist");
	floorlist3D.RemoveAllChildNodes();

	for(size_t i=0;i<floorlist.size();i++){
		CRenderFloor* pFloor = floorlist.at(i);
		if(!pFloor->IsVisible())
			continue;

		C3DFloorNode floor3D =  floorlist3D.GetCreateChildNode(pFloor->GetUID());
		floor3D.UpdateGrid(pFloor,true);		
	}
}

void CRenderLandside3D::UpdateArea( CLandsideArea* pArea )
{
	//CString strArea3DName=_T("Area-")+pArea->name;
	LandsideArea3D areaNode = OgreUtil::createOrRetrieveSceneNode(pArea->getGuid(),getScene());
	areaNode.RemoveAllObject();
	areaNode.Update3D(pArea);

	AddChild(C3DSceneNode(areaNode.GetSceneNode()));
}

