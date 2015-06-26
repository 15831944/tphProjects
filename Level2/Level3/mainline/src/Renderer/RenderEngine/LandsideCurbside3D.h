#pragma once
#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
#include "Common/ARCPipe.h"

class LandsideCurbSide;
class LandsideStretchSegment;
class CubrsideControlPoint3D;
class LandsideLinkStretchObject;

class LandsideCurbside3D: public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideCurbside3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideCurbside3D)

	virtual void OnUpdate3D();
	
	//
	//virtual void OnUpdateEditMode(BOOL b);
	//build a 3d object accord to the position you picked on the stretch

	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();

	virtual void MoveAlongStretch(const Vector3& MouseMove);
protected:	
	LandsideCurbSide* getCurbside();

public:
	
};


class ControlPath3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(ControlPath3D, CRenderScene3DNode);
	void Update3D(const CPath2008* path, const CPoint2008& ptCenter);

	CPath2008* getPath();
};
class ControlPoint3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(ControlPoint3D, CRenderScene3DNode)
		PROTOTYPE_DECLARE(ControlPoint3D);
	void Update3D(const CPath2008* path, int idx);

	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
	{
		SetInDragging(TRUE);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const{ return true; }


	CPath2008* getPath();
	int getIndex();

};
