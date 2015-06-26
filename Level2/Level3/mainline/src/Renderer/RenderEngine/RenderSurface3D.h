#pragma once
#include "3DSceneNode.h"
#include "InputAirside/Surface.h"
class Surface;

class CRenderSurface3D :
	public C3DSceneNode
{
	SCENE_NODE(CRenderSurface3D,C3DSceneNode)
public:
	void Setup3D(ALTObject*);
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj);

	
 protected:
 	void Build(ALTObject* pBaseObject, Ogre::ManualObject* pObj);

};


