#include "StdAfx.h"
#include ".\selectionmanager.h"
#include "RenderEngine.h"
#include <ogre\OgrePanelOverlayElement.h>


namespace Ogre
{
#define SHOWDEBUG_TEX 0

#define STARTINDX 2
#define CUSTOMPARA_INDEX 100 // should be preserved by system
#define SEL_MATRIAL "PlainColor_CG"
//#define SEL_MATRIAL_GL "PlainColor_GL"
#define SEL_MAT_INVISIBLE "PlainColor_Invisible"

#define SELBUFFER_TEXNAME "SelectionPassTex"

const static int deltaX=2;
const static int deltaY=2;
const static int TexWidth = deltaX;
const static  int TexHeight = deltaY;

const static int selectX = 2;
const static int selectY = 2;

//switch renderable mat
class MaterialSwitcher : public Ogre::MaterialManager::Listener
{
public:
	MaterialSwitcher(){
		selmat = MaterialManager::getSingleton().load(SEL_MATRIAL,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		selmat_invisible  = MaterialManager::getSingleton().load(SEL_MAT_INVISIBLE,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	virtual Technique* handleSchemeNotFound(unsigned short schemeIndex, 
		const String& schemeName, Material* originalMaterial, unsigned short lodIndex, 
		const Renderable* rend);

	Technique* getTech(int nColorIdx);
	Technique* getTechInvisible();
	Technique* getTechNoDCheck(int nColorIdx);	

protected:
	MaterialPtr selmat_invisible;
	//MaterialPtr selmat_gl;
	MaterialPtr selmat;
};



#if SHOWDEBUG_TEX
	//debug window
	inline void createRTTOverlays()
	{
		Overlay* mDebugOverlay  = OverlayManager::getSingleton().create("OverlayName");
		PanelOverlayElement* panel = (PanelOverlayElement*)OverlayManager::getSingleton().createOverlayElement("Panel","PanelName0");
		panel->setMetricsMode(GMM_PIXELS);
		panel->setPosition(10,10);
		panel->setDimensions(deltaX,deltaY);
		panel->setMaterialName("SelectionDebugMaterial");
		mDebugOverlay->add2D(panel);
		mDebugOverlay->show();
	}

	//debug material
	inline void updateRTTMaterial()
	{
		//create debug material
		ResourcePtr ret = MaterialManager::getSingleton().createOrRetrieve("SelectionDebugMaterial",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first;
		Material* pSelectBufMat = (Material*)ret.get();
		Pass* pas =  pSelectBufMat->getTechnique(0)->getPass(0);
		TextureUnitState* pTexStat = NULL;
		if(pas->getNumTextureUnitStates())
		{
			pTexStat = pas->getTextureUnitState(0);			
		}
		else
		{
			pTexStat = pas->createTextureUnitState();
		}	
		pTexStat->setTextureName(SELBUFFER_TEXNAME);
	}

#endif


	IndexColorSelectionBuffer::IndexColorSelectionBuffer()
	{
		//mpTexViewport = NULL;
		mpSelectionMatSwitcher = NULL;
	}

	SelectID IndexColorSelectionBuffer::SelectionPick( int x, int y, Viewport* pvp, uint32 mask   )
	{
		double vpWidth = pvp->getActualWidth();
		double vpHeight = pvp->getActualHeight();
		double left = pvp->getActualLeft();
		double top = pvp->getActualTop();

		SceneManager* pScene = pvp->getCamera()->getSceneManager();

		TexturePtr mpTex = updateTexture(TexWidth,TexHeight);
		if(mpTex.isNull())		
			return 0;		

		RenderTexture* pRenderTex = mpTex->getBuffer()->getRenderTarget();
		if(!pRenderTex)
			return 0;

		pRenderTex->setAutoUpdated(false);
		pRenderTex->setPriority(0);
		Camera* pCam = pvp->getCamera();
		Camera* pSelectCam = pCam->getSceneManager()->createCamera("SceneSelectCamera");
		pSelectCam->setCustomViewMatrix(true,pCam->getViewMatrix() );		

		const Matrix4& projMat = pCam->getProjectionMatrix();

		Matrix4 pickMat = Matrix4::IDENTITY;
		pickMat.setScale( Vector3(vpWidth/deltaX,vpHeight/deltaY,1.0) );
		pickMat.setTrans( Vector3( (vpWidth- 2*(x-left))/deltaX, -(vpHeight-2*(y-top))/deltaY,0 ) );

		mpTexViewport = pRenderTex->addViewport( pSelectCam );
		mpTexViewport->setShadowsEnabled(false);
		pSelectCam->setCustomProjectionMatrix(true, pickMat*projMat);

		mpTexViewport->setVisibilityMask( mask );
		mpTexViewport->setBackgroundColour(ColourValue::ZERO);
		mpTexViewport->setOverlaysEnabled(false);
		mpTexViewport->setClearEveryFrame(true);
		mpTexViewport->setMaterialScheme("you cant find me");

		//pRenderTex->addListener(&mRenderList);
		if(!mpSelectionMatSwitcher){  mpSelectionMatSwitcher = new MaterialSwitcher(); }
		MaterialManager::getSingleton().addListener(mpSelectionMatSwitcher);
		//TRACE("begin select draw\n");
		try
		{
			pRenderTex->update();
		}		
		catch (...)
		{
		}
		//TRACE("end select draw\n");
		//pRenderTex->removeListener(&mRenderList);
		MaterialManager::getSingleton().removeListener(mpSelectionMatSwitcher);

		pCam->getSceneManager()->destroyCamera(pSelectCam);
		pRenderTex->removeAllViewports();
		mpTexViewport = NULL;

		HardwarePixelBufferSharedPtr pixelbuffer = mpTex->getBuffer();
		int nPixelSize = (int)PixelUtil::getNumElemBytes(pixelbuffer->getFormat());	
		int bufferSize = selectX * selectY * (int)pixelbuffer->getDepth() * nPixelSize;
		BYTE mem[selectX*selectY*8];// = new BYTE[bufferSize];

		PixelBox pb(selectX,selectY,pixelbuffer->getDepth(),pixelbuffer->getFormat(),mem);
		int xOffset = (TexWidth-selectX)/2; int yOffset = (TexHeight-selectY)/2;
		pixelbuffer->blitToMemory(Image::Box( xOffset ,yOffset ,xOffset+selectX,yOffset+selectY),pb);			

		size_t ret_idx = 0;
		for(int i=0;i<(int)bufferSize;i+=nPixelSize)
		{			
			{
				size_t nidx = getColorIndex(mem[2+i],mem[1+i],mem[i],mem[3+i]);
				if(SelectionManager::getSingleton().IsValidID(nidx))
				{
					ret_idx = nidx;
					break;
				}
			}	
		}
		//delete mem;	
		return ret_idx;
		//pRenderTex->writeContentsToFile("C:\\debug.bmp");
	}

	TexturePtr IndexColorSelectionBuffer::updateTexture( int width, int height )
	{
		bool bNeedCreate = false;
		TexturePtr mpTex = TextureManager::getSingleton().getByName(SELBUFFER_TEXNAME);
		if(!mpTex.isNull())
		{
			if(mpTex->getWidth() !=  width)
				bNeedCreate = true;
			if(mpTex->getHeight() != height)
				bNeedCreate = true;

			if(bNeedCreate){
				mpTex->unload();
				TextureManager::getSingleton().remove(mpTex->getName());
			}
		}
		else
		{
			bNeedCreate = true;
		}

		if(bNeedCreate)
		{
			mpTex = TextureManager::getSingleton().createManual( SELBUFFER_TEXNAME,
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				TEX_TYPE_2D,
				width,
				height,
				0,
				PF_R8G8B8A8,
				TU_RENDERTARGET);	

#if SHOWDEBUG_TEX
			updateRTTMaterial();			
			createRTTOverlays();
#endif

		}
		return mpTex;
	}



	Ogre::Vector4 SelectionManager::getCustomParemeter( size_t val32 )
	{
		//Convert from 32bit pattern
		Ogre::Vector4 ret;
		// a
		ret.w = ((val32 >> 24) & 0xFF) / 255.0;

		// b
		ret.z = ((val32 >> 16) & 0xFF) / 255.0;

		// g
		ret.y = ((val32 >> 8) & 0xFF) / 255.0;

		// r
		ret.x = (val32 & 0xFF) / 255.0;
		return ret;
	}


	SelectID SelectionManager::Register( Ogre::MovableObject* pobj , int iorder)
	{
		if(!pobj)return -1;
		if(iorder==1)
		{
			pobj->removeVisibilityFlags(SelectableFirstFlag);
			pobj->addVisibilityFlags(SelectableFlag);
		}
		else if(iorder>1)
		{
			pobj->removeVisibilityFlags(SelectableFlag);
			pobj->addVisibilityFlags(SelectableFirstFlag);
		}
		else
			return -1;

		ObjectRegList::iterator itr =std::find(mSeletableObjList.begin(),mSeletableObjList.end(),pobj);
		size_t idx = itr - mSeletableObjList.begin()+STARTINDX;
		if(itr==mSeletableObjList.end())
		{
			mSeletableObjList.push_back(pobj);	
		}
		OgreUtil::SetCustomParameter(pobj,CUSTOMPARA_INDEX,getCustomParemeter(idx) );
		return idx;
	}

	MovableObject* SelectionManager::GetObject( SelectID colorIndex )
	{
		if(IsValidID(colorIndex))
		{
			return mSeletableObjList[colorIndex-STARTINDX];
		}

		return NULL;
	}

	SelectionManager* SelectionManager::msSingleton = NULL;
	//static SelectionManager* instSelectionManager= NULL;
	//SelectionManager& SelectionManager::getSingleton()
	//{
	//	//static SelectionManager inst;
	//	return *instSelectionManager;
	//}

	MovableObject* SelectionManager::SelectionPick( int x, int y, Viewport* pvp ) 
	{
		MovableObject* pObj  = GetObject( mSelectionBuffer.SelectionPick(x,y,pvp,SelectableFirstFlag) );
		if(!pObj)
			pObj = GetObject( mSelectionBuffer.SelectionPick(x,y,pvp,SelectableFlag) );
		return pObj;
	}

	bool SelectionManager::IsValidID( SelectID sid ) const
	{
		return 	sid>=STARTINDX && sid<mSeletableObjList.size()+STARTINDX;
	}

	void SelectionManager::UnRegister( Ogre::MovableObject* pobj )
	{
		ObjectRegList::iterator itr =std::find(mSeletableObjList.begin(),mSeletableObjList.end(),pobj);
		if( itr!=mSeletableObjList.end() )
			mSeletableObjList.erase(itr);
		OgreUtil::SetCustomParameter(pobj,CUSTOMPARA_INDEX,Vector4::ZERO);
		pobj->removeVisibilityFlags(SelectableFlag);
		pobj->removeVisibilityFlags(SelectableFirstFlag);
	}

	bool SelectionManager::HasObject( Ogre::MovableObject* pobj ) const
	{
		return std::find(mSeletableObjList.begin(),mSeletableObjList.end(),pobj)!=mSeletableObjList.end();
	}

	void SelectionManager::Clear()
	{
		mSeletableObjList.clear();
	}

	size_t IndexColorSelectionBuffer::getColorIndex( BYTE r,BYTE g,BYTE b,BYTE a )
	{
		uint32 val32 = 0;
		// Convert to 32bit pattern
		// (RGBA = 8888)

		// Red		
		val32 = a << 24;

		// Green
		val32 += b << 16;

		// Blue
		val32 += g << 8;

		// Alpha
		val32 += r;

		return val32;
	}

	IndexColorSelectionBuffer::~IndexColorSelectionBuffer()
	{
		delete mpSelectionMatSwitcher;
	}

	Technique* MaterialSwitcher::handleSchemeNotFound( unsigned short schemeIndex, const String& schemeName, Material* originalMaterial, unsigned short lodIndex, const Renderable* rend )
	{
		try
		{
			if(rend->getCustomParameter(CUSTOMPARA_INDEX)==Vector4::ZERO)
				return getTechInvisible();
		}
		catch(Ogre::Exception& e)//item not found
		{
			e.getDescription();
			//Renderable* pRender = const_cast<Renderable*> (rend);
			//pRender->setCustomParameter( CUSTOMPARA_INDEX,Vector4(0,0,0,0) );
			return getTechInvisible();
		}			
		return getTech(0);
	}

	Technique* MaterialSwitcher::getTech( int nColorIdx )
	{
		try//if(mpMat.isNull())
		{	
			//MaterialPtr mpMat;
		/*	if(RenderEngine::getSingleton().IsCurrentOpenGL())
				mpMat = MaterialManager::getSingleton().load(SEL_MATRIAL_GL,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			else*/
			//	mpMat = MaterialManager::getSingleton().load(SEL_MATRIAL,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			ASSERT(selmat.get());
			return selmat->getTechnique(0);
		}
		catch(Ogre::Exception&) //some error
		{
			return getTechInvisible();
		}
	}

	Technique* MaterialSwitcher::getTechInvisible()
	{
		//MaterialPtr mpMat = MaterialManager::getSingleton().load(SEL_MAT_INVISIBLE,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		ASSERT(selmat_invisible.get());
		return selmat_invisible->getTechnique(0);
	}

	Technique* MaterialSwitcher::getTechNoDCheck( int nColorIdx )
	{
		try//if(mpMat.isNull())
		{	
			//MaterialPtr mpMat;
			//if(RenderEngine::getSingleton().IsCurrentOpenGL())
			//	mpMat = MaterialManager::getSingleton().load(SEL_MATRIAL_GL,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			//else
			//	mpMat = MaterialManager::getSingleton().load(SEL_MATRIAL,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			ASSERT(selmat.get());
			return selmat->getTechnique(1);
		}
		catch(Ogre::Exception&)
		{
			return getTechInvisible();
		}
		return NULL;
	}
}

