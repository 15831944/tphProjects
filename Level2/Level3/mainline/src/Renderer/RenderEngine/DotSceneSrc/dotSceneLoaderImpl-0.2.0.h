/***************************************************************************************************/
#ifndef INC_IDOTSCENELOADERIMPL020_H
#define INC_IDOTSCENELOADERIMPL020_H
/***************************************************************************************************/
#include "IDotSceneLoaderImpl.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		class dotSceneLoaderImpl020 : public IDotSceneLoaderImpl
		{
		public:
			dotSceneLoaderImpl020();
			~dotSceneLoaderImpl020();
			/// the only thing we must implement in loader is load ;)
			void load(
				TiXmlDocument *pXMLDoc, 
				SceneManager *pSceneManager, 
				RenderWindow *pRWin, 
				const String &groupName,
				SceneNode *pRootNode = NULL, 
				bool doMaterials = false, 
				bool forceShadowBuffers = false, 
				bool updateDSInfo = false);

		private:
			struct sHWGPUBuffer
			{
				/// members
				Ogre::HardwareBuffer::Usage usage;
				bool						shadow;
				/// def. ctor
				sHWGPUBuffer()
				{
					usage = Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;
					shadow = true;
				}
			};
			sHWGPUBuffer *_getHwGpuBuffer(TiXmlElement *pNode);

			void _doNodes(TiXmlElement *pNode);
			void _doNode(SceneNode *pParent, TiXmlElement *pNode);
			void _doAnimations(SceneNode *pParent, TiXmlElement *pNode);
			void _doAnimation(SceneNode *pParent, TiXmlElement *pNode);
	    void _doKeyframe( NodeAnimationTrack* track, 	TiXmlElement *pNode);
      void _doEnvironment(TiXmlElement *pNode);
			void _doEntity(SceneNode *pParent, TiXmlElement *pNode);
			void _doLight(SceneNode *pParent, TiXmlElement *pNode);
			void _doViewport(SceneNode *pParent, TiXmlElement *pNode);
			void _doCamera(SceneNode *pParent, TiXmlElement *pNode);
			void _doBillboardSet(SceneNode *pParent, TiXmlElement *pNode);
			void _doBillboard(BillboardSet *pParent, TiXmlElement *pNode);
      void _doParticleSystem(SceneNode *pParent, TiXmlElement *pNode);
      void _doPlane(SceneNode *pParent, TiXmlElement *pNode);
			void _doSkyBox(TiXmlElement *pNode);
			void _doSkyDome(TiXmlElement *pNode);
			void _doSkyPlane(TiXmlElement *pNode);
			void _doClipping(TiXmlElement *pNode, Real &near, Real &far);
			void _doFog(TiXmlElement *pNode);
			void _doTerrain(TiXmlElement *pNode);
			void _doExternals(TiXmlElement *pNode);
			void _doItem(TiXmlElement *pNode);
			void _doLightAttenuation(TiXmlElement *pElem, Real &range, Real &constant, Real &linear, Real &quadratic);
			void _doLightRange(TiXmlElement *pElem, Real &inner, Real &outer, Real &falloff);
			void _doScene(SceneNode *pParent, TiXmlElement *pNode);
			void _updateDSI(Ogre::MovableObject *pMO, Ogre::Vector3 &pos, Ogre::Quaternion &rot, Ogre::Vector3 &scale, bool isStatic);
      //Add Shadow
			void _doShadowProperties(TiXmlElement *pNode);
			// those only queue parsing of targets later...
			void _doSNLTarget(TiXmlElement *pElem, SceneNode *pSN);
			void _doSNTTarget(TiXmlElement *pElem, SceneNode *pSN);
			void _doCamLTarget(TiXmlElement *pElem, Camera *pCam);
			void _doCamTTarget(TiXmlElement *pElem, Camera *pCam);

			// those parse targets ...
			void __doSNLTarget(TiXmlElement *pElem, SceneNode *pSN);
			void __doSNTTarget(TiXmlElement *pElem, SceneNode *pSN);
			void __doCamLTarget(TiXmlElement *pElem, Camera *pCam);
			void __doCamTTarget(TiXmlElement *pElem, Camera *pCam);

			bool _getAttribute(TiXmlElement *pElem, const String &attribute, String &attributeValue);
			bool _getAttribute(TiXmlElement *pElem, const String &attribute, bool &attributeValue);
			bool _getAttribute(TiXmlElement *pElem, const String &attribute, Real &attributeValue);
      //Modify Name according OMK syntaxe and SceneGraph nodes
      Ogre::String setName(const Ogre::String & original, const Ogre::String& type, Ogre::SceneManager* smgr);

			Ogre::String _doFile(TiXmlElement *pNode);
			Ogre::String _getNodeName(TiXmlElement *pNode);
			Ogre::String _autoName();
       
		private:
			SceneNode		*mpRoot;
			SceneManager	*mpSceneManager;
			RenderWindow	*mpWin;
			bool			mDoMats;
			bool			mShadow;
			bool			mDoInfo;
			std::map< String, unsigned short> _viewportsNameToIndexMap ;
		};
		/***************************************************************************************************/
	} // namespace dsi
	/***************************************************************************************************/
} // namespace Ogre
/***************************************************************************************************/
#endif // #ifndef INC_IDOTSCENELOADERIMPL020_H
/***************************************************************************************************/
