#pragma once
#include "RenderScene3DNode.h"
class LandsideRoundabout;
class ARCPath3;
class ARCPipe;

class LandsideRoundAboout3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideRoundAboout3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideRoundAboout3D)

	void					OnUpdate3D();
	LandsideRoundabout* getAbout();
	void GetPipeMidPath( const ARCPipe& pipe,ARCPath3& path, double dDist );

private:
	void UpdateRadiusEdit(BOOL bShow);
	void UpdateShape(const CPoint2008& ptC,BOOL bEdit);
};


class AboutRadiusControlPoint3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(AboutRadiusControlPoint3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(AboutRadiusControlPoint3D)

	void OnUpdate3D(LandsideRoundAboout3D& parent,bool bInner);
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;

	BOOL isInnerRadius()const;
	LandsideRoundabout* getAbout();
protected:
	

};
