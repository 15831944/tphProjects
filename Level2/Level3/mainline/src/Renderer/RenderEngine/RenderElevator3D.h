#pragma once
#include "renderprocessor3d.h"

class CRenderElevator3D :
	public CRenderProcessor3D
{
	SCENE_NODE(CRenderElevator3D,CRenderProcessor3D)
public:
	void Setup3D(CProcessor2Base* pProc, int idx);
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);
};



