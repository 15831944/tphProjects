#include "StdAfx.h"
#include ".\sharpmodebackground.h"

using namespace Ogre;

const static String Sharp_bgTex = "SharpTexture";

void CreateMaterial()
{
	ResourcePtr ret = MaterialManager::getSingleton().createOrRetrieve(Sharp_bgTex,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first;
	
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
	pTexStat->setTextureName(Sharp_bgTex);
}

void UpdateRenderTexture(int width, int height)
{
	bool bNeedCreate = false;
	TexturePtr mpTex = TextureManager::getSingleton().getByName(Sharp_bgTex);
	if(!mpTex.isNull())
	{
		if(mpTex->getWidth() !=  width
			|| mpTex->getHeight() !=height)
		{
			mpTex->unload();
			TextureManager::getSingleton().remove(Sharp_bgTex);
		}	
		else
		{
			return ;
		}
	}		
	
	mpTex = TextureManager::getSingleton().createManual( Sharp_bgTex,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			width,
			height,
			0,
			PF_R8G8B8A8,
			TU_RENDERTARGET);	

	CreateMaterial();

}