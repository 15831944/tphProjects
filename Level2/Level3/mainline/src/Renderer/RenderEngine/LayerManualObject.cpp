#include "StdAfx.h"
#include ".\layermanualobject.h"

using namespace Ogre;

void LayerManualObject::begin( const String& materialName, RenderOperation::OperationType opType /*= RenderOperation::OT_TRIANGLE_LIST*/, const String & groupName /*= ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME*/ )
{
	if (mCurrentSection)
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"You cannot call begin() again until after you call end()",
			"ManualObject::begin");
	}
	mCurrentSection = OGRE_NEW LayerManualObjectSection(this, materialName, opType, groupName);
	mCurrentUpdating = false;
	mCurrentSection->setUseIdentityProjection(mUseIdentityProjection);
	mCurrentSection->setUseIdentityView(mUseIdentityView);
	mSectionList.push_back(mCurrentSection);
	mFirstVertex = true;
	mDeclSize = 0;
	mTexCoordIndex = 0;
}


//-----------------------------------------------------------------------------
String LayerManualObjectFactory::FACTORY_TYPE_NAME = "LayerManualObject";
//-----------------------------------------------------------------------------
const String& LayerManualObjectFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------------
MovableObject* LayerManualObjectFactory::createInstanceImpl(
	const String& name, const NameValuePairList* params)
{
	return OGRE_NEW LayerManualObject(name);
}
//-----------------------------------------------------------------------------
void LayerManualObjectFactory::destroyInstance( MovableObject* obj)
{
	OGRE_DELETE obj;
}

Real Ogre::LayerManualObject::LayerManualObjectSection::getSquaredViewDepth( const Ogre::Camera * pcam ) const
{
	Real orgin = ManualObjectSection::getSquaredViewDepth(pcam);
	return orgin;
}

Ogre::LayerManualObject::LayerManualObjectSection::LayerManualObjectSection( ManualObject* parent, const String& materialName, RenderOperation::OperationType opType, const String & groupName /*= ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME*/ ) 
:ManualObjectSection(parent,materialName,opType,groupName)
{

}