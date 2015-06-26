#pragma once
#include "RenderProcessor3D.h"

class CRenderConvey3D :
	public CRenderProcessor3D
{
	SCENE_NODE(CRenderConvey3D,CRenderProcessor3D)
public:
	virtual void Setup3D(CProcessor2Base* pProc, int idx);
	static Ogre::TextureUnitState* GetTextureUnit();
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);

};
