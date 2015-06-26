#include "StdAfx.h"
#include "AircraftLayoutScene.h"
#include <InputOnBoard/AircraftLayoutEditContext.h>
#include <InputOnBoard/AircaftLayOut.h>
#include <InputOnBoard/AircraftPlacements.h>
#include <Inputonboard/Deck.h>
#include <Inputonboard/OnboardCorridor.h>
#include <Shlwapi.h>
#include <OgrePlane.h>
#include <OgreVector3.h>
#include <boost/tuple/tuple.hpp>
#include <common\FileInDB.h>
#include <InputOnBoard\CInputOnboard.h>
#include "RenderEngine.h"
#include "scenestate.h"
#include "OgreBillboardObject.h"
#include "ColoredTextAreaOverlayElement.h"
#include <InputOnBoard/AircraftElmentShapeDisplay.h>
#include <common/ARCStringConvert.h>
#include "ARCRenderTargetListener.h"


#define GridXYName _T("SceneGrid")
#define ACModelNodeName  _T("AircraftModel")
#define DECKListName _T("DeckList")
#define LAYOUTNODE _T("AircraftLayoutNode")
#define PICK_LINE _T("PickLine")

using namespace Ogre;

CAircraftLayoutScene::CAircraftLayoutScene()
{
	ResetMouseState();
	ClearDragInfo();
	mpEditContext = NULL;
}
void CAircraftLayoutScene::Setup( CAircraftLayoutEditContext* pEdCtx )
{
	mpEditContext = pEdCtx;
	if(!MakeSureInited())
	{
		ASSERT(FALSE);
		return;
	}	

	//set up camera	
	{		
		mGridXY = AddSceneNode(GridXYName);
		mGridXY.Update(pEdCtx->GetSceneGrid(),CGrid3D::XYExtent);
	}


	
	//add layout node
	mLayout3DNode = AddSceneNode(LAYOUTNODE);
	mLayout3DNode.LoadLayout(pEdCtx->GetInputOnboard(),pEdCtx->GetEditLayOut(),GetRoot());
	//mLayout3DNode.UpdateAddDelElements(*this);

	mPickLine = GetSceneNode(PICK_LINE);
	mPickLine.AttachTo(GetRoot());

	
	//seat array path
	m_pathNode = GetSceneNode(_T("EditPathNode"));
	UpdateEditPath();
	//OnEnableEditSeatArrayPath();

	m_acModel3DNode = AddSceneNode("AC_Model");
	m_acModel3DNode.Load(mpEditContext->GetEditLayOut()->GetAcType(),mpEditContext->GetInputOnboard());
	m_acModel3DNode.SetFadeMaterial("", RGB(220,220,220) );
	m_acModel3DNode.EnableSelect(false);

	//overlaytext
	overlaytext.UpdateElmentsTags(this);
	
}

BOOL CAircraftLayoutScene::OnUpdateOperation( CAircraftLayoutEditCommand* pOP )
{
	if(!pOP)return FALSE;

	ACLayoutEditOp op = pOP->getOpType();
	if(op==OP_addDeck || op == OP_delDeck || op==OP_modifyDeckList)
	{
		mLayout3DNode.UpdateDecks(pOP->ToDeckEditCommand()->mpDeckman);
		return TRUE;
	}
	if( op== OP_modifyDeck)
	{
		CModifyDeckCommand *pCmd = (CModifyDeckCommand *)pOP;
		mLayout3DNode.UpdateDeck(pCmd->mpDeck);
		return TRUE;
	}

	if(op == OP_addElments || op == OP_delElments)
	{
		mLayout3DNode.UpdateAddDelElements();
		overlaytext.UpdateElmentsTags(this);
		UpdateEditPath();
		return TRUE;
	}
	if(op ==  OP_editElement)
	{
		CAircraftElmentCommand* pElmsCmd = pOP->ToElmentEditCommand();
		ASSERT(pElmsCmd);		
		mLayout3DNode.mElmentsList.UpdateElements(pElmsCmd->GetElmList(),mLayout3DNode);
		overlaytext.UpdateElmentsTags(this);
		UpdateEditPath();
		return TRUE;
	}

	if(op== OP_moveElements || op== OP_rotateElements )
	{
		CAircraftElmentCommand* pElmsCmd = pOP->ToElmentEditCommand();
		ASSERT(pElmsCmd);		
		mLayout3DNode.mElmentsList.UpdateElements(pElmsCmd->GetElmList(),mLayout3DNode);
		UpdateEditPath();
		return TRUE;
	}
	
	if(op == OP_moveShapes || op==OP_moveShapesZ0 || op== OP_scaleShapes || op==OP_rotateShapes
		 || op == OP_modifyShapeTranf)
	{
		CAircraftElmentCommand* pElmsCmd = pOP->ToElmentEditCommand();
		ASSERT(pElmsCmd);		
		mLayout3DNode.mElmentsList.UpdateElements(pElmsCmd->GetElmList(),mLayout3DNode);		
		return TRUE;
	}

	if(CEditSeatRowCmd* pCmd = pOP->ToSeatRowCmd() )
	{
		mLayout3DNode.UpdateAddDelElements();		
		mLayout3DNode.mElmentsList.UpdateElements( pCmd->GetCurrentElements(),mLayout3DNode );		
		overlaytext.UpdateElmentsTags(this);
		return TRUE;
	}

	if(CSeatGroupEditCmd* pCmd = pOP->ToSeatArrayCmd() )
	{
		mLayout3DNode.UpdateAddDelElements();		
		mLayout3DNode.mElmentsList.UpdateElements( pCmd->GetCurrentElements(),mLayout3DNode);	
		overlaytext.UpdateElmentsTags(this);
		UpdateEditPath();
	}

	return FALSE;
}

void CAircraftLayoutScene::OnDeckChange( CDeck* pdeck )
{
	mLayout3DNode.UpdateDeck(pdeck);
}




CAircaftLayOut* CAircraftLayoutScene::GetEditLayout()
{
	if(mpEditContext)
		return mpEditContext->GetEditLayOut();
	return NULL;
}



BOOL CAircraftLayoutScene::OnLButtonDown( const MouseSceneInfo& mouseInfo )
{
	BOOL norefresh = TRUE;	
	if(mouseInfo.bInPlane)
	{	

		if(mMouseState == _default)
		{
			Ogre::MovableObject* pobj=  mouseInfo.mOnObject;
			if(pobj)
			{
				const Any& userdata  = pobj->getUserAny();
				const CAircraftElementShapePtr* pelmPtr = any_cast<CAircraftElementShapePtr>(&userdata);
				if(pelmPtr)
				{
					mpEditContext->OnSelectElement(*pelmPtr,(mouseInfo.mnFlags& MK_CONTROL)!=0 );	
				}
				if( !mpEditContext->IsViewLocked() ) //set about to drag object
				{
					m_pDragObject = pobj;	
					m_bOnDragging = false;
				}		
			}
		}
	}
	C3DSceneBase::OnLButtonDown(mouseInfo);
	return FALSE;
}

BOOL CAircraftLayoutScene::OnRButtonUp( const MouseSceneInfo& mouseInfo )
{
	BOOL norefresh = TRUE;	
	if(mMouseState == _getonepoint|| mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure )
	{
		if(mouseInfo.bInPlane)
		{
			m_vMousePosList.push_back( ARCVECTOR(mouseInfo.mWorldPos) );
		}	
		if(mMouseState == _distancemeasure){ EndDistanceMeasure(); }
		else
			EndPickPoints();
	}
	return C3DSceneBase::OnRButtonUp(mouseInfo);
}

BOOL CAircraftLayoutScene::OnMouseMove( const MouseSceneInfo& mouseInfo )
{
	BOOL norefresh = TRUE;	
	
	if(mouseInfo.bInPlane)
	{
		if(mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure)
		{
			ARCPath3 vPath = m_vMousePosList;
			vPath.push_back( ARCVECTOR(mouseInfo.mWorldPos) );
			mPickLine.Update(vPath);
			norefresh = FALSE;
		}


		if(mMouseState == _default)
		{
			const MouseSceneInfo& lastMoveInfo = mState->m_LastMoseMoveInfo;
			ARCVector3 worldOffset = ARCVECTOR(mouseInfo.mWorldPos-lastMoveInfo.mWorldPos);
			CPoint ptDiff = mouseInfo.mpt-lastMoveInfo.mpt;

			if(CAircraftLayoutEditCommand *pCmd = mpEditContext->OnMouseDiff(worldOffset,ptDiff))
			{				
				OnUpdateOperation(pCmd);
			}
			else if(m_pDragObject) //first d
			{
				if(m_bOnDragging)
				{
					OnDragObject(mouseInfo);
				}
				else
				{
					OnBeginDrag(mouseInfo);
				}
			}			
		}
	}

	norefresh |= C3DSceneBase::OnMouseMove(mouseInfo);
	return norefresh;
}

BOOL CAircraftLayoutScene::OnLButtonUp( const MouseSceneInfo& mouseInfo )
{

	if(mMouseState == _getonepoint || mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure )
	{
		m_vMousePosList.push_back( ARCVECTOR(mouseInfo.mWorldPos) );
		mPickLine.Update(m_vMousePosList);	
		int nMousePtCount = (int)m_vMousePosList.size();
		if(mMouseState==_getonepoint && nMousePtCount>=1)
		{
			EndPickPoints();
		}
		if(mMouseState == _gettwopoints && nMousePtCount>=2)
		{
			EndPickPoints();
		}	
	}		
	if(mMouseState == _default)
	{
		mpEditContext->DoneCurEdit();	
		m_pDragObject = NULL;
	}
	ClearDragInfo();

	return C3DSceneBase::OnLButtonUp(mouseInfo);
}

void CAircraftLayoutScene::StartDistanceMeasure()
{
	ResetMouseState();
	mMouseState = _distancemeasure;	
// 	mPickLine.AttachTo(mLayout3DNode);
}

void CAircraftLayoutScene::EndDistanceMeasure()
{
	DEAL_LISTENER_HANDLER( OnFinishDistanceMesure(m_vMousePosList.GetLength()) );
	ResetMouseState();
// 	mPickLine.Detach();
	mPickLine.Update(CLines3D::VertexList());
}

void CAircraftLayoutScene::EndPickPoints()
{
	DEAL_LISTENER_HANDLER( OnFinishPickPoints(m_vMousePosList) );
	ResetMouseState();
// 	mPickLine.Detach();	
	mPickLine.Update(CLines3D::VertexList());
}

void CAircraftLayoutScene::ResetMouseState()
{
	m_vMousePosList.resize(0);
	mMouseState = _default;	
}

BOOL CAircraftLayoutScene::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{

	if(!mouseInfo.mOnObject)
		return FALSE ;

	do 
	{
		int nPathIndex = -1;
		int nNodeIndex = -1;
		if (m_pathNode.HitTest(mouseInfo.mOnObject, nPathIndex, nNodeIndex))
		{
			const CAircraftElementShapePtr* pelmPtr = any_cast<CAircraftElementShapePtr>(&m_pathNode.GetAny());
			if(pelmPtr)
			{
				DEAL_LISTENER_HANDLER( OnRButtonDownOnEditPath(*pelmPtr, nPathIndex, nNodeIndex, mouseInfo.mpt) );
			}
			break;
		}

		const Any& userdata  = mouseInfo.mOnObject->getUserAny();
		const CAircraftElementShapePtr* pelmPtr = any_cast<CAircraftElementShapePtr>(&userdata);
		if(pelmPtr)
		{
			CAircraftElementShapePtr pSelectElm = *pelmPtr;			
			if(pSelectElm)
			{
				DEAL_LISTENER_HANDLER( OnRButtonDownOnElement(pSelectElm,mouseInfo.mpt) );
			}		
		}	
	} while (false);
	
	return C3DSceneBase::OnRButtonDown(mouseInfo);
}

void CAircraftLayoutScene::StartPickOnPoint()
{
	ResetMouseState();
	mMouseState = _getonepoint;
// 	mPickLine.AttachTo(mLayout3DNode);
	
}

void CAircraftLayoutScene::StartPickTwoPoint()
{
	ResetMouseState();
	mMouseState = _gettwopoints;
// 	mPickLine.AttachTo(mLayout3DNode);	
}

void CAircraftLayoutScene::StartPickManyPoint()
{
	ResetMouseState();
	mMouseState = _getmanypoints;
// 	mPickLine.AttachTo(mLayout3DNode);
	
}

void CAircraftLayoutScene::StartPickRect()
{
	ResetMouseState(); 
	mMouseState =_getonerect;	
}

BOOL CAircraftLayoutScene::IsSelectObject() const
{
	return mMouseState==_default;
}

Ogre::Plane CAircraftLayoutScene::getFocusPlan() const
{
	if(mpEditContext)
	{
		CDeckManager* pDeckMan = mpEditContext->GetEditLayOut()->GetDeckManager();
		if(pDeckMan)
		{
			CDeck* pActiveDeck = pDeckMan->GetActivedDeck();
			if(pActiveDeck)
			{
				return Ogre::Plane(Vector3::UNIT_Z,Vector3(0,0,pActiveDeck->Altitude()) );
			}
		}
	}

	return Ogre::Plane(Vector3::UNIT_Z,Vector3::ZERO );
}

void CAircraftLayoutScene::OnDragObject( const MouseSceneInfo& moveInfo )
{		
	ASSERT(m_pDragObject);
	if(m_pDragObject)
	{
		const MouseSceneInfo& lastMoveInfo = mState->m_LastMoseMoveInfo;
		ARCVector3 worlddiff = ARCVECTOR(moveInfo.mWorldPos - lastMoveInfo.mWorldPos);		

		//drag a seat group point
		EditPointKey ptKey;
		if(m_pathNode.OnDrag(m_pDragObject, worlddiff, ptKey))
		{			
			if( const CAircraftElementShapePtr* pShapePtr = any_cast<CAircraftElementShapePtr>(&m_pathNode.GetAny()))
			{
				CAircraftElementShapePtr pShape = *pShapePtr;
				if(pShape)
				{
					if(pShape->GetType() == COnBoardWall::TypeString)
					{
						COnBoardWall* pwall = (COnBoardWall* )pShape->mpElement;
						pwall->setPath(m_pathNode.getPath());	
						mLayout3DNode.mElmentsList.UpdateElement(pShape,mLayout3DNode);
					}
					if(pShape->GetType() == COnboardSurface::TypeString)
					{
						COnboardSurface* pSurface = (COnboardSurface* )pShape->mpElement;
						pSurface->setPath(m_pathNode.getPath());	
						mLayout3DNode.mElmentsList.UpdateElement(pShape,mLayout3DNode);
					}
					if(pShape->GetType() == COnboardCorridor::TypeString)
					{
						COnboardCorridor* pCorridor = (COnboardCorridor* )pShape->mpElement;
						COnboardCorridor::PathIndex nPathIndex = (COnboardCorridor::PathIndex)ptKey.nPathIndex;
						pCorridor->SetPathByIndex(m_pathNode.getPath(nPathIndex), nPathIndex);

						mLayout3DNode.mElmentsList.UpdateElement(pShape, mLayout3DNode);

						CPath2008& pathToModify = pCorridor->GetPathByIndex(nPathIndex);
						if (ptKey.nNodeIndex == 0 || ptKey.nNodeIndex == pathToModify.getCount() - 1)
							UpdateEditPath(); // 
					}
				}
			}
		
		}

	}
	
}


void CAircraftLayoutScene::UpdateSelectRedSquares()
{
	mSelectSquare.HideAll();
	int nCount = mpEditContext->m_vSelectedElments.Count();
	for(int i=0;i<nCount;i++)
	{
		CAircraftElementShapePtr pElm = mpEditContext->m_vSelectedElments.GetItem(i);
		CAircraftElmentNodeList::NodeTypePtr pNode  = mLayout3DNode.mElmentsList.FindItem(pElm);
		if(pNode.get())
		{
			MovableObject* pObj = mSelectSquare.GetObject(i);
			pObj->setVisible(true);
			pNode->AddObject(pObj);
		}		
	}	
}


void CAircraftLayoutScene::UpdateGridACShow()
{
	
	if(GetEditLayout()->IsShowAC())
		m_acModel3DNode.AttachTo(GetRoot());
	else
		m_acModel3DNode.Detach();
	//m_acModel3DNode.Load(,mpEditContext->GetEditLayOut()->get)
	//mLayout3DNode.UpdateAircraftModel(*this);

	if(GetEditLayout()->IsShowGrid())
		mGridXY.AttachTo(GetRoot());
	else
		mGridXY.Detach();
}

void CAircraftLayoutScene::OnUpdateSelectChange()
{
	if (!mpEditContext)
		return;

	UpdateSelectRedSquares();
	UpdateEditPath();
}

void CAircraftLayoutScene::SetupCameraLight()
{
	Ogre::Light* l = OgreUtil::createOrRetrieveLight( StrCameraLight, getSceneManager() );	
	l->setSpecularColour( ColourValue::White );
	l->setDiffuseColour( ColourValue(0.9f, 0.9f, 0.9f, 1.0f )  );
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-Vector3::UNIT_Z);	
	l->setVisible(false);
}


void CAircraftLayoutScene::UpdateEditPath()
{
	m_pathNode.Detach();
	m_pathNode.SetPosition(ARCVector3(0.0, 0.0, 0.0));

	//
	if(CAircraftElementShapePtr pElmPtr = mpEditContext->m_lastSelectElm)
	{
		//update seat group path
		if(pElmPtr->GetType() == CSeat::TypeString)
		{
			CSeatGroup* pSeatGrp = mpEditContext->GetSelectSeatGroup();
			CSeat* pSeat = pElmPtr->IsSeat();
			if(pSeatGrp && mpEditContext->IsEditArrayPath())
			{
				m_pathNode.setOnlyPath(pSeatGrp->getPath());
				m_pathNode.Update();
				m_pathNode.SetAny(Any(pSeatGrp));
				m_pathNode.AttachTo( mLayout3DNode.GetDeckNode(pSeat->GetDeck()) );
				
			}
		}
		if(pElmPtr->GetType()==COnBoardWall::TypeString)
		{
			if(mpEditContext->IsEditWall())
			{
				COnBoardWall* pWall = (COnBoardWall*)pElmPtr->mpElement;
				m_pathNode.setOnlyPath(pWall->getPath());
				m_pathNode.Update();
				m_pathNode.SetAny(Any(pElmPtr));
				m_pathNode.AttachTo(mLayout3DNode.GetDeckNode(pWall->GetDeck()));
			}			
		}
		if(pElmPtr->GetType() == COnboardSurface::TypeString)
		{
			if(mpEditContext->IsEditArea())
			{
				COnboardSurface* pWall = (COnboardSurface*)pElmPtr->mpElement;
				m_pathNode.setOnlyPath(pWall->getPath());
				m_pathNode.Update();
				m_pathNode.SetAny(Any(pElmPtr));
				m_pathNode.setLoop(true);
				m_pathNode.AttachTo(mLayout3DNode.GetDeckNode(pWall->GetDeck()));
			}
		}
		if(pElmPtr->GetType() == COnboardCorridor::TypeString)
		{
			if(mpEditContext->IsEditCorridor() && mpEditContext->GetSelectElement())
			{
				COnboardCorridor* pCorridor = (COnboardCorridor*)pElmPtr->mpElement;
				const CPath2008& pathCenter = pCorridor->GetPathCenterLine();
				const CPath2008& pathLeft = pCorridor->GetPathLeftLine();
				const CPath2008& pathRight = pCorridor->GetPathRightLine();
				if (pathCenter.getCount()>1 && pathLeft.getCount()>0 && pathRight.getCount()>0)
				{
					m_pathNode.InitPathConfig(COnboardCorridor::drawPathCount);
					m_pathNode.setPath(pathCenter, COnboardCorridor::drawCenterPath);
					m_pathNode.setPath(pathLeft, COnboardCorridor::drawLeftPath);
					m_pathNode.setPath(pathRight, COnboardCorridor::drawRightPath);
					m_pathNode.setPointSize(10.0, COnboardCorridor::drawCenterPath);

					CPath2008 path;
					path.init(2);

					path[0] = pathCenter[0];
					path[1] = pathLeft[0];
					m_pathNode.setPath(path, COnboardCorridor::drawCenterToLeftHead);
					m_pathNode.setShowNode(false, COnboardCorridor::drawCenterToLeftHead);

					path[0] = pathCenter[0];
					path[1] = pathRight[0];
					m_pathNode.setPath(path, COnboardCorridor::drawCenterToRightHead);
					m_pathNode.setShowNode(false, COnboardCorridor::drawCenterToRightHead);

					path[0] = pathCenter[pathCenter.getCount() - 1];
					path[1] = pathLeft[pathLeft.getCount() - 1];
					m_pathNode.setPath(path, COnboardCorridor::drawCenterToLeftTail);
					m_pathNode.setShowNode(false, COnboardCorridor::drawCenterToLeftTail);

					path[0] = pathCenter[pathCenter.getCount() - 1];
					path[1] = pathRight[pathRight.getCount() - 1];
					m_pathNode.setPath(path, COnboardCorridor::drawCenterToRightTail);
					m_pathNode.setShowNode(false, COnboardCorridor::drawCenterToRightTail);

					m_pathNode.Update();
					m_pathNode.SetAny(Any(pElmPtr));
					m_pathNode.AttachTo(mLayout3DNode.GetDeckNode(pCorridor->GetDeck()));
				}
			}
		}

	}
}

void CAircraftLayoutScene::OnBeginDrag( const MouseSceneInfo& moveInfo )
{
	if(!m_pDragObject)
		return;
	m_bOnDragging  =true;

	const MouseSceneInfo& lastMoveInfo = mState->m_lastLBtnDownInfo;
	ARCVector3 worlddiff = ARCVECTOR(moveInfo.mWorldPos - lastMoveInfo.mWorldPos);		

	const Any& userdata  = m_pDragObject->getUserAny();
	const CAircraftElementShapePtr* pelmPtr = any_cast<CAircraftElementShapePtr>(&userdata);
	if(pelmPtr ) //drag  a element
	{
		CAircraftElementShapePtr pSelectElm = *pelmPtr;			
		if(pSelectElm&& !pSelectElm->IsLocked())
		{										
			//drag select elements
			CMoveElementsCmd* pCmd = mpEditContext->StartMoveSelectElements();
			if(pCmd)
			{
				pCmd->AddOffset( worlddiff );
				OnUpdateOperation(pCmd);
				return;				
			}
		}		
	}	


	EditPointKey ptKey;
	if(m_pathNode.OnDrag(m_pDragObject,worlddiff,ptKey)) //
	{
		if( CSeatGroup*const* pGroupPtr = any_cast<CSeatGroup*>(&m_pathNode.GetAny()))
		{
			CSeatGroup* pGroup = *pGroupPtr;		
			mpEditContext->StartNewCommand( new CEditSeatGroupPropCmd(pGroup,GetEditLayout()) );
			return ;
		}
		if (const CAircraftElementShapePtr* pElmPtr = any_cast<CAircraftElementShapePtr>(&m_pathNode.GetAny()))
		{
			mpEditContext->StartNewCommand( new CModifyElementsPropCmd(*pElmPtr, GetEditLayout(), CModifyElementsPropCmd::_Prop) );
			return;
		}
	}
}

void CAircraftLayoutScene::OnEndDrag( const MouseSceneInfo& moveInfo )
{
	
	if( CSeatGroup*const* pGroupPtr = any_cast<CSeatGroup*>(&m_pathNode.GetAny()))
	{
		CSeatGroup* pGroup = *pGroupPtr;	
		pGroup->setPath(m_pathNode.getPath());		
		OnUpdateOperation(mpEditContext->DoneCurEdit());		
		return ;
	}
	if (CAircraftElementShapePtr*const* pElmPtr = any_cast<CAircraftElementShapePtr*>(&m_pathNode.GetAny()))
	{
		OnUpdateOperation(mpEditContext->DoneCurEdit());		
	}
	//
	ClearDragInfo();
}

void CAircraftLayoutScene::ClearDragInfo()
{
	m_pDragObject = NULL; m_bOnDragging = false;
}


void CAircraftLayoutScene::UpdateDeckAltitude( CDeck *pDeck )
{
	//ASSERT(0);//need to be implemented by Ben
	CBaseFloor3DList::NodeTypePtr item = mLayout3DNode.mDeckList.FindItem(pDeck);
	if(item.get())
	{
		item->UpdatePos();

		int nElement = mLayout3DNode.mElmentsList.GetCount();
		for(int i=0;i<nElement;i++)
		{
			CAircraftElmentNodeList::NodeTypePtr pNode = mLayout3DNode.mElmentsList.GetItem(i);
			CString strType = pNode-> GetElement()->GetType();
			if(strType == COnBoardStair::TypeString || strType == COnBoardEscalator::TypeString || strType == COnBoardElevator::TypeString)
			{
				pNode->Update(mLayout3DNode);
			}
		}

	}
}

bool CAircraftLayoutScene::AddEditPointToAircraftElement( CAircraftElmentShapeDisplay* pathRelElem, int nPathIndex, const ARCVector3& posNewPoint )
{
	if (pathRelElem->mpElement && pathRelElem->mpElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCorridor = (COnboardCorridor*)pathRelElem->mpElement;
		if (pCorridor->AddPathPoint((COnboardCorridor::DrawPathIndex)nPathIndex, CPoint2008(posNewPoint.x, posNewPoint.y, posNewPoint.z)))
		{
			// ...
			return true;
		}
	}
	return false;
}

bool CAircraftLayoutScene::RemoveEditPointFromAircraftElement( CAircraftElmentShapeDisplay* pathRelElem, int nPathIndex, int nNodeIndex )
{
	if (pathRelElem->mpElement && pathRelElem->mpElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCorridor = (COnboardCorridor*)pathRelElem->mpElement;
		if (pCorridor->RemovePathPoint((COnboardCorridor::PathIndex)nPathIndex, nNodeIndex))
		{
			// ...
			return true;
		}
	}
	return false;
}

//overlay text display


CAircraftLayoutOverlayDisplay::CAircraftLayoutOverlayDisplay()
{
	mpLayoutScene=NULL;
}

void CAircraftLayoutOverlayDisplay::PreDrawTo( Ogre::Viewport* pvp )
{	
	UpdatePos(pvp);
	for(size_t i=0;i<mOpenOverlays.size();i++)
	{
		mOpenOverlays[i]->show();
	}	
}

void CAircraftLayoutOverlayDisplay::EndDrawTo( Ogre::Viewport* )
{
	for(size_t i=0;i<mOpenOverlays.size();i++)
	{
		mOpenOverlays[i]->hide();
	}
}


const static CString Prefix = _T("ElmTO"); //element text overlay
const static CString templateName = _T("SimpleTextOverlay");
const static CString stextArea = _T("Desc");

Ogre::Overlay* CAircraftLayoutOverlayDisplay::UpdateElementText( CAircraftElmentShapeDisplay* pelm)
{	
	DispProp& dsp = pelm->GetDisplay(CAircraftElmentShapeDisplay::DSPTYPE_NAME);
	//if(dsp.bOn)
	{//update content
		CString idName = pelm->GetName().GetIDString();
		CString instName = Prefix  + ARCStringConvert::toString((int)pelm);
	
		Overlay* pOverlay = OgreUtil::createOrRetriveOverlay(instName);
		if(pOverlay)
		{
			CString contName = instName + _T("/") + templateName;
			OverlayContainer* pContainer = (OverlayContainer*)pOverlay->getChild(contName.GetString());
			if(!pContainer){
				pContainer  = (OverlayContainer*)OverlayManager::getSingleton().cloneOverlayElementFromTemplate(templateName.GetString(),instName.GetString());
				pOverlay->add2D(pContainer);
			}	
			CString areaInst = instName + _T("/") + stextArea;
			TextAreaOverlayElement* pText = (TextAreaOverlayElement*)OverlayManager::getSingleton().getOverlayElement(areaInst.GetString());
			ASSERT(pText);
			String caption = idName.GetString();
			if( pText->getCaption() != caption )
			{
				pText->setCaption(caption);				
			}

			ColourValue color = OGRECOLOR(dsp.cColor);
			if(pText->getColour()!=color)
				pText->setColour( color );
		}
		return pOverlay;	
	}
	return NULL;
}





void CAircraftLayoutOverlayDisplay::UpdatePos( Ogre::Viewport* pvp)
{
	if(!mpLayoutScene)
		return;

	mOpenOverlays.resize(0);
	if( !mpLayoutScene->mpEditContext->IsShowText() )
		return;

	CAircraftElmentNodeList& nodelist = mpLayoutScene->mLayout3DNode.mElmentsList;
	for(int i=0;i<nodelist.GetCount();i++)
	{
		CAircraftElmentNodeList::NodeTypePtr elmnode = nodelist.GetItem(i);
		CAircraftElementShapePtr pelm = elmnode->GetElement();
		DispProp& dsp = pelm->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_NAME];
		if(!dsp.bOn){
			continue;
		}
		Ogre::Vector3 vpos = elmnode->GetSceneNode()->_getDerivedPosition();
		Ogre::Vector3 scrnpos = _getScreenCoordinates(vpos, pvp);

		CString instName = Prefix  + ARCStringConvert::toString((int)pelm);
		Overlay* pOverlay = OgreUtil::createOrRetriveOverlay(instName);
		pOverlay->setScroll(scrnpos.x,scrnpos.y);	
		mOpenOverlays.push_back(pOverlay);		
	}

}

void CAircraftLayoutOverlayDisplay::UpdateElmentsTags( CAircraftLayoutScene* playout )
{
	mpLayoutScene =playout;
	CAircraftElmentNodeList& nodelist = mpLayoutScene->mLayout3DNode.mElmentsList;
	for(int i=0;i<nodelist.GetCount();i++)
	{
		CAircraftElmentNodeList::NodeTypePtr elmnode = nodelist.GetItem(i);
		UpdateElementText(elmnode->GetElement());
	}
}
