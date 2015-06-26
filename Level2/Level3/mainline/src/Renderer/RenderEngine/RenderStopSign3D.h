#pragma once
#include "3DSceneNode.h"


#include <InputAirside/StopSign.h>


class CRenderStopSign3D : 
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderStopSign3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);

protected:
	void Build(StopSign* pStopSign, Ogre::ManualObject* pObj);
};

//#include "Terminal3DList.h"
//class StopSign;
//class CRenderStopSign3DList : 
//	public CAirportObject3DList<CRenderStopSign3DList, StopSign, CRenderStopSign3D>
//{
//
//};
