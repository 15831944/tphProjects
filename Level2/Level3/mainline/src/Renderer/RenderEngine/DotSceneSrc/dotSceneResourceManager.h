/***************************************************************************************************/
#ifndef _DotSceneResourceManager_H__
#define _DotSceneResourceManager_H__
/***************************************************************************************************/
#include "dotSceneResource.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		/** Manages the locating and loading of .scene and .bin files.
		Like other ResourceManager specialisations it manages the location and loading
		of a specific type of resource, in this case files containing .scene XML data data.
		NB Implemented just to have access to scenes in Ogre search paths
		*/
		class dotSceneResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<dotSceneResourceManager>
		{
		public:
			OGRE_AUTO_MUTEX dotSceneResourceManager();
			~dotSceneResourceManager();
			/** Creates a DotScene resource - mainly used internally. */
			Resource	*createImpl(const String& name, ResourceHandle handle, 
				const String& group, bool isManual, ManualResourceLoader* loader,
				const NameValuePairList* params);
			/** Override standard Singleton retrieval.
			Why do we do this? Well, it's because the Singleton implementation is in a .h file,
			which means it gets compiled into anybody who includes it. This is needed for the Singleton
			template to work, but we actually only want it compiled into the implementation of the
			class based on the Singleton, not all of them. If we don't change this, we get link errors
			when trying to use the Singleton-based class from an outside dll.</p>
			This method just delegates to the template version anyway, but the implementation stays in this
			single compilation unit, preventing link errors.
			*/
			static dotSceneResourceManager &getSingleton(void);
		};
		/***************************************************************************************************/
	}
	/***************************************************************************************************/
}
/***************************************************************************************************/
#endif
/***************************************************************************************************/
