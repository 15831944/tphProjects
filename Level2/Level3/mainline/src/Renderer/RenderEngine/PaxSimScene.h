#pragma once
#include "3DScene.h"
#include "RenderScene3DNode.h"

 

class RENDERENGINE_API PaxSimScene : public C3DSceneBase	
{
public:
	PaxSimScene(void);

	void Setup();
	~PaxSimScene(void);

	virtual BOOL OnLButtonDblClk(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnMouseMove(const MouseSceneInfo& mouseInfo);
};
