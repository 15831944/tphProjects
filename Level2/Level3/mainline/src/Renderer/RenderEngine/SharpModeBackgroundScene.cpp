#include "StdAfx.h"
#include ".\SharpModeBackGroundScene.h"
#include "OgreConvert.h"
#include "OgreUtil.h"
#include "ShapeBuilder.h"
#include "MaterialDef.h"

using namespace Ogre;

const static CString SharpModeBgMat = "SharpModeBackgroundMat";
const static String SharpModeBgTexture = "SharpModeBgTextue";

void CSharpModeBackGroundScene::Setup()
{
	
	//setup material
	MaterialPtr material = OgreUtil::createOrRetrieveMaterial(SharpModeBgMat);
	Technique* technique = material->createTechnique();
	technique->createPass();
	technique->getPass(0)->setLightingEnabled(false);
	technique->getPass(0)->createTextureUnitState(SharpModeBgTexture);
	
	//setup draw rect
	Rectangle2D* rect = new Rectangle2D(true);
	rect->setCorners(-1.0, 1.0, 1.0, -1.0);
	rect->setRenderQueueGroup(RENDER_QUEUE_BACKGROUND);
	rect->setBoundingBox(AxisAlignedBox(-100000.0 * Vector3::UNIT_SCALE, 100000.0 * Vector3::UNIT_SCALE));	
	rect->setMaterial(SharpModeBgMat.GetString());
	getSceneManager()->getRootSceneNode()->attachObject(rect);
	//set up  texture

	
}


const static String SharpModeScene = "SharpModeScene";

//void CSharpModeBackGroundScene::UpdateTexture()
//{
//	//check the texture
//	TexturePtr texture = TextureManager::getSingleton().createManual("BackgroundTex", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 600, 600, 0, PF_R8G8B8, TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
//
//	TexturePtr texture = TextureManager::getSingleton().getByName(textTurename.GetString());
//	if(texture.isNull())
//		texture = TextureManager::getSingleton().createManual( textTurename.GetString(), 
//		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
//		1024, 1024, 0, PF_R8G8B8A8, TU_RENDERTARGET );
//	RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();	
//
//	//
//	SceneManager* scene = rttTex->create
//	CCameraNode camera = OgreUtil::createOrRetrieveCamera(_T("ZCamera"), pScene);
//	CCameraData camdata;
//	camera.Load(camdata);
//
//	//
//	Viewport *v = rttTex->addViewport( camera.mpCamera );
//	v->setClearEveryFrame( true );
//	v->setBackgroundColour(ColourValue(0,0,0,0));
//
//
//
//	rttTex->update();	
//
//}
//

