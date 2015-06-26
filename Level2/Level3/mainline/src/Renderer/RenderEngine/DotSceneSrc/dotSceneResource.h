/***************************************************************************************************/
#ifndef __dotSceneResourceHeader__
#define __dotSceneResourceHeader__
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		class dotSceneResourceManager; // fwd decl...
		class dotSceneResourcePtr;
		/***************************************************************************************************/
		/// implemented just to have access to scenes in Ogre search paths
		class dotSceneResource : public Ogre::Resource
		{
			friend class dsi::dotSceneResourceManager;
		public:
			/** Overridden from Resource.
			*/
			void loadImpl(void);
			/** Unloads the material, frees resources etc.
			@see
			Resource
			*/
			void unloadImpl(void);
			/// @copydoc Resource::calculateSize
			size_t calculateSize(void) const { return 0; } // TODO 
			// 
			dotSceneResource(ResourceManager *creator, const String &name, ResourceHandle handle, const String &group, bool isManual=false, ManualResourceLoader *loader=0);
			~dotSceneResource();
			// get data strean
			DataStreamPtr getDataStream();
		private:
			dotSceneResource();
			DataStreamPtr mData;
		};
		/** Specialisation of SharedPtr to allow SharedPtr to be assigned to dotSceneResourcePtr 
		@note Has to be a subclass since we need operator=.
		We could templatise this instead of repeating per Resource subclass, 
		except to do so requires a form VC6 does not support i.e.
		ResourceSubclassPtr<T> : public SharedPtr<T>
		*/
		class dotSceneResourcePtr : public SharedPtr<dotSceneResource> 
		{
		public:
			dotSceneResourcePtr() : SharedPtr<dotSceneResource>() {}
			explicit dotSceneResourcePtr(dotSceneResource* rep) : SharedPtr<dotSceneResource>(rep) {}
			dotSceneResourcePtr(const dotSceneResourcePtr& r) : SharedPtr<dotSceneResource>(r) {} 
			dotSceneResourcePtr(const ResourcePtr& r);
			dotSceneResourcePtr& operator=(const ResourcePtr& r);
		};
		/***************************************************************************************************/
	}
	/***************************************************************************************************/
}
/***************************************************************************************************/
#endif
/***************************************************************************************************/
