#pragma once
#include "3DSceneNode.h"
#include <InputAirside/Stand.h>

class CRenderStand3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderStand3D,C3DSceneNode)
	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);

protected:

};

//#include "Terminal3DList.h"
//class Stand;
//class CRenderStand3DList : 
//	public CAirportObject3DList<CRenderStand3DList,Stand , CRenderStand3D>
//{
//
//};
