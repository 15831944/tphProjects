#pragma once
#include "RenderScene3DNode.h"
class CLandsideArea;

class LandsideArea3D: public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideArea3D, CLayoutObject3DNode)
		PROTOTYPE_DECLARE(LandsideArea3D)
	void Update3D(CLandsideArea *pLandsideArea);
	virtual	void OnUpdate3D();
	CLandsideArea *GetArea();
	virtual void	StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void	OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void	EndDrag();
	bool IsDraggable()const{return true;}
};
