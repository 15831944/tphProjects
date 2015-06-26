#pragma once
#include "CustomizedRenderQueue.h"
#include "..\Ogre\include\OGRE\OgreRenderable.h"
#include "..\Ogre\include\OGRE\OgreRenderObjectListener.h"
#include "..\Ogre\include\OGRE\OgreRenderQueueListener.h"
using namespace Ogre;

class FloorSurfaceRender : public RenderObjectListener, public RenderQueueListener
{
public:
	static FloorSurfaceRender *m_pFSR;
	const static Ogre::String StencilValue;
	Ogre::uint8 m_curQueueGroup;

	FloorSurfaceRender():m_curQueueGroup(-1){m_nStencilVal = 0;}
	static FloorSurfaceRender* getIntance();
	void IncreaseStencilValue(){m_nStencilVal++;}
	int GetStencilValue(){return m_nStencilVal;}
	void SetStencilValue(int v){m_nStencilVal = v;}
	void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
	void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);  
	void notifyRenderSingleObject(Renderable* rend, const Pass* _pass, const AutoParamDataSource* source, 
		const LightList* pLightList, bool suppressRenderStateChanges);
private:
	int m_nStencilVal;
};