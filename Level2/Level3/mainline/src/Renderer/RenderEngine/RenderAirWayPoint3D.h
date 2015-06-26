#pragma once
#include "3DSceneNode.h"
#include "InputAirside\AirWayPoint.h"


class AirWayPoint;

class CRenderAirWayPoint3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderAirWayPoint3D,C3DSceneNode)

public:
	virtual void Setup3D( AirWayPoint *pWayPointInput );
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj);

	
protected:
	void Build(ALTObject* pBaseObject, Ogre::ManualObject* pObj);
	CPoint2008 GetLocation(  AirWayPoint *pWayPointInput  ) const;

};

