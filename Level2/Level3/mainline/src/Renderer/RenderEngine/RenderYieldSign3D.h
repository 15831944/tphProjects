#pragma once
#include "3DSceneNode.h"

#include <InputAirside/YieldSign.h>


class CRenderYieldSign3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderYieldSign3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);

protected:
	void Build(YieldSign* pYieldSign, Ogre::ManualObject* pObj);

};
//
//#include "Terminal3DList.h"
//class YieldSign;
//class CRenderYieldSign3DList : 
//	public CAirportObject3DList<CRenderYieldSign3DList, YieldSign, CRenderYieldSign3D>
//{
//
//};
