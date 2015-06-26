#include "StdAfx.h"
#include "FloorSurfaceRender.h"

const String FloorSurfaceRender::StencilValue = _T("StencilValue");
FloorSurfaceRender* FloorSurfaceRender::m_pFSR = NULL;

FloorSurfaceRender* FloorSurfaceRender::getIntance()
{
	if(m_pFSR == NULL)
	{
		m_pFSR = new FloorSurfaceRender();
	}
	return m_pFSR;
}

void FloorSurfaceRender::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation) 
{
	m_curQueueGroup = queueGroupId;
}

void FloorSurfaceRender::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)  
{
	if(m_curQueueGroup == RenderFloors_Solid || m_curQueueGroup == RenderFloors_SurfaceMap)
	{
		Ogre::RenderSystem *rs = Ogre::Root::getSingleton().getRenderSystem();
		rs->setStencilCheckEnabled(false);
		rs->setStencilBufferParams();
	}	
}

void FloorSurfaceRender::notifyRenderSingleObject(Renderable* rend, const Pass* _pass, const AutoParamDataSource* source,
	const LightList* pLightList, bool suppressRenderStateChanges)
{
	Ogre::RenderSystem *rs = Ogre::Root::getSingleton().getRenderSystem();
	if (m_curQueueGroup == RenderFloors_Solid)
	{
		const Any& any = _pass->getUserObjectBindings().getUserAny(FloorSurfaceRender::StencilValue);
		if(!any.isEmpty())
		{
			int nStecilVal  = any_cast<int>(any);
			//rs->clearFrameBuffer(Ogre::FBT_STENCIL, Ogre::ColourValue::Black, 1.0,);
			rs->setStencilCheckEnabled(true);
			//rs->_setColourBufferWriteEnabled(false, false, false, false);
			rs->setStencilBufferParams(
				Ogre::CMPF_ALWAYS_PASS, // compare
				nStecilVal, // refvalue
				0xFFFFFFFF, // mask
				Ogre::SOP_REPLACE, /*Ogre::SOP_REPLACE*/Ogre::SOP_KEEP, // stencil fail, depth fail
				Ogre::SOP_REPLACE, // stencil pass + depth pass
				false); // two-sided operation? no
		}
	}		
	if(m_curQueueGroup == RenderFloors_SurfaceMap)
	{
		const Any& any = _pass->getUserObjectBindings().getUserAny(FloorSurfaceRender::StencilValue);
		if(!any.isEmpty())
		{
			int nStecilVal  = any_cast<int>(any);
			rs->setStencilCheckEnabled(true);
			rs->setStencilBufferParams(
				Ogre::CMPF_EQUAL, // compare
				nStecilVal, // refvalue
				0xFFFFFFFF, // mask
				Ogre::SOP_KEEP, Ogre::SOP_KEEP, // stencil fail, depth fail
				Ogre::SOP_KEEP, // stencil pass + depth pass
				false); // two-sided operation? no
		}
	}
}