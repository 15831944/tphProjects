#pragma once
#include "renderscene3dnode.h"
#include "Common/Path2008.h"
class LandsideDecisionPoint;

class LandsideDecisionPoint3D :
	public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideDecisionPoint3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideDecisionPoint3D)

	void	OnUpdate3D();
	void	OnUpdateEditMode(BOOL b);
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	LandsideDecisionPoint* getLinkStretchObject();


	//LandsideRoundabout* getAbout();
private:	
	void UpdateShape(BOOL bEdit);
	void BuildSidePath();

	CPath2008 m_centerPath;
	CPath2008 m_leftPath;
	CPath2008 m_rightPath;
};
