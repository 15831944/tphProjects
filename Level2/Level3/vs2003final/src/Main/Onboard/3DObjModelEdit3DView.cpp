#include "StdAfx.h"
#include ".\3dobjmodeledit3dview.h"
#include "../resource.h"

#include "../XPStyle/NewMenu.h"

#include <Renderer/RenderEngine/RenderEngine.h>
#include <Renderer/RenderEngine/3DObjModelEditContext.h>

#include <Common\UnitsManager.h>
#include <Common/OleDragDropManager.h>
#include "../ACComponentShapesManager.h"
#include "../OnBoardGridOption.h"
#include "OnboardViewMsg.h"

#include <common/FileInDB.h>

//#include <Ogre/OgreRay.h>
#include <boost/tuple/tuple.hpp>

#define PERSP_VisibleFlag 64

IMPLEMENT_DYNCREATE(C3DObjModelEdit3DView,C3DDragDropView)


BEGIN_MESSAGE_MAP(C3DObjModelEdit3DView, C3DDragDropView)
	ON_COMMAND(ID_MOVE_MOVEX, OnMoveX)
	ON_COMMAND(ID_MOVE_MOVEY, OnMoveY)
	ON_COMMAND(ID_MOVE_MOVEZ, OnMoveZ)
	ON_COMMAND(ID_MOVE_MOVEXY, OnMoveXY)
	ON_COMMAND(ID_MOVE_MOVEYZ, OnMoveYZ)
	ON_COMMAND(ID_MOVE_MOVEZX, OnMoveZX)
	ON_COMMAND(ID_SCALE_SCALEXYZ, OnScaleXYZ)
	ON_COMMAND(ID_SCALE_SCALEX, OnScaleX)
	ON_COMMAND(ID_SCALE_SCALEY, OnScaleY)
	ON_COMMAND(ID_SCALE_SCALEZ, OnScaleZ)
	ON_COMMAND(ID_ROTATE_ROTATEX, OnRotateX)
	ON_COMMAND(ID_ROTATE_ROTATEY, OnRotateY)
	ON_COMMAND(ID_ROTATE_ROTATEZ, OnRotateZ)
	ON_COMMAND(ID_MIRROR_MIRRORXY, OnMirrorXY)
	ON_COMMAND(ID_MIRROR_MIRRORYZ, OnMirrorYZ)
	ON_COMMAND(ID_MIRROR_MIRRORZX, OnMirrorZX)
	ON_COMMAND(ID_3DNODEOBJECTEDIT_DELETE, OnDelete)
	ON_COMMAND(ID_3DNODEOBJECTEDIT_UNDO, OnUndo)
	ON_COMMAND(ID_3DNODEOBJECTEDIT_REDO, OnRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_REDO, OnRedo)
	ON_COMMAND(ID_3DNODEOBJECTEDIT_CONFIGUREGRID, OnConfigGrid)
	ON_COMMAND(ID_LAYOUTLOCK, OnLayoutLock)
	ON_COMMAND(ID_DISTANCEMEASURE, OnDistanceMeasure)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
	ON_UPDATE_COMMAND_UI(ID_LAYOUTLOCK, OnUpdateLayoutLock)
	ON_UPDATE_COMMAND_UI(ID_DISTANCEMEASURE, OnUpdateDistanceMeasure)
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
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FPS, OnUpdateFPS)
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_LAYOUT_FLOORADJUST, OnFloorAdjust)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_FLOORADJUST, OnUpdateFloorAdjust)
	ON_COMMAND(ID_SHOWSHAPESTOOLBAR, OnShowShapesToolbar)
	ON_UPDATE_COMMAND_UI(ID_SHOWSHAPESTOOLBAR, OnUpdateShowShapesToolbar)
	ON_COMMAND(ID_3DNODEOBJECTEDIT_PROPERTY, On3DNodeObjectProperty)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


C3DObjModelEdit3DView::C3DObjModelEdit3DView(void)
	: mpModelEditScene(NULL)
	, m_bConfigureGrid(FALSE)
{

}

C3DObjModelEdit3DView::~C3DObjModelEdit3DView(void)
{
	if (mpModelEditScene)
	{
		delete mpModelEditScene;
		mpModelEditScene = NULL;
	}
}


int C3DObjModelEdit3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (C3DDragDropView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	//
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	//GetEditContext()->ChangeModle(pDoc->GetInputOnboard()->GetEditModel());
	return 0;
}

void C3DObjModelEdit3DView::OnInitialUpdate()
{
	C3DDragDropView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

}

#define CAMERA_NAME _T("View Camera")
void C3DObjModelEdit3DView::SetupViewport()
{
	ASSERT(NULL == mpModelEditScene);
	mpModelEditScene = CreateScene();
	if (NULL == mpModelEditScene)
		return;

	//set up scene add view port main
	if(!mpModelEditScene->IsValid())
	{
		mpModelEditScene->Setup(GetEditContext());
	}

	
	CCameraNode camera = mpModelEditScene->GetCamera(CAMERA_NAME);
	camera.Load(GetEditContext()->GetEditModel()->mActiveCam);

	Ogre::Camera* pActiveCam = camera.mpCamera;
	int nVpIndex = m_rdc.AddViewport( pActiveCam, true );
	m_vpCamera = m_rdc.GetViewport(nVpIndex);
	m_vpCamera.SetVisibleFlag(NormalVisibleFlag|PERSP_VisibleFlag);
	m_vpCamera.SetClear(RGB(164,164,164));

	SetupCoordScene(m_coordDisplayScene, pActiveCam, CRect(0,0,120,120), VPAlignPos(VPA_Left,VPA_Bottom));

	GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_ReloadData);
	mpModelEditScene->AddListener(this);
}

bool C3DObjModelEdit3DView::MoveCamera( UINT nFlags, const CPoint& ptTo , int vpIdx)
{
	if (NULL == mpModelEditScene)
		return false;

	bool bCamChange = C3DDragDropView::MoveCamera(nFlags,ptTo, vpIdx);
	//update 
	if(bCamChange)
	{
		Ogre::Camera* pcam = m_rdc.GetViewport(vpIdx).getCamera();
		m_coordDisplayScene.UpdateCamera( pcam );

		CCameraNode cam = pcam;
		cam.Save(GetEditContext()->GetEditModel()->mActiveCam);
	}
	return bCamChange;
}





C3DSceneBase* C3DObjModelEdit3DView::GetScene( Ogre::SceneManager* pScene )
{
	if( mpModelEditScene && pScene == mpModelEditScene->getSceneManager() )
		return mpModelEditScene;
	if(pScene == m_coordDisplayScene.getSceneManager() )
		return &m_coordDisplayScene;
	return NULL;
}

void C3DObjModelEdit3DView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	C3DDragDropView::OnRButtonDown(nFlags, point);
	if( ::GetAsyncKeyState(VK_MENU) < 0 )
	{
	}
	else if (GetModelEditScene()->GetMouseState() == C3DObjModelEditScene::Measure_state)
	{
	}
	else if( GetEditContext() )
	{
		bool bOn3Dobj = GetEditContext()->GetSelNodeObj();
		if (bOn3Dobj)
		{
			bool bLayoutLock = GetEditContext()->IsViewLocked();
			CNewMenu menu, *pCtxMenu = NULL;
			menu.LoadMenu(IDR_MENU_3DOBJECT_EDIT);
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu( 0 ) );
			pCtxMenu->SetMenuTitle(_T("Edit Model"),MFT_GRADIENT | MFT_TOP_TITLE);
			UINT uDisableFmt = MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
			if (/*!bOn3Dobj || */bLayoutLock)
			{
				pCtxMenu->EnableMenuItem(ID_MOVE_MOVEX, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MOVE_MOVEY, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MOVE_MOVEZ, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MOVE_MOVEXY, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MOVE_MOVEYZ, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MOVE_MOVEZX, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_SCALE_SCALEXYZ, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_SCALE_SCALEX, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_SCALE_SCALEY, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_SCALE_SCALEZ, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_ROTATE_ROTATEX, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_ROTATE_ROTATEY, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_ROTATE_ROTATEZ, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MIRROR_MIRRORXY, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MIRROR_MIRRORYZ, uDisableFmt);
				pCtxMenu->EnableMenuItem(ID_MIRROR_MIRRORZX, uDisableFmt);
				//
				pCtxMenu->EnableMenuItem(ID_3DNODEOBJECTEDIT_DELETE, uDisableFmt);
			}
			if (GetEditContext()->IsViewLocked() || !GetEditContext()->getHistory().CanUndo())
			{
				pCtxMenu->EnableMenuItem(ID_3DNODEOBJECTEDIT_UNDO, uDisableFmt);
			}
			if (GetEditContext()->IsViewLocked() || !GetEditContext()->getHistory().CanRedo())
			{
				pCtxMenu->EnableMenuItem(ID_3DNODEOBJECTEDIT_REDO, uDisableFmt);
			}
			CPoint screenPt  = point;
			ClientToScreen(&screenPt);
			//------------------------------------------------------------
			// temporarily hide view
			pCtxMenu->DeleteMenu(8, MF_BYPOSITION); // "View"
			pCtxMenu->DeleteMenu(7, MF_BYPOSITION); // "-" separator
			//------------------------------------------------------------
			pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, screenPt.x, screenPt.y,this);		
		}

	}
}

C3DObjModelEditFrame* C3DObjModelEdit3DView::GetParentFrame() const
{
	CFrameWnd* pParentFrame = C3DDragDropView::GetParentFrame();
	if (pParentFrame && pParentFrame->IsKindOf(RUNTIME_CLASS(C3DObjModelEditFrame)))
	{
		return (C3DObjModelEditFrame*)pParentFrame;
	}
	return NULL;
}

C3DObjModelEditContext* C3DObjModelEdit3DView::GetEditContext() const
{
	C3DObjModelEditFrame* pParentFrame = GetParentFrame();
	if (pParentFrame)
	{
		return pParentFrame->GetModelEditContext();
	}
	return NULL;
}

void C3DObjModelEdit3DView::OnMoveX()
{
	GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_X);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnMoveY()
{
	GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_Y);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnMoveZ()
{
	GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_Z);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnMoveXY()
{
	GetEditContext()->SetStartOffset(ARCVector3(0.0, 0.0, 0.0));
	GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_XY);
}

void C3DObjModelEdit3DView::OnMoveYZ()
{
	GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_YZ);
}

void C3DObjModelEdit3DView::OnMoveZX()
{
	GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_ZX);
}

void C3DObjModelEdit3DView::OnScaleXYZ()
{
	GetEditContext()->StartNewEditOp(OP_scale3DObj, C3DObjectCommand::COORD_XYZ);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnScaleX()
{
	GetEditContext()->StartNewEditOp(OP_scale3DObj, C3DObjectCommand::COORD_X);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnScaleY()
{
	GetEditContext()->StartNewEditOp(OP_scale3DObj, C3DObjectCommand::COORD_Y);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnScaleZ()
{
	GetEditContext()->StartNewEditOp(OP_scale3DObj, C3DObjectCommand::COORD_Z);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnRotateX()
{
	GetEditContext()->StartNewEditOp(OP_rotate3DObj, C3DObjectCommand::COORD_X);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnRotateY()
{
	GetEditContext()->StartNewEditOp(OP_rotate3DObj, C3DObjectCommand::COORD_Y);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnRotateZ()
{
	GetEditContext()->StartNewEditOp(OP_rotate3DObj, C3DObjectCommand::COORD_Z);
	SetCurOpStartPos();
}

void C3DObjModelEdit3DView::OnMirror(C3DObjectCommand::EditCoordination coord)
{
	ASSERT(!GetEditContext()->IsViewLocked());
	C3DNodeObject nodeObj = GetEditContext()->GetSelNodeObj();
	if (nodeObj)
	{
		GetEditContext()->StartNewEditOp(OP_mirror3DObj);
		ARCVector3 scaler = nodeObj.GetScale();
		switch (coord)
		{
		case C3DObjectCommand::COORD_XY:
			{
				scaler.z = -scaler.z;
			}
			break;
		case C3DObjectCommand::COORD_YZ:
			{
				scaler.x = -scaler.x;
			}
			break;
		case C3DObjectCommand::COORD_ZX:
			{
				scaler.y = -scaler.y;
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
		nodeObj.SetScale(scaler);
		GetEditContext()->DoneCurEditOp();
		Invalidate(FALSE);
	}
}

void C3DObjModelEdit3DView::OnMirrorXY()
{
	OnMirror(C3DObjectCommand::COORD_XY);
}


void C3DObjModelEdit3DView::OnMirrorYZ()
{
	OnMirror(C3DObjectCommand::COORD_YZ);
}


void C3DObjModelEdit3DView::OnMirrorZX()
{
	OnMirror(C3DObjectCommand::COORD_ZX);
}

void C3DObjModelEdit3DView::OnDelete()
{
	GetParentFrame()->UpdateViews(NULL, C3DObjModelEditFrame::Type_DeleteItem, (DWORD)GetEditContext()->GetSelNodeObj().GetSceneNode());
}

void C3DObjModelEdit3DView::OnUndo()
{
	if (!GetEditContext()->IsViewLocked())
	{
		C3DObjectCommand* pCommand = (C3DObjectCommand*)GetEditContext()->getHistory().GetNextUndoCommand();
		if (pCommand)
		{
			if (pCommand->getEditOp() == OP_add3DObj)
			{
				GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_DeleteItem, (DWORD)pCommand->GetNodeObj().GetSceneNode());
			}
			else if (pCommand->getEditOp() == OP_del3DObj)
			{
				GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_AddNewItem, (DWORD)pCommand->GetNodeObj().GetSceneNode());
			}
		}
		GetEditContext()->getHistory().Undo();
		Invalidate(FALSE);
	}
}

void C3DObjModelEdit3DView::OnRedo()
{
	if (!GetEditContext()->IsViewLocked())
	{
		C3DObjectCommand* pCommand = (C3DObjectCommand*)GetEditContext()->getHistory().GetNextRedoCommand();
		if (pCommand)
		{
			if (pCommand->getEditOp() == OP_add3DObj)
			{
				GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_AddNewItem, (DWORD)pCommand->GetNodeObj().GetSceneNode());
			}
			else if (pCommand->getEditOp() == OP_del3DObj)
			{
				GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_DeleteItem, (DWORD)pCommand->GetNodeObj().GetSceneNode());
			}
		}
		GetEditContext()->getHistory().Redo();
		Invalidate(FALSE);
	}
}

void C3DObjModelEdit3DView::DoDrop( const DragDropDataItem& dropItem, CPoint point )
{
	if (mpModelEditScene && m_vpCamera.IsPointIn(point))
	{
		ASSERT(dropItem.GetCount() == 4);
		CString strMeshName     = (LPCTSTR)dropItem.GetItem(0).GetData();
		CString strMeshFileName = (LPCTSTR)dropItem.GetItem(1).GetData();
		CString strTmpWorkDir   = (LPCTSTR)dropItem.GetItem(2).GetData();
		DistanceUnit dHeight = *(DistanceUnit*)dropItem.GetItem(3).GetData();

		ResourceFileArchive::AddDirectory(strTmpWorkDir,strTmpWorkDir);
	
		ARCVector3 dropPos;
		if(GetMouseWorldPos(point,dropPos))
		{
			dropPos[VZ] = dHeight;		
			C3DNodeObject nodeComponent = mpModelEditScene->AddNewComponent(strMeshFileName, dropPos);
			GetEditContext()->SetSelNodeObj(nodeComponent);
			GetEditContext()->StartNewEditOp(OP_add3DObj);
			GetEditContext()->DoneCurEditOp();
			GetParentFrame()->UpdateViews(NULL, C3DObjModelEditFrame::Type_AddNewItem, (DWORD)nodeComponent.GetSceneNode());
		}
	}
}

void C3DObjModelEdit3DView::OnConfigGrid()
{
	if (NULL == mpModelEditScene)
		return;

	m_bConfigureGrid = TRUE;
	CGrid& grid = mpModelEditScene->GetModelGrid();
	COnBoardGridOption dlg(grid);
	if(dlg.DoModal() == IDOK)
	{
		dlg.GetGridData(grid);
		mpModelEditScene->UpdateGrid();
	}
	m_bConfigureGrid = FALSE;
}

void C3DObjModelEdit3DView::UpdateView( C3DObjModelEditFrame::UpdateViewType updateType /*= C3DObjModelEditFrame::Type_ReloadData*/, DWORD dwData /*= 0*/ )
{
	switch (updateType)
	{
	case C3DObjModelEditFrame::Type_ReloadData:
		{
			Invalidate(FALSE);
		}
		break;
	case C3DObjModelEditFrame::Type_AddNewItem:
		{
			Invalidate(FALSE);
		}
		break;
	case C3DObjModelEditFrame::Type_DeleteItem:
		{
			if (!GetEditContext()->IsViewLocked())
			{
				C3DNodeObject nodeObj((Ogre::SceneNode*)dwData);
				if (nodeObj)
				{
					GetEditContext()->StartNewEditOp(nodeObj, OP_del3DObj);
					nodeObj.Detach();
					GetEditContext()->DoneCurEditOp();
				}
				Invalidate(FALSE);
			}
		}
		break;
	case C3DObjModelEditFrame::Type_SelectItem:
		{
			GetEditContext()->SetSelNodeObj((Ogre::SceneNode*)dwData);
			OnUpdateDrawing();
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
}


void C3DObjModelEdit3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (VK_DELETE == nChar)
	{
		OnDelete();
	}
	else if (VK_RETURN == nChar)
	{
		GetEditContext()->DoneCurEditOp();
	}
	else if (VK_ESCAPE == nChar)
	{
		GetEditContext()->CancelCurEditOp();
		Invalidate(FALSE);
	}
	C3DDragDropView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void C3DObjModelEdit3DView::OnLayoutLock()
{
	// TODO: Add your command handler code here
	GetEditContext()->flipLockView();
}

void C3DObjModelEdit3DView::OnUpdateUndo(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->Enable(!GetEditContext()->IsViewLocked() && GetEditContext()->getHistory().CanUndo());
}

void C3DObjModelEdit3DView::OnUpdateRedo(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->Enable(!GetEditContext()->IsViewLocked() && GetEditContext()->getHistory().CanRedo());
}

void C3DObjModelEdit3DView::OnUpdateLayoutLock(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->SetCheck(GetEditContext()->IsViewLocked());
}


void C3DObjModelEdit3DView::OnDistanceMeasure()
{
	// TODO: Add your command handler code here
	if (NULL == mpModelEditScene)
		return;

	mpModelEditScene->StartDistanceMeasure();
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateDistanceMeasure(CCmdUI* pCmdUI)
{
	if (NULL == mpModelEditScene)
		return;

	pCmdUI->SetCheck(mpModelEditScene->GetMouseState() != C3DObjModelEditScene::Default_state);
}

void C3DObjModelEdit3DView::OnUpdateDrawing()
{
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnFinishMeasure( double dDistance )
{
	CString strText;
	dDistance = UNITSMANAGER->ConvertLength(dDistance);
	strText.Format("Distance: %.2f %s", dDistance, UNITSMANAGER->GetLengthUnitString());

	AfxMessageBox(strText, MB_OK|MB_ICONINFORMATION);
}

C3DObjModelEditScene* C3DObjModelEdit3DView::CreateScene() const
{
	ASSERT(FALSE);
	return FALSE;
}

void C3DObjModelEdit3DView::OnViewSolid()
{
	// TODO: Add your command handler code here
	GetModelEditScene()->SetModelViewType(C3DObjModelEditScene::VT_Solid);
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateViewSolid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetModelEditScene()->GetModelViewType() == C3DObjModelEditScene::VT_Solid);
}

void C3DObjModelEdit3DView::OnViewWireframe()
{
	// TODO: Add your command handler code here
	GetModelEditScene()->SetModelViewType(C3DObjModelEditScene::VT_Wireframe);
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateViewWireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetModelEditScene()->GetModelViewType() == C3DObjModelEditScene::VT_Wireframe);
}

void C3DObjModelEdit3DView::OnViewSolidwireframe()
{
	// TODO: Add your command handler code here
	GetModelEditScene()->SetModelViewType(C3DObjModelEditScene::VT_Solid_Wireframe);
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateViewSolidwireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetModelEditScene()->GetModelViewType() == C3DObjModelEditScene::VT_Solid_Wireframe);
}

void C3DObjModelEdit3DView::OnViewTransfarent()
{
	// TODO: Add your command handler code here
	GetModelEditScene()->SetModelViewType(C3DObjModelEditScene::VT_Transparent);
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateViewTransfarent(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetModelEditScene()->GetModelViewType() == C3DObjModelEditScene::VT_Transparent);
}

void C3DObjModelEdit3DView::OnViewHiddenwireframe()
{
	// TODO: Add your command handler code here
	GetModelEditScene()->SetModelViewType(C3DObjModelEditScene::VT_Hidden_Wire);
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateViewHiddenwireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetModelEditScene()->GetModelViewType() == C3DObjModelEditScene::VT_Hidden_Wire);
}

void C3DObjModelEdit3DView::OnViewTransparentwireframe()
{
	// TODO: Add your command handler code here
	GetModelEditScene()->SetModelViewType(C3DObjModelEditScene::VT_Transparent_Wireframe);
	Invalidate(FALSE);
}

void C3DObjModelEdit3DView::OnUpdateViewTransparentwireframe(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(GetModelEditScene()->GetModelViewType() == C3DObjModelEditScene::VT_Transparent_Wireframe);
}

void C3DObjModelEdit3DView::SetCurOpStartPos()
{
	POINT point;
	if (GetCursorPos(&point))
	{
		ScreenToClient(&point);
		GetEditContext()->SetOpStartPoint(point);
	}
}

void C3DObjModelEdit3DView::SetupCoordScene( CCoordinateDisplayScene& theCoordScene, Ogre::Camera* pActiveCam, CRect rect, VPAlignPos alg )
{
	//set up coord displace scene
	theCoordScene.Setup();
	Ogre::Camera* pCoordCam = theCoordScene.GetLocalCamera();
	theCoordScene.UpdateCamera( pActiveCam);
	int nVpIndex = m_rdc.AddViewport( pCoordCam,false);
	m_rdc.GetViewport(nVpIndex).SetClear(0,false);
	m_rdc.SetViewportDim(nVpIndex,rect,alg,true);    	
	theCoordScene.UpdateCamera(pActiveCam);
}

void C3DObjModelEdit3DView::OnSelect3DObj( C3DNodeObject nodeObj )
{
	// update m_coord3DObj here, not implemented
// 	if (nodeObj)
// 	{
// 		nodeObj = C3DNodeObject(NULL);
// 	}
// 	else
// 	{
// 		nodeObj = C3DNodeObject(NULL);
// 	}
	GetParentFrame()->UpdateViews(NULL, C3DObjModelEditFrame::Type_SelectItem, (DWORD)nodeObj.GetSceneNode());
}

bool C3DObjModelEdit3DView::GetViewPortMousePos(ARCVector3& pos)
{
	POINT point;
	if (GetCursorPos(&point))
	{
		ScreenToClient(&point);
		return GetMouseWorldPos(point,pos);
	}
	return false;
}

void C3DObjModelEdit3DView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	C3DDragDropView::OnMouseMove(nFlags, point);
	C3DObjModelEditFrame* p3DFrame = GetParentFrame();
	p3DFrame->GetStatusBar().OnUpdateCmdUI(p3DFrame, FALSE);
}

void C3DObjModelEdit3DView::OnUpdateFPS(CCmdUI* pCmdUI)
{
	CString s;
	s.Format("\tFPS: %5.2f",m_dFrameRate);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText( s );
}


void C3DObjModelEdit3DView::OnFloorAdjust()
{
	OnConfigGrid();
}

void C3DObjModelEdit3DView::OnUpdateFloorAdjust(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bConfigureGrid ? BST_CHECKED : BST_UNCHECKED);
}

void C3DObjModelEdit3DView::OnShowShapesToolbar()
{
	ACCOMPONENTSHAPESMANAGER.ShowShapesBar(!ACCOMPONENTSHAPESMANAGER.IsShapesBarShow());
}

void C3DObjModelEdit3DView::OnUpdateShowShapesToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ACCOMPONENTSHAPESMANAGER.IsShapesBarShow() ? BST_CHECKED : BST_UNCHECKED);
}


void C3DObjModelEdit3DView::On3DNodeObjectProperty()
{
	GetParentFrame()->On3DNodeObjectProperty();
}

void C3DObjModelEdit3DView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	C3DDragDropView::OnLButtonDblClk(nFlags, point);

	if (GetEditContext()->GetSelNodeObj()) // 
	{
		GetParentFrame()->On3DNodeObjectProperty();
	}
}
