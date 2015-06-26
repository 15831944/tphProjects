#pragma once
//#include "terminal3dobject.h"
//
//#include "Airside3DObjectWrapper.h"
#include "3DSceneNode.h"

#include <InputAirside/Stretch.h>

class CRenderStretch3D : 
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderStretch3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj );
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);




};

//#include "Terminal3DList.h"
//class Stretch;
//class CRenderStretch3DList : 
//	public CAirportObject3DList<CRenderStretch3DList,Stretch , CRenderStretch3D>
//{
//
//};
