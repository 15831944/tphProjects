#pragma once

#include <common/ARCPath.h>

class CRender3DScene;
class MouseSceneInfo;

class IMouse3DState
{
public:
	IMouse3DState(){
		m_hFallbackWnd = NULL;
		m_lFallbackParam = NULL;
	}
	virtual ~IMouse3DState(){}


	virtual CString getHintText()const=0;

	void SetFallbackWnd(HWND fbwnd, LPARAM reseaon)
	{
		m_hFallbackWnd = fbwnd;
		m_lFallbackParam =reseaon;
	};
	HWND m_hFallbackWnd;
	LPARAM m_lFallbackParam;
};

class R3DSceneDistanceMeasure;
class R3DScenePickPoints;
class R3DScenePlaceMarker;
class R3DSceneIntersectionLinkStretch;
class R3DSceneSelectLaneNodes;
class R3DScenePickStretchPos;
class R3DScenePickStretchSides;

class IRenderSceneMouseState : public IMouse3DState
{
public:
	virtual void OnMouseMove(CRender3DScene& scene,const MouseSceneInfo& mouseInfo){};
	virtual void OnPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo){}
	virtual void OnLastPick(CRender3DScene& scene,const MouseSceneInfo& mouseInfo){}
	virtual void OnDoneMouseOp(CRender3DScene& scene){}


public://
	virtual R3DSceneDistanceMeasure* toDistanceMeasureState(){ return NULL; }
	virtual R3DScenePickPoints* toPickPoints(){ return NULL; }
	virtual R3DScenePlaceMarker* toPlaceMaker(){ return NULL; }
	virtual R3DSceneIntersectionLinkStretch* toLinkstretch(){ return NULL; }
	virtual R3DSceneSelectLaneNodes* toSelectLaneNodes(){ return NULL; } 
	virtual R3DScenePickStretchPos* toPickStretchPos(){ return NULL; }
	virtual R3DScenePickStretchSides* toPickStretchSides(){ return NULL; }

	static IRenderSceneMouseState* createState(int nType);
};

