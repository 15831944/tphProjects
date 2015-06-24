#include "StdAfx.h"
#include ".\meshsegedit3dview.h"
#include <InputOnBoard\ComponentModel.h>
#include <InputOnBoard\ComponentEditContext.h>
#include "OnboardViewMsg.h"
#include "MeshEditFrame.h"


IMPLEMENT_DYNCREATE(CMeshSegEdit3DView,CMeshEdit3DView)

BEGIN_MESSAGE_MAP(CMeshSegEdit3DView, CMeshEdit3DView)	
END_MESSAGE_MAP()



CMeshSegEdit3DView::CMeshSegEdit3DView(void)
{
}

CMeshSegEdit3DView::~CMeshSegEdit3DView(void)
{
}

#include <renderer/RenderEngine/RenderEngine.h>
#include <common/TmpFileManager.h>


void CMeshSegEdit3DView::SetupViewport()
{
	CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();

	//set up scene add view port main
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();
	if(pMeshScene)
	{		
		if(!pMeshScene->IsValid())
		{
			//ResourceFileArchive::AddDirectory(GetEditContext()->GetCurrentComponent()->msTmpWorkDir);
			pMeshScene->Setup(GetEditContext());
		}
		pMeshScene->AddListener(this);		
		CCameraNode camera = pMeshScene->GetCamera( mViewCameraName );	
		camera.Load(pModel->mSegViewCam);

		Ogre::Camera *pcam = camera.mpCamera;
		int idx = m_rdc.AddViewport( pcam ,true );
		ViewPort vp = m_rdc.GetViewport(idx);
		vp.SetClear(RGB(164,164,164));
		vp.SetVisibleFlag(NormalVisibleFlag | XZVisibleFlag);


		m_coordDisplayScene.Setup();
		m_coordDisplayScene.UpdateCamera( pcam);
		Ogre::Camera* pCoordCam = m_coordDisplayScene.GetLocalCamera();
		idx = m_rdc.AddViewport( pCoordCam,false);
		m_rdc.GetViewport(idx).SetClear(0,false);
		m_rdc.SetViewportDim(idx,CRect(0,0,60,60),VPAlignPos(VPA_Left,VPA_Bottom),true);    	
	}

}

C3DSceneBase* CMeshSegEdit3DView::GetScene( Ogre::SceneManager* pScene )
{
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();
	if( pMeshScene && pScene == pMeshScene->getSceneManager() )
		return pMeshScene;
	if(pScene == m_coordDisplayScene.getSceneManager() )
		return &m_coordDisplayScene;
	return NULL;
}



void CMeshSegEdit3DView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	if(lHint == VM_ONBOARD_COMPONENT_GRID_CHANGE)
	{	
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_COMPONET_SCENE_CHANGE)
	{
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_COMPONENT_SAVE_MODEL)
	{
		CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
		CCameraNode camera = GetMeshEditScene()->GetCamera( mViewCameraName );	
		camera.Save(pModel->mSegViewCam);
	}
}

bool CMeshSegEdit3DView::MoveCamera( UINT nFlags, const CPoint& ptTo, int vpidx /*=0 */ )
{
	bool bCamChange = C3DBaseView::MoveCamera(nFlags,ptTo, vpidx);
	//update 
	if(bCamChange)
	{
		Ogre::Camera* pcam = m_rdc.GetViewport(vpidx).getCamera();
		m_coordDisplayScene.UpdateCamera( pcam );		
	}
	return bCamChange;
}

