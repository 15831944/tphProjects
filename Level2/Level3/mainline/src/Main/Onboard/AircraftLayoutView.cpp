#include "StdAfx.h"
#include ".\aircraftlayoutview.h"
#include "../TermPlanDoc.h"
#include "../../InputOnboard/CInputOnboard.h"
#include "OnboardViewMsg.h"
#include "../termplan.h"
#include "../../InputOnBoard/AircraftLayoutEditContext.h"
#include <InputOnBoard/Seat.h>
#include <InputOnBoard/AircraftPlacements.h>
#include <inputOnboard/AircraftFurnishings.h>
#include "../DlgSeatRowProp.h"
#include "../OnBoardSeatDlg.h"
#include "../../InputOnBoard/Door.h"
#include "../../InputOnBoard/Storage.h"
#include "../../InputOnBoard/CabinDivider.h"
#include "../AnimationTimeManager.h"
#include "../OnBoardCabinDividerDlg.h"
#include "../OnBoardStorageDlg.h"
#include "../OnBoardDoorDlg.h"
#include "../DlgOnBoardObjectDisplayProperty.h"
#include "../LayOutFrame.h"
#include "../../InputAirside/ARCUnitManager.h"
#include "../../common/UnitsManager.h"
#include <InputOnboard/AircaftLayOut.h>
#include <Common/OleDragDropManager.h>
#include "AircraftLayoutEditor.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const UINT	PANZOOMTIMEOUT = 1000;
static const UINT	ONSIZETIMEOUT = 1000;
static const UINT	MOUSEMOVETIMEOUT = 1000;

using namespace Ogre;

IMPLEMENT_DYNCREATE(CAircraftLayoutView, C3DSceneView)
BEGIN_MESSAGE_MAP(CAircraftLayoutView, C3DSceneView)	
	//{{AFX_MSG_MAP(CAircraftLayoutView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()	
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_ANIMPAX,OnAnimPax)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_PROPERTIES,OnAircraftElementProperties)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_DISPLAYPROPERTIES,OnAircraftElementDisplayProp)
	ON_COMMAND(ID_SEAT_CREATEROW, OnCreateSeatRow)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_COPY, OnCopyElements)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_DELETE, OnDeleteElements)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_MOVE, OnMoveElements)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_ROTATE, OnRotateElements)
	ON_COMMAND(ID_SEAT_SCALE, OnScaleElements)

	ON_COMMAND(ID_SEATROW_UNROW, OnUnRow)
	ON_COMMAND(ID_SEATROW_CREATEARRAY,OnCreateSeatArray)
	ON_COMMAND(ID_SEATROW_ROTATE, OnRotateSeatRow)
	ON_COMMAND(ID_SEATROW_MOVEXY, OnMoveSeatRow)
	ON_COMMAND(ID_SEATROW_DELETEROW,OnDeleteRow)

	ON_COMMAND(ID_SEATARRAY_PROPERTIES, OnSeatArrayProp)
	ON_COMMAND(ID_SEATARRAY_DISPLAYPROPERTIES, OnSeatArrayProp)
	ON_COMMAND(ID_SEATARRAY_UNARRAY, OnUnArraySeats)
	ON_COMMAND(ID_SEATARRAY_DELETEARRAY, OnDeleteArray)
	ON_COMMAND(ID_SEATARRAY_EDITARRAYPATH, OnEditArrayEditPoint)

	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GRIDSPACING, OnUpdateGridSpacing)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FPS, OnUpdateFPS)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMTIME, OnUpdateAnimTime)
	ON_UPDATE_COMMAND_UI(ID_ANIMPAX,OnUpdateAnimPax)


	ON_COMMAND(ID_SEATROW_COPY, OnSeatrowCopy)
	ON_UPDATE_COMMAND_UI(ID_SEATARRAY_EDITARRAYPATH, OnUpdateSeatarrayEditarraypath)
	ON_COMMAND(ID_ARRAYPOINT_DELETE, OnArraypointDelete)
	ON_COMMAND(ID_AIRCRAFTELEMENTPROP_MOVEZ, OnMoveElmentsZ)
	ON_COMMAND(ID_SCENEPICK_RESTOREDEFAULTCAMERA, OnScenepickRestoredefaultcamera)
	
	ON_COMMAND(ID_MenuRuler, OnMenuruler)
	ON_COMMAND(ID_SCENEPICK_STARTANIM, OnScenepickStartanim)
	ON_COMMAND(ID_SCENEPICK_STOPANIM, OnScenepickStopanim)
	ON_COMMAND(ID_SCENEPICK_SHOWTEXT, OnScenepickShowtext)
	ON_UPDATE_COMMAND_UI(ID_SCENEPICK_SHOWTEXT, OnUpdateScenepickShowtext)
END_MESSAGE_MAP()


CAircraftLayoutView::CAircraftLayoutView(void)
{
	m_pDragShape = NULL;
	m_eMouseState = _default;
	g_ptCmdUI = NULL;
	m_bEditArrayPath = false;
	double m_dFrameRate = 0.0; 
	m_nMouseMoveTimeout = 5;
}

CAircraftLayoutView::~CAircraftLayoutView(void)
{
}

CAircraftLayoutEditContext* CAircraftLayoutView::GetEditContext() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	
	InputOnboard* pInputOnboard= NULL;
	if(pDoc)
		return pDoc->GetAircraftLayoutEditor();

	//if(pInputOnboard)
	//	return pInputOnboard->GetAircraftLayOutEditContext();

	return NULL;

}

void CAircraftLayoutView::SetupViewport()
{
	Ogre::Camera* pCam = m_aircraftlayoutScene.GetActiveCamera();
	Ogre::Viewport* pViewport = m_sceneWnd.AddViewport(pCam);
	pViewport->setBackgroundColour(SceneConfig::BackGroundColor);
	pViewport->setOverlaysEnabled(false);
}

void CAircraftLayoutView::SetupScene()
{
	m_aircraftlayoutScene.SetupScene();
	
	//GetEditContext()->GetEditLayOut()->UpdateDecksOutline(GetEditContext()->GetCurACModel());
	//GetEditContext()->GetEditLayOut()->UpdateCabinDividers(GetEditContext()->GetCurACModel());
	m_aircraftlayoutScene.BuildScene(GetEditContext());
}

void CAircraftLayoutView::OnMouseMove(UINT nFlags, CPoint point)
{

	// TODO: Add your message handler code here and/or call default
	if(!IsViewSetuped())
		return;

	//get mouse pos	
	CPoint mouseDiff = point - m_ptFrom;
	m_ptFrom = point;	

	ARCVector3 curWorldPos = GetWorldMousePos(point);
	ARCVector3 posDiff = curWorldPos - m_ptMouseWorldPos; 
	m_ptMouseWorldPos = curWorldPos;

	m_nMouseMoveTimeout = SetTimer(m_nMouseMoveTimeout, MOUSEMOVETIMEOUT, NULL);

	CLayOutFrame* pFrame = (CLayOutFrame*)GetParentFrame();
	// status bar
	pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);

	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{
		Ogre::Camera* pCam = m_aircraftlayoutScene.GetActiveCamera();
		
		if(nFlags & MK_LBUTTON)
		{  
			MyCameraOp::PanCamera(pCam, mouseDiff, m_aircraftlayoutScene.m_curActivePlane);
			m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
			Invalidate(FALSE);				
		}
		else if(nFlags & MK_MBUTTON)
		{ 
			MyCameraOp::TumbleCamera(pCam, mouseDiff, m_aircraftlayoutScene.m_curActivePlane);
			m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
			Invalidate(FALSE);		
		}
		else if(nFlags & MK_RBUTTON)
		{ 
			MyCameraOp::ZoomInOutCamera(pCam,mouseDiff, m_aircraftlayoutScene.m_curActivePlane);
			m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
			Invalidate(FALSE);
		}
	}
	else
	{
		switch(m_eMouseState)
		{	
		case _moveSeatArrayPoint:
			GetEditContext()->OnMoveSeatArrayPathPoint(posDiff,mSelectArrayPtIdx);
			m_aircraftlayoutScene.UpdateModifySeatArray(GetEditContext()->m_pSelectSeatGroup);
			Invalidate(FALSE);
			break;
		case _moveSeatArray:
			GetEditContext()->OnMoveSeatArray(posDiff);
			m_aircraftlayoutScene.UpdateModifySeatArray(GetEditContext()->m_pSelectSeatGroup);
			Invalidate(FALSE);
			break;
		case _moveSeatRow:
			GetEditContext()->OnMoveSeatRow(posDiff);
			//m_aircraftlayoutScene.UpdateModifySeatRow( GetEditContext()->m_pSelectSeatRow );
			Invalidate(FALSE);
			break;
		case _rotateSeatRow:
			GetEditContext()->OnRoatateSeatRow(mouseDiff.x);			
			//m_aircraftlayoutScene.UpdateModifySeatRow( GetEditContext()->m_pSelectSeatRow );
			Invalidate(FALSE);
			break;
		case _moveElements:
			GetEditContext()->OnMoveSelectElements(posDiff);
			m_aircraftlayoutScene.UpdateModifyElments( GetEditContext()->m_vSelectedElments );
			Invalidate(FALSE);
			break;	
		case _moveElementsZ :
			GetEditContext()->OnMoveSelectElements( ARCVector3(0,0,mouseDiff.x) );
			m_aircraftlayoutScene.UpdateModifyElments( GetEditContext()->m_vSelectedElments );
			Invalidate(FALSE);
			break;
		case _rotateElements:
			GetEditContext()->OnRotateSelectElements(mouseDiff.x);
			m_aircraftlayoutScene.UpdateModifyElments( GetEditContext()->m_vSelectedElments );			
			Invalidate(FALSE);			
			break;
		case _getarrayline:
		case _getmirrorline:
		case _getonepoint:
		case _gettwopoints:
		case _getmanypoints:		
		case _distancemeasure:
			Invalidate(FALSE);
			break;
		case _default:
			if(nFlags&MK_LBUTTON) {
				//if(m_bMovingProc) {
				//	m_bProcHasMoved = TRUE;
				//	//GetDocument()->OnMoveProcs(ptDiffPos[VX],ptDiffPos[VY],point,this);
				//	Invalidate(FALSE);
				//}				
			}
			break;
		}	
	}
	C3DSceneView::OnMouseMove(nFlags, point);
}

void CAircraftLayoutView::OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class
	if(lHint == VM_ONBOARD_LAYOUT_CHANGE)
	{		
		m_aircraftlayoutScene.BuildScene( GetEditContext());
	}
	else if(lHint == VM_ONBOARD_DECKMAPCHANGE)
	{
		CDeck* pDeck = (CDeck*)pHint;
		m_aircraftlayoutScene.UpdateDeckMap(pDeck);
	}
	else if(lHint == VM_ONBOARD_ACTIVE_DECK_CHANGE)
	{
		m_aircraftlayoutScene.UpdateActiveDeck(GetEditContext()->IsShowText());
	}
	else if(lHint ==VM_ONBOARD_DECK_HIDESHOW)
	{
		m_aircraftlayoutScene.UpdateDeckHideShow();
	}
	else if(lHint == VM_ONBOARD_AIRCRAFTELEMENT_DISPLAYPROPERTY)
	{
		CAircraftElement* pElement = (CAircraftElement*)pHint;
		ShowDisplayPropertyDialog(pElement);
	}
	else if(lHint == VM_ONBOARD_DEL_AIRCRAFTELEMENT)
	{
		CAircraftElement* pElement = (CAircraftElement*)pHint;
		GetEditContext()->GetEditLayOut()->GetPlacements()->DeleteElement(pElement);	
		m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
	}
	else if(lHint == VM_ONBOARD_DECK_CHANGED)
	{		
		m_aircraftlayoutScene.GetLayOut3D().UpdateAddOrRemoveDeck(GetEditContext()->GetEditLayOut());
	}

	Invalidate(FALSE);
	CView::OnUpdate(pSender, lHint, pHint);
}

//////////////////////////////////////////////////////////////////////////
DROPEFFECT CAircraftLayoutView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject)
		&& dataItem.GetDataType() == DragDropDataItem::Type_AircraftFurnishing
		)
	{
		m_pDragShape = *(CAircraftFurnishing**)dataItem.GetItem(0).GetData();
		//m_pDragShape = GetInputOnboard()->GetFurnishingData()->GetItem(0);
		// TRACE("Drag enter with shape: %s\n", m_pDragShape->Name());
		return DROPEFFECT_COPY;
	}
	m_pDragShape = NULL;
	return DROPEFFECT_NONE;
}

DROPEFFECT CAircraftLayoutView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	OnMouseMove(dwKeyState, point);

	if(m_pDragShape != NULL)
		return DROPEFFECT_COPY;
	else		
		return DROPEFFECT_NONE;
}

void CAircraftLayoutView::OnDragLeave() 
{
	CView::OnDragLeave();
	if(m_pDragShape != NULL) {
		m_pDragShape = NULL;
	}
}


BOOL CAircraftLayoutView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CView::OnDrop(pDataObject, dropEffect, point);

	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject)
		&& dataItem.GetDataType() == DragDropDataItem::Type_AircraftFurnishing
		)
	{
		m_pDragShape = *(CAircraftFurnishing**)dataItem.GetItem(0).GetData();
		if (m_pDragShape)
		{
			CAircaftLayOut * playout =  GetEditContext()->GetEditLayOut();
			CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
			if(!pDeck)
			{
				AfxMessageBox("Please select a active deck first");
				return FALSE;			
			}
			//double dHeight = pDeck->GetGroundVal();
			//m_aircraftlayoutScene.SetActivePlane(dHeight);
			
			Ogre::Ray mouseRay;		
			if( m_sceneWnd.getPtMouseRay(point, mouseRay) )
			{
				Ogre::Vector3 dropPos = m_aircraftlayoutScene.GetRayIntesectPos(mouseRay);
				int nProjID = ((CTermPlanDoc *)GetDocument())->GetProjectID();

				if(m_pDragShape && m_pDragShape->GetType() == CSeat::TypeString )
				{
					CSeat* pNewSeat = new CSeat;				
					pNewSeat->SetDeck( pDeck );
					pNewSeat->SetShape(m_pDragShape->GetFileName());
					pNewSeat->SetPosition(ARCVector3(dropPos.x, dropPos.y,0));		
					pNewSeat->SetLayOut(GetEditContext()->GetEditLayOut());
					
					COnBoardSeatDlg dlg(pNewSeat,playout,nProjID,this);
					if(dlg.DoModal() ==IDOK){
						playout->GetPlacements()->AddElement(pNewSeat);
						GetDocument()->UpdateAllViews(this,VM_ONBOARD_ADD_AIRCRAFTELEMENT,(CObject *)pNewSeat);
						m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
						Invalidate(FALSE);
					}
					else
						delete pNewSeat;
				}
				else if(m_pDragShape && m_pDragShape->GetType() == CDoor::TypeString )
				{
					CDoor* pNewDoor = new CDoor;
					pNewDoor->SetDeck(pDeck);
					pNewDoor->m_ShapeName = m_pDragShape->GetFileName();
					pNewDoor->SetPosition( ARCVector3(dropPos.x, dropPos.y,0) );
					pNewDoor->SetLayOut(GetEditContext()->GetEditLayOut());

					//test
					COnBoardDoorDlg dlg(pNewDoor,playout,nProjID,this);
					if(dlg.DoModal() == IDOK)
					{
						//pNewDoor->SetIDName(ALTObjectID("Door-A-1"));
						GetDocument()->UpdateAllViews(this,VM_ONBOARD_ADD_AIRCRAFTELEMENT,(CObject *)pNewDoor);
						playout->GetPlacements()->AddElement(pNewDoor);
						m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
						Invalidate(FALSE);
					}
					else
						delete pNewDoor;

					
					/*
					COnBoardSeatDlg dlg(pNewDoor,playout,nProjID,this);
					if(dlg.DoModal() ==IDOK){
						playout->GetPlacements()->GetElementDataSet()->AddNewItem(pNewDoor);
						m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
						Invalidate(FALSE);
					}
					else
						delete pNewDoor;
						*/

				}
				else if(m_pDragShape && m_pDragShape->GetType() == CStorage::TypeString  )
				{
					CStorage * pnewStorage = new CStorage;
					pnewStorage->SetDeck(pDeck);
					pnewStorage->SetShape( m_pDragShape->GetFileName() );
					pnewStorage->SetPosition( ARCVector3(dropPos.x, dropPos.y,0) );
					pnewStorage->SetLayOut(GetEditContext()->GetEditLayOut());
					/*
					
					*/
					//pnewStorage->SetIDName( ALTObjectID("Storage-A-1") );
					COnBoardStorageDlg dlg(pnewStorage,playout,nProjID,this);
					if(dlg.DoModal() == IDOK)
					{
						GetDocument()->UpdateAllViews(this,VM_ONBOARD_ADD_AIRCRAFTELEMENT,(CObject *)pnewStorage);
						playout->GetPlacements()->AddElement(pnewStorage);
						m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
						Invalidate(FALSE);
					}
					else 
						delete pnewStorage;

					
				}
				else if(m_pDragShape && m_pDragShape->GetType() == CCabinDivider::TypeString)
				{
					CCabinDivider* pCabinDiv = new CCabinDivider;
					pCabinDiv->SetPos( dropPos.x );
					pCabinDiv->SetDeck(pDeck);
					pCabinDiv->SetLayOut(GetEditContext()->GetEditLayOut());
					/*

					*/
					COnBoardCabinDividerDlg dlg(pCabinDiv,playout,nProjID,this);
					if(dlg.DoModal() == IDOK)
					{
						GetDocument()->UpdateAllViews(this,VM_ONBOARD_ADD_AIRCRAFTELEMENT,(CObject *)pCabinDiv);
						//pCabinDiv->SetIDName( ALTObjectID("CabinDiv-A-1") );
// 						pCabinDiv->UpdateOutLine( GetEditContext()->GetCurACModel(), GetEditContext()->GetEditLayOut() );
						playout->GetPlacements()->AddElement(pCabinDiv);
						m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
						Invalidate(FALSE);
					}
					else
						delete pCabinDiv;

				}
			}
			return TRUE;
		}
	}	
	return FALSE;
}



int CAircraftLayoutView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (C3DSceneView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here
	m_oleDropTarget.Register(this);
	return 0;
}



CAircraftLayoutView::EmSelectItemType CAircraftLayoutView::SelectScene( const CPoint& point )
{
	Ogre::Ray curmouseray;
	if(m_sceneWnd.getPtMouseRay(point,curmouseray))
	{	
		
		if(::GetAsyncKeyState(VK_CONTROL)<0) //ctrl is pressed
		{		
		}
		else
		{
			m_aircraftlayoutScene.ShowBoundBox( GetEditContext()->m_vSelectedElments, FALSE);
			GetEditContext()->ClearSelect();
			
		}
		Ogre::MovableObject* pSelObj = m_aircraftlayoutScene.GetMouseSelect(curmouseray);
		if(pSelObj && pSelObj->getQueryFlags() == ACELEMENT )
		{
			CAircraftElement* pElm = (CAircraftElement*)pSelObj->getUserObject();			
			if(pElm && pElm->GetType() ==CSeat::TypeString)
			{
				CAircraftSeatsManager* pSeatMan = GetEditContext()->GetEditLayOut()->GetPlacements()->GetSeatManager();
				CSeat* pSeat = (CSeat*)pElm;
				if( pSeatMan && pSeat)
				{
					CSeatRow* pSeatRow = NULL;
					CSeatGroup* pSeatGroup = NULL;
					//pSeatRow = pSeatMan->GetSeatRow(pSeat);
					//pSeatGroup = pSeatMan->GetSeatGroup(pSeatRow);

					if(pSeatGroup)
					{
						GetEditContext()->m_pSelectSeatGroup = pSeatGroup;
						return SEAT_ARRAY;
					}
					else if(pSeatRow)
					{
						//GetEditContext()->m_pSelectSeatRow = pSeatRow;
						return SEAT_ROW;
					}
					if(!pSeatRow && !pSeatGroup)
					{
						GetEditContext()->SelectElement(pSeat);
						return SEAT;
					}
				}

			}
			
			if(pElm){
				GetEditContext()->SelectElement(pElm);
				return AIRCRAFT_ELMENT;
			}
		}		
		if(pSelObj && pSelObj->getQueryFlags() == SEATARRAY_CTRLPOINT)
		{
			CSeatGroup::PathPointInfo ptIfo = Ogre::any_cast<CSeatGroup::PathPointInfo>(pSelObj->getUserAny());			
			GetEditContext()->m_pSelectSeatGroup = ptIfo.pSeatGrp;
			mSelectArrayPtIdx = ptIfo.ptidx;
			return SEAT_ARRAY_PATH_POINT;
		}

		m_aircraftlayoutScene.ShowBoundBox( GetEditContext()->m_vSelectedElments, TRUE);
		
	}
	return NONE;
}

void CAircraftLayoutView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_ptFrom = point;

	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);

	//m_aircraftlayoutScene.StartAnimation();
	if(::GetAsyncKeyState(VK_MENU)>=0) 
	{//ALT is NOT pressed 
		switch(m_eMouseState)
		{
		case _pan:
		case _dolly:
		case _tumble:
			SetCapture();
			m_bTrackLButton = TRUE;
			break;
		case _floormanip:
			SetCapture();
			m_bTrackLButton = TRUE;
			break;
		case _getonerect:
			SetCapture();
			m_vMousePosList.push_back(m_ptMouseWorldPos);
			break;
		case _placemarker:
		case _placeproc:
		case _scaleproc:
		case _scaleprocX:
		case _scaleprocY:
		case _scaleprocZ:
		case _rotateproc:
		case _rotateairsideobject:
		case _moveprocs:
		case _rotateshapes:
		case _moveshapes:
		case _getonepoint:
		case _getmanypoints:
			SetCapture();
			break;
		case _pickgate:
		case _pickproc:		
		case _default:
			EmSelectItemType selItemType = SelectScene(point);		
			switch (selItemType)
			{
			case AIRCRAFT_ELMENT :
			case SEAT:
				m_eMouseState = _moveElements;
				break;
			case SEAT_ARRAY:
				m_eMouseState = _moveSeatArray;
				break;
			case SEAT_ROW :
				m_eMouseState = _moveSeatRow;
				break;
			case SEAT_ARRAY_PATH_POINT :
				m_eMouseState = _moveSeatArrayPoint;
				break;
			default:
				break;
			}
			break;
			
		}
	}
	else 
	{
		SetCapture();
	}
	Invalidate(FALSE);
	C3DSceneView::OnLButtonDown(nFlags, point);
}

void CAircraftLayoutView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//m_aircraftlayoutScene.EndAnimation();
	if(::GetAsyncKeyState(VK_MENU)>=0) {//ALT is NOT pressed 
		switch(m_eMouseState)
		{
		case _pan:
		case _dolly:
		case _tumble:
			ReleaseCapture();
			break;
		case _floormanip:
			ReleaseCapture();
			break;
		
		case _moveSeatRow : 
		case _rotateSeatRow : 			
			ReleaseCapture();
			m_eMouseState = _default;
			GetEditContext()->DoneEditSelectSeatRow();
			break;
		case _moveSeatArray:
			ReleaseCapture();
			m_eMouseState = _default;	
			GetEditContext()->DoneEditSelectSeatGroup();
			break;
		case _moveElements : 
		case _rotateElements :
		case _scaleElements : 
		case _moveElementsZ :
			ReleaseCapture();			
			m_eMouseState = _default;	
			GetEditContext()->DoneEditSelectElments();
			break;
		case _moveSeatArrayPoint:
			ReleaseCapture();
			m_eMouseState = _default;		
			GetEditContext()->DoneEditSelectSeatGroup();
			break;
		case _placemarker:
			ReleaseCapture();
			m_eMouseState = _default;
			//set marker location to active floor			
			Invalidate(FALSE);
			break;
		case _distancemeasure:
			m_vMousePosList.push_back(m_ptMouseWorldPos);
			break;
		case _gettwopoints:
		case _getmirrorline:
		case _getarrayline:
			m_vMousePosList.push_back(m_ptMouseWorldPos);
			if(m_vMousePosList.size() == 2) {
				if(m_eMouseState==_gettwopoints)
				{
					::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
				}
				m_eMouseState = _default;
			}
			Invalidate(FALSE);
			break;
		case _getonepoint:
			ReleaseCapture();
			m_vMousePosList.push_back(m_ptMouseWorldPos);
			::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = _default;
			Invalidate(FALSE);
			break;
		case _getmanypoints:
			ReleaseCapture();
			m_vMousePosList.push_back(m_ptMouseWorldPos);			
			Invalidate(FALSE);
			break;		
		case _getonerect:
			ReleaseCapture();
			::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = _default;
			Invalidate(FALSE);
			break;
		case _pickgate:
		case _pickproc:
			ReleaseCapture();
			::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState=_default;
			Invalidate(FALSE);
			break;
		case _pasteproc:
			{
				m_eMouseState=_default;
				Invalidate(FALSE);
			}
			break;
		case _default:
			ReleaseCapture();		
			break;
		};
	}
	else {
		ReleaseCapture();
	}
	m_bTrackLButton = FALSE;
	C3DSceneView::OnLButtonUp(nFlags, point);
}

void CAircraftLayoutView::OnRButtonDown(UINT nFlags, CPoint point)
{	
	m_ptMouseWorldPos = GetWorldMousePos(point);
	SetCapture();
	// TODO: Add your message handler code here and/or call default
	if(::GetAsyncKeyState(VK_MENU) >=0 )
	{
		switch(m_eMouseState)
		{
		case _pan:
		case _dolly:
		case _tumble:
		case _floormanip:
		case _placeproc:
		case _scaleproc:
		case _scaleprocX:
		case _scaleprocY:
		case _scaleprocZ:
		case _rotateproc:
		case _rotateairsideobject:
		case _moveprocs:
		case _getmirrorline:
		case _getarrayline:
		case _getonepoint:
		case _gettwopoints:
			break ;
		case _getmanypoints:
			m_vMousePosList.push_back(m_ptMouseWorldPos);			
			::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = _default;			
			Invalidate(FALSE);
			break ;
		case _getonerect:
			break;
		case _default:		
			EmSelectItemType selItemType = SelectScene(point);
			CPoint pt(point);
			ClientToScreen(&pt);			
			CNewMenu menu, *pCtxMenu = NULL;
			switch (selItemType)
			{
			case SEAT:
				menu.LoadMenu(IDR_MENU_SEATPROP);	
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
				pCtxMenu->SetMenuTitle(_T("Seat"), MFT_GRADIENT|MFT_TOP_TITLE);
				break;
			case SEAT_ARRAY :				
				menu.LoadMenu(IDR_MENU_SEATPROP);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(2));		
				pCtxMenu->SetMenuTitle(_T("Seat Array"), MFT_GRADIENT|MFT_TOP_TITLE);				
				break;
			case SEAT_ROW :
				menu.LoadMenu(IDR_MENU_SEATPROP);		
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(1));	
				pCtxMenu->SetMenuTitle(_T("Seat Row"), MFT_GRADIENT|MFT_TOP_TITLE);
				break;
			case SEAT_ARRAY_PATH_POINT:
				menu.LoadMenu(IDR_MENU_SEATPROP);				
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(3));		
				pCtxMenu->SetMenuTitle(_T("Seat Array Point"), MFT_GRADIENT|MFT_TOP_TITLE);		
				break;
			case AIRCRAFT_ELMENT:
				menu.LoadMenu(IDR_MENU_AIRCEAFT_ELEMENT);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));		
				pCtxMenu->SetMenuTitle(_T("Aircraft Elment"), MFT_GRADIENT|MFT_TOP_TITLE);	
				break;
			default :
				menu.LoadMenu(IDR_MENU_ONBOARD_SCENE);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
				pCtxMenu->SetMenuTitle(_T("Scene"), MFT_GRADIENT|MFT_TOP_TITLE);	
				break;
			}
			if(pCtxMenu)
			{					
				pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
			}	
			break;
		}
	}
	C3DSceneView::OnRButtonDown(nFlags, point);

}
void CAircraftLayoutView::OnUpdateAnimPax( CCmdUI* pCmdUI )
{

}

void CAircraftLayoutView::OnUpdateAnimTime( CCmdUI *pCmdUI )
{
	g_ptCmdUI = pCmdUI;

	 CTermPlanDoc *pTermPlanDoc = (CTermPlanDoc *)GetDocument();
	if(pTermPlanDoc->m_eAnimState != CTermPlanDoc::anim_none) {
		long nTime = pTermPlanDoc->m_animData.nLastAnimTime;
		BOOL bMoveDay = pTermPlanDoc->m_animData.bMoveDay;

		int nHour = nTime / 360000;
		int nMin = (nTime - nHour*360000)/6000;
		int nSec = (nTime - nHour*360000 - nMin*6000)/100;


		//	Modified by kevin
		CString s(_T(""));

		CStartDate startDate = pTermPlanDoc->m_animData.m_startDate;
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		startDate.GetDateTime( ElapsedTime(nTime / TimePrecision), bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			s = date.Format(_T("%Y-%m-%d"));
		}	
		else
		{
			if(bMoveDay)
				nDtIdx -= 1;
			s.Format(_T("Day %d"), nDtIdx + 1);
		}
		s += _T("  ") + time.Format(_T("%H:%M:%S"));

		//		FILE *fw = fopen("C:\\debug.txt","a+");
		//		fprintf(fw, "%s\n", (char*)(const char*)s);
		//		fclose(fw);

		if( NULL == pCmdUI )
		{
			if(g_ptCmdUI)
			{
				g_ptCmdUI->Enable(TRUE);
				g_ptCmdUI->SetText( s );

				g_ptCmdUI->m_pOther->UpdateWindow();
			}
		}
		else
		{
			pCmdUI->Enable(TRUE);
			pCmdUI->SetText( s );

			pCmdUI->m_pOther->UpdateWindow();
		}
	}
	else
	{
		if(pCmdUI){
			//if( GetParentFrame()->IsLoadingModel() )
			//{
			//	CString loadingText;
			//	loadingText.Format("Loading Shapes (%d %)...", (int)GetParentFrame()->GetLoadingPercent() * 100);
			//	pCmdUI->Enable(TRUE);
			//	pCmdUI->SetText(loadingText);
			//	pCmdUI->m_pOther->UpdateWindow();
			//}
			//else
			{
				pCmdUI->Enable(FALSE);
			}
		}
	}
}

void CAircraftLayoutView::OnUpdateFPS( CCmdUI *pCmdUI )
{
	CString s(_T(""));
	s.Format("\tFPS: %5.2f",m_dFrameRate);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText( s );
}

void CAircraftLayoutView::OnUpdateGridSpacing( CCmdUI *pCmdUI )
{

	CAircraftLayoutView* p3DView = this;
	if(p3DView)
	{ // if its valid
		//CTermPlanDoc *pTermPlanDoc = (CTermPlanDoc *)GetDocument();
		//CGrid* pGrid = p3DView->GetDocument()->GetActiveFloor()->GetGrid();
		//ASSERT(pGrid != NULL);
		//double ds = pGrid->dLinesEvery;//= p3DView->GetDocument()->GetActiveFloor()->GridSpacing();
		double ds = 500.0;
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		CTermPlanDoc * pDoc = (CTermPlanDoc *)GetDocument();
		//if(pDoc && pDoc->m_systemMode == EnvMode_AirSide)
		//	s.Format("\tGrid Spacing: %5.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),ds),CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		//else
		s.Format("\tGrid Spacing: %5.2f %s",pUM->ConvertLength(ds),pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void CAircraftLayoutView::OnAnimPax()
{

}

void CAircraftLayoutView::OnEditUndo()
{

}

void CAircraftLayoutView::OnEditRedo()
{

}

void CAircraftLayoutView::OnTimer( UINT nIDEvent )
{

}

void CAircraftLayoutView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch(nChar)
	{
	case VK_DELETE:
		OnDeleteElements();		
		break;
	case VK_INSERT:
		OnCopyElements();		
		break;
	case VK_ESCAPE:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


//////////////////////////////////////////////////////////////////////////
void CAircraftLayoutView::OnAircraftElementProperties()
{
	if(GetEditContext()->GetSelectElementCount())
	{
		CAircraftElement* pElm = GetEditContext()->GetSelectElement(0);
		if(pElm->GetType() == CSeat::TypeString)
		{
			CSeat* pSeat = (CSeat*)pElm;
			int nProjID = ((CTermPlanDoc *)GetDocument())->GetProjectID();		
			CAircaftLayOut* playout = GetEditContext()->GetEditLayOut();
			COnBoardSeatDlg dlg(pSeat,playout,nProjID,this);
			if(dlg.DoModal() ==IDOK)
			{
				m_aircraftlayoutScene.UpdateModifyElments( (std::vector<CAircraftElement*>)(pSeat,1) );
				GetDocument()->UpdateAllViews(this,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT,(CObject *)pSeat);
				Invalidate(FALSE);
			}
		}
		else if(pElm->GetType() == CDoor::TypeString)
		{
			CDoor* pDoor = (CDoor*)pElm;
			int nProjID = ((CTermPlanDoc *)GetDocument())->GetProjectID();		
			CAircaftLayOut* playout = GetEditContext()->GetEditLayOut();
			COnBoardDoorDlg dlg(pDoor,playout,nProjID,this);
			if(dlg.DoModal() ==IDOK)
			{
				m_aircraftlayoutScene.UpdateModifyElments( (std::vector<CAircraftElement*>)(pDoor,1) );
				GetDocument()->UpdateAllViews(this,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT,(CObject *)pDoor);
				Invalidate(FALSE);
			}
		}
		else if(pElm->GetType() == CStorage::TypeString)
		{
			CStorage* pStorage = (CStorage*)pElm;
			int nProjID = ((CTermPlanDoc *)GetDocument())->GetProjectID();		
			CAircaftLayOut* playout = GetEditContext()->GetEditLayOut();
			COnBoardStorageDlg dlg(pStorage,playout,nProjID,this);
			if(dlg.DoModal() ==IDOK)
			{
				m_aircraftlayoutScene.UpdateModifyElments( (std::vector<CAircraftElement*>)(pStorage,1) );
				GetDocument()->UpdateAllViews(this,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT,(CObject *)pStorage);
				Invalidate(FALSE);
			}
		}
		else if(pElm->GetType() == CCabinDivider::TypeString)
		{
			CCabinDivider* pCanbinDivider = (CCabinDivider*)pElm;
			int nProjID = ((CTermPlanDoc *)GetDocument())->GetProjectID();		
			CAircaftLayOut* playout = GetEditContext()->GetEditLayOut();
			COnBoardCabinDividerDlg dlg(pCanbinDivider,playout,nProjID,this);
			if(dlg.DoModal() ==IDOK)
			{
				m_aircraftlayoutScene.UpdateModifyElments( (std::vector<CAircraftElement*>)(pCanbinDivider,1) );
				GetDocument()->UpdateAllViews(this,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT,(CObject *)pCanbinDivider);
				Invalidate(FALSE);
			}
		}

	}

}

void CAircraftLayoutView::OnAircraftElementDisplayProp()
{
	int nCount = GetEditContext()->GetSelectElementCount();
	if(nCount > 0)
	//for (int nSelect = 0; nSelect < nCount; ++ nSelect)
	{
		CAircraftElement *pElement = GetEditContext()->GetSelectElement(0);
		ShowDisplayPropertyDialog(pElement);
	}

}
//create a row from a seat
void CAircraftLayoutView::OnCreateSeatRow()
{
	if(GetEditContext()->GetSelectElementCount())
	{
		CAircraftElement* pElm = GetEditContext()->GetSelectElement(0);
		if(pElm && pElm->GetType() == CSeat::TypeString )
		{
			CDlgSeatRowProp dlg;
			if(dlg.DoModal() == IDOK)
			{
				CSeat* pSeat = (CSeat*)pElm;
				/*CSeatRow* pnewRow = GetEditContext()->CreateRowFromSeat(pSeat, dlg.m_rowCount);
				if(pnewRow)
				{
					for(int i=0;i<pnewRow->GetSeatCount();i++)
					{
						CSeat* pNewSeat = pnewRow->GetSeat(i);
						if(pNewSeat!=pSeat)
							GetDocument()->UpdateAllViews(this, VM_ONBOARD_ADD_AIRCRAFTELEMENT, (CObject*)pNewSeat);
					}
				}				*/
				m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
				m_aircraftlayoutScene.UpdateShowHideText(GetEditContext()->IsShowText());
				//GetDocument()->UpdateAllViews(this, VM_ONBOARD_LAYOUT_SEATS_CHANGE);
				Invalidate(FALSE);
			}			
		}
	}
}

void CAircraftLayoutView::OnCopyElements()
{
	std::vector<CAircraftElement *> vCopyElements = GetEditContext()->CopySelectElements();
	int nCopyCount = static_cast<int>(vCopyElements.size());
	for (int nCopy =0; nCopy < nCopyCount; ++ nCopy)
	{
		GetDocument()->UpdateAllViews(this,VM_ONBOARD_ADD_AIRCRAFTELEMENT,(CObject *)vCopyElements.at(nCopy));
	}

	m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
	m_aircraftlayoutScene.UpdateShowHideText(GetEditContext()->IsShowText());
	Invalidate(FALSE);
}

void CAircraftLayoutView::OnDeleteElements()
{

	//delete from 3d view
	int nCount = GetEditContext()->GetSelectElementCount();
	for (int nSelect = 0; nSelect < nCount; ++ nSelect)
	{
		CAircraftElement *pElement = GetEditContext()->GetSelectElement(nSelect);
		GetDocument()->UpdateAllViews(this,VM_ONBOARD_DEL_AIRCRAFTELEMENT,(CObject *)pElement);
	}
	GetEditContext()->DeleteSelectElements();
	m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
	Invalidate(FALSE);
}

void CAircraftLayoutView::OnMoveElements()
{
	m_eMouseState = _moveElements;
	/*DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);

	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);*/
	SetCapture();	
}
void CAircraftLayoutView::OnMoveElmentsZ()
{
	m_eMouseState = _moveElementsZ;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);

	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	SetCapture();	
}
void CAircraftLayoutView::OnRotateElements()
{

	m_eMouseState = _rotateElements;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);

	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	SetCapture();	
}

void CAircraftLayoutView::OnScaleElements()
{
	//delay for doing so
	m_eMouseState = _scaleElements;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	SetCapture();	
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CAircraftLayoutView::OnUnRow()
{
	if(	GetEditContext()->UnArraySelectRow() )
	{
		Invalidate(FALSE);
	}	
}


void CAircraftLayoutView::OnDeleteRow()
{
	/*CSeatRow *pSelSeatRow = GetEditContext()->m_pSelectSeatRow;
	if(GetEditContext()->DeleteSelectRow())
	{			
		m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
		for(int i=0;i<pSelSeatRow->GetSeatCount();i++)
		{
			GetDocument()->UpdateAllViews(this, VM_ONBOARD_DEL_AIRCRAFTELEMENT, (CObject*)pSelSeatRow->GetSeat(i) );
		}
		Invalidate(FALSE);
	}	*/
}

#include "../DlgSeatArraySpec.h"

void CAircraftLayoutView::OnCreateSeatArray()
{
	/*CSeatRow* pSeatRow = GetEditContext()->m_pSelectSeatRow;
	if(pSeatRow)
	{
		CSeatGroup* pnewGroup = new CSeatGroup();
		pnewGroup->SetOriginRow(pSeatRow);		
		CDlgSeatArraySpec dlgSeatArray(pnewGroup, GetEditContext()->GetEditLayOut()->GetPlacements(), this);
		if( dlgSeatArray.DoModal() ==IDOK)
		{	
			GetEditContext()->AddNewSeatArray(pnewGroup);
			m_aircraftlayoutScene.UpdateModifySeatArray(pnewGroup);
			m_aircraftlayoutScene.UpdateShowHideText(GetEditContext()->IsShowText());
			GetDocument()->UpdateAllViews(this, VM_ONBOARD_RELOAD_SEATELEMENT);
			Invalidate(FALSE);
		}
	}*/
}

void CAircraftLayoutView::OnMoveSeatRow()
{
	//if(GetEditContext()->m_pSelectSeatRow)
	//{
	//	m_eMouseState = _moveSeatRow;	
	//	
	//	DWORD dwPos = ::GetMessagePos();
	//	m_ptFrom.x = (int) LOWORD(dwPos);
	//	m_ptFrom.y = (int) HIWORD(dwPos);
	//	this->ScreenToClient(&m_ptFrom);

	//	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	//	SetCapture();
	//}
}

void CAircraftLayoutView::OnRotateSeatRow()
{
	/*if(GetEditContext()->m_pSelectSeatRow)
	{
		m_eMouseState = _rotateSeatRow;

		DWORD dwPos = ::GetMessagePos();
		m_ptFrom.x = (int) LOWORD(dwPos);
		m_ptFrom.y = (int) HIWORD(dwPos);
		this->ScreenToClient(&m_ptFrom);

		m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
		SetCapture();	
	}*/
}
//////////////////////////////////////////////////////////////////////////
void CAircraftLayoutView::OnUnArraySeats()
{
	if(GetEditContext()->UnArraySelectArray())
	{
		Invalidate(FALSE);
	}
}

void CAircraftLayoutView::OnDeleteArray()
{
	if( GetEditContext()->DeleteSelectSeatArray() )
	{
		m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_RELOAD_SEATELEMENT);
		Invalidate(FALSE);
	}
}

ARCVector3 CAircraftLayoutView::GetWorldMousePos( const CPoint& pt )
{
	Ogre::Ray ray;
	m_sceneWnd.getPtMouseRay(pt, ray);
	Ogre::Vector3 vpos = m_aircraftlayoutScene.GetRayIntesectPos(ray);
	return ARCVector3(vpos.x, vpos.y, vpos.z);
}

void CAircraftLayoutView::OnRButtonUp( UINT nFlags, CPoint point )
{
	CView::OnRButtonUp(nFlags, point);
	ReleaseCapture();
	
	if(::GetAsyncKeyState(VK_MENU)>=0) {//ALT is NOT pressed
		switch(m_eMouseState)
		{
		case _gettwopoints:
		case _getmirrorline:
		case _getarrayline:
			m_vMousePosList.push_back(m_ptMouseWorldPos);
			if(m_vMousePosList.size() == 2) {
				if(m_eMouseState==_gettwopoints)
				{
					::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
				}
				m_eMouseState = _default;
			}
			Invalidate(FALSE);
			break;
		case _distancemeasure:
			m_vMousePosList.push_back(m_ptMouseWorldPos);
			ShowDistanceDialog();
			m_vMousePosList.clear();
			m_eMouseState = _default;
			Invalidate(FALSE);
			break;
		case _getmanypoints:
			m_vMousePosList.push_back(m_ptMouseWorldPos);		
			::SendMessage(m_hFallbackWnd, TP_ONBOARD_DATABACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = _default;			
			Invalidate(FALSE);
			break;
		
		};
	}	

}
#include <common/UnitsManager.h>
void CAircraftLayoutView::ShowDistanceDialog()
{
	CString msg;
	double dist = 0.0;
	int nPtCount=m_vMousePosList.size();
	for(int i=0; i<nPtCount-1; i++) {
		dist+=(m_vMousePosList[i+1]-m_vMousePosList[i]).Magnitude();
	}	
	dist = UNITSMANAGER->ConvertLength(dist);
	msg.Format("Distance: %.2f %s", dist, UNITSMANAGER->GetLengthUnitString());	
	AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);
}


void CAircraftLayoutView::OnEditArrayEditPoint()
{	
	m_bEditArrayPath = !m_bEditArrayPath;	
	if(!m_bEditArrayPath)
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_RELOAD_SEATELEMENT);
	Invalidate(FALSE);
}

void CAircraftLayoutView::OnSeatArrayProp()
{
	if(GetEditContext()->m_pSelectSeatGroup)
	{
		CSeatGroup* pGrp = GetEditContext()->m_pSelectSeatGroup;
		CDlgSeatArraySpec dlg(GetEditContext()->m_pSelectSeatGroup, GetEditContext()->GetEditLayOut()->GetPlacements(), this);
		if( dlg.DoModal() == IDOK)
		{
			//pGrp->UpdateSeatRows(GetEditContext()->GetEditLayOut()->GetPlacements());
			GetEditContext()->DoneEditSelectSeatGroup();	
			m_aircraftlayoutScene.UpdateModifySeatArray(pGrp);
			Invalidate(FALSE);
		}
	}
}

//prepare drawing content on draw
 void CAircraftLayoutView::PreOnDraw()
{
	//update drawing picking line
	if(m_vMousePosList.size() > 0 &&
		(m_eMouseState == _getmanypoints || m_eMouseState == _gettwopoints || m_eMouseState == _distancemeasure || m_eMouseState == _getmirrorline || m_eMouseState == _getarrayline || m_eMouseState == _selectlandproc || m_eMouseState == _pickstretchpos ) ) 
	{
		std::vector<ARCVector3> vPts = m_vMousePosList;
		vPts.push_back( m_ptMouseWorldPos );	
		m_aircraftlayoutScene.UpdatePickLine(vPts);
	}
	else
	{
		m_aircraftlayoutScene.ClearPickLine();
	}

	//seat group edit path
	if(m_bEditArrayPath && GetEditContext()->m_pSelectSeatGroup)
	{
		m_aircraftlayoutScene.UpdateEditSeatGroupPath(GetEditContext()->m_pSelectSeatGroup);
	}
	else
	{
		m_aircraftlayoutScene.ClearEditSeatGroupPath();
	}

	//update pax
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		m_aircraftlayoutScene.AnimatePax(true);
		UpdateAnimationPax();
	}
	else
	{
		m_aircraftlayoutScene.AnimatePax(false);
	}

	CalculateFrameRate();
}
void CAircraftLayoutView::CalculateFrameRate()
{
	// Below we create a bunch of static variables because we want to keep the information
	// in these variables after the function quits.  We could make these global but that would
	// be somewhat messy and superfluous.  Note, that normally you don't want to display this to
	// the window title bar.  This is because it's slow and doesn't work in full screen.
	// Try using the 3D/2D font's.  You can check out the tutorials at www.gametutorials.com.

	static float framesPerSecond    = 0.0f;							// This will store our fps
	static float lastTime			= 0.0f;							// This will hold the time from the last frame

	// Here we get the current tick count and multiply it by 0.001 to convert it from milliseconds to seconds.
	// GetTickCount() returns milliseconds (1000 ms = 1 second) so we want something more intuitive to work with.
	float currentTime = GetTickCount() * 0.001f;				

	// Increase the frame counter
	++framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored.  If it is greater than 1
	// that means a second has passed and we need to display the new frame rate.  Of course, the first time
	// will always be greater than 1 because lastTime = 0.  The first second will NOT be true, but the remaining
	// ones will.  The 1.0 represents 1 second.  Let's say we got 12031 (12.031) from GetTickCount for the currentTime,
	// and the lastTime had 11230 (11.230).  Well, 12.031 - 11.230 = 0.801, which is NOT a full second.  So we try again
	// the next frame.  Once the currentTime - lastTime comes out to be greater than a second (> 1), we calculate the
	// frames for this last second.
	if( currentTime - lastTime > 1.0f )
	{
		// Here we set the lastTime to the currentTime.  This will be used as the starting point for the next second.
		// This is because GetTickCount() counts up, so we need to create a delta that subtract the current time from.
		lastTime = currentTime;

		m_dFrameRate = framesPerSecond;

		// Reset the frames per second
		framesPerSecond = 0;
	}
}
void CAircraftLayoutView::OnSeatrowCopy()
{
	// TODO: Add your command handler code here
	/*CSeatRow * pNewRow = GetEditContext()->CopySelectRow();
	if( pNewRow )
	{
		m_aircraftlayoutScene.GetLayOut3D().UpdateAddRemoveElements();
		m_aircraftlayoutScene.UpdateShowHideText(GetEditContext()->IsShowText());
		for(int i=0;i<pNewRow->GetSeatCount();i++)
		{
			GetDocument()->UpdateAllViews(this, VM_ONBOARD_ADD_AIRCRAFTELEMENT, (CObject*) pNewRow->GetSeat(i) );
		}
		
		Invalidate(FALSE);
	}	*/
}

void CAircraftLayoutView::OnUpdateSeatarrayEditarraypath(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_bEditArrayPath)
	{
		pCmdUI->SetText("Disable Edit Array Path");
	}
	else
	{
		pCmdUI->SetText("Enable Edit Array Path");
	}
}

void CAircraftLayoutView::OnArraypointDelete()
{
	// TODO: Add your command handler code here
	if( GetEditContext()->DeleteSelectArrayPoint(mSelectArrayPtIdx) )
	{
		m_aircraftlayoutScene.UpdateModifySeatArray(GetEditContext()->m_pSelectSeatGroup);
		Invalidate(FALSE);
	}
}




void CAircraftLayoutView::OnScenepickRestoredefaultcamera()
{
	// TODO: Add your command handler code here
	m_aircraftlayoutScene.RestoreDefaultCamera();
	Invalidate(FALSE);
}


void CAircraftLayoutView::OnMenuruler()
{
	// TODO: Add your command handler code here
	m_eMouseState = _distancemeasure;
	Invalidate(FALSE);
}

void CAircraftLayoutView::OnScenepickStartanim()
{
	// TODO: Add your command handler code here
	ANIMTIMEMGR->StartTimer();
}

void CAircraftLayoutView::OnScenepickStopanim()
{
	// TODO: Add your command handler code here
	ANIMTIMEMGR->EndTimer();
}

#include "..\termplanDoc.h"
#include "..\..\Results\OUT_TERM.H"
#include "..\..\Results\EventLogBufManager.h"


void CAircraftLayoutView::UpdateAnimationPax()
{
	CTermPlanDoc* pDoc =(CTermPlanDoc* ) GetDocument();
	AnimationData* pAnimData = &(pDoc->m_animData);
	long nTime = pAnimData->nLastAnimTime;

	// load data if necessary
	if(pDoc->m_eAnimState==CTermPlanDoc::anim_pause) {
		if(m_bAnimDirectionChanged) {
			pDoc->GetTerminal().m_pLogBufManager->InitBuffer();
			m_bAnimDirectionChanged = FALSE;
		}
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, (CEventLogBufManager::ANIMATION_MODE)m_nAnimDirection );
	}
	else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playF) {
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, CEventLogBufManager::ANIMATION_FORWARD );
	}
	else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playB) {
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, CEventLogBufManager::ANIMATION_BACKWARD );
	}

	//update pax status
	MobLogEntry element;
	PaxLog* pPaxLog = pDoc->GetTerminal().paxLog;
	const PaxEventBuffer& logData = pDoc->GetTerminal().m_pLogBufManager->getLogDataBuffer();

	CPaxDispProps& paxdispprops = pDoc->m_paxDispProps;
	const std::vector<int>& paxdisppropidx = pDoc->m_vPaxDispPropIdx;
	const std::vector<int>& paxtagpropidx = pDoc->m_vPaxTagPropIdx;
	CPaxTags& paxtagprops = pDoc->m_paxTags;

	for( PaxEventBuffer::const_iterator it = logData.begin(); it!= logData.end(); ++it )
	{
		CPaxDispPropItem* pPDPI = paxdispprops.GetDispPropItem(paxdisppropidx[it->first]);
		CPaxTagItem* pPTI = paxtagprops.GetPaxTagItem(paxtagpropidx[it->first]);
		pPaxLog->getItem(element, it->first);	

		MobDescStruct pds = element.GetMobDescStruct();
		int nPaxiD= element.getID();
		COnboardAnimaPax3D* pPax3D = m_aircraftlayoutScene.GetPax3DList().GetAddPax3D(nPaxiD, pPDPI, pPTI);		

		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();
		if(nStartTime <= nTime && nEndTime >= nTime) 
		{
			const part_event_list& part_list = it->second;

			long nextIdx = -1;
			for(long jj=0; jj<static_cast<int>(part_list.size()); jj++)
			{
				if( part_list[jj].time >= nTime )
				{
					nextIdx = jj;
					break;
				}
			}
			if(nextIdx > 0)
			{				

				MobEventStruct pesB = part_list[nextIdx];
				MobEventStruct pesA = part_list[nextIdx-1];
				float r = 0 ;
				//ASSERT(pesB.time == pesA.time) ;
				if((pesA.speed==0&& pesB.speed==0))
				{
					int w = pesB.time - pesA.time;
					int d = pesB.time - nTime;
					r = ((float) d) / w;
				}
				else
				{	
					double rate = static_cast<double>(nTime-pesA.time)/static_cast<double>(pesB.time-pesA.time);				
					double nCurSpeed = (1-rate)*pesA.speed+rate*pesB.speed;				
					double flyedLength = (pesA.speed+nCurSpeed)*(nTime-pesA.time);//pesA.speed*(nCurTime-pesA.time)+accel*(nCurTime-pesA.time)*(nCurTime-pesA.time)/2;
					double flyingLength  = (pesA.speed+pesB.speed)*(pesB.time-pesA.time);//pesA.speed*(pesB.time-pesA.time)+accel*(pesB.time-pesA.time)*(pesB.time-pesA.time)/2;			
					r = 1.0f - static_cast<float>(flyedLength/flyingLength);
				}
				//3)interpolate position based on the two PaxEventStructs and the time
				//int w = pesB.time - pesA.time;
				//int d = pesB.time - nTime;
				//float r = ((float) d) / w;
				float xpos = (1.0f-r)*pesB.x + r*pesA.x;
				float ypos = (1.0f-r)*pesB.y + r*pesA.y;
				float dAlt = (1.0f-r)*pesB.z + r*pesA.z;				
	
				//calc direction
				long tmpIdx = nextIdx;
				ARCVector2 dir;
				double dMag;
				do
				{
					tmpIdx--;
					MobEventStruct pesTemp = part_list[tmpIdx];
					dir[VX] = pesB.x - pesTemp.x;
					dir[VY] = pesB.y - pesTemp.y;
				} while((dMag=dir.Magnitude())<ARCMath::EPSILON && (nextIdx-tmpIdx)<10 && tmpIdx > 0);
				double anglefromX = 0;
				if(dMag) {
					dir.Normalize();					
					anglefromX = ARCMath::RadiansToDegrees(acos(dir[VX]));
					if(dir[VY] < 0)
						anglefromX = -anglefromX;
				}
				if(pesA.state == SIT && pds.arrFlight==-1 )// debug. Enplane Agent
				{
					anglefromX += 180.0;
				}
				//draw pax			
				
				if(pPax3D)
				{
					pPax3D->SetPosition( Ogre::Vector3(xpos,ypos,dAlt) );
					pPax3D->SetRotation( anglefromX + 90 );
					
					dir[VX] = pesB.x - pesA.x;
					dir[VY] = pesB.y - pesA.y;
					if(pesA.state == 2)
					{
						pPax3D->SetState(SIT, nTime*0.02);
					}
					else if(dir.Magnitude()<ARCMath::EPSILON)
					{
						pPax3D->SetState(STAND,nTime * 0.02 );		
					}
					else
					{
						pPax3D->SetState(WALK, nTime*0.02);
					}					
				
					pPax3D->Show(TRUE);
					if(pPTI->IsShowTags())
					{
						CString sPaxTag;
						pDoc->BuildPaxTag(pPTI->GetTagInfo() , pds, pesA, sPaxTag);
						pPax3D->UpdateTag(sPaxTag);
					}
				}
				continue;
			}						
		}

		//default hide the pax
		if(pPax3D){
			pPax3D->Show(false);
		}
		
	}	
}

void CAircraftLayoutView::ShowDisplayPropertyDialog( CAircraftElement* pElement )
{
	if(pElement == NULL)
	{
		ASSERT(0);
		return;
	}

	CAircraftElement3D *pElement3D = m_aircraftlayoutScene.GetLayOut3D().GetPlacement3D()->GetElement3D(pElement);
	if(pElement3D)
	{
		/*CDlgOnBoardObjectDisplayProperty dlg(pElement3D->GetDisplayProperty(),pElement3D->GetElement());
		if(dlg.DoModal() == IDOK)
		{
			pElement3D->Update3D();
		}*/

	}
}



void CAircraftLayoutView::OnScenepickShowtext()
{
	// TODO: Add your command handler code here
	GetEditContext()->ShowText( !GetEditContext()->IsShowText() );
	m_aircraftlayoutScene.UpdateShowHideText(GetEditContext()->IsShowText());
	Invalidate(FALSE);
	
}

void CAircraftLayoutView::OnUpdateScenepickShowtext(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(GetEditContext()->IsShowText())
	{
		pCmdUI->SetText("Hide Text");
	}
	else
	{
		pCmdUI->SetText("Show Text");
	}
}
