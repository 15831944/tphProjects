#pragma once
//#include "terminal3dobject.h"
//#include "Airside3DObjectWrapper.h"
#include <InputAirside/Deicepad.h>
#include "3DSceneNode.h"

class CRenderDeicePad3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderDeicePad3D,C3DSceneNode);

	virtual void Update3D(ALTObject* pBaseObj);

protected:
	void Build(DeicePad* pDeicepad);

private:
	/*C3DSceneNode m_shapeStandBar;
	C3DSceneNode m_shapeTruck1;
	C3DSceneNode m_shapeTruck2;*/
};

//#include "Terminal3DList.h"
//class DeicePad;
//class CRenderDeicePad3DList : 
//	public CAirportObject3DList<CRenderDeicePad3DList,DeicePad , CRenderDeicePad3D>
//{
//
//};
