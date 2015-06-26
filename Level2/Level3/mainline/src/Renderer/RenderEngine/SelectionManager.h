#pragma once

#include "../Ogre/OgreMaterialManager.h"
#include "../Ogre/OgreRenderTargetListener.h"

namespace Ogre
{


	

	/*class SelectionRenderListener : public RenderTargetListener
	{
	public:
		void preRenderTargetUpdate(const RenderTargetEvent& evt)
		{			
			MaterialManager::getSingleton().addListener(&mMatSwitch);
		}
		void postRenderTargetUpdate(const RenderTargetEvent& evt)
		{
			MaterialManager::getSingleton().removeListener(&mMatSwitch);
		}
	public:
		MaterialSwitcher mMatSwitch;
	};*/

	class MaterialSwitcher;

	/** select by color index
	*/
	class IndexColorSelectionBuffer
	{
	public:
		IndexColorSelectionBuffer();
		~IndexColorSelectionBuffer();
		SelectID SelectionPick(int x, int y, Viewport* pvp, uint32 flag);
		Viewport* getTextureViewport()const{ return mpTexViewport; }

		static SelectID getColorIndex(BYTE r,BYTE g,BYTE b,BYTE a);		
	protected:
		Viewport* mpTexViewport; //last select viewport
		//SelectionRenderListener* mp;
		MaterialSwitcher* mpSelectionMatSwitcher;
		//TexturePtr mpTex;
		TexturePtr updateTexture(int width, int height);
	};
	
	class SelectionManager : public Singleton<SelectionManager>
	{
	public:
		//call this after the shape is rebuild
		SelectID Register(Ogre::MovableObject* pobj, int iorder);
		void UnRegister(Ogre::MovableObject* pobj);

		MovableObject* GetObject(SelectID colorIndex);
		bool IsValidID(SelectID sid)const;
		bool HasObject(Ogre::MovableObject* pobj)const;

		void Clear();

		static Ogre::Vector4 getCustomParemeter(size_t val32);	
		//static SelectionManager& getSingleton();

		MovableObject* SelectionPick(int x, int y, Viewport* pvp);
		IndexColorSelectionBuffer mSelectionBuffer; //selection buffer use color
	
	protected:
		typedef std::vector<Ogre::MovableObject*> ObjectRegList;
		ObjectRegList mSeletableObjList;		 
	};

	#define REG_SELECT(x, iorder ) SelectionManager::getSingleton().Register(x, iorder)
	#define UNREG_SELECT(x) SelectionManager::getSingleton().UnRegister(x)
}
