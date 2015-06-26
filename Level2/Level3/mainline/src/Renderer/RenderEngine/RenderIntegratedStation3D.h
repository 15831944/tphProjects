#pragma once
#include "RenderProcessor3D.h"

#include <vector>

class CRenderIntegratedStation3D : public CRenderProcessor3D
{
public:
	SCENE_NODE(CRenderIntegratedStation3D,CRenderProcessor3D)

	virtual void Setup3D(CProcessor2Base* pProc, int idx);
protected:

	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);

};
