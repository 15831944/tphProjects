#pragma once
#include "3DSceneNode.h"
#include "InputAirside/AirRoute.h"
class CAirRoute;

class CRenderAirRoute3D :
	public C3DSceneNode
{
	SCENE_NODE(CRenderAirRoute3D,C3DSceneNode)
	void Setup3D(ALTObject* pBaseObj);
	
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj);


protected:
	void Build(ALTObject* pAirWayPoint, Ogre::ManualObject* pObj, Ogre::ManualObject* pLineObj);

};
