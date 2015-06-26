#include "StdAfx.h"
#include ".\modelsnapshot.h"

#include <common/FileInDB.h>
#include "./3DScene.h"
#include "ogreutil.h"
#include ".\ARCRenderTargetListener.h"
#include ".\RenderEngine.h"
#define TextNameX _T("RttTexX")
#define TextNameY _T("RttTexY")
#define TextNameZ _T("RttTexZ")
#define TextNameP _T("RttTexP")

#define CameraName _T("SnapShotCamera")
#define SnapObj _T("SnapObject")
using namespace Ogre;

bool CModelSnapShot::DoSnapeShot( CModel& model )
{

	CString tmpFolder = model.msTmpWorkDir;
	//prepare scene
	C3DSceneBase scene;
	scene.MakeSureInited();
	if(!scene.IsValid())
		return false;

	ResourceFileArchive::AddDirectory(tmpFolder,model.mguid); 
	Ogre::Entity* pEnt = OgreUtil::createOrRetrieveEntity(SnapObj,model.GetMeshFileName(), scene.getSceneManager() );
	ASSERT(pEnt);
	if(!pEnt)
		return false;

	scene.GetRoot().AddObject(pEnt);
	Real dRadius = pEnt->getBoundingRadius();
	if(dRadius<1000)
		scene.GetRoot().GetSceneNode()->setScale(1000/dRadius,1000/dRadius,1000/dRadius);
	//const AxisAlignedBox& boundBox0 = pEnt->getBoundingBox();
	scene.GetRoot().GetSceneNode()->_updateBounds();
	//const AxisAlignedBox& boundBox1 = scene.GetRoot().GetSceneNode()->_getWorldAABB();
	
	DoNode(scene.GetRoot(),tmpFolder);
	return true;

}

void CModelSnapShot::DoNode(C3DNodeObject& node, CString outfolder)
{


	const Ogre::AxisAlignedBox& boundBox = node.GetSceneNode()->_getWorldAABB();
	Ogre::Vector3 vCenter = boundBox.getCenter();
	Ogre::Vector3 vLen = boundBox.getMaximum() - boundBox.getMinimum();
	double dMaxDim = vLen.length();//std::max(vLen.x, std::max(vLen.y, vLen.z) );

	node.AddVisibleFlag(SnapCameraVisibleFlag,true);

	{//prepare texture x
		TexturePtr texture = TextureManager::getSingleton().createManual( TextNameX, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
			dSizeX, dSizeY, 0, PF_R8G8B8, TU_RENDERTARGET );
		RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
		rttTex->setAutoUpdated(false);
		rttTex->addListener(ARCRenderTargetListener::GetInstance());

		Camera* pCamera = OgreUtil::createOrRetrieveCamera(CameraName, node.GetScene() );
		Viewport *v = rttTex->addViewport( pCamera );
		v->setClearEveryFrame( true );
		v->setBackgroundColour(ColourValue::Black);
		v->setVisibilityMask(SnapCameraVisibleFlag);

		CCameraNode camera(pCamera);	
		{ //set front view -x
			double distToCenter = dMaxDim;
			camera.LookAt( ARCVECTOR(vCenter+Vector3::UNIT_X*distToCenter), ARCVECTOR(vCenter),ARCVECTOR(Vector3::UNIT_Z) );
			camera.Set2D(dMaxDim,dMaxDim);
			camera.SetNearFar(1,distToCenter*2);
			rttTex->update(false);
			rttTex->writeContentsToFile( (outfolder+STR_THUMBFILE_X).GetString() );
		}	

		TextureManager::getSingleton().remove(texture->getHandle());
	}
	{//prepare texture y
		TexturePtr texture = TextureManager::getSingleton().createManual( TextNameY, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
			dSizeX, dSizeY, 0, PF_R8G8B8, TU_RENDERTARGET );
		RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
		rttTex->setAutoUpdated(false);
		rttTex->addListener(ARCRenderTargetListener::GetInstance());

		Camera* pCamera = OgreUtil::createOrRetrieveCamera(CameraName,  node.GetScene() );
		Viewport *v = rttTex->addViewport( pCamera );
		v->setClearEveryFrame( true );
		v->setBackgroundColour(ColourValue::Black);
		v->setVisibilityMask(SnapCameraVisibleFlag);
		CCameraNode camera(pCamera);	
		{ //set front view -y
			double distToCenter = dMaxDim;
			camera.LookAt( ARCVECTOR(vCenter+Vector3::UNIT_Y*distToCenter), ARCVECTOR(vCenter), ARCVECTOR(Vector3::UNIT_Z) );
			camera.Set2D(dMaxDim,dMaxDim);
			camera.SetNearFar(1,distToCenter*2);
			rttTex->update(false);
			rttTex->writeContentsToFile( (outfolder+STR_THUMBFILE_Y).GetString() );	
		}	

		TextureManager::getSingleton().remove(texture->getHandle());
	}	

	{//prepare texture z
		TexturePtr texture = TextureManager::getSingleton().createManual( TextNameZ, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
			dSizeX, dSizeY, 0, PF_R8G8B8, TU_RENDERTARGET );
		RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
		rttTex->setAutoUpdated(false);
		rttTex->addListener(ARCRenderTargetListener::GetInstance());

		Camera* pCamera = OgreUtil::createOrRetrieveCamera(CameraName,  node.GetScene() );
		Viewport *v = rttTex->addViewport( pCamera );
		v->setClearEveryFrame( true );
		v->setBackgroundColour(ColourValue::Black);
		v->setVisibilityMask(SnapCameraVisibleFlag);
		CCameraNode camera(pCamera);	
		{ //set left view -z
			double distToCenter = dMaxDim;
			camera.LookAt( ARCVECTOR(vCenter+Vector3::UNIT_Z*distToCenter), ARCVECTOR(vCenter),ARCVECTOR(Vector3::UNIT_X) );
			camera.Set2D(dMaxDim,dMaxDim);
			camera.SetNearFar(1,distToCenter*2);
			rttTex->update(false);
			rttTex->writeContentsToFile( (outfolder+STR_THUMBFILE_Z).GetString() );
		}

		TextureManager::getSingleton().remove(texture->getHandle());
	}	


	{//prepare texture
		TexturePtr texture = TextureManager::getSingleton().createManual( TextNameP, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
			dSizeX, dSizeY, 0, PF_R8G8B8, TU_RENDERTARGET );
		RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
		rttTex->setAutoUpdated(false);
		rttTex->addListener(ARCRenderTargetListener::GetInstance());

		Camera* pCamera = OgreUtil::createOrRetrieveCamera(CameraName,  node.GetScene() );
		Viewport *v = rttTex->addViewport( pCamera );
		v->setClearEveryFrame( true );
		v->setBackgroundColour(ColourValue::Black);
		v->setVisibilityMask(SnapCameraVisibleFlag);
		CCameraNode camera(pCamera);	
		{//set 
			Vector3 vCamdir(1,1,0.5);vCamdir.normalise();
			double distToCenter = dMaxDim;
			Vector3 vUp = vCamdir.crossProduct(Vector3::UNIT_Z).crossProduct(vCamdir);
			camera.LookAt( ARCVECTOR(vCenter+vCamdir*distToCenter),ARCVECTOR(vCenter),ARCVECTOR(vUp) );
			camera.Set2D(dMaxDim,dMaxDim);
			camera.SetNearFar(1,distToCenter*2);
			rttTex->update(false);
			rttTex->writeContentsToFile( (outfolder+STR_THUMBFILE_P).GetString() );
		}

		TextureManager::getSingleton().remove(texture->getHandle());
	}	

	node.RemoveVisibleFlag(SnapCameraVisibleFlag,true);

}