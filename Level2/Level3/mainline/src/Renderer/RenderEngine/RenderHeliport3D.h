#pragma once
#include "3DSceneNode.h"
#include <InputAirside/Heliport.h>

class CRenderHeliport3D :
	C3DSceneNode
{
public:
	SCENE_NODE(CRenderHeliport3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);
};

