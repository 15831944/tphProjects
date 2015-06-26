#pragma once
#include "3DSceneNode.h"
#include <InputAirside/Intersections.h>

class CRenderRoadIntersection3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderRoadIntersection3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj );



};

//#include "Terminal3DList.h"
//
//class CRenderRoadIntersection3DList : 
//	public CAirportObject3DList<CRenderRoadIntersection3DList, Intersections, CRenderRoadIntersection3D>
//{
//
//};
