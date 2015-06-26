#include "StdAfx.h"
#include ".\3dscenesnapshot.h"


#include <Common/Camera.h>
#include "ARCRenderTargetListener.h"

#include <Ogre/OgreTextAreaOverlayElement.h>
#include <Ogre/OgreTexture.h>

using namespace Ogre;

C3DSceneSnapShot::C3DSceneSnapShot()
	: m_nSnapshotSizeX(800)
	, m_nSnapshotSizeY(600)
	, m_bSnapshotDispARCLogo(true)
{

}

C3DSceneSnapShot::C3DSceneSnapShot( int nSnapshotSizeX, int nSnapshotSizeY, bool bSnapshotDispARCLogo /*= true*/ )
	: m_nSnapshotSizeX(nSnapshotSizeX)
	, m_nSnapshotSizeY(nSnapshotSizeY)
	, m_bSnapshotDispARCLogo(bSnapshotDispARCLogo)
{

}

C3DSceneSnapShot::~C3DSceneSnapShot(void)
{
}

const Ogre::String C3DSceneSnapShot::GetSnapshotTag() const
{
	return m_pSnapshotTimeTextOverlay->getCaption();
}

void C3DSceneSnapShot::SetSnapshotTag( Ogre::String strSnapshotTag )
{
	m_pSnapshotTimeTextOverlay->setCaption(strSnapshotTag);
}

bool C3DSceneSnapShot::InitSnapshotBasicOverlay()
{
	m_pSnapshotLogoOverlay = OgreUtil::createOrRetriveOverlay("C3DSceneSnapshot/Snapshot");
	if (m_pSnapshotLogoOverlay)
	{
		m_pSnapshotLogoOverlay->hide();
		OverlayContainer* pContainer = m_pSnapshotLogoOverlay->getChild("TimeTagPanel");
		pContainer->show();
		m_pSnapshotTimeTextOverlay = (TextAreaOverlayElement*)pContainer->getChild("TimeTag");
		if (m_pSnapshotTimeTextOverlay)
		{
			m_pSnapshotTimeTextOverlay->show();
			return true;
		}
	}
	return false;
}


#define TextNameSnapshot     _T("Text3DSceneSnapShot")


void C3DSceneSnapShot::TakeSnapshot(int nWidth,int nHeight,BYTE* pBits,const CCameraData& cameraData)//for make movie ogre
{
	// create texture
	int nFSAA = 0;

	TexturePtr texture = TextureManager::getSingleton().createManual( TextNameSnapshot,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		m_nSnapshotSizeX, m_nSnapshotSizeY, 0, PF_R8G8B8, TU_RENDERTARGET,
		NULL, false, nFSAA);
	RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
	rttTex->setAutoUpdated(true);
	rttTex->addListener(ARCRenderTargetListener::GetInstance());

	// add camera and view port
	SetupSnapshotRenderTarget(cameraData,rttTex);

	// take the snapshot
	if (m_bSnapshotDispARCLogo)
	{
		m_pSnapshotTimeTextOverlay->show();
		m_pSnapshotLogoOverlay->show();
	}

	//rttTex->update(false);
	Ogre::Root::getSingleton().renderOneFrame();

	EncodeIntoBuffer(texture, nWidth, nHeight, pBits);

	m_pSnapshotTimeTextOverlay->hide();
	m_pSnapshotLogoOverlay->hide();

	// remove
	TextureManager::getSingleton().remove(texture->getHandle());
}

void C3DSceneSnapShot::TakeSnapshot( const String& strFileName, bool isShowText )
{
	// create texture
	int nFSAA = 0;
// 	const ConfigOptionMap& configOps = Root::getSingleton().getRenderSystem()->getConfigOptions();
// 	ConfigOptionMap::const_iterator ite = configOps.find("FSAA");
// 	if (configOps.end() != ite)
// 	{
// 		const ConfigOption& configOp = ite->second;
// 		for (StringVector::const_iterator ite = configOp.possibleValues.begin();
// 			ite != configOp.possibleValues.end();ite++)
// 		{
// 			int nFSAA2 = atoi(ite->c_str());
// 			if (nFSAA2 > nFSAA)
// 				nFSAA = nFSAA2;
// 		}
// 	}

	TexturePtr texture = TextureManager::getSingleton().createManual( TextNameSnapshot,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		m_nSnapshotSizeX, m_nSnapshotSizeY, 0, PF_R8G8B8, TU_RENDERTARGET,
		NULL, false, nFSAA);
	RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
	rttTex->setAutoUpdated(false);
	rttTex->addListener(ARCRenderTargetListener::GetInstance());

	// add camera and view port
	SetupSnapshotRenderTarget(rttTex, isShowText);

	// take the snapshot
	if (m_bSnapshotDispARCLogo)
	{
		m_pSnapshotTimeTextOverlay->show();
		m_pSnapshotLogoOverlay->show();
	}

	rttTex->update(false);
	rttTex->writeContentsToFile(strFileName);
	m_pSnapshotTimeTextOverlay->hide();
	m_pSnapshotLogoOverlay->hide();

	// remove
	TextureManager::getSingleton().remove(texture->getHandle());
}

void C3DSceneSnapShot::TakeSnapshot( int nWidth,int nHeight,BYTE* pBits )
{
	// create texture
	int nFSAA = 0;

	TexturePtr texture = TextureManager::getSingleton().createManual( TextNameSnapshot,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		m_nSnapshotSizeX, m_nSnapshotSizeY, 0, PF_R8G8B8, TU_RENDERTARGET,
		NULL, false, nFSAA);
	RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
	rttTex->setAutoUpdated(false);
	rttTex->addListener(ARCRenderTargetListener::GetInstance());

	// add camera and view port
	SetupSnapshotRenderTarget(rttTex);

	// take the snapshot
	if (m_bSnapshotDispARCLogo)
	{
		m_pSnapshotTimeTextOverlay->show();
		m_pSnapshotLogoOverlay->show();
	}

	rttTex->update(false);

	EncodeIntoBuffer(texture, nWidth, nHeight, pBits);

	m_pSnapshotTimeTextOverlay->hide();
	m_pSnapshotLogoOverlay->hide();

	// remove
	TextureManager::getSingleton().remove(texture->getHandle());

}

void C3DSceneSnapShot::TakeSnapshot( int nWidth,int nHeight,BYTE* pBits,bool isShowText )
{
	// create texture
	int nFSAA = 0;

	TexturePtr texture = TextureManager::getSingleton().createManual( TextNameSnapshot,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		m_nSnapshotSizeX, m_nSnapshotSizeY, 0, PF_R8G8B8, TU_RENDERTARGET,
		NULL, false, nFSAA);
	RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();
	rttTex->setAutoUpdated(false);
	rttTex->addListener(ARCRenderTargetListener::GetInstance());

	// add camera and view port
	SetupSnapshotRenderTarget(rttTex,isShowText);

	// take the snapshot
	if (m_bSnapshotDispARCLogo)
	{
		m_pSnapshotTimeTextOverlay->show();
		m_pSnapshotLogoOverlay->show();
	}

	rttTex->update(false);

	EncodeIntoBuffer(texture, nWidth, nHeight, pBits);

	m_pSnapshotTimeTextOverlay->hide();
	m_pSnapshotLogoOverlay->hide();

	// remove
	TextureManager::getSingleton().remove(texture->getHandle());

}

Ogre::Camera* C3DSceneSnapShot::createOrRetrieveSnapshotCamera( const CString& strName,Ogre::SceneManager* pScene )
{
	return OgreUtil::createOrRetrieveCamera(strName, pScene);
}

void C3DSceneSnapShot::EncodeIntoBuffer( TexturePtr texture, int nWidth, int nHeight, BYTE* pBits )
{
	RenderTarget * rttTex = texture->getBuffer()->getRenderTarget();
	HardwarePixelBufferSharedPtr pixelbuffer = texture->getBuffer();
	int nPixelSize = (int)PixelUtil::getNumElemBytes(rttTex->suggestPixelFormat());	
	int bufferSize = nWidth * nHeight * (int)pixelbuffer->getDepth() * nPixelSize;
	BYTE* pTemBuffer = new BYTE[bufferSize];

	PixelBox pb(nWidth,nHeight,pixelbuffer->getDepth(),pixelbuffer->getFormat(),pTemBuffer);
	pixelbuffer->blitToMemory(Image::Box( 0 ,0 ,nWidth,nHeight),pb);

	for( int i =0;i<nHeight ;i++)
	{
		int nCurrentCol = 0;
		for( int j=0;j<nWidth*4;j+=4)
		{
			int nRow = nHeight-1-i;
			int nCol = j;
			pBits[i*nWidth*3+nCurrentCol] = pTemBuffer[nRow*nWidth*4+nCol];   
			pBits[i*nWidth*3+nCurrentCol+1] = pTemBuffer[nRow*nWidth*4+nCol+1]; 
			pBits[i*nWidth*3+nCurrentCol+2] = pTemBuffer[nRow*nWidth*4+nCol+2];
			nCurrentCol += 3;
		}
	}

	delete []pTemBuffer;
}

void C3DSceneSnapShot::SetupSnapshotRenderTarget( Ogre::RenderTarget* pRenderTarget )
{
	ViewPort vp = GetSnapshotViewPort();
	CCameraData cameraData;
	CCameraNode cameraScene(vp.getCamera());
	cameraScene.Save(cameraData); // prepare camera data

	Camera* pCameraSnapshot = OgreUtil::createOrRetrieveCamera(_T("C3DSceneSnapShot.SnapshotCamera"), vp.getScene());
	CCameraNode cameraSnapshot(pCameraSnapshot);
	cameraSnapshot.Load(cameraData);

	Viewport* v = pRenderTarget->addViewport( pCameraSnapshot );
	v->setBackgroundColour(vp.mvp->getBackgroundColour());
}

#define PERSP_VisibleFlag 64
void C3DSceneSnapShot::SetupSnapshotRenderTarget( Ogre::RenderTarget* pRenderTarget,bool isShowText)
{
	ViewPort vp = GetSnapshotViewPort();
	CCameraData cameraData;
	CCameraNode cameraScene(vp.getCamera());
	cameraScene.Save(cameraData); // prepare camera data

	Camera* pCameraSnapshot = OgreUtil::createOrRetrieveCamera(_T("C3DSceneSnapShot.SnapshotCamera"), vp.getScene());
	CCameraNode cameraSnapshot(pCameraSnapshot);
	cameraSnapshot.Load(cameraData);

	Viewport* v = pRenderTarget->addViewport( pCameraSnapshot );
	v->setBackgroundColour(vp.mvp->getBackgroundColour());
	ViewPort nVP(v);
	nVP.SetVisibleFlag(NormalVisibleFlag|PERSP_VisibleFlag);
	if(isShowText)
		nVP.addVisibilityFlags(ProcessorTagVisibleFlag);
	else
		nVP.removeVisibilityFlags(ProcessorTagVisibleFlag);
}

void C3DSceneSnapShot::SetupSnapshotRenderTarget( const CCameraData& cameraData,Ogre::RenderTarget* pRenderTarget )
{
	ViewPort vp = GetSnapshotViewPort();

	Camera* pCameraSnapshot = OgreUtil::createOrRetrieveCamera(_T("C3DSceneSnapShot.SnapshotCamera"), vp.getScene());
	CCameraNode cameraSnapshot(pCameraSnapshot);
	cameraSnapshot.Load(cameraData);

	Viewport* v = pRenderTarget->addViewport( pCameraSnapshot );
	v->setBackgroundColour(vp.mvp->getBackgroundColour());
}
