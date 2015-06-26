#include "StdAfx.h"
#include ".\3dscene.h"
#include "Camera.h"
#include <Common/ARCVector.h>

#include "RenderEngine.h"
#include "ShapeBuilder.h"
#include "SceneNodeCoordinate.h"
#include "SceneState.h"

#include "./ogreutil.h"
#include "./ARCRenderTargetListener.h"
#include "OgreBillboardObject.h"
#include <common/ARCStringConvert.h>
#include "common/Guid.h"
//
using namespace Ogre;

void C3DSceneBase::Destroy()
{
    if(mpScene)
	{
		mpScene->removeListener(&mState->mNodeUpdater);		
		RenderEngine::getSingleton().DestroySceneManager(mpScene);
		mpScene = NULL;
	}
}

void C3DSceneBase::RemoveSceneNode( const CString& nodeName )
{
	if(mpScene->hasSceneNode(nodeName.GetString()))
	{
		SceneNode* pNode = mpScene->getSceneNode(nodeName.GetString());
		SceneNode* pParent = pNode->getParentSceneNode();
		if(pParent)
			pParent->removeChild(pNode);
	}
}

C3DSceneBase::C3DSceneBase()
{
	mpScene = NULL;	
	mState = new SceneState;	
}

SceneNode* C3DSceneBase::AddSceneNode( const CString& nodeName )
{
	Ogre::SceneNode* pNode = OgreUtil::createOrRetrieveSceneNode(nodeName,mpScene);
	if(pNode->getParent())
		pNode->getParent()->removeChild(pNode);

	mpScene->getRootSceneNode()->addChild(pNode);
	return pNode;
}


SceneNode* C3DSceneBase::AddSceneNodeTo( const CString& nodeName, const CString& parentNodeName )
{
	SceneNode* pNode = OgreUtil::createOrRetrieveSceneNode(nodeName,mpScene);
	SceneNode* pParent = OgreUtil::createOrRetrieveSceneNode(parentNodeName,mpScene);
	OgreUtil::AttachNodeTo(pNode,pParent);
	return pNode;
}


bool C3DSceneBase::MakeSureInited()
{
	if(mpScene)
		return true;
	else
	{		
		mpScene = RenderEngine::getSingleton().CreateSceneManager();
		if(mpScene){
			mpScene->addListener(&mState->mNodeUpdater);
			InitShareObject();
			//set up lighting
			mpScene->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f, 1.0f));
			SetupCameraLight();
		}
	}
	return mpScene!=NULL;
}

void C3DSceneBase::EnableAutoScale( const CString& objName, double dWinSize )
{
	if(mState)
	{
		mState->mNodeUpdater.AddNode(objName.GetString(),dWinSize);
	}
}

C3DSceneBase::~C3DSceneBase()
{
	Destroy();
	delete mState;
	mState = NULL;
}


//
Ogre::Plane C3DSceneBase::getFocusPlan() const
{
	return Ogre::Plane(Vector3::UNIT_Z,Vector3::ZERO);
}




BOOL C3DSceneBase::OnMouseMove( const MouseSceneInfo& mouseInfo  )
{
	mState->m_LastMoseMoveInfo = mouseInfo;
	return TRUE;
}
BOOL C3DSceneBase::OnLButtonDown( const MouseSceneInfo& mouseInfo  )
{
	mState->m_lastLBtnDownInfo = mouseInfo;
	return TRUE;
}

BOOL C3DSceneBase::OnMButtonDown( const MouseSceneInfo& mouseInfo  )
{	
	mState->m_lastMbtnDownInfo = mouseInfo;
	return TRUE;
}

BOOL C3DSceneBase::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	mState->m_lastRBtnDownInfo = mouseInfo;
	return TRUE;
}

Ogre::SceneNode* C3DSceneBase::GetSceneNode( const CString& nodeName )
{
	if(!mpScene)
		return NULL;
	return OgreUtil::createOrRetrieveSceneNode(nodeName,mpScene);
}

CCameraNode C3DSceneBase::GetCamera( const CString& camName ) const
{
	return OgreUtil::createOrRetrieveCamera(camName,mpScene);
}

void C3DSceneBase::Clear()
{
	if(mpScene){ mpScene->clearScene(); }
}

C3DNodeObject C3DSceneBase::GetSel3DNodeObj( const MouseSceneInfo &mouseInfo )
{
	if (mouseInfo.mOnObject)
	{
		return C3DNodeObject(mouseInfo.mOnObject->getParentSceneNode());
	}
	return C3DNodeObject();
}

void C3DSceneBase::SetupCameraLight()
{
	Ogre::Light* l = OgreUtil::createOrRetrieveLight( StrCameraLight, getSceneManager() );	
	l->setSpecularColour( ColourValue::White );
	l->setDiffuseColour( ColourValue(0.9f, 0.9f, 0.9f, 1.0f )  );
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-Vector3::UNIT_Z);	
	l->setAttenuation(8000,1,0.0005,0);
	l->setVisible(false);
}

void C3DSceneBase::InitShareObject()
{
	mSelectSquare.Init(0,mpScene);
}

C3DNodeObject C3DSceneBase::GetRoot()
{
	return C3DNodeObject(getSceneManager()->getRootSceneNode());
}

BOOL C3DSceneBase::IsSelectObject() const
{
	return TRUE;
}


C3DNodeObject C3DSceneBase::GetObjectNode( const CGuid& guid )
{
	return OgreUtil::createOrRetrieveSceneNode(guid,getSceneManager() );
}

void C3DSceneBase::addVisibilityFlags( uint32 flags )
{
	if(!mpScene)return;

	uint32 mVisibilityFlags = mpScene->getVisibilityMask();
	mVisibilityFlags |= flags;
	mpScene->setVisibilityMask(mVisibilityFlags);
}

void C3DSceneBase::removeVisibilityFlags( uint32 flags )
{
	if(!mpScene)return;

	uint32 mVisibilityFlags = mpScene->getVisibilityMask();
	mVisibilityFlags &= ~flags;
	mpScene->setVisibilityMask(mVisibilityFlags);
}

//////////////////////////////////////////////////////////////////////////

void SelectRedSquareBuffer::Init( int nBuffSize, Ogre::SceneManager* pScene )
{
	const static float dSize = 5.0f;
	mpRedSquare = OgreUtil::createOrRetrieveManualObject(_T("RedBlineSquare"), pScene);
	mpRedSquare->clear();
	mpRedSquare->begin(_T("twinkle"),RenderOperation::OT_LINE_STRIP);
	mpRedSquare->position(-dSize,-dSize,0);
	mpRedSquare->colour(1,0,0);
	mpRedSquare->position(dSize,-dSize,0);
	mpRedSquare->position(dSize,dSize,0);
	mpRedSquare->position(-dSize,dSize,0);
	mpRedSquare->position(-dSize,-dSize,0);
	mpRedSquare->end();	
	mpScene = pScene;

	Resize(nBuffSize);
}

void SelectRedSquareBuffer::Resize( int nNewSize )
{	
	if(nNewSize<m_nSize)
		return;

	for(int i=m_nSize;i<nNewSize;i++)
	{
		Ogre::BillboardObject * pbill = OgreUtil::createOrRetriveBillBoardObject(GetObjName(i),mpScene);
		pbill->Clear();
		pbill->RefOtherRenderable(mpRedSquare->getSection(0));
		pbill->setVisibilityFlags(NormalVisibleFlag);
	}
	m_nSize = nNewSize;
}

Ogre::MovableObject* SelectRedSquareBuffer::GetObject( int idx )
{
	if(idx>=m_nSize){ Resize(idx+1); }
	return OgreUtil::createOrRetriveBillBoardObject(GetObjName(idx),mpScene);
}

CString SelectRedSquareBuffer::GetObjName( int idx ) const
{
	const static CString mPrefix = _T("SelectRedSquares");
	return mPrefix + "/" + ARCStringConvert::toString(idx);
}

void SelectRedSquareBuffer::HideAll()
{
	for(int i=0;i<m_nSize;i++)
	{
		GetObject(i)->setVisible(false);
	}
}