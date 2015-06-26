#pragma once
#include "3DSceneNode.h"
#include <InputAirside/StartPosition.h>

class CRenderStartPosition3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderStartPosition3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj );
	virtual void Update3D(ALTObjectDisplayProp* pDispObj );

protected:
	void Build(CStartPosition* pStartPosition, Ogre::ManualObject* pObj);
	Taxiway* GetTaxiway(CStartPosition* pStartPosition) const;
	bool GetLocation(CStartPosition* pStartPosition, ARCPoint3& pos) const;
};

//#include "Terminal3DList.h"
//class CStartPosition;
//class CRenderStartPosition3DList : 
//	public CAirportObject3DList<CRenderStartPosition3DList, CStartPosition, CRenderStartPosition3D>
//{
//
//};
