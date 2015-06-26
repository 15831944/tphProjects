/***************************************************************************************************/
#ifndef INC_DOTSCENEINFO_H
#define INC_DOTSCENEINFO_H
/***************************************************************************************************/
#include "dotSceneInterfacePrerequisites.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		namespace types
		{
			/// struct containing extended movable object info
			struct DOTSCENEINTERFACE_API MovableObjectEx
			{
			public:
				Ogre::MovableObject *pMovable;
				Ogre::Quaternion	rotation;
				Ogre::Vector3		position;
				Ogre::Vector3		scale;
				bool				isStatic;

			public:
				/// default ctor
				MovableObjectEx(Ogre::MovableObject *pMO, Ogre::Vector3 &pos, Ogre::Quaternion &rot, Ogre::Vector3 &sca, bool isStat);

			private:
				/// this ctor should not be used
				MovableObjectEx() {};
			};
			/***************************************************************************************************/
			/// progress listener abstract class
			/***************************************************************************************************/
			/// progress listener class, object that receive progress update from progressMaker classes it's registered with
			class DOTSCENEINTERFACE_API progressListener
			{
			public:
				virtual void onProgress(const Ogre::String &text) = 0;
				virtual ~progressListener(){}
			};
			/***************************************************************************************************/
			/// progress maker class
			/***************************************************************************************************/
			/// progress maker class, objects that makes progress can implement this, so can update progress listeners with progress info Strings
			class DOTSCENEINTERFACE_API progressMaker
			{
			private:
				typedef std::set<progressListener*> tPLSet;
			
			protected:
				tPLSet pls;
			
			public:
				virtual ~progressMaker(){}
				/// add a new progressListener to the stack
				virtual void addProgressListener(progressListener *pNewPL)
				{
					// check if it's already in first
					if (pls.find(pNewPL) == pls.end())
						pls.insert(pNewPL);
					else
						OGRE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "the specified progressListener is already in the list", "progressListener::addProgressListener");
				}
				/// update all registered progress listeners with the given message
				void updateProgressListeners(const Ogre::String &text)
				{
					tPLSet::iterator endIT = pls.end();
          String prefix = "[DotScene] ";
					for (tPLSet::iterator it = pls.begin(); it != endIT; ++it)
						(*it)->onProgress( prefix + text );
				}
				/// remove a specific progressListener from the stack
				void removeProgressListener(progressListener *pPL)
				{
					tPLSet::iterator it = pls.find(pPL);
					if (it != pls.end())
						pls.erase(it);
					else
						OGRE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "the specified progressListener is not in the list", "progressListener::removeProgressListener");
				}
			};
			/***************************************************************************************************/
			/// skyDome class
			/***************************************************************************************************/
			struct DOTSCENEINTERFACE_API skyDome
			{
				Ogre::String		material;// CDATA #REQUIRED
				float				curvature;// CDATA "10"
				float				tiling;// CDATA "8"
				float				distance;// CDATA "4000"
				bool				drawFirst;// (true | false) "true"
				bool				used;// = false;
				Ogre::Quaternion	orientation;

				skyDome()
				{
					material = "";
					curvature = 10;
					tiling = 8;
					distance = 4000;
					drawFirst = true;
					used = false;
					orientation = Ogre::Quaternion::IDENTITY;
				}
				const static skyDome DEFAULT;
			};
			/***************************************************************************************************/
			/// skyPlane class
			/***************************************************************************************************/
			struct DOTSCENEINTERFACE_API skyPlane
			{
				Ogre::String	material;// CDATA #REQUIRED
				float			planeX;// CDATA "0"
				float			planeY;// CDATA "-1"
				float			planeZ;// CDATA "0"
				float			planeD;// CDATA "5000"
				float			scale;// CDATA "1000"
				float			tiling;// CDATA "10"
				float			bow;// CDATA "0" 
				bool			drawFirst;// (true | false) "true"
				bool			used;// false

				skyPlane()
				{
					material = "";// CDATA #REQUIRED
					planeX = 0;
					planeY = -1;
					planeZ = 0;
					planeD = 5000;
					scale = 1000;
					tiling = 10;
					drawFirst= true;
					used = false;
				}
				const static skyPlane DEFAULT;
			};
			/***************************************************************************************************/
			/// skyBox class
			/***************************************************************************************************/
			struct DOTSCENEINTERFACE_API skyBox
			{
				Ogre::String		material;
				bool				drawFirst;
				bool				used;
				Ogre::Quaternion	orientation;
				float				distance;

				skyBox()
				{
					material = "";
					drawFirst = true;
					used = false;
					distance = 500;
					orientation = Ogre::Quaternion::IDENTITY;
				}
				const static skyBox DEFAULT;
			};
			/***************************************************************************************************/
			/// scene Fog class
			/***************************************************************************************************/
			struct DOTSCENEINTERFACE_API sceneFog
			{
				Ogre::FogMode		mode;
				Ogre::ColourValue	color;
				float				expDensity;
				float				linearStart;
				float				linearEnd;

				sceneFog()
				{
					mode = Ogre::FOG_NONE;
					color = Ogre::ColourValue::White;
					expDensity = 0.001f;
					linearStart = 0.0f;
					linearEnd = 1.0f;
				}
				const static sceneFog DEFAULT;
			};
			/***************************************************************************************************/
		}// namespace types
		/***************************************************************************************************/
		class DOTSCENEINTERFACE_API dotSceneInfo
		{
			friend class dotSceneLoader;
		public :
			static types::skyBox		*getSkyBox()			{return mBox.used ? &mBox : NULL;}
			static types::skyPlane		*getSkyPlane()			{return mPlane.used ? &mPlane : NULL;}
			static types::skyDome		*getSkyDome()			{return mDome.used ? &mDome : NULL;}
			static types::sceneFog		*getFog()				{return mFog.mode == Ogre::FOG_NONE ? NULL : &mFog;}
			static bool					hasFog()				{return mFog.mode == Ogre::FOG_NONE ? false : true;}
			static bool					hasSkyBox()				{return mBox.used;}
			static bool					hasSkyPlane()			{return mPlane.used;}
			static bool					hasSkyDome()			{return mDome.used;}
			static size_t				numTrisInScene()		{return mNumTris;}
			static Ogre::ColourValue	getAmbientLightColor()	{return mAmbLightColor;}
			static Ogre::ColourValue	getBackgroundColor()	{return mBgndColor;}
			static bool					isUpToDateWithScene()	{return mIsUpToDateWithScene;}
			/************************************************************
			* using those methods prefixed by _ u fuck'up the sceneInfo	*
			* if u do not use them correctly,						    *
			* ie. first update scene manager, then set those :)		    *
			************************************************************/
			/// set the upToDateFlag
			static void _setUpToDate(bool is)	{mIsUpToDateWithScene = is;}
			/// reset all scene info
			static void _reset();
			/// add a new movableObject info geometry to the DSI graph
			static void _addMovable(Ogre::MovableObject *pMO, Ogre::Vector3 &pos, Ogre::Quaternion &rot, Ogre::Vector3 &scale, bool isStatic);
			/// add a target SceneNode 
			/// (ie. an empty SN wich act as a target for lights/cameras) to the DSI graph
			static void _addTargetSceneNode(Ogre::SceneNode *pSN);
			/// log a loading error, implemented here so we can have a stack of errors that the client app can analize/show
			static void _logLoadError(const Ogre::String &txt);
			/// log a loading warning, implemented here so we can have a stack of warning that the client app can analize/show
			static void _logLoadWarning(const Ogre::String &txt);
			/// log a loading info, implemented here so we can have a stack of infos that the client app can analize/show
			static void _logLoadInfo(const Ogre::String &txt);
			/// set the ambient light
			static void	_setAmbientLightColor(const Ogre::ColourValue &cv)	{mAmbLightColor = cv;}
			/// set the background color
			static void	_setBackgroundColor(const Ogre::ColourValue &cv)	{mBgndColor = cv;}
			/// set the sky box
			static void _setSkyBox(const Ogre::String &materialName, float distance, const Ogre::Quaternion &orientation, bool drawFirst);
			/// set the sky dome
			static void _setSkyDome(const Ogre::String &materialName, float curvature, float tiling, float distance, const Ogre::Quaternion &orientation, bool drawFirst = true);
			/// set the sky plane
			static void _setSkyPlane(const Ogre::String &materialName, float planeX, float planeY, float planeZ, float planeD, float scale, float tiling, float bow, bool drawFirst = true);
			/// set the scene fog
			static void _setFog(Ogre::FogMode mode, Ogre::ColourValue color, float expDensity, float linearStart, float linearEnd);
			
		private:
			typedef std::map<Ogre::String,	types::MovableObjectEx*>	tMapMovableEx;
			typedef std::map<Ogre::String,	Ogre::SceneNode*>			tMapSceneNode;
			typedef std::vector<Ogre::String>							tVecLoadLog;

			static types::sceneFog		mFog;
			static types::skyBox		mBox;
			static types::skyPlane		mPlane;
			static types::skyDome		mDome;
			static size_t				mNumTris;
			static bool					mIsUpToDateWithScene;

			static Ogre::ColourValue	mAmbLightColor;
			static Ogre::ColourValue	mBgndColor;

			static tMapMovableEx		mMOStatic;
			static tMapMovableEx		mMODynamic;
			static tMapMovableEx		mMOAll;
			static tMapSceneNode		mSNTargets;
			static tVecLoadLog			mLoadLog;
		
		public:
			typedef	Ogre::MapIterator<tMapMovableEx>			itMovableEx;
			typedef	Ogre::MapIterator<tMapSceneNode>			itSceneNode;
			typedef	Ogre::VectorIterator<tVecLoadLog>			itLoadLog;

			static const itMovableEx	getStaticMovablesIterator()		{return itMovableEx(mMOStatic.begin(), mMOStatic.end());}
			static const itMovableEx	getDynamicMovablesIterator()	{return itMovableEx(mMODynamic.begin(), mMODynamic.end());}
			static const itMovableEx	getAllMovablesIterator()		{return itMovableEx(mMOAll.begin(), mMOAll.end());}
			static const itSceneNode	getTargetSceneNodesIterator()	{return itSceneNode(mSNTargets.begin(), mSNTargets.end());}
			static const itLoadLog		getSceneLoadingLog()			{return itLoadLog(mLoadLog.begin(), mLoadLog.end());}

			static bool isMovableStatic(const Ogre::String &name)		{return mMOStatic.find(name) != mMOStatic.end() ? true : false;}
			static bool isMovableStatic(const Ogre::MovableObject &mo)	{return isMovableStatic(mo.getName());}
			static bool isMovableStatic(const Ogre::MovableObject *pMO)	{assert(pMO); return isMovableStatic(pMO->getName());}

			static bool isMovableDynamic(const Ogre::String &name)		{return mMODynamic.find(name) != mMODynamic.end() ? true : false;}
			static bool isMovableDynamic(const Ogre::MovableObject &mo)	{return isMovableDynamic(mo.getName());}
			static bool isMovableDynamic(const Ogre::MovableObject *pMO){assert(pMO); return isMovableDynamic(pMO->getName());}

			static bool isTarget(const Ogre::String &name)	{return mSNTargets.find(name) != mSNTargets.end() ? true : false;}
			static bool isTarget(const Ogre::SceneNode &sn)	{return isTarget(sn.getName());}
			static bool isTarget(const Ogre::SceneNode *pSN){assert(pSN); return isTarget(pSN->getName());}
		};
		/***************************************************************************************************/
	} // namespace dsi
	/***************************************************************************************************/
} // namespace Ogre
/***************************************************************************************************/
#endif // #ifndef INC_DOTSCENEINFO_H
/***************************************************************************************************/
