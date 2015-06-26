#pragma once
#include "3DSceneNode.h"

#include <InputAirside/MeetingPoint.h>

class CRenderMeetingPoint3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderMeetingPoint3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);

protected:
	void Build(CMeetingPoint* pMeetingPoint, Ogre::ManualObject* pObj);
	Taxiway* GetTaxiway(CMeetingPoint* pMeetingPoint) const;
	bool GetLocation(CMeetingPoint* pMeetingPoint, ARCPoint3& pos) const;


};

//#include "Terminal3DList.h"
//class CMeetingPoint;
//class CRenderMeetingPoint3DList : 
//	public CAirportObject3DList<CRenderMeetingPoint3DList,CMeetingPoint , CRenderMeetingPoint3D>
//{
//
//};
