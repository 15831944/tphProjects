#pragma once
#include "3DSceneNode.h"

class CProcessor2Base;

class CRenderBillboard3D : public C3DSceneNode
{
public:
	SCENE_NODE(CRenderBillboard3D,C3DSceneNode)

	virtual void Setup3D(CProcessor2Base* pProc, int idx);
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc, int idx);

};
