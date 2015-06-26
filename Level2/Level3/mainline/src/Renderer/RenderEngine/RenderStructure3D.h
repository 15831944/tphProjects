#pragma once
#include "3DSceneNode.h"
//////////////////////////////////////////////////////////////////////////
#include "CommonData/StructureBase.h"
class CRenderStructure3D : public C3DSceneNode
{
	SCENE_NODE(CRenderStructure3D,C3DSceneNode)
public:
	void Setup3D(CStructureBase* pStructure,int idx,CString texturefilename);
};
