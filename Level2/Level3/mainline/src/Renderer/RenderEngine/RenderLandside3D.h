#pragma once

#include "3DSceneNode.h"
class CRender3DScene;
class ILandsideEditCommand;
class IARCportLayoutEditContext;
class ILayoutObject;
class QueryData;
class LandsideFacilityLayoutObject;
class CLandsidePortal;
class ILandsideEditContext;
class CLandsideArea;
//////////////////////////////////////////////////////////////////////////
//
//utility functions in landside
//////////////////////////////////////////////////////////////////////////
class CLandsideVehicle3D;
class IRenderSceneMouseState;
class CBaseFloor;
class LandsideLayoutObject;

class CRenderLandside3D : public C3DSceneNode
{
	SCENE_NODE(CRenderLandside3D,C3DSceneNode)
public:
	void Setup(IARCportLayoutEditContext* pEditContext);
	bool OnCommandUpdate(ILandsideEditCommand* pCmd);
	void OnUpdatePickMode(IRenderSceneMouseState*); //update scene when pick something

	void DeleteLayoutObject(LandsideFacilityLayoutObject* pObj);
	void DeleteLayoutObject(LandsideLayoutObject* pObj);


	bool OnUpdate( LPARAM lHint, CObject* pHint );
protected:
	void _updateFloors(ILandsideEditContext* landside, bool updateALL);
	void _updateFloorGrids(ILandsideEditContext* landside);


	void UpdateAllObject(ILandsideEditContext* pLandsideCtx );
	void UpdateStretchRelateObjects(LandsideFacilityLayoutObject* pStretch);
	//update object and if it is stretch 
	void OnUpdateLayoutObject(LandsideFacilityLayoutObject* pObj, bool bUpdateRelate = true);
	void OnUpdateLayoutObject(LandsideLayoutObject* pObj, bool bUpdateRelate = true);
	void UpdatePortal(CLandsidePortal *pPortal);	
	void UpdateArea(CLandsideArea* pArea);
	void OnFloorAltChange( CBaseFloor* pFloor );
};
