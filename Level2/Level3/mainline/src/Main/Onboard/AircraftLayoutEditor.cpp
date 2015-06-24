#include "StdAfx.h"
#include ".\aircraftlayouteditor.h"
#include "../Resource.h"
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
#include "../DlgOnBoardObjectDisplayProperty.h"
#include "../TermPlanDoc.h"
#include "../DlgSeatRowProp.h"
#include "../DlgSeatArraySpec.h"
#include "../OnBoardVerticalDeviceDlg.h"
#include "../onboardstairdlg.h"
#include "../OnBoardEmExitDlg.h"
#include "../OnBoardGalleyDlg.h"
#include "../OnBoardEscalatorDlg.h"
#include "../OnBoardElevatorDlg.h"
#include "../DlgOnboardWall.h"
#include "../DlgOnboardPortal.h"
#include "../DlgOnboardSurface.h"
#include "InputOnBoard/OnBoardWall.h"
#include "InputOnBoard/OnboardSurface.h"
#include "InputOnBoard/OnboardPortal.h"
#include "../DlgFloorThickness.h"
#include "../DlgOnboardWall.h"
#include "../DlgOnboardPortal.h"
#include "../DlgOnboardSurface.h"
#include "InputOnBoard/OnBoardWall.h"
#include "InputOnBoard/OnBoardSurface.h"
#include "InputOnBoard/OnBoardPortal.h"
#include "..\DlgShapeTranformProp.h"
#include "InputOnboard/OnboardCorridor.h"
#include "../DlgOnBoardCorridor.h"




#include <Renderer/RenderEngine/AircraftLayoutScene.h>

CAircraftLayoutEditor::CAircraftLayoutEditor( CDocument* pDoc)
{
	SetDocument(pDoc);
}
CAircraftLayoutEditor::~CAircraftLayoutEditor(void)
{
}

CAddDeckCommand* CAircraftLayoutEditor::OnNewDeck()
{
	CAircaftLayOut *pLayout = GetEditLayOut();
 
	int nLevel = pLayout->GetDeckManager()->getCount();
	CString sName = pLayout->GetDeckManager()->GetNextDeckName();
	CDeck* pNewDeck = new CDeck(nLevel);	
	pNewDeck->SetName(sName);
	if (0 == nLevel)
		pNewDeck->IsActive(TRUE); // if the only deck, active it
	CAddDeckCommand* pCmd = new CAddDeckCommand(pNewDeck,pLayout->GetDeckManager());
	pCmd->Do();
	AddCommand(pCmd);
	return pCmd;
}

CDelDeckCommand* CAircraftLayoutEditor::OnDelDeck( CDeck* pDeck )
{
	CAircaftLayOut *pLayout = GetEditLayOut();
	if(pLayout->IsDeckBeUsed(pDeck))
	{
		AfxMessageBox(_T("Cannot delete Deck.\nPlease remove all Placements from floor before deleting,"),
			MB_OK | MB_ICONWARNING);
		return NULL;
	}

	CString sMsg;
	sMsg.Format("Are you sure you want to delete the deck \'%s\'?", pDeck->GetName().GetString());
	if(AfxMessageBox(sMsg, MB_YESNO) == IDNO) {
		return NULL;
	}


	CDelDeckCommand* pCmd = new CDelDeckCommand(pDeck,pLayout->GetDeckManager());
	pCmd->Do();
	AddCommand(pCmd);
	return pCmd;
}


CDelAircraftElementsCmd* CAircraftLayoutEditor::OnDelElement( const CAircraftElmentShapeDisplayList& vlist )
{
	if(!vlist.Count())
		return NULL;

	CAircaftLayOut *pLayout = GetEditLayOut();	
	CDelAircraftElementsCmd* pCmd = new CDelAircraftElementsCmd(vlist,pLayout);
	pCmd->Do();
	AddCommand(pCmd);
	return pCmd;
}

CAddAircraftElementsCmd* CAircraftLayoutEditor::OnAddNewElement( CAircraftElementShapePtr pelm)
{
	CAircaftLayOut *pLayout = GetEditLayOut();
	CAircraftElmentShapeDisplayList list;
	list.Add(pelm);
	CAddAircraftElementsCmd* pCmd = new CAddAircraftElementsCmd(list,pLayout);
	pCmd->Do();
	AddCommand(pCmd);
	return pCmd;	
}

CAddAircraftElementsCmd* CAircraftLayoutEditor::OnNewElement( ARCVector3 dropPos, CAircraftFurnishingModel* pFurnishing,CWnd* pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return NULL;
	}		
	if( pFurnishing )
	{
		dropPos[VZ] = 0;
		CAircraftElmentShapeDisplay* pElmDsp = new CAircraftElmentShapeDisplay;
		pElmDsp->m_vLocation = dropPos;		
		pElmDsp->mpDeck = pDeck;
		pElmDsp->mShapeID = pFurnishing->mguid;		
		pElmDsp->m_idName = playout->GetPlacements()->mvElementsList.GetValidShapeName(pFurnishing->GetType());
		return OnAddNewElement(pElmDsp);		
	}
	return NULL;
}

COnBoardObjectBaseDlg* CAircraftLayoutEditor::GetElementDlg(CAircraftElement* pElm,CWnd* pFromWnd)
{
	if(pElm == NULL)
		return NULL;

	if(pElm->GetType() == CSeat::TypeString)
	{
		return new  COnBoardSeatDlg((CSeat*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == CStorage::TypeString)
	{
		return  new COnBoardStorageDlg((CStorage*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == CDoor::TypeString)
	{
		return  new COnBoardDoorDlg((CDoor*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == CEmergencyExit::TypeString)
	{
		return new  COnBoardEmExitDlg((CEmergencyExit*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnboardGalley::TypeString)
	{
		return  new COnBoardGalleyDlg((COnboardGalley*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnBoardStair::TypeString)
	{
		return  new COnBoardStairDlg((COnBoardStair*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnBoardEscalator::TypeString)
	{
		return  new COnBoardEscalatorDlg((COnBoardEscalator*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnBoardElevator::TypeString)
	{
		return  new COnBoardElevatorDlg((COnBoardElevator*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnBoardWall::TypeString)
	{
		return  new CDlgOnboardWall((COnBoardWall*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnboardPortal::TypeString)
	{
		return  new CDlgOnboardPortal((COnboardPortal*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnboardSurface::TypeString)
	{
		return  new CDlgOnboardSurface((COnboardSurface*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	if(pElm->GetType() == COnboardCorridor::TypeString)
	{
		return  new CDlgOnboardCorridor((COnboardCorridor*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	}
	//if(pElm->GetType() == COnboardCorridor::TypeString)
	//{
	//	return  new CDlgOnBoardCorridor((COnboardCorridor*)pElm,GetEditLayOut(),GetProjectID(),pFromWnd);
	//}




	return NULL;
}

CModifyElementsPropCmd* CAircraftLayoutEditor::OnElementProperties(CAircraftElmentShapeDisplay* pElement, CWnd* pFromWnd )
{
	if(!pElement)
		return NULL;

//	INT_PTR bOperation;
	CAircaftLayOut* playout = GetEditLayOut();
	COnBoardObjectBaseDlg* pDlg = NULL;	
	pDlg = GetElementDlg(pElement->mpElement,pFromWnd);	

	if( pDlg &&  IDOK == pDlg->DoModal() )
	{
		CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pElement,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
		pCmd->EndOp();
		if(AddCommand(pCmd))
		{
			return pCmd;
		}else 
			delete pCmd;
	}
	delete pDlg;
	pDlg = NULL;	
	return NULL;
}




CModifyElementsPropCmd* CAircraftLayoutEditor::OnElementDisplayProperties(const CAircraftElmentShapeDisplayList& vlist, CWnd* pFromWnd )
{
	if(vlist.Count()==0)
		return NULL;
	
	CDlgOnBoardObjectDisplayProperty dlg(vlist.GetItem(0),pFromWnd);
	if(dlg.DoModal() == IDOK)
	{
		CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(m_lastSelectElm,GetEditLayOut(),CModifyElementsPropCmd::_DisplayProp);
		for(int i=0;i<vlist.Count();i++)
			vlist.GetItem(i)->mDspProps = dlg.mDispPropEdit;
		pCmd->EndOp();
		AddCommand(pCmd);
        return pCmd;		
	}		
	return NULL;
}

void CAircraftLayoutEditor::OnCopySelectElements()
{	
	if(m_vSelectedElments.Count()<=0)
		return;

	CAircraftElmentShapeDisplayList vCopyElements;
	for(int i=0;i<m_vSelectedElments.Count();i++)
	{
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		CAircraftElmentShapeDisplay* pNewElm = placement->CopyElement(m_vSelectedElments.GetItem(i));		
		if(pNewElm)
		{		
			vCopyElements.Add(pNewElm);
			placement->AddElementShape(pNewElm);
		}
	}
	CAddAircraftElementsCmd* pCmd = new CAddAircraftElementsCmd(vCopyElements,GetEditLayOut());
	pCmd->EndOp();
	AddCommand(pCmd);
    NotifyOperations(pCmd);	
}

int CAircraftLayoutEditor::GetProjectID() const
{
	ASSERT(mpParentDoc && mpParentDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)) );
	CTermPlanDoc* pDoc = DYNAMIC_DOWNCAST(CTermPlanDoc,mpParentDoc);
	return pDoc->GetProjectID();
}

bool CAircraftLayoutEditor::OnDeleteSelectElements()
{
	if(!m_vSelectedElments.Count())
		return false;	
	
	CDelAircraftElementsCmd* pCmd = new CDelAircraftElementsCmd( m_vSelectedElments,GetEditLayOut() );
	pCmd->Do();
	AddCommand(pCmd);
	
	ClearSelect();
	NotifyOperations(pCmd);	
	return true;
}

InputOnboard* CAircraftLayoutEditor::GetInputOnboard() const
{
	ASSERT(mpParentDoc);
	ASSERT(mpParentDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pDoc = DYNAMIC_DOWNCAST(CTermPlanDoc,mpParentDoc);
	return pDoc->GetInputOnboard();
}

void CAircraftLayoutEditor::OnUndo()
{
	if (!IsViewLocked())
	{
		mpParentDoc->UpdateAllViews(NULL,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)Undo() );
	}
}

void CAircraftLayoutEditor::OnRedo()
{
	if (!IsViewLocked())
	{
		mpParentDoc->UpdateAllViews(NULL,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)Redo() );
	}
}

bool CAircraftLayoutEditor::OnSelectSeat()
{
	if(!m_lastSelectElm)
		return false;	
	mCurSelSeatType = _selectseat;		
	OnSelectElement(m_lastSelectElm,false);
	return true;
}


bool CAircraftLayoutEditor::OnSelectSeatRow()
{
	if(!m_lastSelectElm)
		return false;	
	mCurSelSeatType = _selectseatrow;		
	OnSelectElement(m_lastSelectElm,false);
	return true;
}

bool CAircraftLayoutEditor::OnSelectSeatGroup()
{
	if(!m_lastSelectElm)
		return false;	
	mCurSelSeatType = _selectseatarray;		
	OnSelectElement(m_lastSelectElm,false);
	return true;
}

void CAircraftLayoutEditor::OnEditSeatRow( CWnd* pFromWnd )
{
	if(mCurSelSeatType != _selectseatrow)
		return;
	if(!m_pEditLayOut)
		return;

	if(m_vSelectedElments.Count() <= 0)
		return;

	CAircraftElementShapePtr pElement = m_vSelectedElments.GetItem(0);
	CSeat *pSeat = pElement->IsSeat();
	if(pSeat == NULL)
		return;

	CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
	CSeatRow *pSeatRow = pPlacement->GetOrCreateSeatRow(pSeat);
	ARCPath3 vPath = pPlacement->GetRowPath(pSeatRow, pElement);


	CDlgSeatRowProp dlg(mSeatRowOrGroup.GetIDString(),vPath,pFromWnd);
	if(dlg.DoModal() == IDOK)
	{		
		//pPlacement->UpdateSeatRow(pSeatRow,dlg.mDestPath);
		CEditSeatRowPropCmd* pCmd = new CEditSeatRowPropCmd(pSeatRow,m_pEditLayOut);
		pCmd->SetBeginState(vPath);
		pCmd->SetEndState(dlg.mDestPath);
		pCmd->OnDo();
		AddCommand(pCmd);
		NotifyOperations(pCmd);
	}	
	
}

void CAircraftLayoutEditor::OnCopySeatRow()
{
	if(mCurSelSeatType != _selectseatrow)
		return;
	if(!m_pEditLayOut)
		return;

	if(m_vSelectedElments.Count() <= 0)
		return;

	CAircraftElementShapePtr pElement = m_vSelectedElments.GetItem(0);
	CSeat *pSeat = pElement->IsSeat();
	if(pSeat == NULL)
		return;
	if(pSeat->GetID() < 0)
		return;
	CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();

	CSeatRow *pSeatRow = pPlacement->GetOrCreateSeatRow(pSeat);

	CAircraftElmentShapeDisplayList reflectseats = pPlacement->CopySeatRow(pSeatRow);
	if(reflectseats.Count())
	{
		CAddAircraftElementsCmd* pCmd = new CAddAircraftElementsCmd(reflectseats,GetEditLayOut());
		pCmd->EndOp();
		AddCommand(pCmd);
		NotifyOperations(pCmd);		
	}
}

void CAircraftLayoutEditor::OnDeletSeatRow()
{
	if(mCurSelSeatType != _selectseatrow)
		return;
	if(!m_pEditLayOut)
		return;

	if(m_vSelectedElments.Count() <= 0)
		return;

	CAircraftElementShapePtr pElement = m_vSelectedElments.GetItem(0);
	CSeat *pSeat = pElement->IsSeat();
	if(pSeat == NULL)
		return;

	CAircraftElmentShapeDisplayList vRowSeat =  m_pEditLayOut->GetPlacements()->GetRowSeats(pSeat->getRow()->GetID());
	CAircraftLayoutEditCommand* pCmd  = OnDelElement(vRowSeat);
	NotifyOperations(pCmd);
}

void CAircraftLayoutEditor::OnEditSeatGroup( CWnd* pFromWnd )
{
	if(mCurSelSeatType != _selectseatarray)
		return;
	if(!m_pEditLayOut)
		return;

	if(m_vSelectedElments.Count() <= 0)
		return;

	CAircraftElementShapePtr pElement = m_vSelectedElments.GetItem(0);
	CSeat *pSeat = pElement->IsSeat();
	if(pSeat == NULL)
		return;
	if(pSeat->getRow() == NULL)
	{
		MessageBox(NULL,_T("Please create the row first by \"Edit Seat Row\" in the Menu."), _T("Create Array"), MB_OK);
		return;
	}

	CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
	CSeatGroup* pGrp = pPlacement->GetOrCreateSeatArray(pSeat);
	pGrp->SaveData(m_pEditLayOut->GetID());
	CDlgSeatArraySpec dlg(pGrp,pPlacement , pFromWnd);
	CEditSeatGroupPropCmd* pCmd = new CEditSeatGroupPropCmd(pGrp,m_pEditLayOut);
	pCmd->BeginOp();
	if( dlg.DoModal() == IDOK)
	{				
		pCmd->EndOp();		
		pCmd->OnDo();
		AddCommand(pCmd);
		NotifyOperations(pCmd);		
	}
}

void CAircraftLayoutEditor::OnDeleteSeatGroup()
{
	if(mCurSelSeatType != _selectseatarray)
		return;
	if(!m_pEditLayOut)
		return;
	
	if(m_vSelectedElments.Count() <= 0)
		return;

	CAircraftElementShapePtr pElement = m_vSelectedElments.GetItem(0);
	CSeat *pSeat = pElement->IsSeat();
	if(pSeat == NULL)
		return;

	CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
	CSeatGroup* pGrp = pPlacement->GetOrCreateSeatArray(pSeat);
	if(pGrp)
	{
		CDelSeatGroupCmd* pCmd = new CDelSeatGroupCmd(pGrp,m_pEditLayOut);
		pCmd->Do();
		AddCommand(pCmd);
		NotifyOperations(pCmd);		
	}
}

CModifyElementsPropCmd* CAircraftLayoutEditor::OnSelectElementProperties( CWnd* pFromWnd )
{
	return OnElementProperties(m_lastSelectElm,pFromWnd);
}

void CAircraftLayoutEditor::NotifyOperations( CAircraftLayoutEditCommand* pOP, CView* pexcludeView)
{
	mpParentDoc->UpdateAllViews(pexcludeView,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pOP );
}
bool CAircraftLayoutEditor::GetElementMenu( CAircraftElementShapePtr pelm, int& resId, int& menuIdx )
{
	if(!pelm)
		return false;

	if(pelm->mpElement)
	{
		resId = IDR_MENU_AIRCEAFT_ELEMENT;
		menuIdx = 1;
	}
	else
	{
		resId = IDR_MENU_AIRCEAFT_ELEMENT;
		menuIdx = 0;
	}
	return true;
}


void CAircraftLayoutEditor::OnChangeUndefinedToSeat(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	CSeat* pnewseat = new CSeat;
	pnewseat->SetDeck(pUndefElm->GetDeck());
	pnewseat->SetPosition(pUndefElm->GetShapePos());	
	COnBoardSeatDlg dlg(pnewseat,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pnewseat;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}	
}

void CAircraftLayoutEditor::OnChangeUndefinedToStorage(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	CStorage* pStorage = new CStorage;
	pStorage->SetDeck(pUndefElm->GetDeck());
	pStorage->SetPosition(pUndefElm->GetShapePos());
	pStorage->SetRotation(pUndefElm->m_dRotation);
	COnBoardStorageDlg dlg(pStorage,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pStorage;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}

void CAircraftLayoutEditor::OnChangeUndefinedToDoor(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	CDoor* pDoor = new CDoor;
	pDoor->SetDeck(pUndefElm->GetDeck());
	pDoor->SetPosition(pUndefElm->GetShapePos());	
	COnBoardDoorDlg dlg(pDoor,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pDoor;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}

void CAircraftLayoutEditor::OnChangeUndefinedToGalley(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	COnboardGalley* pGalley = new COnboardGalley;
	pGalley->SetDeck(pUndefElm->GetDeck());
	pGalley->SetPosition(pUndefElm->GetShapePos());	
	COnBoardGalleyDlg dlg(pGalley,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pGalley;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}

void CAircraftLayoutEditor::OnChangeUndefinedToEmergencyExit(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	CEmergencyExit* pEmExit = new CEmergencyExit;
	pEmExit->SetDeck(pUndefElm->GetDeck());
	pEmExit->SetPosition(pUndefElm->GetShapePos());	
	COnBoardEmExitDlg dlg(pEmExit,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pEmExit;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}

void CAircraftLayoutEditor::OnChangeUndefinedToStair(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	COnBoardStair* pDevice = new COnBoardStair;
	COnBoardStairDlg dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pDevice;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}

void CAircraftLayoutEditor::OnChangeUndefinedToEscaltor(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	COnBoardEscalator* pDevice = new COnBoardEscalator;
	COnBoardEscalatorDlg dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pDevice;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}

void CAircraftLayoutEditor::OnChangeUndefinedToElevator(CWnd* pFromWnd)
{
	CAircraftElementShapePtr pUndefElm = m_lastSelectElm;
	if(!pUndefElm)
		return;

	ASSERT(pUndefElm->mpElement==NULL);
	COnBoardElevator* pDevice = new COnBoardElevator;
	COnBoardElevatorDlg dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);
	CModifyElementsPropCmd* pCmd = new CModifyElementsPropCmd(pUndefElm,GetEditLayOut(),CModifyElementsPropCmd::_Prop);
	if(dlg.DoModal() == IDOK)
	{
		pUndefElm->mpElement = pDevice;
		CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
		placement->AddElementShape(pUndefElm);
		pCmd->EndOp();
		if(AddCommand(pCmd)){
			NotifyOperations(pCmd);
		}else{ delete pCmd; pCmd = NULL; }
	}
}
void CAircraftLayoutEditor::OnNewWall( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}

	COnBoardWall* pWall = new COnBoardWall;
	pWall->SetDeck(pDeck);

	CDlgOnboardWall dlg(pWall,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pWall;

		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);

		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pWall;
		}
	}
	else
	{
		delete pWall;
	}
}
void CAircraftLayoutEditor::OnNewSurface( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}

	COnboardSurface* pDevice = new COnboardSurface;
	pDevice->SetDeck(pDeck);
	CDlgOnboardSurface dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pDevice;

		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);


		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pDevice;
		}
	}
	else
	{
		delete pDevice;
	}
}
void CAircraftLayoutEditor::OnNewCorridor( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}

	COnboardCorridor* pDevice = new COnboardCorridor;
	pDevice->SetDeck(pDeck);
	CDlgOnboardCorridor dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pDevice;

		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);


		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pDevice;
		}
	}
	else
	{
		delete pDevice;
	}
}


void CAircraftLayoutEditor::OnNewStair( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}

	COnBoardStair* pDevice = new COnBoardStair;
	COnBoardStairDlg dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pDevice;

		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);


		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pDevice;
		}
	}
	else
	{
		delete pDevice;
	}
}


void CAircraftLayoutEditor::OnNewEscalator( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}

	COnBoardEscalator* pDevice = new COnBoardEscalator;
	COnBoardEscalatorDlg dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pDevice;

		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);


		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pDevice;
		}
	}
	else
	{
		delete pDevice;
	}
}


void CAircraftLayoutEditor::OnNewElevator( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}

	COnBoardElevator* pDevice = new COnBoardElevator;
	COnBoardElevatorDlg dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pDevice;

		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);


		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pDevice;
		}
	}
	else
	{
		delete pDevice;
	}
}


void CAircraftLayoutEditor::OnNewPortal( CWnd*pFromWnd )
{
	CAircaftLayOut * playout =  GetEditLayOut();
	CDeck* pDeck = playout->GetDeckManager()->GetActivedDeck();
	if(!pDeck)
	{
		AfxMessageBox("Please select a active deck first");		
		return ;
	}
	
	COnboardPortal* pDevice = new COnboardPortal;
	pDevice->SetDeck(pDeck);
	CDlgOnboardPortal dlg(pDevice,GetEditLayOut(),GetProjectID(),pFromWnd);

	if(dlg.DoModal() == IDOK)
	{
		CAircraftElmentShapeDisplay* pDisplay = new CAircraftElmentShapeDisplay;

		pDisplay->mpDeck = pDeck;
		pDisplay->mpElement = pDevice;
		
		CAddAircraftElementsCmd* pCmd = OnAddNewElement(pDisplay);
		CAircraftPlacements* pPlacement = m_pEditLayOut->GetPlacements();
		if(pPlacement)
			pPlacement->AddElementShape(pDisplay);

		if(pCmd)
		{
			NotifyOperations(pCmd);
		}
		else
		{ 
			delete pCmd; 
			pCmd = NULL; 
			delete pDisplay;
			delete pDevice;
		}
	}
	else
	{
		delete pDevice;
	}

}


void CAircraftLayoutEditor::OnSeatCopy()
{
	if(GetSelSeatType()==_selectseat)
	{
		OnCopySelectElements();
	}
	if(GetSelSeatType() == _selectseatrow)
	{
		OnCopySeatRow();
	}
}

void CAircraftLayoutEditor::OnSeatDelete()
{
	if(GetSelSeatType()==_selectseat)
	{
		OnDeleteSelectElements();
	}
	if(GetSelSeatType() == _selectseatrow)
	{
		OnDeletSeatRow();
	}
	if(GetSelSeatType()== _selectseatarray)
	{
		OnDeleteSeatGroup();
	}
}

CRotateShapesCmd* CAircraftLayoutEditor::StartRotateSelectShapes()
{
	DoneCurEdit();
	if(m_vSelectedElments.Count())
	{
		m_pCurCommand = new CRotateShapesCmd(m_vSelectedElments,m_pEditLayOut );
		m_pCurCommand->BeginOp();
		return (CRotateShapesCmd*)m_pCurCommand;
	}
	return NULL;
}

void CAircraftLayoutEditor::CancelCurEdit(CAircraftLayoutScene* pScene)
{
	if (m_pCurCommand)
	{
		m_pCurCommand->Undo();
		pScene->OnUpdateOperation(m_pCurCommand);
		delete m_pCurCommand;
		m_pCurCommand = NULL;
	}
}

CMoveShapesCmd* CAircraftLayoutEditor::StartMoveSelectShapes( CMoveShapesCmd::Move_TYPE t )
{
	DoneCurEdit();
	if(m_vSelectedElments.Count())
	{
		CMoveShapesCmd* pShapeCmd  =  new CMoveShapesCmd(m_vSelectedElments,m_pEditLayOut );		
		pShapeCmd->SetMoveType(t);
		m_pCurCommand = pShapeCmd;
		m_pCurCommand->BeginOp();
		return (CMoveShapesCmd*)m_pCurCommand;
	}
	return NULL;
}

CScaleShapesCmd* CAircraftLayoutEditor::StartScaleSelectShapes( CScaleShapesCmd::Scale_TYPE t )
{
	DoneCurEdit();
	if(m_vSelectedElments.Count())
	{
		CScaleShapesCmd* pShapeCmd = new CScaleShapesCmd(m_vSelectedElments,m_pEditLayOut );
		pShapeCmd->SetScaleType(t);
		m_pCurCommand = pShapeCmd;
		m_pCurCommand->BeginOp();
		return (CScaleShapesCmd*)m_pCurCommand;
	}
	return NULL;
}

CMoveShapesZ0Cmd* CAircraftLayoutEditor::DoMoveShapesZ0()
{
	DoneCurEdit();
	if(m_vSelectedElments.Count())
	{
		CMoveShapesZ0Cmd* pShapeCmd = new CMoveShapesZ0Cmd(m_vSelectedElments,m_pEditLayOut);
		pShapeCmd->DoMoveZ0();
		pShapeCmd->EndOp();
		AddCommand(pShapeCmd);
		NotifyOperations(pShapeCmd);
		return pShapeCmd;
	}
	return NULL;
}

CModiftShapesTranCmd* CAircraftLayoutEditor::OnEditShapeTranform( CWnd* pFromWnd )
{
	// TODO: Add your command handler code here	
	if(!m_lastSelectElm)
		return NULL;

	CAircraftElmentShapeDisplayList vlist;
	vlist.Add(m_lastSelectElm);
	CModiftShapesTranCmd * pModCmd = new CModiftShapesTranCmd(vlist,GetEditLayOut());
	CDlgShapeTranformProp dlg(m_lastSelectElm,GetProjectID(),pFromWnd);
	if(dlg.DoModal()==IDOK)
	{	
		pModCmd->EndOp();		
	}
	else
	{
		pModCmd->Undo();
	}
	NotifyOperations(pModCmd);
	return pModCmd;
}

void CAircraftLayoutEditor::DoArrayElements(const ARCPath3& vPts, int nCopies)
{	
	if(m_vSelectedElments.Count()<=0)
		return;

	CAircraftElmentShapeDisplayList vCopyElements;
	ARCVector3 vStep = (vPts.back() - vPts.front())/nCopies;
	for(int i=0;i<nCopies;i++)
	{
		ARCVector3 vOffset = (i + 1)*vStep;
		for(int i=0;i<m_vSelectedElments.Count();i++)
		{
			CAircraftPlacements* placement = m_pEditLayOut->GetPlacements();
			CAircraftElmentShapeDisplay* pNewElm = placement->CopyElement(m_vSelectedElments.GetItem(i));
			if(pNewElm)
			{
				pNewElm->OffsetProperties(vOffset);
				vCopyElements.Add(pNewElm);
				placement->AddElementShape(pNewElm);
			}
		}
	}
	CAddAircraftElementsCmd* pCmd = new CAddAircraftElementsCmd(vCopyElements,GetEditLayOut());
	pCmd->EndOp();
	AddCommand(pCmd);
	NotifyOperations(pCmd);	
}

double CAircraftLayoutEditor::GetActiveFloorHeight() const
{
	return 0;
}

