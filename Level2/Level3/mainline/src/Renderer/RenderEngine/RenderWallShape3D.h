#pragma once
#include "3DSceneNode.h"


#include "CommonData/WallShapeBase.h"
class CRenderWallShape3D : public C3DSceneNode
{
	SCENE_NODE(CRenderWallShape3D,C3DSceneNode)
public:
	void Setup3D(WallShapeBase* p, int idx);
};
