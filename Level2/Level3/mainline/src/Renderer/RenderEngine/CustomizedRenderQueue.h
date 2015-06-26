#pragma once

#include <Ogre/OgreRenderQueue.h>

enum CustomizedRenderQueue
{
	RenderFloors_Solid = Ogre::RENDER_QUEUE_3,
	RenderFloors_SurfaceMap =Ogre::RENDER_QUEUE_4,

	RenderObject_Base = Ogre::RENDER_QUEUE_MAIN, // do not use this

	RenderObject_Surface,
	RenderObject_Mark,
	RenderObject_EditHelper, // edit help object, for example, edit point and edit line which have more selection priority
	RenderObject_AnimationObject,
	RenderObject_Tag,

	RenderObject_Limit, // do not use this
};


// if compile error here, render queue definition has exceeded the limit Ogre::RENDER_QUEUE_6
const static int g_sCustomizedRenderQueueLimitCheck[Ogre::RENDER_QUEUE_6 - RenderObject_Limit + 1];
