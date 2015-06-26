#pragma once
#include "RenderScene3DNode.h"
//class LandsideRoundabout;
#include "Landside/LandsideControlDevice.h"
class LandsideControlDevice3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideControlDevice3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideControlDevice3D)

	void	OnUpdate3D();
	void	OnUpdateEditMode(BOOL b);
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	LandsideControlDevice* getLinkStretchObject();


	//LandsideRoundabout* getAbout();
private:	
	void UpdateShape(BOOL bEdit);

};

