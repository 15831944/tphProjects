#pragma once
#include "3DSceneNode.h"


#include <InputAirside/VehiclePoolParking.h>

class CRenderPoolParking3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderPoolParking3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj );
	virtual void Update3D(ALTObjectDisplayProp* pDispObj );


};

//#include "Terminal3DList.h"
//class VehiclePoolParking;
//class CRenderPoolParking3DList : 
//	public CAirportObject3DList<CRenderPoolParking3DList,VehiclePoolParking , CRenderPoolParking3D>
//{
//
//};
