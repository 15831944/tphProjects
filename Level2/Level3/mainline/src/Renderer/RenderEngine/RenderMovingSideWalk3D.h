#pragma once
#include "renderprocessor3d.h"

class CRenderMovingSideWalk3D :
	public CRenderProcessor3D
{
public:
	SCENE_NODE(CRenderMovingSideWalk3D,CRenderProcessor3D)

	void Setup3D(CProcessor2Base* pProc, int idx);
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);
};
