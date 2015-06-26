#ifndef __LandsideTaxiQueue3D_H__
#define __LandsideTaxiQueue3D_H__

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
#include "../../Landside/LandsideTaxiQueue.h"

class LandsideTaxiQueue;

class LandsideTaxiQueue3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideTaxiQueue3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideTaxiQueue3D)

	void					OnUpdate3D();

	LandsideTaxiQueue*		getTaxiQueue();
	
	//dynamic drag
	void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	void EndDrag();

	void MoveAlongStretch(Vector3 MouseMove);

protected:
	void					UpdateWaitingPath(LandsideTaxiQueue* pBusStation, CPoint2008& center);

	void					_buildTaxiQueue(CPath2008& path, CPoint2008& center);
};
#endif