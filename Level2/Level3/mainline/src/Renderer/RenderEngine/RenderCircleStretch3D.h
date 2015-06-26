#pragma once
#include "3DSceneNode.h"

#include <InputAirside/CircleStretch.h>

class CRenderCircleStretch3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderCircleStretch3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);



};

//#include "Terminal3DList.h"
//class CircleStretch;
//class CRenderCircleStretch3DList : 
//	public CAirportObject3DList<CRenderCircleStretch3DList, CircleStretch, CRenderCircleStretch3D>
//{
//
//};
