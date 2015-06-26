#include "StdAfx.h"
#include "FloorAlignMarker.h"
#include "OgreUtil.h"

using namespace Ogre;

bool CFloorAlignMarker::Load(Ogre::SceneManager* pManager)
{
	if (mpNode)
	{
		Entity * pEnt = OgreUtil::createOrRetrieveEntity(GetIDName(),"FloorAlignMarker.mesh",pManager);
		if (pEnt)
		{
			OgreUtil::AttachMovableObject(pEnt,mpNode);
			return true;
		}
	}
	return false;
}