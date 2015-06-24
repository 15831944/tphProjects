#include "StdAfx.h"
#include ".\meshedit3Dview.h"
#include "../../Renderer/RenderEngine/RenderEngine.h"
#include "MeshEditFrame.h"
#include "OnboardViewMsg.h"
#include <inputonboard/ComponentEditContext.h>
#include <InputOnBoard/ComponentModel.h>
#include "../XPStyle/NewMenu.h"
#include "../Resource.h"
#include "../OnBoardGridOption.h"
#include "MeshEditFrame.h"
#include "OnboardViewMsg.h"
#include "MeshEditFuncs.h"
#include <InputOnBoard\ComponentModel.h>
#include <InputOnBoard\ComponentEditContext.h>
#include <common/UnitsManager.h>
#include <common/WinMsg.h>


//////////////////////////////////////////////////////////////////////////
#ifdef TEST_CODE
#include "../../Renderer/Test/mainDoc.h"
CComponentEditContext* CMeshEdit3DView::GetEditContext()
{
	return &((CmainDoc*)GetDocument())->mEditMeshContext;
}
#else
#include "..\TermPlanDoc.h"
#include "..\..\InputOnBoard\CInputOnboard.h"
CComponentEditContext* CMeshEdit3DView::GetEditContext()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	return pDoc->GetInputOnboard()->GetComponentEditContext();
}
#endif

IMPLEMENT_DYNCREATE(CMeshEdit3DView, C3DBaseView)
BEGIN_MESSAGE_MAP(CMeshEdit3DView, C3DBaseView)	
	ON_COMMAND(ID_ONBOARDCOMPONENTEDIT_ADDSTATION, OnOnboardcomponenteditAddstation)
	ON_COMMAND(ID_ONBOARDCOMPONENTEDIT_CONFIGUREGRID, OnOnboardcomponenteditConfiguregrid)
	ON_COMMAND(ID_ONBOARDCOMPONENTEDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_ONBOARDCOMPONENTEDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)

	ON_COMMAND(ID_EDITSECTION_DELETE, OnEditsectionDelete)
	ON_COMMAND(ID_EDITSECTION_COPY, OnEditsectionCopy)
	ON_COMMAND(ID_EDITSECTION_PROP, OnEditsectionProp)
	ON_COMMAND(ID_VIEW_SOLID, OnViewSolid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOLID, OnUpdateViewSolid)
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, OnUpdateViewWireframe)
	ON_COMMAND(ID_VIEW_SOLIDWIREFRAME, OnViewSolidwireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOLIDWIREFRAME, OnUpdateViewSolidwireframe)
	ON_COMMAND(ID_VIEW_TRANSFARENT, OnViewTransfarent)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRANSFARENT, OnUpdateViewTransfarent)
	ON_COMMAND(ID_VIEW_TRANSPARENTWIREFRAME, OnViewTransparentwireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRANSPARENTWIREFRAME, OnUpdateViewTransparentwireframe)
	ON_COMMAND(ID_VIEW_HIDDENWIREFRAME, OnViewHiddenwireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDDENWIREFRAME, OnUpdateViewHiddenwireframe)
	ON_COMMAND(ID_3DNODEOBJECTEDIT_CONFIGUREGRID, OnOnboardcomponenteditConfiguregrid)
	ON_COMMAND(ID_LAYOUTLOCK, OnLayoutLock)
	ON_COMMAND(ID_DISTANCEMEASURE, OnDistanceMeasure)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
	ON_UPDATE_COMMAND_UI(ID_LAYOUTLOCK, OnUpdateLayoutLock)
	ON_UPDATE_COMMAND_UI(ID_DISTANCEMEASURE, OnUpdateDistanceMeasure)
END_MESSAGE_MAP()


void CMeshEdit3DView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	if(lHint == VM_ONBOARD_COMPONET_SCENE_CHANGE)
	{
		Invalidate(FALSE);
	}
}

CSimpleMeshEditScene* CMeshEdit3DView::GetMeshEditScene()
{
	if( GetParentFrame() && GetParentFrame()->IsKindOf(RUNTIME_CLASS(COnBoardMeshEditFrame)))
	{
		COnBoardMeshEditFrame* pFrame =  (COnBoardMeshEditFrame*)GetParentFrame();
		return &pFrame->mMeshEditScene;
	}
	return NULL;
}

void CMeshEdit3DView::OnRBtnDownSection( CComponentMeshSection* pSection,const CPoint& pt )
{
	if( GetParentFrame()->GetActiveView() == this)
	{
		CNewMenu menu, *pCtxMenu = NULL;
		menu.LoadMenu(IDR_MENU_ONBOARD_SECTION);
		pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu( 1 ) );
		pCtxMenu->SetMenuTitle(pSection->getName(),MFT_GRADIENT | MFT_TOP_TITLE);
		CPoint screenPt  = pt;
		ClientToScreen(&screenPt);


		CWnd* pWndPopupOwner = this;
		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, screenPt.x, screenPt.y,pWndPopupOwner);	
	}
}

void CMeshEdit3DView::OnEditUndo()
{	
	GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_EDIT_OPERATION,(CObject*)GetEditContext()->Undo());

}

void CMeshEdit3DView::OnEditRedo()
{
	GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_EDIT_OPERATION,(CObject*)GetEditContext()->Redo());
}

void CMeshEdit3DView::OnOnboardcomponenteditConfiguregrid()
{
	// TODO: Add your command handler code here
	CGrid& grid = GetEditContext()->GetCurrentComponent()->mGrid;
	COnBoardGridOption dlg(grid);
	if(dlg.DoModal() == IDOK)
	{
		dlg.GetGridData(grid);
		GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_GRID_CHANGE);	
	}
}

void CMeshEdit3DView::OnOnboardcomponenteditAddstation()
{
	// TODO: Add your command handler code here

	if(AddSectionCommand*pCmd = MeshEditFuncs::OnAddSection(GetEditContext(),mLastMousePosOnScene[VX]) )
	{
		GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_EDIT_OPERATION,(CObject*)pCmd);
	}	
}


void CMeshEdit3DView::OnRBtnDownScene(const CPoint& pt, const ARCVector3& worldPos)
{
	if( GetParentFrame()->GetActiveView() == this)
	{
		mLastMousePosOnScene = worldPos;
		CNewMenu menu, *pCtxMenu = NULL;
		menu.LoadMenu(IDR_MENU_ONBOARD_COMPONENT);
		pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu( 0 ) );
		pCtxMenu->SetMenuTitle(_T("Edit Component"),MFT_GRADIENT | MFT_TOP_TITLE);
		CPoint screenPt  = pt;
		ClientToScreen(&screenPt);
		CWnd* pWndPopupOwner = this;
		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, screenPt.x, screenPt.y,pWndPopupOwner );	
	}
}

void CMeshEdit3DView::OnEditsectionDelete()
{
	// TODO: Add your command handler code here
	CComponentMeshSection* pSection = GetEditContext()->GetActiveSection();
	if( DelSectionCommand * pCmd = MeshEditFuncs::OnDelSection(GetEditContext(),pSection) )
	{
		GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_EDIT_OPERATION,(CObject*)pCmd);
	}
}

void CMeshEdit3DView::OnEditsectionCopy()
{
	// TODO: Add your command handler code here
	CComponentMeshSection* pSection = GetEditContext()->GetActiveSection();
	if( AddSectionCommand* pCmd = MeshEditFuncs::OnCopySection(GetEditContext(),pSection) )
	{
		GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_EDIT_OPERATION,(CObject*)pCmd);
	}	
}


void CMeshEdit3DView::OnEditsectionProp()
{
	// TODO: Add your command handler code here
	CComponentMeshSection* pSection = GetEditContext()->GetActiveSection();
	if(EditSectionPropCommand* pCmd = MeshEditFuncs::OnEditSection(GetEditContext(),pSection))
	{
		CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
		pMeshScene->OnUpdateOperation(pCmd);
		GetEditContext()->UpdateAllViews();			
	}	
}


void CMeshEdit3DView::OnViewSolid()
{
	// TODO: Add your command handler code here
	GetEditContext()->SetMeshViewType(MV_Solid);
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
	pMeshScene->UpdateMeshViewType();
	GetEditContext()->UpdateAllViews();		
}

void CMeshEdit3DView::OnUpdateViewSolid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetEditContext()->m_mviewType==MV_Solid);
}

void CMeshEdit3DView::OnViewWireframe()
{
	// TODO: Add your command handler code here
	GetEditContext()->SetMeshViewType(MV_Wireframe);
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
	pMeshScene->UpdateMeshViewType();
	GetEditContext()->UpdateAllViews();		
}

void CMeshEdit3DView::OnUpdateViewWireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetEditContext()->m_mviewType==MV_Wireframe);

}

void CMeshEdit3DView::OnViewSolidwireframe()
{
	// TODO: Add your command handler code here
	GetEditContext()->SetMeshViewType(MV_Solid_Wireframe);
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
	pMeshScene->UpdateMeshViewType();
	GetEditContext()->UpdateAllViews();		
}

void CMeshEdit3DView::OnUpdateViewSolidwireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetEditContext()->m_mviewType==MV_Solid_Wireframe);

}

void CMeshEdit3DView::OnViewTransfarent()
{
	// TODO: Add your command handler code here
	GetEditContext()->SetMeshViewType(MV_Transparent);
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
	pMeshScene->UpdateMeshViewType();
	GetEditContext()->UpdateAllViews();		
}

void CMeshEdit3DView::OnUpdateViewTransfarent(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetEditContext()->m_mviewType==MV_Transparent);

}

void CMeshEdit3DView::OnViewHiddenwireframe()
{
	// TODO: Add your command handler code here
	GetEditContext()->SetMeshViewType(MV_Hidden_Wire);
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
	pMeshScene->UpdateMeshViewType();
	GetEditContext()->UpdateAllViews();	
}

void CMeshEdit3DView::OnUpdateViewHiddenwireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetEditContext()->m_mviewType==MV_Hidden_Wire);
}



void CMeshEdit3DView::OnViewTransparentwireframe()
{
	// TODO: Add your command handler code here
	GetEditContext()->SetMeshViewType(MV_Transparent_Wireframe);
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();		
	pMeshScene->UpdateMeshViewType();
	GetEditContext()->UpdateAllViews();		
}

void CMeshEdit3DView::OnUpdateViewTransparentwireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetEditContext()->m_mviewType==MV_Transparent_Wireframe);
}

void CMeshEdit3DView::OnLayoutLock()
{
	GetEditContext()->flipLockView();
}

void CMeshEdit3DView::OnUpdateUndo( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( GetEditContext()->getHistory().CanUndo());
}

void CMeshEdit3DView::OnUpdateRedo( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( GetEditContext()->getHistory().CanRedo());
}

void CMeshEdit3DView::OnUpdateLayoutLock( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( GetEditContext()->IsViewLocked() );
}

void CMeshEdit3DView::OnDistanceMeasure()
{
	GetMeshEditScene()->StartDistanceMeasure();
}

void CMeshEdit3DView::OnUpdateDistanceMeasure( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck(GetMeshEditScene()->mMouseState ==  CSimpleMeshEditScene::_distancemeasure);
}


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSimpleMeshEditView,CMeshEdit3DView)
BEGIN_MESSAGE_MAP(CSimpleMeshEditView, CMeshEdit3DView)	
END_MESSAGE_MAP()


CSimpleMeshEditView::CSimpleMeshEditView(void)
{
	
}

CSimpleMeshEditView::~CSimpleMeshEditView(void)
{
}


#include <renderer/RenderEngine/RenderEngine.h>
#include <common/TmpFileManager.h>

void CSimpleMeshEditView::SetupViewport()
{
	CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
	
	//set up scene add view port main
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();
	if(pMeshScene)
	{		
		if(!pMeshScene->IsValid())
		{
			pMeshScene->Setup(GetEditContext());
		}
		pMeshScene->AddListener(this);

		//mMeshScene.AddListener(this);		
		CCameraNode camera = pMeshScene->GetCamera( mViewCameraName );
		Ogre::Camera* pcam = camera.mpCamera;
		camera.Load(pModel->mActiveCam);

		int idx = m_rdc.AddViewport( pcam,true );
		ViewPort vp = m_rdc.GetViewport(idx);
		vp.SetVisibleFlag(NormalVisibleFlag|PERSP_VisibleFlag);
		vp.SetClear(RGB(164,164,164));

		//set up coord displace scene
		m_coordDisplayScene.Setup();
		Ogre::Camera* pCoordCam = m_coordDisplayScene.GetLocalCamera();
		m_coordDisplayScene.UpdateCamera( pcam);
		idx = m_rdc.AddViewport( pCoordCam,false);
		m_rdc.GetViewport(idx).SetClear(0,false);
		m_rdc.SetViewportDim(idx,CRect(0,0,120,120),VPAlignPos(VPA_Left,VPA_Bottom),true);    	
		m_coordDisplayScene.UpdateCamera(pcam);
	}

}

bool CSimpleMeshEditView::MoveCamera( UINT nFlags, const CPoint& ptTo , int vpIdx)
{
	bool bCamChange = C3DBaseView::MoveCamera(nFlags,ptTo, vpIdx);
	//update 
	if(bCamChange)
	{
		Ogre::Camera* pcam = m_rdc.GetViewport(vpIdx).getCamera();
		m_coordDisplayScene.UpdateCamera( pcam );
	}
	return bCamChange;
}





C3DSceneBase* CSimpleMeshEditView::GetScene( Ogre::SceneManager* pScene )
{
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();
	if( pMeshScene && pScene == pMeshScene->getSceneManager() )
		return pMeshScene;
	if(pScene == m_coordDisplayScene.getSceneManager() )
		return &m_coordDisplayScene;
	return NULL;
}

void CSimpleMeshEditView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	if(lHint == VM_ONBOARD_COMPONENT_GRID_CHANGE)
	{
		GetMeshEditScene()->OnUpdateGrid(GetEditContext()->GetCurrentComponent());
		GetDocument()->UpdateAllViews(this,VM_ONBOARD_COMPONET_SCENE_CHANGE);
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_COMPONENT_EDIT_OPERATION)
	{
		CComponentEditOperation* pOp = (CComponentEditOperation*)pHint;
		GetMeshEditScene()->OnUpdateOperation(pOp);
		GetDocument()->UpdateAllViews(this,VM_ONBOARD_COMPONET_SCENE_CHANGE);
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_COMPONENT_SAVE_MODEL)
	{
		GetMeshEditScene()->SaveModel();
		CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
		CCameraNode camera = GetMeshEditScene()->GetCamera( mViewCameraName );		
		camera.Save(pModel->mActiveCam);
	}
	CMeshEdit3DView::OnUpdate(pSender,lHint,pHint);
}

void CSimpleMeshEditView::OnFinishDistanceMesure( double dist)
{
	dist = UNITSMANAGER->ConvertLength(dist);
	CString msg;
	msg.Format("Distance: %.2f %s", dist, UNITSMANAGER->GetLengthUnitString());	
	AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);

}

void CSimpleMeshEditView::OnActiveView()
{

}