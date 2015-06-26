#pragma once
#include "renderprocessor3d.h"
class ARCPipe;
class ARCPath3;
class CPath2008;
class ARCColor3;
class Path;

class CRenderStairProc3D :
	public CRenderProcessor3D
{
public:
	SCENE_NODE(CRenderStairProc3D,CRenderProcessor3D)
	void Setup3D(CProcessor2Base* pProc, int idx);
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);

protected:
	void DrawStairTexturePath( Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3&path ,double dWidth, double dSideHeight, bool bLoop = false );
};
