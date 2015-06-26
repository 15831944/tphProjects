#ifndef __LandsideCrosswalk3D_H__
#define __LandsideCrosswalk3D_H__

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
#include "../../Landside/LandsideCrossWalk.h"

//class LandsideCrosswalk;

class LandsideCrosswalk3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideCrosswalk3D, CLayoutObject3DNode)
		PROTOTYPE_DECLARE(LandsideCrosswalk3D)

	virtual void OnUpdate3D();

	//
	virtual void OnUpdateEditMode(BOOL b);


	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();

	LandsideCrosswalk* getCrosswalk();

	void SetLight(Cross_LightState eState);
	void SetRed(Ogre::Entity *light);
	void SetGreen(Ogre::Entity *light);
	void SetYellow(Ogre::Entity *light);

};

#endif