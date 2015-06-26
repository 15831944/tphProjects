#pragma once
#include "renderprocessor3d.h"

class CRenderFloorChangeProc3D :
	public CRenderProcessor3D
{
public:
	SCENE_NODE(CRenderFloorChangeProc3D,CRenderProcessor3D)
	virtual void Setup3D(CProcessor2Base* pProc, int idx);
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	//virtual void Update3D(CProcessor2Base* pProc);
};
