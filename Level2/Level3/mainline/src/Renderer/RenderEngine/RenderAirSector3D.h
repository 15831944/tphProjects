#pragma once
#include "3DSceneNode.h"
#include "InputAirside/AirSector.h"


class AirSector;

class CRenderAirSector3D :
	public C3DSceneNode
{
	SCENE_NODE(CRenderAirSector3D,C3DSceneNode)
	void Setup3D(ALTObject* pBaseObj);
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj);

protected:
	void Build(ALTObject* pBaseObject, Ogre::ManualObject* pObj,const CPoint2008& offset);

};
