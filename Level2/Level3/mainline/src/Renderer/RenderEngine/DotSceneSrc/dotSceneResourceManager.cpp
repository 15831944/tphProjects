/***************************************************************************************************/
#include "stdafx.h"
/***************************************************************************************************/
#include "dotSceneResourceManager.h"
/***************************************************************************************************/
namespace Ogre 
{
	/***************************************************************************************************/
	template<> dsi::dotSceneResourceManager *Ogre::Singleton<dsi::dotSceneResourceManager>::msSingleton = 0;
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		dotSceneResourceManager::dotSceneResourceManager()
		{
			// Resource type
			mResourceType = "dotScene";
			// Register with resource group manager
			ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
		}
		/***************************************************************************************************/
		dotSceneResourceManager::~dotSceneResourceManager()
		{
		}
		/***************************************************************************************************/
		Resource *dotSceneResourceManager::createImpl(const String& name, ResourceHandle handle, 
			const String& group, bool isManual, ManualResourceLoader* loader,
            const NameValuePairList* params)
		{
			return new dotSceneResource(this, name, handle, group, isManual, loader);
		}
		/***************************************************************************************************/
		dotSceneResourceManager& dotSceneResourceManager::getSingleton(void)
		{
			return Ogre::Singleton<dotSceneResourceManager>::getSingleton();
		}
		/***************************************************************************************************/
	}
	/***************************************************************************************************/
}
/***************************************************************************************************/
