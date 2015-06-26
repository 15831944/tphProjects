/***************************************************************************************************/
#ifndef INC_DOTSCENELOADER_H
#define INC_DOTSCENELOADER_H
/***************************************************************************************************/
#include "IDotSceneLoaderImpl.h"
//#include "OBTSingleton.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		class DOTSCENEINTERFACE_API dotSceneLoader : public Singleton<dotSceneLoader>, public types::progressMaker
		{
		public:
			dotSceneLoader();
			virtual ~dotSceneLoader();

			void load( const Ogre::String &fileName, 
				const Ogre::String &groupName, 
				SceneManager *pSceneManager, 
				RenderWindow *pRWin, 
				SceneNode *pRootNode = NULL, 
				bool doMaterials = false, 
				bool forceShadowBuffers = false, 
				bool updateDSInfo = false);

			void load( const String &fileName, 
				const String &groupName, 
				const String &sceneManagerName,
				SceneNode *pRootNode = NULL, 
				bool doMaterials = false, 
				bool forceShadowBuffers = false, 
				bool updateDSInfo = false);

			/// overide this to add the new registered listener to all implementations
			void addProgressListener(types::progressListener *pNewPL);

		private:
			typedef std::map<Ogre::String, IDotSceneLoaderImpl*> tMapDSLI;
			tMapDSLI	mDSLoaders;

			String mGroupName ;
		};
		/***************************************************************************************************/
	} // namespace dsi
	/***************************************************************************************************/
} // namespace Ogre
typedef Ogre::Singleton< Ogre::dsi::dotSceneLoader > dotSceneLoaderSingleton ;

template class DOTSCENEINTERFACE_API Ogre::Singleton< Ogre::dsi::dotSceneLoader > ;
/***************************************************************************************************/
#endif // #ifndef INC_DOTSCENELOADER_H
/***************************************************************************************************/
