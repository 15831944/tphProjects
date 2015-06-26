#pragma once
#include "3DSceneNode.h"

#include <InputAirside/TollGate.h>


class CRenderTollGate3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderTollGate3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj );

protected:
	void Build(TollGate* pTollGate);
};

//#include "Terminal3DList.h"
//class TollGate;
//class CRenderTollGate3DList : 
//	public CAirportObject3DList<CRenderTollGate3DList,TollGate , CRenderTollGate3D>
//{
//
//};
