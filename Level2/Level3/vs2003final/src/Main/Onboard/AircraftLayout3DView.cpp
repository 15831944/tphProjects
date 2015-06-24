#include "StdAfx.h"
#include ".\aircraftlayout3dview.h"
#include "../Resource.h"
#include <Common/OleDragDropManager.h>
//#include <OgreRay.h>
//#include <OgreCamera.h>
#include <Renderer/RenderEngine/RenderEngine.h>
#include <InputOnBoard/AircraftLayoutEditContext.h>

#include "OnboardViewMsg.h"
#include <InputOnBoard/AircraftFurnishingSection.h>
#include <InputOnBoard/Seat.h>
#include <InputOnBoard/Door.h>
#include <InputOnBoard/Storage.h>
#include <InputOnBoard/AircraftPlacements.h>
#include <InputOnBoard/AircaftLayOut.h>
#include "../DlgSeatRowProp.h"
#include "../OnBoardSeatDlg.h"
#include "../OnBoardDoorDlg.h"
#include "../OnBoardStorageDlg.h"
#include <common/UnitsManager.h>
#include <common/WinMsg.h>
#include "./AircraftLayoutEditor.h"
#include "AircraftLayoutFrame.h"
#include <boost/tuple/tuple.hpp>
#include "../DlgOnboardDeckAdjust.h"
#include "../DlgArrayCopies.h"

//////////////////////////////////////////////////////////////////////////
#ifdef TEST_CODE
#include "../../Renderer/Test/mainDoc.h"
CAircraftLayoutEditContext* CAircraftLayout3DView::GetEditContext()
{
	return &((CmainDoc*)GetDocument())->mEditLayoutContext;
}
#else
#include "..\TermPlanDoc.h"
#include "..\..\InputOnBoard\CInputOnboard.h"
CAircraftLayoutEditor* CAircraftLayout3DView::GetEditor()const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	return pDoc->GetAircraftLayoutEditor();
}
#endif
//////////////////////////////////////////////////////////////////////////

static const UINT	PANZOOMTIMEOUT = 1000;
static const UINT	ONSIZETIMEOUT = 1000;
static const UINT	MOUSEMOVETIMEOUT = 1000;


IMPLEMENT_DYNCREATE(CAircraftLayout3DView, C3DDragDropView)
BEGIN_MESSAGE_MAP(CAircraftLayout3DView, C3DDragDropView)	
	//{{AFX_MSG_MAP(CAircraftLayoutView)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GRIDSPACING, OnUpdateGridSpacing)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FPS, OnUpdateFPS)
	ON_MESSAGE(TP_ONBOARDLAYOUT_FALLBACK,OnDlgFallback)

	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_REDO, OnRedo)
	ON_COMMAND(ID_LAYOUTLOCK, OnLayoutLock)
	ON_COMMAND(ID_DISTANCEMEASURE, OnDistanceMeasure)
	ON_COMMAND(ID_LAYOUT_HIDEPROCTAGS, OnShowText)
	ON_COMMAND(ID_SHOWSHAPESTOOLBAR, OnShowShapes)
	ON_COMMAND(ID_LAYOUT_FLOORADJUST, OnFloorAdjust)

	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
	ON_UPDATE_COMMAND_UI(ID_LAYOUTLOCK, OnUpdateLayoutLock)
	ON_UPDATE_COMMAND_UI(ID_DISTANCEMEASURE, OnUpdateDistanceMeasure)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_HIDEPROCTAGS, OnUpdateShowText)
	ON_UPDATE_COMMAND_UI(ID_SHOWSHAPESTOOLBAR, OnUpdateShowShapes)

	//}}AFX_MSG_MAP
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_COPY, OnCtxElementCopy)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_DELETE, OnCtxElementDelete)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_PROPERTIES,OnCtxElementProperties)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_DISPLAYPROPERTIES,OnCtxElementDisplayProp)


	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_MOVE, OnCtxElementMove)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_ROTATE, OnCtxElementRotate)

	ON_COMMAND(ID_CTX_MOVESHAPE_Z, OnCtxMoveshapeZ)
	ON_COMMAND(ID_CTX_MOVESHAPE_DECKHIGHT, OnCtxMoveshapeZ0)
	ON_COMMAND(ID_CTX_MOVESHAPE, OnCtxMoveShapes)
	ON_COMMAND(ID_CTX_ROTATESHAPE, OnCtxRotateShapes)
	ON_COMMAND(ID_CTX_SCALEPROCESSORS, OnCtxScaleShape)
	ON_COMMAND(ID_CTX_SCALEPROCESSORSX, OnCtxScaleShapeX)
	ON_COMMAND(ID_CTX_SCALEPROCESSORSY, OnCtxScaleShapeY)
	ON_COMMAND(ID_CTX_SCALEPROCESSORSZ, OnCtxScaleShapeZ)


	//ON_COMMAND(ID_SEAT_CREATEROW, OnCtxSeatSelectRow)
	//ON_COMMAND(ID_SEAT_SELECTARRAY, OnSeatSelectGroup)
	ON_COMMAND(ID_CTX_SELECTROWS, OnCtxSelectrows)
	ON_COMMAND(ID_CTX_SELECTARRAYS, OnCtxSelectarrays)
	ON_COMMAND(ID_CTX_SELECTSEATS, OnCtxSelectseats)
	ON_UPDATE_COMMAND_UI(ID_CTX_SELECTSEATS, OnUpdateCtxSelectseats)
	ON_UPDATE_COMMAND_UI(ID_CTX_SELECTROWS, OnUpdateCtxSelectrows)
	ON_UPDATE_COMMAND_UI(ID_CTX_SELECTARRAYS, OnUpdateCtxSelectarrays)
	ON_COMMAND(ID_SEAT_EDITSEATROW, OnSeatEditseatrow)
	ON_COMMAND(ID_SEAT_EDITSEATARRAY, OnSeatEditseatarray)
	ON_COMMAND(ID_SEAT_COPY, OnSeatCopy)
	ON_COMMAND(ID_SEAT_DELETE, OnSeatDelete)
	ON_COMMAND(ID_SEAT_ENABLEEDITSEATARRAYPATH, OnSeatEnableeditseatarraypath)
	ON_UPDATE_COMMAND_UI(ID_SEAT_ENABLEEDITSEATARRAYPATH, OnUpdateSeatEnableeditseatarraypath)
	ON_COMMAND(ID_AIRCRAFTELEMENT_LOCK, OnAircraftelementLock)
	ON_UPDATE_COMMAND_UI(ID_AIRCRAFTELEMENT_LOCK, OnUpdateAircraftelementLock)
	ON_COMMAND(ID_TRANSFORMSHAPE_TRANSFORMPROPERTIES, OnTransformshapeTransformproperties)
	ON_COMMAND(ID_ARRAY_AIRCRAFT_ELEMENTS, OnArrayElements)
	ON_COMMAND(ID_CORRIDOR_ENABLEPATHEDIT, OnCorridorEnableeditseatpath)
	ON_UPDATE_COMMAND_UI(ID_CORRIDOR_ENABLEPATHEDIT, OnUpdateCorridorEnableeditseatpath)
	ON_COMMAND(ID_ADD_ADDEDITPOINT, OnPathAddEditPoint)
	ON_COMMAND(ID_REMOVE_REMOVEEDITPOINT, OnPathRemoveEditPoint)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CAircraftLayout3DView::CAircraftLayout3DView(void)
: m_pDlgDeckAdjust(NULL)
, m_hFallbackWnd(NULL)
, m_eViewStatus(viewStatus_None)
, m_pathRelatedElement(NULL)
, m_nEditedPathIndex(-1)
, m_nEditedPathNodeIndex(-1)
{
	
}

int CAircraftLayout3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (C3DDragDropView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	DRAG_DROP_VIEW_REGIST(this);
	return 0;
}

#define ACTIVECAM _T("ActiveCamera")
void CAircraftLayout3DView::SetupViewport()
{
	if(!mLayoutScene.IsValid())
		mLayoutScene.Setup(GetEditor());

	mLayoutScene.AddListener(this);

	CCameraNode camera = mLayoutScene.GetCamera( mViewCameraName );
	Ogre::Camera* pcam = camera.mpCamera;
	int idx = m_rdc.AddViewport( pcam,true );
	m_vpCamera = m_rdc.GetViewport(idx);
	m_vpCamera.SetVisibleFlag(NormalVisibleFlag);
	m_vpCamera.SetClear(RGB(164,164,164));
	mLayoutScene.overlaytext.RegShowInViewport(m_vpCamera);
	

	//set up coord displace scene
	m_coordDisplayScene.Setup();
	Ogre::Camera* pCoordCam = m_coordDisplayScene.GetLocalCamera();
	m_coordDisplayScene.UpdateCamera( pcam);
	idx = m_rdc.AddViewport( pCoordCam,false);
	m_rdc.GetViewport(idx).SetClear(0,false);
	m_rdc.SetViewportDim(idx,CRect(0,0,120,120),VPAlignPos(VPA_Left,VPA_Bottom),true);    	
// 	m_coordDisplayScene.UpdateCamera(pcam);

	ReadCamerasSettings();
	LoadCamerasSettings();
}

C3DSceneBase* CAircraftLayout3DView::GetScene( Ogre::SceneManager* pScene )
{
	if(mLayoutScene.getSceneManager() == pScene)
		return &mLayoutScene;
	return NULL;
}

bool CAircraftLayout3DView::MoveCamera( UINT nFlags, const CPoint& ptTo , int vpIdx)
{
	bool bCamChange = C3DDragDropView::MoveCamera(nFlags,ptTo, vpIdx);
	//update 
	if(bCamChange)
	{
		Ogre::Camera* pcam = m_rdc.GetViewport(vpIdx).getCamera();
		m_coordDisplayScene.UpdateCamera( pcam );

		CCameraNode cam = pcam;
		cam.Save(m_cameras.GetUserCamera().GetDefaultCameraData());
	}
	return bCamChange;
}

void CAircraftLayout3DView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	if(lHint == VM_ONBOARD_ACLAYOUT_EDIT_OPERATION)
	{
		mLayoutScene.OnUpdateOperation((CAircraftLayoutEditCommand*)pHint);
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_DECK_CHANGED)
	{
		mLayoutScene.OnDeckChange((CDeck*)pHint);
		Invalidate(FALSE);
	}

	if(lHint == VM_ONBOARD_SELECTELEMENS_CHANGE)
	{
		mLayoutScene.OnUpdateSelectChange();
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_ACLAYOUT_GRID_MODEL_CHANGE)
	{
		mLayoutScene.UpdateGridACShow();
		Invalidate(FALSE);
	}
}

void CAircraftLayout3DView::OnUpdateFPS( CCmdUI *pCmdUI )
{
	CString s(_T(""));
	s.Format("\tFPS: %5.2f",m_dFrameRate);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText( s );
}

void CAircraftLayout3DView::OnUpdateGridSpacing( CCmdUI *pCmdUI )
{		
	double ds = 500.0;
	CString s(_T(""));
	CUnitsManager* pUM = CUnitsManager::GetInstance();		
	s.Format("\tGrid Spacing: %5.2f %s",pUM->ConvertLength(ds),pUM->GetLengthUnitString(pUM->GetLengthUnits()));
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText( s );		
}

void CAircraftLayout3DView::OnFinishDistanceMesure( double dist )
{
	dist = UNITSMANAGER->ConvertLength(dist);
	CString msg;
	msg.Format("Distance: %.2f %s", dist, UNITSMANAGER->GetLengthUnitString());	
	AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);
}

void CAircraftLayout3DView::OnFinishPickPoints( const ARCPath3& vPts  )
{
	if (m_hFallbackWnd)
	{
		::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &vPts, NULL);
		m_hFallbackWnd = NULL;
	}
	else
	{
		// 
		switch (m_eViewStatus)
		{
		case viewStatus_None:
			{
				//
			}
			break;
		case viewStatus_ArrayElements:
			{
				if (vPts.size() == 2)
				{
					CDlgArrayCopies dlg;
					if (IDOK == dlg.DoModal())
					{
						GetEditor()->DoArrayElements(vPts, dlg.GetNumCopies());
					}
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
		m_eViewStatus = viewStatus_None;
	}
}

LRESULT CAircraftLayout3DView::OnDlgFallback( WPARAM wParam, LPARAM lParam )
{
	FallbackReason reason = (FallbackReason) lParam;

	//decode the fallback data
	FallbackData* pData = (FallbackData*) wParam;
	m_hFallbackWnd = (HWND) pData->hSourceWnd;	

	switch(reason)
	{
	case PICK_ONEPOINT:
		mLayoutScene.StartPickOnPoint();
		break;
	case PICK_TWOPOINTS:
		mLayoutScene.StartPickTwoPoint();
		break;
	case PICK_MANYPOINTS:
		mLayoutScene.StartPickManyPoint();
		break;
	case PICK_ONERECT:
		mLayoutScene.StartPickRect();
		break;	
	default:
		ASSERT(FALSE);
		return FALSE;
	};	
	return TRUE;
}



void CAircraftLayout3DView::OnRButtonDownOnElement( CAircraftElementShapePtr pelm ,const CPoint &point )
{
	CPoint pt(point);
	ClientToScreen(&pt);			
	CNewMenu menu, *pCtxMenu = NULL;

	CString sTitle;
	do 
	{
		GetEditor()->OnSelectElement(pelm,false);
		GetEditor()->mLastRightButtonDownPt = point;
		if (CSeat*pSeat=  pelm->IsSeat())
		{							
			sTitle = pelm->GetName().GetIDString();
			menu.LoadMenu(IDR_MENU_SEATPROP);
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0) );
			break;
		} else if (pelm && pelm->mpElement)
		{
			sTitle = pelm->GetName().GetIDString();
			menu.LoadMenu(IDR_MENU_AIRCEAFT_ELEMENT);
			if (pelm->GetType() == COnboardCorridor::TypeString)
			{
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(2)); // corridor
			} 
			else
			{
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(1)); // element defined
			}
			break;
		}
		else if (pelm)
		{
			sTitle = pelm->GetName().GetIDString();
			menu.LoadMenu(IDR_MENU_AIRCEAFT_ELEMENT);
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
			break;
		}
	} while (FALSE);


	if(pCtxMenu)
	{
		pCtxMenu->SetMenuTitle(sTitle, MFT_GRADIENT|MFT_TOP_TITLE);
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
	}
}
void CAircraftLayout3DView::OnCtxElementProperties()
{
	if( CModifyElementsPropCmd* pCmd = GetEditor()->OnSelectElementProperties(this) )
	{		
		GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pCmd);		
	}
}

void CAircraftLayout3DView::OnCtxElementDisplayProp()
{
	if(CModifyElementsPropCmd* pCmd = GetEditor()->OnElementDisplayProperties(GetEditor()->m_vSelectedElments, this))
	{		
		GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pCmd);		
	}
}


void CAircraftLayout3DView::OnUndo()
{
	GetEditor()->OnUndo();

}

void CAircraftLayout3DView::OnRedo()
{
	GetEditor()->OnRedo();
}

void CAircraftLayout3DView::OnLayoutLock()
{
	GetEditor()->flipLockView();
}


void CAircraftLayout3DView::OnUpdateUndo(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->Enable( GetEditor()->getHistory().CanUndo());
}

void CAircraftLayout3DView::OnUpdateRedo(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->Enable( GetEditor()->getHistory().CanRedo());
}

void CAircraftLayout3DView::OnUpdateLayoutLock(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->SetCheck(GetEditor()->IsViewLocked());
}


void CAircraftLayout3DView::OnDistanceMeasure()
{
	// TODO: Add your command handler code here
	mLayoutScene.StartDistanceMeasure();	
	Invalidate(FALSE);
}

void CAircraftLayout3DView::OnUpdateDistanceMeasure(CCmdUI* pCmdUI)
{	
	pCmdUI->SetCheck(mLayoutScene.mMouseState ==  CAircraftLayoutScene::_distancemeasure);
}


void CAircraftLayout3DView::OnCtxElementCopy()
{
	// TODO: Add your command handler code here
	GetEditor()->OnCopySelectElements();
	
}

void CAircraftLayout3DView::OnCtxElementDelete()
{
	// TODO: Add your command handler code here
	GetEditor()->OnDeleteSelectElements();
}

void CAircraftLayout3DView::OnCtxElementMove()
{
	// TODO: Add your command handler code here
	GetEditor()->StartMoveSelectElements();
}

void CAircraftLayout3DView::OnCtxElementRotate()
{
	// TODO: Add your command handler code here
	GetEditor()->StartRotateSelectElements();
}




void CAircraftLayout3DView::OnSeatarrayEditarraypath()
{
	// TODO: Add your command handler code here
	GetEditor()->EditArrayPath(!GetEditor()->IsEditArrayPath());
	mLayoutScene.UpdateEditPath();
	Invalidate(FALSE);
}





void CAircraftLayout3DView::OnCtxSelectrows()
{
	// TODO: Add your command handler code here
	GetEditor()->OnSelectSeatRow();
}

void CAircraftLayout3DView::OnCtxSelectarrays()
{
	// TODO: Add your command handler code here
	GetEditor()->OnSelectSeatGroup();
}

void CAircraftLayout3DView::OnCtxSelectseats()
{
	// TODO: Add your command handler code here
	GetEditor()->OnSelectSeat();
}

bool CAircraftLayout3DView::IsAnimation()const
{
	return GetEditor()->m_vSelectedElments.Count()>0;
}

void CAircraftLayout3DView::OnShowText()
{
	GetEditor()->flipShowText();
	Invalidate(FALSE);
}

void CAircraftLayout3DView::OnShowShapes()
{

	
}

void CAircraftLayout3DView::OnFloorAdjust()
{
	
	if(m_pDlgDeckAdjust && m_pDlgDeckAdjust->IsWindowVisible()) 
	{
		//hide (close) window
		m_pDlgDeckAdjust->DestroyWindow();
		delete m_pDlgDeckAdjust;
		m_pDlgDeckAdjust = NULL;
	}
	else 
	{
		delete m_pDlgDeckAdjust;
		m_pDlgDeckAdjust = new CDlgOnboardDeckAdjust(GetEditor()->GetEditLayOut()->GetDeckManager(), this);
		m_pDlgDeckAdjust->Create(CDlgOnboardDeckAdjust::IDD, this);
		CRect rc, rcView;
		m_pDlgDeckAdjust->GetWindowRect(&rc);
		
		GetWindowRect(&rcView);
		rc.OffsetRect(rcView.TopLeft()-rc.TopLeft());
		m_pDlgDeckAdjust->MoveWindow(&rc);
		m_pDlgDeckAdjust->ShowWindow(SW_SHOW);
	}
}

void CAircraftLayout3DView::OnUpdateShowText( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck(GetEditor()->IsShowText());
}


void CAircraftLayout3DView::OnUpdateShowShapes( CCmdUI* pCmdUI )
{

}

bool CAircraftLayout3DView::GetViewPortMousePos(ARCVector3& pos)
{
	CPoint point;
	if (GetCursorPos(&point))
	{
		ScreenToClient(&point);
		return GetMouseWorldPos(point, pos);
	}
	return false;
}

void CAircraftLayout3DView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	C3DDragDropView::OnMouseMove(nFlags, point);
	CAircraftLayOutFrame* pACLayoutFrame = GetParentFrame();
	pACLayoutFrame->m_wndStatusBar.OnUpdateCmdUI(pACLayoutFrame, FALSE);
}


CAircraftLayOutFrame* CAircraftLayout3DView::GetParentFrame() const
{
	CFrameWnd* pFrame = C3DDragDropView::GetParentFrame();
	if (pFrame && pFrame->IsKindOf(RUNTIME_CLASS(CAircraftLayOutFrame)))
	{
		return (CAircraftLayOutFrame*)pFrame;
	}
	ASSERT(FALSE);
	return NULL;
}


void CAircraftLayout3DView::OnUpdateCtxSelectseats(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio( GetEditor()->GetSelSeatType()== CAircraftLayoutEditContext::_selectseat );
}

void CAircraftLayout3DView::OnUpdateCtxSelectrows(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio( GetEditor()->GetSelSeatType()== CAircraftLayoutEditContext::_selectseatrow );
}

void CAircraftLayout3DView::OnUpdateCtxSelectarrays(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio( GetEditor()->GetSelSeatType()== CAircraftLayoutEditContext::_selectseatarray );
}

void CAircraftLayout3DView::OnSeatEditseatrow()
{
	// TODO: Add your command handler code here
	GetEditor()->OnEditSeatRow(this);
}

void CAircraftLayout3DView::OnSeatEditseatarray()
{
	// TODO: Add your command handler code here
	GetEditor()->OnEditSeatGroup(this);
}

void CAircraftLayout3DView::OnSeatCopy()
{
	// TODO: Add your command handler code here
	GetEditor()->OnSeatCopy();
}

void CAircraftLayout3DView::OnSeatDelete()
{
	// TODO: Add your command handler code here
	GetEditor()->OnSeatDelete();
}

void CAircraftLayout3DView::OnSeatEnableeditseatarraypath()
{
	// TODO: Add your command handler code here
	GetEditor()->EditArrayPath(!GetEditor()->IsEditArrayPath());
}

void CAircraftLayout3DView::OnUpdateSeatEnableeditseatarraypath(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(GetEditor()->IsEditArrayPath())
	{
		pCmdUI->SetText(_T("Disable Edit Array Path"));
	}
	else
	{
		pCmdUI->SetText(_T("Enable Edit Array Path"));
	}
	
	pCmdUI->Enable(GetEditor()->GetSelSeatType()==CAircraftLayoutEditContext::_selectseatarray);
	

}

void CAircraftLayout3DView::OnAircraftelementLock()
{
	// TODO: Add your command handler code here
	CAircraftElementShapePtr lastSelect  = GetEditor()->m_lastSelectElm;
	if(lastSelect)
	{
		lastSelect->Lock(!lastSelect->IsLocked());		
	}
}

void CAircraftLayout3DView::OnUpdateAircraftelementLock(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CAircraftElementShapePtr lastSelect  = GetEditor()->m_lastSelectElm;	
	if(lastSelect)
	{
		if(lastSelect->IsLocked())
		{						
			pCmdUI->SetText(_T("UnLock"));
		}
		else
		{
			pCmdUI->SetText(_T("Lock"));
		}
	}
}

void CAircraftLayout3DView::OnCtxScaleShape()
{
	OnMessageMove();
	GetEditor()->StartScaleSelectShapes(CScaleShapesCmd::_scale_xyz);
}

void CAircraftLayout3DView::OnCtxScaleShapeX()
{
	OnMessageMove();
	GetEditor()->StartScaleSelectShapes(CScaleShapesCmd::_scale_x);
}

void CAircraftLayout3DView::OnCtxScaleShapeY()
{
	OnMessageMove();
	GetEditor()->StartScaleSelectShapes(CScaleShapesCmd::_scale_y);
}

void CAircraftLayout3DView::OnCtxScaleShapeZ()
{
	OnMessageMove();
	GetEditor()->StartScaleSelectShapes(CScaleShapesCmd::_scale_z);
}

void CAircraftLayout3DView::OnCtxMoveShapes()
{
	GetEditor()->StartMoveSelectShapes(CMoveShapesCmd::_move_xy);
}

void CAircraftLayout3DView::OnCtxRotateShapes()
{
	OnMessageMove();
	GetEditor()->StartRotateSelectShapes();
}

void CAircraftLayout3DView::OnCtxMoveshapeZ()
{
	OnMessageMove();
	GetEditor()->StartMoveSelectShapes(CMoveShapesCmd::_move_z);
}

void CAircraftLayout3DView::OnCtxMoveshapeZ0()
{
	GetEditor()->DoMoveShapesZ0();
}


void CAircraftLayout3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_DELETE:		
		OnCtxElementDelete();
		break;
	case VK_INSERT:
		OnCtxElementCopy();
		break;
	/*case VK_LEFT:
		GetEditor->startGetDocument()->OnRotateShape(static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	case VK_RIGHT:
		GetDocument()->OnRotateShape(-static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	case VK_UP:
		GetDocument()->OnRotateProc(static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	case VK_DOWN:
		GetDocument()->OnRotateProc(-static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;*/
	case VK_RETURN:
		GetEditor()->DoneCurEdit();
		break;
	case VK_ESCAPE:
		GetEditor()->CancelCurEdit(&mLayoutScene);
		break;
	}
	C3DDragDropView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CAircraftLayout3DView::OnTransformshapeTransformproperties()
{
	GetEditor()->OnEditShapeTranform(this);
}

void CAircraftLayout3DView::OnMessageMove()
{
	DWORD dwPos = ::GetMessagePos();
	CPoint pt;
	pt.x = (int) LOWORD(dwPos);
	pt.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&pt);
	C3DDragDropView::OnMouseMove(0,pt);
}

void CAircraftLayout3DView::ShowDeckAdjustDialog()
{
	OnFloorAdjust();
}

void CAircraftLayout3DView::UpdateFloorAltitude( CDeck *pDeck )
{
	mLayoutScene.UpdateDeckAltitude(pDeck);
	Invalidate(FALSE);
}
void CAircraftLayout3DView::OnDestroy()
{
	C3DDragDropView::OnDestroy();

	if(m_pDlgDeckAdjust) 
	{
		//hide (close) window
		m_pDlgDeckAdjust->DestroyWindow();
		delete m_pDlgDeckAdjust;
		m_pDlgDeckAdjust = NULL;
	}
	// TODO: Add your message handler code here
}

void CAircraftLayout3DView::OnArrayElements()
{
	m_eViewStatus = viewStatus_ArrayElements;
	mLayoutScene.StartPickTwoPoint();
}

bool CAircraftLayout3DView::IsDropAllowed( const DragDropDataItem& dropItem ) const
{
	return /*!GetEditContext()->IsViewLocked() &&*/ dropItem.GetDataType() == DragDropDataItem::Type_AircraftFurnishing;
}

void CAircraftLayout3DView::DoDrop( const DragDropDataItem& dropItem, CPoint point )
{
	ASSERT(dropItem.GetCount() == 1);
	CString strPtr  = (LPCTSTR)dropItem.GetItem(0).GetData();
	int nPtr = atoi(strPtr.GetString());
	CAircraftFurnishingModel* pFurnishing = (CAircraftFurnishingModel*)nPtr;	

	ARCVector3 dropPos;
	if( GetMouseWorldPos(point,dropPos) && pFurnishing )
	{
		CAddAircraftElementsCmd* pCmd =  GetEditor()->OnNewElement(dropPos,pFurnishing,this);
		if(pCmd){
			GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pCmd);
		}
	}
}

void CAircraftLayout3DView::OnCorridorEnableeditseatpath()
{
	GetEditor()->EditCorridor(!GetEditor()->IsEditCorridor());
	mLayoutScene.UpdateEditPath();
}

void CAircraftLayout3DView::OnUpdateCorridorEnableeditseatpath( CCmdUI *pCmdUI )
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(GetEditor()->IsEditCorridor() ? _T("Disable Edit Path") : _T("Enable Edit Path"));
}

void CAircraftLayout3DView::OnRButtonDownOnEditPath( CAircraftElementShapePtr pElem, int nPathIndex, int nNodeIndex, const CPoint& point )
{
	m_pathRelatedElement = pElem;
	m_nEditedPathIndex = nPathIndex;
	m_nEditedPathNodeIndex = nNodeIndex;
	m_ptPathEditPos = point;

	CPoint pt(point);
	ClientToScreen(&pt);

	CNewMenu menu, *pCtxMenu = NULL;

	CString sTitle;
	do 
	{
		if (pElem->mpElement && pElem->mpElement->GetType() == COnboardCorridor::TypeString)
		{
			if (nNodeIndex<0)
			{
				if (nPathIndex>=0/* && nPathIndex<COnboardCorridor::drawLinkPath_Start*/)
				{
					// path line selected
					menu.LoadMenu(IDR_MENU_PATHEDIT_POPUP);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
				}
			}
			else
			{
				// path node selected
				menu.LoadMenu(IDR_MENU_PATHEDIT_POPUP);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(1));
			}
			break;
		}
	} while (FALSE);


	if(pCtxMenu)
	{
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
	}
}

void CAircraftLayout3DView::OnPathAddEditPoint()
{
	ARCVector3 worldPos;
	if (GetMouseWorldPos(m_ptPathEditPos, worldPos))
	{
		if (GetEditor()->StartNewCommand(new CModifyElementsPropCmd(m_pathRelatedElement, mLayoutScene.GetEditLayout(), CModifyElementsPropCmd::_Prop)))
		{
			if (mLayoutScene.AddEditPointToAircraftElement(m_pathRelatedElement, m_nEditedPathIndex, worldPos))
			{
				mLayoutScene.OnUpdateOperation(GetEditor()->DoneCurEdit());
			}
			else
			{
				GetEditor()->CancelCurEdit(&mLayoutScene);
			}
		}
	}
}

void CAircraftLayout3DView::OnPathRemoveEditPoint()
{
	if (GetEditor()->StartNewCommand(new CModifyElementsPropCmd(m_pathRelatedElement, mLayoutScene.GetEditLayout(), CModifyElementsPropCmd::_Prop)))
	{
		if (mLayoutScene.RemoveEditPointFromAircraftElement(m_pathRelatedElement, m_nEditedPathIndex, m_nEditedPathNodeIndex))
		{
			mLayoutScene.OnUpdateOperation(GetEditor()->DoneCurEdit());
		}
		else
		{
			GetEditor()->CancelCurEdit(&mLayoutScene);
		}
	}
}

void CAircraftLayout3DView::SaveCamerasSettings()
{
	CAircaftLayOut* pLayout = GetEditor()->GetEditLayOut();
	m_cameras.SaveIntoDatabase(C3DViewCameras::enumAircraftLayout, pLayout->GetID());
}

void CAircraftLayout3DView::AddNewNamedView(CString strViewName)
{
	m_cameras.AddSavedCameraDesc(strViewName);
}

void CAircraftLayout3DView::ReadCamerasSettings()
{
	CAircraftLayoutEditor* pEditor = GetEditor();
	if (pEditor)
	{
		m_cameras.ReadFromDatabase(C3DViewCameras::enumAircraftLayout, pEditor->GetEditLayOut()->GetID());
	}
	else
	{
		m_cameras.InitCameraData();
	}
}

void CAircraftLayout3DView::LoadCamerasSettings()
{
	CCameraNode camera = mLayoutScene.GetCamera( mViewCameraName );
	camera.Load(m_cameras.GetUserCamera().GetDefaultCameraData());

	m_coordDisplayScene.UpdateCamera(camera.mpCamera);

 	Invalidate(FALSE);
}

C3DViewCameras& CAircraftLayout3DView::Get3DViewCameras()
{
	return m_cameras;
}

const C3DViewCameras& CAircraftLayout3DView::Get3DViewCameras() const
{
	return m_cameras;
}
