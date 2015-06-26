#pragma once
#include "InputAirside/AirHold.h"
#include "3DSceneNode.h"

class AirHold;

class CRenderAirHold3D :
	public C3DSceneNode
{
	SCENE_NODE(CRenderAirHold3D,C3DSceneNode)
	void Setup3D(ALTObject* pBaseObj);
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj);


protected:
	void Build(ALTObject* pBaseObject, Ogre::ManualObject* pObj);

};
