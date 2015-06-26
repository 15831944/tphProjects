#pragma once
#include "RenderProcessor3D.h"

#include <vector>
class ARCPipe;
class ARCPath3;
class CPath2008;
class ARCColor3;
class Path;
class CProcessor2Base;

class CRenderEscalator3D : public CRenderProcessor3D
{
public:
	SCENE_NODE(CRenderEscalator3D, CRenderProcessor3D)

	void Setup3D(CProcessor2Base* pProc,int idx);
protected:

	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);

	void DrawStairTexturePath( Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3&path ,double dWidth, double dSideHeight, bool bLoop = false );

};
