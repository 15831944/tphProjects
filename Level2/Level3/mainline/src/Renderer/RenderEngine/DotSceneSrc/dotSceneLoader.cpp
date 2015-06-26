#include "stdafx.h"
/***************************************************************************************************/
#include "dotSceneLoader.h"
/**************************************************************************************************/
#include "dotSceneResourceManager.h"
/***************************************************************************************************/
#include "dotSceneLoaderImpl-0.2.0.h"
/***************************************************************************************************/

namespace Ogre
{
	/***************************************************************************************************/
	template<> dsi::dotSceneLoader *Singleton<dsi::dotSceneLoader>::msSingleton = 0;
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		dotSceneLoader::dotSceneLoader()
		{
			// I register here all loaderImplementations. maybe it's not the best place, but I suppose
			// it should work even without breaking the interface after compiling...
			// ie. after adding a new implementation and adding it here the client app should not require
			// to be recompiled to against the new DLL library, at least I hope so :(
			// ***************************************************************************
			// IT IS VERY IMPORTANT TO INSERT THE NEWEST AVAILABLE IMPLEMENTATION LAST !!!
			// ***************************************************************************
			mDSLoaders.insert(tMapDSLI::value_type("0.2.0", new dsi::dotSceneLoaderImpl020()));
		}
		/***************************************************************************************************/
		dotSceneLoader::~dotSceneLoader()
		{
			tMapDSLI::iterator it = mDSLoaders.begin();
			for (; it != mDSLoaders.end(); ++it)
				delete it->second;
			// clean'up all
			mDSLoaders.clear();
			// must call this to clear the internal scene graph
			dotSceneInfo::_reset();
		}
		/***************************************************************************************************/
		void dotSceneLoader::load( 
			const String &fileName, 
			const String &groupName, 
			SceneManager *pSceneManager, 
			RenderWindow *pRWin, 
			SceneNode *pRootNode, 
			bool doMaterials, 
			bool forceShadowBuffers, 
			bool updateDSInfo) 
		{ 
			// for scene nesting
			if ( groupName != "parentGroup" )
			{
				mGroupName = groupName ;
			}

			// we must atleast have one loader registered, so check this first 
			if (mDSLoaders.empty()) 
				OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "there is no registered sceneLoaderImplementation available, this should not happen :(", "dotSceneLoader::load"); 
			// load the .scene 
			TiXmlDocument *pXMLDoc = new TiXmlDocument(fileName); 
			if (!pXMLDoc->LoadFile()) 
			{ 
				pXMLDoc->ClearError(); 

				DataStreamPtr pStream ;
				try
				{
					// try to look for the file in the resource paths... 
					pStream = ResourceGroupManager::getSingleton().openResource(fileName, mGroupName); 
				}
				catch ( Ogre::FileNotFoundException& e )
				{
					dotSceneInfo::_logLoadError( e.getFullDescription() ) ;
					return;
				}

				if(pStream->size()) 
				{ 
					String s = pStream->getAsString() + "\0"; 
					pXMLDoc->Parse(s.c_str()); 

					// check for errors 
					if (pXMLDoc->Error()) 
					{ 
						String errDesc = pXMLDoc->ErrorDesc(); 
						delete pXMLDoc; 
						OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "something went wrong while loading the .scene u specified :( : \n" + errDesc, "dotSceneLoader::load"); 
					} 
				} 
				else 
					OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "something went wrong while loading the .scene u specified :(", "dotSceneLoader::load"); 
			} 
			// say what we're doing to the interested public 
			this->updateProgressListeners("loading '" + fileName + "'"); 
			//TiXmlElement *pElem = NULL; 
			TiXmlElement *pRootElem = pXMLDoc->RootElement(); 
			// check version first, fail if it's not what we want 
			String ver = StringUtil::BLANK; 
			ver = pRootElem->Attribute("formatVersion") ? pRootElem->Attribute("formatVersion") : StringUtil::BLANK; 
			// find a suitable loader implementation 
			if (ver == StringUtil::BLANK) 
			{ 
				// not found default to the newest version available 
				dotSceneInfo::_logLoadWarning(".scene file does not have a versionFormat attribute, will default/trye with the newest..."); 
				mDSLoaders.begin()->second->load(pXMLDoc, pSceneManager, pRWin, mGroupName, pRootNode, doMaterials, forceShadowBuffers, updateDSInfo); 
			} 
			else 
			{ 
				// try to find exact match first 
				tMapDSLI::iterator it = mDSLoaders.find(ver); 
				if (it == mDSLoaders.end()) 
				{ 
					// not found default to the newest version available 
					dotSceneInfo::_logLoadWarning(".scene file does not have a versionFormat attribute, will default/trye with the newest..."); 
					mDSLoaders.begin()->second->load(pXMLDoc, pSceneManager, pRWin, mGroupName, pRootNode, doMaterials, forceShadowBuffers, updateDSInfo); 
				} 
				// use the loaderImpl to load the scene 
				else 
				{
					try
					{
						it->second->load(pXMLDoc, pSceneManager, pRWin, mGroupName, pRootNode, doMaterials, forceShadowBuffers, updateDSInfo); 
					}
					catch ( Ogre::FileNotFoundException& e )
					{
						dotSceneInfo::_logLoadError( e.getFullDescription() ) ;
						assert( false ) ;
					}
				}
			} 
			// clean all that we made dirty 
			delete pXMLDoc; 
		} 

		/***************************************************************************************************/
		void dotSceneLoader::load( const String &fileName, 
			const String &groupName, 
			const String &sceneManagerName,
			SceneNode *pRootNode, 
			bool doMaterials, 
			bool forceShadowBuffers, 
			bool updateDSInfo )
		{
			SceneManager* sceneManager( Root::getSingleton().getSceneManager( sceneManagerName ) ) ;
			assert( ( sceneManager != NULL ) && "Incorrect sceneManagerName" ) ;
			load( fileName, groupName, sceneManager, Root::getSingleton().getAutoCreatedWindow(), pRootNode, doMaterials, forceShadowBuffers, updateDSInfo ) ;
		}
		/***************************************************************************************************/
		void dotSceneLoader::addProgressListener(types::progressListener *pNewPL)
		{
			tMapDSLI::iterator it = mDSLoaders.begin();
			for (; it != mDSLoaders.end(); ++it)
				it->second->addProgressListener(pNewPL);
		}
		/***************************************************************************************************/
	}// namespace dsi
	/*******************************************************************************************************/
}// namespace Ogre
/*******************************************************************************************************/
