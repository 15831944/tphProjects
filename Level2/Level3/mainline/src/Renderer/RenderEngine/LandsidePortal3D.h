#pragma once
#include "RenderScene3DNode.h"
class CLandsidePortal;

class LandsidePortal3D: public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsidePortal3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsidePortal3D)
	void Update3D(CLandsidePortal *pPortal);
	virtual	void OnUpdate3D();
	CLandsidePortal *GetPortal();
	virtual void	StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void	OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void	EndDrag();
	bool IsDraggable()const{return true;}
};
