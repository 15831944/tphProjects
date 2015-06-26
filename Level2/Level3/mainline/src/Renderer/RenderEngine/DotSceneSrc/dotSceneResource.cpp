/***************************************************************************************************/
#include "stdafx.h"
/***************************************************************************************************/
#include "dotSceneResource.h"
/***************************************************************************************************/
#include "dotSceneResourceManager.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		dotSceneResourcePtr::dotSceneResourcePtr(const ResourcePtr& r) : SharedPtr<dotSceneResource>()
		{
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<dotSceneResource*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
		/***************************************************************************************************/
		dotSceneResourcePtr	&dotSceneResourcePtr::operator=(const ResourcePtr& r)
		{
			if (pRep == static_cast<dotSceneResource*>(r.getPointer()))
				return *this;
			release();
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<dotSceneResource*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}
		/***************************************************************************************************/
		dotSceneResource::dotSceneResource()
		{
		}
		/***************************************************************************************************/
		dotSceneResource::~dotSceneResource()
		{
			unload(); 
		}
		/***************************************************************************************************/
		dotSceneResource::dotSceneResource(ResourceManager *creator, const String &name, ResourceHandle handle, const String &group, bool isManual, ManualResourceLoader *loader)
			:Resource(creator, name, handle, group, isManual, loader)
		{
		}
		/***************************************************************************************************/
		void dotSceneResource::loadImpl()
		{
			if (mData.isNull())
				 mData = ResourceGroupManager::getSingleton().openResource(mName, mGroup);
		}
		/***************************************************************************************************/
		void dotSceneResource::unloadImpl()
		{
			if (!mData.isNull())
			{
			//	delete mData.getPointer();
				mData.setNull();
			}
		}
		/***************************************************************************************************/
		DataStreamPtr dotSceneResource::getDataStream()
		{
			if (isLoaded() && !mData.isNull())
				return mData;
			else
				return DataStreamPtr();
		}
	}
	/***************************************************************************************************/
}
/***************************************************************************************************/
