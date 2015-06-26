#ifndef __LandsideBusStation3D_H__
#define __LandsideBusStation3D_H__

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
#include "../../Landside/LandsideBusStation.h"


class LandsideBusStation3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideBusStation3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideBusStation3D)

	void					OnUpdate3D();
	void					OnUpdateEditMode(BOOL b);

	//dynamic drag
	void					StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void					OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	void					EndDrag();
	LandsideBusStation*		getBusStation();

	void					MoveAlongStretch(Vector3 MouseMove);
	void					UpdateWaitingPath(LandsideFacilityLayoutObject* pBusStation, CPoint2008& center);
	//void					UpdateParkingArea(LandsideBusStation* pBusStation, CPoint2008& center);
};
//////////////////////////////////////////////////////////////////////////
enum emLandsideItemType;
class LandsideObject3DDraggableSubItem : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideObject3DDraggableSubItem, CRenderScene3DNode)

	virtual void					OnUpdateEditMode(BOOL b);

	//dynamic drag
	virtual void					StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void					EndDrag();
	virtual bool					IsDraggable()const { return true; }
	virtual LandsideFacilityLayoutObject*	getLayoutObject();
	virtual emLandsideItemType		getType() = 0;

};
//////////////////////////////////////////////////////////////////////////
class LandsideBusStationWaitingPoint3D;
class LandsideBusStationWaitingPath3D : public LandsideObject3DDraggableSubItem
{
public:
	SCENENODE_CONVERSION_DEF(LandsideBusStationWaitingPath3D, LandsideObject3DDraggableSubItem)
	PROTOTYPE_DECLARE(LandsideBusStationWaitingPath3D)

	emLandsideItemType					getType();
	void								SetQueryData(LandsideFacilityLayoutObject* pBusStation);
	int									GetPathIndex();

	void								OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );

	void								Update3D(LandsideFacilityLayoutObject* pBusStation, CPoint2008& center, int bEdit);
	LandsideBusStationWaitingPoint3D	GetControlPoint3D(int idx);

	void								OnRButtonDown(const MouseSceneInfo& mouseInfo);
};
//////////////////////////////////////////////////////////////////////////
class LandsideBusStationWaitingPoint3D : public LandsideObject3DDraggableSubItem
{
public:
	SCENENODE_CONVERSION_DEF(LandsideBusStationWaitingPoint3D, LandsideObject3DDraggableSubItem)
	PROTOTYPE_DECLARE(LandsideBusStationWaitingPoint3D)

	emLandsideItemType		getType();
	void					SetQueryData(LandsideFacilityLayoutObject* pBusStation, int ipoint);
	int						GetPointIndex();

	void					OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );

	void					Update3D(LandsideFacilityLayoutObject* pBusStation,int ipoint);
};
//////////////////////////////////////////////////////////////////////////
//class LandsideBusStationParkingAreaPoint3D;
//class LandsideBusStationParkingArea3D : public LandsideObject3DDraggableSubItem
//{
//public:
//	SCENENODE_CONVERSION_DEF(LandsideBusStationParkingArea3D, LandsideObject3DDraggableSubItem)
//	PROTOTYPE_DECLARE(LandsideBusStationParkingArea3D)
//
//	emLandsideItemType		getType();
//	void					SetQueryData(LandsideBusStation* pBusStation);
//	LandsideBusStationParkingAreaPoint3D					
//							GetControlPoint3D(int idx);
//
//	void					OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
//
//	void					Update3D(LandsideBusStation* pBusStation, CPoint2008& center, const ARCPipe& pipe, int bEdit);
//};
////////////////////////////////////////////////////////////////////////////
//class LandsideBusStationParkingAreaPoint3D : public LandsideObject3DDraggableSubItem
//{
//public:
//	enum LSBSPAPointType {LEFT, RIGHT, TOP, BOTTOM};
//public:
//	Ogre::Vector3 m_lastCrossPoint;
//public:
//	SCENENODE_CONVERSION_DEF(LandsideBusStationParkingAreaPoint3D, LandsideObject3DDraggableSubItem)
//	PROTOTYPE_DECLARE(LandsideBusStationParkingAreaPoint3D)
//
//	emLandsideItemType		getType();
//	void					SetQueryData(LandsideBusStation* pBusStation, LSBSPAPointType type);
//	LSBSPAPointType			GetPointType();
//
//	void					StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
//	void					OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
//
//	void					Update3D(LandsideBusStation* pBusStation,LSBSPAPointType ipoint);
//};


#endif