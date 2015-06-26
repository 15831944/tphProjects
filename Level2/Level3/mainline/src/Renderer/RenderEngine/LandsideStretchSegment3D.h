#pragma once
#include "renderscene3dnode.h"
#include "SceneState.h"
#include "CommonData/3DViewCommon.h"
#include "Common/ARCPipe.h"

class LandsideStretchSegment;

class LandsideStretchSegment3D :
	public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideStretchSegment3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideStretchSegment3D)

	virtual void OnUpdate3D();
	virtual void OnUpdateEditMode(BOOL b);

	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();

protected:	
	LandsideStretchSegment* getStretchSegment();
};
