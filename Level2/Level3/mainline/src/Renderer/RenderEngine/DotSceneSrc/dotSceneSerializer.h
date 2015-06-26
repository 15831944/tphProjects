/***************************************************************************************************/
#ifndef INC_DOTSCENESERIALIZER_H
#define INC_DOTSCENESERIALIZER_H
/***************************************************************************************************/
/***************************************************************************************************/
#include "dotSceneInfo.h"
#include "Common\tinyxml\tinyxml.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/** 
		anything in the scene (nodes, lights, cameras, movables, entities, billboardSets...)
		that use this string as a part of the 
		name or as a name will not be serialized by the dotScene serializer
		**/
		static Ogre::String nonDSIPrefix = "nonDSI.";
		/***************************************************************************************************/
		class DOTSCENEINTERFACE_API dotSceneSerializer : public Singleton<dotSceneSerializer>, public types::progressMaker
		{
		public:
			dotSceneSerializer();
			virtual ~dotSceneSerializer();
			dotSceneSerializer &getSingleton(void);
			void saveScene(const Ogre::String &fileName, SceneManager *pSceneManager, bool doStatic,bool doEnvironment = true);
			TiXmlDocument *parseScene(SceneManager *pSceneManager, bool doStatic);

		private:
			unsigned long		mID;
			Ogre::SceneManager	*mpSceneManager;

			void _removeEmptySceneNodes(TiXmlDocument *pXMLDoc);
			void __removeEmptySceneNodes(TiXmlElement *pNodes);
			void _doNodes(TiXmlElement *pParent, bool doStatic);
			void _doNode(TiXmlElement *pParent, Ogre::SceneNode *pNode, bool doStatic);
			void _doEnvironment(TiXmlElement *pParent);
			void _doEntity(TiXmlElement *pParent, Entity *pEnt, bool doStatic);
			void _doLight(TiXmlElement *pParent, Light *pLight);
			void _doCamera(TiXmlElement *pParent, Camera *pCam);
			void _doBillboardSet(TiXmlElement *pParent, BillboardSet *pBS);
			void _doBillboard(TiXmlElement *pParent, Billboard *pBill);
			void _doSkyBox(TiXmlElement *pParent, types::skyBox *pSB);
			void _doSkyDome(TiXmlElement *pParent, types::skyDome *pSD);
			void _doSkyPlane(TiXmlElement *pParent, types::skyPlane *pSP);
			void _doClipping(TiXmlElement *pParent, Real fcd, Real ncd);
			void _doFog(TiXmlElement *pParent);
	    std::string getNameWithoutDelimiterOMK(const std::string& name);
    };
		/***************************************************************************************************/
	} // namespace dsi
	/***************************************************************************************************/
} // namespace Ogre
/***************************************************************************************************/
#endif // #ifndef INC_DOTSCENESERIALIZER_H
/***************************************************************************************************/
