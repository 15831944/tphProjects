/***************************************************************************************************/
#ifndef INC_IDOTSCENELOADERIMPL_H
#define INC_IDOTSCENELOADERIMPL_H
/***************************************************************************************************/
#include "dotSceneInfo.h"
/***************************************************************************************************/
#include <common\tinyxml\tinyxml.h>
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		class IDotSceneLoaderImpl : public types::progressMaker
		{
		public:
			/// method load wich must be implemented in every versionSpecificImplementation of the loader
			virtual void load(
				TiXmlDocument *pXMLDoc, 
				SceneManager *pSceneManager, 
				RenderWindow *pRWin, 
				const String &fileName,
				SceneNode *pRootNode = NULL, 
				bool doMaterials = false, 
				bool forceShadowBuffers = false, 
				bool updateDSInfo = false) = 0;
		
		protected:
			String	mGroupName; // we need this to get the group name for loading
		};
		/***************************************************************************************************/
	} // namespace dsi
	/***************************************************************************************************/
} // namespace Ogre
/***************************************************************************************************/
#endif // #ifndef INC_IDOTSCENELOADERIMPL_H
/***************************************************************************************************/
