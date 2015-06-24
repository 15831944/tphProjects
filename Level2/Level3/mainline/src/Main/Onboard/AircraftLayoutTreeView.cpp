// LayoutView.cpp : implementation file
//

#include "stdafx.h"
//#include "TermPlan.h"
#include "../resource.h"
#include "AircraftLayoutTreeView.h"
#include <InputOnBoard/Deck.h>
#include <InputOnBoard/AircaftLayOut.h>
#include "../XPStyle/NewMenu.h"
// CLayoutView
//#include "../TermPlanDoc.h"
#include <Inputs/IN_TERM.H>
#include <InputOnBoard/CInputOnboard.h>
#include <InputOnBoard/AircraftLayoutEditContext.h>

#include "../DlgDeckProperties.h"
#include "../DlgVisibleRegions.h"
#include "../DlgDeckProperties.h"
#include "OnboardViewMsg.h"

#include <InputOnboard/Seat.h>
#include <InputOnboard/AircaftLayOut.h>
#include <InputOnboard/AircraftPlacements.h>
//
#include "../OnBoardCabinDividerDlg.h"
#include "../OnBoardStorageDlg.h"
#include "../OnBoardDoorDlg.h"
#include "../OnBoardSeatDlg.h"
#include "../DlgAircraftConfigurationSpecView.h"
#include "../OnBoardVerticalDeviceDlg.h"
#include "AircraftLayoutEditFuncs.h"
#include "../DlgOnBoardObjectDisplayProperty.h"
#include "AircraftLayoutEditor.h"
#include "../DlgFloorThickness.h"
#include "../DlgOnboardWall.h"
#include "../DlgOnboardPortal.h"
#include "../DlgOnboardSurface.h"
#include "InputOnBoard/OnBoardWall.h"
#include "InputOnBoard/OnBoardSurface.h"
#include "InputOnBoard/OnBoardPortal.h"
#include "InputOnBoard/OnboardCorridor.h"

//////////////////////////////////////////////////////////////////////////
#ifdef TEST_CODE
#include "../../Renderer/Test/mainDoc.h"
#include ".\aircraftlayouttreeview.h"







CAircraftLayoutEditContext* CAircraftLayoutTreeView::GetEditContext()
{
	return &((CmainDoc*)GetDocument())->mEditLayoutContext;
}
int CAircraftLayoutTreeView::GetProjectID(){
	return 0;
}
#else
#include "..\TermPlanDoc.h"
#include "..\..\InputOnBoard\CInputOnboard.h"
CAircraftLayoutEditor* CAircraftLayoutTreeView::GetEditor()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	return pDoc->GetAircraftLayoutEditor();
}
int CAircraftLayoutTreeView::GetProjectID(){
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	return pDoc->GetProjectID();
}
#endif
//////////////////////////////////////////////////////////////////////////



IMPLEMENT_DYNCREATE(CAircraftLayoutTreeView, CView)

CAircraftLayoutTreeView::CAircraftLayoutTreeView():m_layout(NULL)
{
}

CAircraftLayoutTreeView::~CAircraftLayoutTreeView()
{
	
}
CARCBaseTree& CAircraftLayoutTreeView::GetTreeCtrl()
{
	return m_wndTreeCtrl;
}
BEGIN_MESSAGE_MAP(CAircraftLayoutTreeView, CView)
	ON_WM_CREATE() 
	ON_WM_SIZE() 
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU() 
//ON_MESSAGE(ON_REFRESH_LAYOUTTREE,OnReFeshTree)
	ON_COMMAND(ID_EDITDECKS,OnEditDeck)
	ON_COMMAND(ID_DECK_COMMENT,OnDeckComment)
	ON_COMMAND(ID_DECK_HELP,OnDeckHelp)
	ON_COMMAND(ID_DECK_PROPERTIES,OnDeckProperies)

	ON_COMMAND(ID_DECK_ACTIVATE,OnDeckActive)
	ON_COMMAND(ID_DECK_ISOLATE,OnIsolate)
	ON_COMMAND(ID_DECK_HIDE,OnDeckHideOrShow)
	ON_COMMAND(ID_DECK_GIRDON,OnDeckGridOn)
	//ON_COMMAND(ID_DECK_COMPOSITION_THICKNESSON,OnIfThickness)
	ON_COMMAND(ID_DECK_COMPOSITION_VISIBLEREGIONS,OnVisibLeregions)
	ON_COMMAND(ID_DECK_COMPOSITION_INVISIBLEREGIONS,OnInvisibLeregion)
	ON_COMMAND(ID_DECK_COMMENT,OnComment)
	ON_UPDATE_COMMAND_UI(ID_DECK_HIDE, OnUpdateBarShowShowHide)
	ON_UPDATE_COMMAND_UI(ID_DECK_GIRDON, OnUpdateBarGridOnOff)
	ON_UPDATE_COMMAND_UI(ID_DECK_ACTIVATE,OnUpdateActiveItem)
	ON_UPDATE_COMMAND_UI(ID_DECK_COMPOSITION_THICKNESSON,OnUpdateThicknessOnOff)
	ON_COMMAND(ID_ONBOARDOBJECT_PROPERTIES, OnOnBoardObjectProperties)
	ON_COMMAND(ID_ONBOARDOBJECT_DISPLAYPROPERTIES, OnOnBoardObjectDisplayProperties)
	ON_COMMAND(ID_ONBOARDOBJECT_DELETE, OnOnBoardObjectDelete)
	ON_COMMAND(ID_ONBOARDOBJECT_COMMENT, OnOnBoardBbjectComment)
	ON_COMMAND(ID_ONBOARDOBJECT_HELP, OnOnBoardBbjectHelp)
	ON_COMMAND(ID_CTX_NEWFLOOR, OnCtxNewDeck)
	ON_COMMAND(ID_DECK_DELETE, OnDeckDelete)
	ON_COMMAND(ID_DECK_RENAME, OnDeckRename)
	ON_COMMAND(ID_CTX_OPAQUEDECK, OnCtxOpaquefloor)
	ON_UPDATE_COMMAND_UI(ID_CTX_OPAQUEDECK, OnUpdateOpaquefloor)
	ON_COMMAND(ID_CTX_DECKTHICKNESS_VALUE, OnCtxFloorthicknessValue)
	ON_COMMAND(ID_DECK_SHOWMAP, OnDeckShowmap)
	ON_UPDATE_COMMAND_UI(ID_DECK_SHOWMAP, OnUpdateDeckShowmap)
	ON_COMMAND(ID_Menu_SH_AC, OnMenuShAc)
	ON_UPDATE_COMMAND_UI(ID_Menu_SH_AC, OnUpdateMenuShAc)
	ON_COMMAND(ID_Menu_SH_GRID, OnMenuShGrid)
	ON_UPDATE_COMMAND_UI(ID_Menu_SH_GRID, OnUpdateMenuShGrid)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDWALL, OnOnboardElementAddWall)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDSURFACE, OnOnboardElementAddSurface)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDPORTAL, OnOnboardElementAddPortal)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDCORRIDOR, OnOnboardElementAddCorridor)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDSTAIR, OnOnboardElementAddStair)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDESCALATOR, OnOnboardElementAddEscalator)
	ON_COMMAND(ID_ONBOARD_ELEMENT_ADDELEVATOR, OnOnboardElementAddElevator)
	ON_COMMAND(ID_ONBOARDOBJECT_LOCK, OnOnboardobjectLock)
	ON_UPDATE_COMMAND_UI(ID_ONBOARDOBJECT_LOCK, OnUpdateOnboardobjectLock)
END_MESSAGE_MAP()
void CAircraftLayoutTreeView::OnEditDeck()
{
	if(m_layout == NULL)
	{
		MessageBox(_T("Please Select one AircraftLayout!"),NULL,MB_OK) ;
		return ;
	}
   // CDlgAircraftConfigurationSpecView DlgEdit(m_layout ,&((CTermPlanDoc *)GetDocument())->GetTerminal(),(CTermPlanDoc *)GetDocument(),this) ;
	//DlgEdit.DoModal() ;
}
void CAircraftLayoutTreeView::OnDeckComment()
{

}
void CAircraftLayoutTreeView::OnDeckHelp()
{

}
CDeck* CAircraftLayoutTreeView::GetCurrentSelDeck()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if(hSelItem)
	{
		CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(hSelItem) ;
		if(nodeData)
		{
			if (nodeData->m_Ty == TY_NODE_DECK)
			{
				CDeck* _deck = (CDeck*)nodeData->m_wData ; 
				return _deck ;
			}
		}
	}
	return NULL;
}

CAircraftElmentShapeDisplay* CAircraftLayoutTreeView::GetCurrentSelElement()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if(hSelItem)
	{
		CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(hSelItem) ;
		if(nodeData)
		{
			if(nodeData->m_Ty == TY_NODE_PLACEMENTS_DATA)//object
			{
				return ((CAircraftElmentShapeDisplay*)nodeData->m_wData);			
			}
		}
	}
	return NULL;
}


void CAircraftLayoutTreeView::OnDeckActive()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
	{
		CDeckManager* deckManger = m_layout->GetDeckManager() ;
		CModifyAllDeckCommand* pCmd = new CModifyAllDeckCommand(_deck,deckManger,CModifyAllDeckCommand::_ACTIVE);
		pCmd->BeginOp();
		{
			deckManger->ActiveDeck(_deck);
			deckManger->WriteDeckToDataBase();
		}
		pCmd->EndOp();		
		NotifyOperations(pCmd);

		HTREEITEM hDecksItem = m_mapPlacementRoot[TY_NODE_DECKS];
		if (m_wndTreeCtrl.ItemHasChildren(hDecksItem))
		{
			HTREEITEM hDeckItem = m_wndTreeCtrl.GetChildItem(hDecksItem);
			while (hDeckItem)
			{
				CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(hDeckItem) ;
				ASSERT(nodeData);
				CDeck* _tempdeck = (CDeck*)nodeData->m_wData;
				ASSERT(_tempdeck);
				int nImage = (_tempdeck == _deck) ? ID_NODEIMG_FLOORACTIVE : ID_NODEIMG_FLOORNOTACTIVE;
				m_wndTreeCtrl.SetItemImage(hDeckItem, nImage, nImage);

				hDeckItem = m_wndTreeCtrl.GetNextItem(hDeckItem, TVGN_NEXT);
			}
		}
	}
}

void CAircraftLayoutTreeView::OnIsolate()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
		_deck->IsVisible(TRUE) ;
	CDeck* otherdeck = NULL ;
	CDeckManager* pManger =  m_layout->GetDeckManager() ;


	CModifyAllDeckCommand* pCmd = new CModifyAllDeckCommand(_deck,pManger,CModifyAllDeckCommand::_ISOLATE);
	pCmd->BeginOp();
	for(int i = 0 ; i < pManger->getCount() ;i++)
	{
		otherdeck = pManger->getItem(i) ;
		if(otherdeck != _deck)
				otherdeck->IsVisible(FALSE) ;   
	}
	pCmd->EndOp();
	NotifyOperations(pCmd);	
}

void CAircraftLayoutTreeView::OnDeckHideOrShow()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	CDeckManager* pManger =  m_layout->GetDeckManager() ;
	if(NULL == _deck)
		return;

	CModifyDeckCommand* pCmd = new CModifyDeckCommand(_deck,pManger,CModifyDeckCommand::_PROP);
	pCmd->BeginOp();
	_deck->IsVisible(!_deck->IsVisible()) ;	
	_deck->SaveDeckProperies();
	pCmd->EndOp();
	NotifyOperations(pCmd);
}

void CAircraftLayoutTreeView::OnDeckGridOn()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	CDeckManager* pManger =  m_layout->GetDeckManager() ;
	if(NULL == _deck)
		return;

	CModifyDeckCommand* pCmd = new CModifyDeckCommand(_deck,pManger,CModifyDeckCommand::_GRID);
	pCmd->BeginOp();	
	_deck->GetGrid()->bVisibility = (!_deck->GetGrid()->bVisibility) ;
	_deck->SaveGridInfo();
	pCmd->EndOp();
	NotifyOperations(pCmd);	
}

void CAircraftLayoutTreeView::OnVisibLeregions()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(NULL == _deck)
		return;	
	CDeckManager* pManger =  m_layout->GetDeckManager() ;

	CModifyDeckCommand* pCmd = new CModifyDeckCommand(_deck,pManger,CModifyDeckCommand::_REGION);
	pCmd->BeginOp();
	CDlgDeckVisibleRegions dlg(_deck,TRUE,(CTermPlanDoc *)GetDocument(),this) ;
	if( dlg.DoModal() ==IDOK)
	{	  
		pCmd->EndOp();
		NotifyOperations(pCmd);
	}
}
void CAircraftLayoutTreeView::OnInvisibLeregion()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(NULL == _deck)
		return;	
	CDeckManager* pManger =  m_layout->GetDeckManager() ;

	CModifyDeckCommand* pCmd = new CModifyDeckCommand(_deck,pManger,CModifyDeckCommand::_REGION);
	pCmd->BeginOp();
	CDlgDeckVisibleRegions dlg(_deck,FALSE,(CTermPlanDoc *)GetDocument(),this) ;
	if(dlg.DoModal()==IDOK)
	{	
		pCmd->EndOp();
		NotifyOperations(pCmd);
	}
}
void CAircraftLayoutTreeView::OnComment()
{

}
void CAircraftLayoutTreeView::OnReFeshTree()
{
   InitLayoutTree() ;
}
// CLayoutView drawing

void CAircraftLayoutTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

void CAircraftLayoutTreeView::OnInitialUpdate()
{
	CView::OnInitialUpdate() ;
	m_layout = GetEditor()->GetEditLayOut();

	{
		if (m_imgList.m_hImageList == NULL)
		{
			m_imgList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);	
		}
		CBitmap bmL;
		bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
		int nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
		bmL.DeleteObject();
		bmL.LoadBitmap(IDB_NODESMALLIMG);
		nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
		bmL.DeleteObject();
		bmL.LoadBitmap(IDB_FLOORSMALLIMG);
		nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
		bmL.DeleteObject();
		bmL.LoadBitmap(IDB_AREASSMALLIMG);
		nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
		bmL.DeleteObject();
		bmL.LoadBitmap(IDB_PORTALSSMALLIMG);
		nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
		bmL.DeleteObject();

		GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
		GetTreeCtrl().SetImageList(&m_imgList,TVSIL_STATE);
	}


	InitLayoutTree() ;
}
void CAircraftLayoutTreeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType,cx,cy) ;
	if(m_wndTreeCtrl.GetSafeHwnd() != NULL)
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}
int CAircraftLayoutTreeView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CView::OnCreate(lpcs) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS;

	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0),
		this, IDC_LAYOUTVIEW_TREE);

	return (bResult ? 0 : -1);
}
void CAircraftLayoutTreeView::InitLayoutTree()
{
	m_mapPlacementNode.clear();
	m_mapPlacementRoot.clear();


	m_NodeData.Clear() ;

	m_wndTreeCtrl.DeleteAllItems();


	if(m_layout == NULL)
		return ;
	m_wndTreeCtrl.DeleteAllItems() ;
	CLayoutNodeData* pLayoutNodeData = m_NodeData.newItem();
	pLayoutNodeData->m_Ty = TY_NODE_LAYOUT ;
	pLayoutNodeData->m_wData = m_layout ;
	pLayoutNodeData->m_resouceID = IDR_MENU_AC_LAYOUT;
    
	HTREEITEM hLayoutNode = AddItem(_T("Layout")) ;
	m_wndTreeCtrl.SetItemData(hLayoutNode,(DWORD)pLayoutNodeData) ;
	CDeckManager* deckManger = m_layout->GetDeckManager() ;
	if(deckManger)
	   LoadDecks(deckManger,hLayoutNode) ;
	

	//
	LoadUnderConstructs(hLayoutNode);
	LoadPlacement(hLayoutNode);
	LoadVerticalDevices(hLayoutNode);
	LoadSurfaceAreas(hLayoutNode);

	CAircraftElmentShapeDisplayList vdefined = m_layout->GetPlacements()->mvElementsList.GetSorted();
	for (int nElement = 0; nElement < vdefined.Count(); ++ nElement)
	{
		CAircraftElmentShapeDisplay* pItem = vdefined.GetItem(nElement);
		if(pItem)
			AddPlacement(pItem);
	}


    GetTreeCtrl().Expand(hLayoutNode,TVE_EXPAND) ;
	LoadProcessorBehavior(TVI_ROOT);
}
void CAircraftLayoutTreeView::LoadDecks(CDeckManager* _manger , HTREEITEM _item)
{
	if(_manger == NULL)
		return ;
	CLayoutNodeData* pDeckNodeData = m_NodeData.newItem();
	pDeckNodeData->m_Ty = TY_NODE_DECKS ;
	pDeckNodeData->m_wData = NULL ;
	pDeckNodeData->m_resouceID = IDR_CTX_FLOORS ; 

	HTREEITEM hDecksNode = AddItem(_T("Decks"),_item,TVI_FIRST) ;
	m_wndTreeCtrl.SetItemData(hDecksNode,(DWORD)pDeckNodeData) ;
	m_mapPlacementRoot[TY_NODE_DECKS] = hDecksNode;
	for (int i = 0 ; i < _manger->getCount();i++)
	{
		AddDeckNodeTree(_manger->getItem(i),i+1,hDecksNode) ;
	}
	m_wndTreeCtrl.Expand(hDecksNode,TVE_EXPAND) ;
}
void CAircraftLayoutTreeView::AddDeckNodeTree(CDeck* _deck,int index,HTREEITEM _item)
{
	if(_deck == NULL)
		return ;
  
   CString str = _deck->GetName();
   int nImage = _deck->IsActive() ? ID_NODEIMG_FLOORACTIVE : ID_NODEIMG_FLOORNOTACTIVE;
   HTREEITEM deckNode = AddItem(str,_item,TVI_LAST,nImage,nImage) ;
   CLayoutNodeData* NodeData = m_NodeData.newItem();
   NodeData->m_Ty = TY_NODE_DECK ;
   NodeData->m_wData = _deck ;
   NodeData->m_resouceID = IDR_MENU_ONBOARD_LAYOUT_DECK ;

   m_wndTreeCtrl.SetItemData(deckNode,(DWORD)NodeData) ;
   m_wndTreeCtrl.SetItemImage(deckNode, nImage, nImage);
}


HTREEITEM CAircraftLayoutTreeView::AddItem( const CString& strNode,HTREEITEM hParent,HTREEITEM hInsertAfter,int nImage ,int nSelectImage )
{
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_NORMAL;
	cni.nImage = nImage;
	cni.nImageSeled = nSelectImage;
	return m_wndTreeCtrl.InsertItem(strNode,cni,FALSE,FALSE,hParent,hInsertAfter);
}


// CLayoutView diagnostics
void CAircraftLayoutTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	CNewMenu menu;
	CMenu *pCtxMenu = NULL;
	UINT uFlags;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(cliPt, &uFlags);
	m_hRightClkItem = hItem;

	if(hItem)
	{
		CLayoutNodeData* nodedata = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;

		if(nodedata->m_Ty == TY_NODE_PLACEMENTS_DATA)//object
		{
			GetEditor()->OnSelectElement((CAircraftElmentShapeDisplay*)nodedata->m_wData);			
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
			}

		}
		else if(nodedata->m_Ty == TY_NODE_SURFACEAREAS_WALLS)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));//wall
			}

		}
		else if (nodedata->m_Ty == TY_NODE_SURFACEAREAS_AREAS)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(1));//surface
			}
		}
		else if (nodedata->m_Ty == TY_NODE_SURFACEAREAS_PORTALS)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(2));//portal
			}
		}

		else if (nodedata->m_Ty == TY_NODE_PLACEMENTS_CORRIDOR)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(3));//corridor
			}
		}
		else if (nodedata->m_Ty == TY_NODE_VERTICALDEVICES_STAIRS)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(4));//stair
			}
		}
		else if (nodedata->m_Ty == TY_NODE_VERTICALDEVICES_ESCALATOR)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(5));//escalator
			}
		}
		else if (nodedata->m_Ty == TY_NODE_VERTICALDEVICES_ELEVATOR)
		{
			if(nodedata->m_resouceID != 0)
			{
				menu.LoadMenu(nodedata->m_resouceID);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(6));//elevator
			}
		}
		else if(nodedata->m_resouceID != 0)
		{
			menu.LoadMenu(nodedata->m_resouceID);
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		}
	}


  if(pCtxMenu != NULL)
	  pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , point.x, point.y, AfxGetMainWnd()) ;
}
void CAircraftLayoutTreeView::OnUpdateBarShowShowHide(CCmdUI* pCmdUI) 
{
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;
	if(_deck->IsVisible())
		pCmdUI->SetText(_T("Hide")) ;
	else
		pCmdUI->SetText(_T("Show")) ;
}
void CAircraftLayoutTreeView::OnUpdateBarGridOnOff(CCmdUI* pCmdUI) 
{
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;
	if(_deck->GetGrid()->bVisibility)
		pCmdUI->SetText(_T("Gird Off")) ;
	else
		pCmdUI->SetText(_T("Gird On")) ;
}
void CAircraftLayoutTreeView::OnUpdateActiveItem(CCmdUI* pCmdUI)
{
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;
	if(_deck->IsActive())
		pCmdUI->Enable(FALSE) ;
	else
		pCmdUI->Enable(TRUE) ;
}
void CAircraftLayoutTreeView::OnUpdateThicknessOnOff(CCmdUI* pCmdUI)
{
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;
	if(_deck->IsOpaque())
		pCmdUI->SetText("Thickness Off") ;
	else
		pCmdUI->SetText("Thickness On") ;
}
#ifdef _DEBUG
void CAircraftLayoutTreeView::AssertValid() const
{
	CView::AssertValid();
}

void CAircraftLayoutTreeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG
void CAircraftLayoutTreeView::OnDeckProperies()
{
	if(m_hRightClkItem == NULL)
		return ;
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;

	CModifyDeckCommand* pCmd = new CModifyDeckCommand(_deck,GetEditor()->GetEditLayOut()->GetDeckManager(),CModifyDeckCommand::_ALL);
	pCmd->BeginOp();
	CDlgDeckProperties dlgDeckPro(_deck, (CTermPlanDoc*) (GetParentFrame()->GetActiveDocument())) ;
	if( dlgDeckPro.DoModal() == IDOK)
	{
		pCmd->EndOp();
		NotifyOperations(pCmd);		
	}
}


// CLayoutView message handlers
void CAircraftLayoutTreeView::LoadPlacement( HTREEITEM hRootItem )
{
	CString str  = _T("Placements");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem();
	NodeData->m_Ty = TY_NODE_PLACEMENTS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData) ;

	LoadSeat(hPlacementNode);
	LoadStorage(hPlacementNode);
	LoadCabinDivider(hPlacementNode);
	LoadGalleys(hPlacementNode);
	LoadDoors(hPlacementNode);
	LoadEmergencyExits(hPlacementNode);	
	LoadCorridor(hPlacementNode);

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadSeat( HTREEITEM hRootItem )
{
	//seat node root

	CString str  = _T("Seats");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData =m_NodeData.newItem();
	NodeData->m_Ty = TY_NODE_PLACEMENTS_SEATS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_SEATS] = hPlacementNode;


	//CSea* deckManger = m_layout->GetPlacements()->GetSeatManager() ;
	//add seat into tree
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadStorage( HTREEITEM hRootItem )
{
	CString str  = _T("Storage");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData =m_NodeData.newItem();
	NodeData->m_Ty = TY_NODE_PLACEMENTS_STORAGE ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_STORAGE] = hPlacementNode;

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);

}

void CAircraftLayoutTreeView::LoadCabinDivider( HTREEITEM hRootItem )
{
	CString str  = _T("Cabin Dividers");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_CABINDIVIDER ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_CABINDIVIDER] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadGalleys( HTREEITEM hRootItem )
{
	CString str  = _T("Galleys");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_GALLEYS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_GALLEYS] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadDoors( HTREEITEM hRootItem )
{
	CString str  = _T("Doors");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_DOORS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_DOORS] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadEmergencyExits( HTREEITEM hRootItem )
{
	CString str  = _T("Emergency Exits");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_EMERGENCYEXITS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_EMERGENCYEXITS] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}
void CAircraftLayoutTreeView::LoadCorridor( HTREEITEM hRootItem )
{
	CString str  = _T("Corridor");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_CORRIDOR ;
	NodeData->m_wData = NULL ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_CORRIDOR] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);

}
void CAircraftLayoutTreeView::LoadVerticalDevices( HTREEITEM hRootItem )
{

	CString str  = _T("Vertical Devices");
	HTREEITEM hVerticalNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hVerticalNode,(DWORD)NodeData);
	LoadStairs(hVerticalNode);
	LoadEscalator(hVerticalNode);
	LoadElevator(hVerticalNode);
	m_wndTreeCtrl.Expand(hVerticalNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadStairs( HTREEITEM hRootItem )
{
	CString str  = _T("Stairs");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES_STAIRS ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	m_mapPlacementRoot[TY_NODE_VERTICALDEVICES_STAIRS] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadEscalator( HTREEITEM hRootItem )
{
	CString str  = _T("Escalator");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES_ESCALATOR ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	//
	m_mapPlacementRoot[TY_NODE_VERTICALDEVICES_ESCALATOR] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadElevator( HTREEITEM hRootItem )
{
	CString str  = _T("Elevator");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES_ELEVATOR ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);

	m_mapPlacementRoot[TY_NODE_VERTICALDEVICES_ELEVATOR] = hPlacementNode;

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);

}

void CAircraftLayoutTreeView::LoadSurfaceAreas( HTREEITEM hRootItem )
{
	CString str  = _T("Surface Areas");
	HTREEITEM hSurfaceNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hSurfaceNode,(DWORD)NodeData);


	LoadWalls(hSurfaceNode);
	LoadAreas(hSurfaceNode);
	LoadPortals(hSurfaceNode);

	m_wndTreeCtrl.Expand(hSurfaceNode,TVE_EXPAND);
	
}

void CAircraftLayoutTreeView::LoadWalls( HTREEITEM hRootItem )
{
	CString str  = _T("Walls");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS_WALLS ;
	NodeData->m_wData = NULL ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;
	
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);

	m_mapPlacementRoot[TY_NODE_SURFACEAREAS_WALLS] = hPlacementNode;

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
	
}

void CAircraftLayoutTreeView::LoadAreas( HTREEITEM hRootItem )
{
	CString str  = _T("Areas");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;

	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS_AREAS ;
	NodeData->m_wData = NULL ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;

	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);
	

	m_mapPlacementRoot[TY_NODE_SURFACEAREAS_AREAS] = hPlacementNode;

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);

}

void CAircraftLayoutTreeView::LoadPortals( HTREEITEM hRootItem )
{
	CString str  = _T("Portals");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS_PORTALS ;
	NodeData->m_wData = NULL ;
	NodeData->m_resouceID = IDR_ONBOARD_POPUP;
	
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD)NodeData);

	m_mapPlacementRoot[TY_NODE_SURFACEAREAS_PORTALS] = hPlacementNode;

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CAircraftLayoutTreeView::LoadProcessorBehavior( HTREEITEM hRootItem )
{
	CString str  = _T("Processor Behaviors");
	HTREEITEM hBehaviorsNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PROCESSORBEHAVIORS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hBehaviorsNode,(DWORD)NodeData);

	
	LoadBehaviorElevator(hBehaviorsNode);
	LoadBehaviorEscalator(hBehaviorsNode);
	m_wndTreeCtrl.Expand(hBehaviorsNode,TVE_EXPAND);
	 ;
}

void CAircraftLayoutTreeView::LoadBehaviorElevator( HTREEITEM hRootItem )
{
	CString str  = _T("Elevators");
	HTREEITEM hBehaviorNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_PROCESSORBEHAVIORS_ELEVATORS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hBehaviorNode,(DWORD)NodeData);
	 ;
}

void CAircraftLayoutTreeView::LoadBehaviorEscalator( HTREEITEM hRootItem )
{
	CString str  = _T("Escalators");
	HTREEITEM hBehaviorNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* pNodeData = m_NodeData.newItem() ;
	pNodeData->m_Ty = TY_NODE_PROCESSORBEHAVIORS_ESCALATORS ;
	pNodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hBehaviorNode,(DWORD)pNodeData);

}

void CAircraftLayoutTreeView::AddPlacement( CAircraftElmentShapeDisplay *pElement )
{
	if (pElement == NULL)
	{
		ASSERT(pElement != NULL);
		return;
	}

	//get root
	HTREEITEM hRootItem = GetPlacementRoot(pElement->GetType());
	if(hRootItem == NULL)
		return;

	CString strText = m_wndTreeCtrl.GetItemText(hRootItem);
	ALTObjectID objName = pElement->GetName();
	bool bUndefind = (pElement->mpElement==NULL);
	
	//four level

	HTREEITEM hParentItem = hRootItem;
	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindChildNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}


			if (objName.m_val[nLevel+1] != _T(""))
			{			
				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);	

				CLayoutNodeData* pNodeData = m_NodeData.newItem() ;
				pNodeData->m_Ty = TY_NODE_PLACEMENTS_GROUP ;
				pNodeData->m_wData = (void*)nLevel ;
				pNodeData->m_resouceID = IDR_MENU_ELEMENT_GROUP;

				m_wndTreeCtrl.SetItemData(hTreeItem,(DWORD)pNodeData);
				hParentItem = hTreeItem;
				 ;
				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);

				CLayoutNodeData* pNodeData = m_NodeData.newItem() ;
				pNodeData->m_Ty = TY_NODE_PLACEMENTS_DATA ;
				pNodeData->m_wData = pElement ;
				pNodeData->m_resouceID = bUndefind?IDR_MENU_ONBOARD_SHAPE:IDR_MENU_ONBOARD_OBJECT;
				m_wndTreeCtrl.SetItemData(hTreeItem,(DWORD)pNodeData);
// 				m_wndTreeCtrl.SelectItem(hTreeItem);

				m_mapPlacementNode[(DWORD_PTR)pElement] = hTreeItem;
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);

			CLayoutNodeData* pNodeData = m_NodeData.newItem() ;
			pNodeData->m_Ty = TY_NODE_PLACEMENTS_DATA ;
			pNodeData->m_wData = pElement ;
			m_wndTreeCtrl.SetItemData(hTreeItem,(DWORD)pNodeData);
			m_wndTreeCtrl.SelectItem(hTreeItem);
			pNodeData->m_resouceID = IDR_MENU_ONBOARD_OBJECT;
			m_mapPlacementNode[(DWORD_PTR)pElement] = hTreeItem;
			break;
		}
	}
}

HTREEITEM CAircraftLayoutTreeView::FindChildNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_wndTreeCtrl.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}
HTREEITEM CAircraftLayoutTreeView::GetPlacementRoot( const CString& typeStr)
{
	HTREEITEM hRootItem = NULL;
	Nodetype nodeT = GetElementTypeNode(typeStr);
	if(nodeT!=TY_NODE_UNKNOW)
	{
		hRootItem = FindRootNodeType(nodeT);
	}
	return hRootItem;
}

void CAircraftLayoutTreeView::UpdatePlacement( CAircraftElmentShapeDisplay *pElement )
{
	DeletePlacement(pElement);
	AddPlacement(pElement);
}

void CAircraftLayoutTreeView::DeletePlacement( CAircraftElmentShapeDisplay *pElement )
{
	CString strChildItem;
	std::map<DWORD_PTR, HTREEITEM>::iterator iter = m_mapPlacementNode.find((DWORD_PTR)pElement);
	if(iter!= m_mapPlacementNode.end())
	{
		HTREEITEM hDeleteNode = (*iter).second;
		
		HTREEITEM hChildItem = hDeleteNode;

		while(hChildItem)
		{
			strChildItem = GetTreeCtrl().GetItemText(hChildItem);

			CLayoutNodeData *pNodeData = (CLayoutNodeData*)GetTreeCtrl().GetItemData(hChildItem);
			if(!pNodeData)
				break;
			if(pNodeData->m_Ty == TY_NODE_PLACEMENTS_GROUP && GetTreeCtrl().GetChildItem(hChildItem))
				break; // if a group and has child, just stop
			if(pNodeData->m_Ty!= TY_NODE_PLACEMENTS_GROUP && pNodeData->m_Ty != TY_NODE_PLACEMENTS_DATA)
				break;

			ASSERT(NULL == GetTreeCtrl().GetChildItem(hChildItem));
			HTREEITEM hItem = GetTreeCtrl().GetParentItem(hChildItem);
			GetTreeCtrl().DeleteItem(hChildItem);
			hChildItem = hItem;	
		}
		m_mapPlacementNode.erase(iter);
	}
}

void CAircraftLayoutTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class

	if(this == pSender)
		return;
	
	switch(lHint)
	{
	case VM_ONBOARD_ADD_AIRCRAFTELEMENT:
		{
			CAircraftElmentShapeDisplay *pElement = (CAircraftElmentShapeDisplay *)pHint;
			AddPlacement(pElement );
		}
		break;
	case VM_ONBOARD_DEL_AIRCRAFTELEMENT:
		{
			CAircraftElmentShapeDisplay *pElement = (CAircraftElmentShapeDisplay *)pHint;
			DeletePlacement(pElement );
		}
		break;
	case VM_ONBOARD_RELOAD_SEATELEMENT:
		{
			ReloadSeat();
		}
		break;
	case VM_ONBOARD_DECK_CHANGED:
        ReloadDeck();
		break ;
	case VM_ONBOARD_LAYOUT_CHANGE:
		{
		   if(m_layout == (CAircaftLayOut *)pHint)
				break ;
			m_layout = (CAircaftLayOut *)pHint ;
			InitLayoutTree() ;
		}
		break;
	case VM_ONBOARD_ACLAYOUT_EDIT_OPERATION:
		{
			OnUpdateOperations((CAircraftLayoutEditCommand*)pHint);
			break;
		}
	default:
		{

		}
		break;
	}

	CView::OnUpdate(pSender, lHint, pHint);
}
void CAircraftLayoutTreeView::ReloadDeck()
{
   std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_DECKS);
   HTREEITEM item = (*iter).second ;
   if(item == NULL)
	   return ;
   HTREEITEM PLayoutItem = GetTreeCtrl().GetParentItem(item) ;
   GetTreeCtrl().DeleteItem(item) ;
    LoadDecks(m_layout->GetDeckManager(),PLayoutItem) ;
}
void CAircraftLayoutTreeView::OnOnBoardObjectProperties()
{
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CAircraftElmentShapeDisplayList vlist = GetItemElementsList(hItemSelected);
	if(!vlist.Count())
		return;

	if(CModifyElementsPropCmd* pCmd = GetEditor()->OnElementProperties(vlist.GetItem(0),this) )
	{
		NotifyOperations(pCmd);
	}

}

void CAircraftLayoutTreeView::OnOnBoardObjectDisplayProperties()
{	
	// TODO: Add your command handler code here
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CAircraftElmentShapeDisplayList vlist = GetItemElementsList(hItemSelected);
	if(vlist.Count())
	{
		CModifyElementsPropCmd* pCmd = GetEditor()->OnElementDisplayProperties(vlist,this);
		NotifyOperations(pCmd,false);
	}
}

void CAircraftLayoutTreeView::OnOnBoardObjectDelete()
{
	// TODO: Add your command handler code here
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;
	CAircraftElmentShapeDisplayList vlist= GetItemElementsList(hItemSelected);

	if( CDelAircraftElementsCmd * pCmd = GetEditor()->OnDelElement(vlist) )
	{
		NotifyOperations(pCmd,true);
	}

}

void CAircraftLayoutTreeView::OnOnBoardBbjectComment()
{
	// TODO: Add your command handler code here
}

void CAircraftLayoutTreeView::OnOnBoardBbjectHelp()
{
	// TODO: Add your command handler code here
}




void CAircraftLayoutTreeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint cliPt = point;
	//m_wndTreeCtrl.ScreenToClient(&cliPt);
//	UINT uFlags;
	//HTREEITEM hItem = m_wndTreeCtrl.HitTest(cliPt, &uFlags);
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem)
	{
		CLayoutNodeData* pNodedata = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(hItem) ;

		if(pNodedata != NULL)
		{
			if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_DATA)//object
			{
				OnOnBoardObjectProperties();
			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_GROUP)
			{
				
				//INT_PTR bOperation;
				//COnBoardSeatDlg dlg((CSeat *)pElement,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
				//bOperation = dlg.DoModal();
				//

				//if (bOperation == IDOK)
				//{
				//	GetDocument()->UpdateAllViews(this,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT,(CObject*)pElement);
				//}
			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_STORAGE) 
			{

			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_CABINDIVIDER) 
			{

			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_GALLEYS) 
			{

			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_DOORS) 
			{

			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_EMERGENCYEXITS) 
			{

			}
			else if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_CORRIDOR) 
			{
				if (GetEditor())
					GetEditor()->OnNewCorridor(this);
			}
			//else if(pNodedata->m_Ty == TY_NODE_VERTICALDEVICES_STAIRS)
			//{
			//	/*COnBoardStair *pStair = new COnBoardStair;
			//	COnBoardVerticalDeviceDlg dlg(pStair,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
			//	if(dlg.DoModal() == IDOK)
			//	{
			//		m_layout->GetPlacements()->GetVerticalDeviceList().AddNewItem(pStair);
			//		AddPlacement(pStair);
			//	}
			//	else
			//	{
			//		delete pStair;
			//	}*/
			//}
			//else if(pNodedata->m_Ty == TY_NODE_VERTICALDEVICES_ESCALATOR)
			//{
			//	COnBoardEscalator *pEscalator = new COnBoardEscalator;
			//	COnBoardVerticalDeviceDlg dlg(pEscalator,m_layout,GetProjectID(),this);
			//	if(dlg.DoModal() == IDOK)
			//	{
			//		m_layout->GetPlacements()->GetVerticalDeviceList().AddNewItem(pEscalator);
			//		AddPlacement(pEscalator);

			//	}
			//	else
			//	{
			//		delete pEscalator;
			//	}
			//}
			//else if(pNodedata->m_Ty == TY_NODE_VERTICALDEVICES_ELEVATOR)
			//{
			//	COnBoardElevator *pElevator = new COnBoardElevator;
			//	COnBoardVerticalDeviceDlg dlg(pElevator,m_layout,GetProjectID(),this);
			//	if(dlg.DoModal() == IDOK)
			//	{
			//		m_layout->GetPlacements()->GetVerticalDeviceList().AddNewItem(pElevator);
			//		AddPlacement(pElevator);
			//	}
			//	else
			//	{
			//		delete pElevator;
			//	}
			//}


		}


	}
	CView::OnLButtonDblClk(nFlags, point);
}


void CAircraftLayoutTreeView::ReloadSeat()
{
	if(m_layout == NULL)
		return;

	HTREEITEM hRootItem = NULL;
	std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_PLACEMENTS_SEATS);
	if(iter!= m_mapPlacementRoot.end())
	{
		hRootItem = (*iter).second;
	}
	if(hRootItem == NULL)
		return;


	HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hRootItem);
	while (hChildItem != NULL)
	{
		GetTreeCtrl().DeleteItem(hChildItem);		
		hChildItem = GetTreeCtrl().GetChildItem(hRootItem);
	}

	CAircraftElmentShapeDisplayList& vElements = m_layout->GetPlacements()->mvElementsList;
	for (int nElement = 0; nElement < vElements.Count(); ++ nElement)
	{
		CAircraftElmentShapeDisplay *pElement = vElements.GetItem(nElement);
		if(pElement->GetType() == CSeat::TypeString)
			AddPlacement(pElement);
	}
}




void CAircraftLayoutTreeView::OnCtxNewDeck()
{
	// TODO: Add your command handler code here
	if( CAddDeckCommand*pCmd = GetEditor()->OnNewDeck() )
	{		
		m_layout->GetDeckManager()->WriteDeckToDataBase();
		ReloadDeck();
		NotifyOperations(pCmd);//GetDocument()->UpdateAllViews(this,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pCmd);
	}

}

void CAircraftLayoutTreeView::OnDeckDelete()
{
	// TODO: Add your command handler code here
	if(m_hRightClkItem == NULL)
		return ;
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;
	
	if( CDelDeckCommand*pCmd = GetEditor()->OnDelDeck(_deck) )
	{
		m_layout->GetDeckManager()->WriteDeckToDataBase();
		ReloadDeck();		
		GetDocument()->UpdateAllViews(this,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pCmd);
	}
}

void CAircraftLayoutTreeView::OnDeckRename()
{
	// TODO: Add your command handler code here
	if(m_hRightClkItem == NULL)
		return ;
// 	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
// 	if(nodeData == NULL)
// 		return ;
// 	CDeck* _deck = (CDeck*)nodeData->m_wData ;
// 	if(_deck == NULL)
// 		return ;
	m_wndTreeCtrl.EditLabel(m_hRightClkItem);
}

void CAircraftLayoutTreeView::OnCtxOpaquefloor()
{
	// TODO: Add your command handler code here
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
	{
		_deck->IsOpaque(_deck->IsOpaque()?FALSE:TRUE) ;
		_deck->SaveDeckProperies();
	}
	GetDocument()->UpdateAllViews(this, VM_ONBOARD_DECK_CHANGED, (CObject*)_deck);
}
void CAircraftLayoutTreeView::OnUpdateOpaquefloor( CCmdUI *pCmdUI )
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(!_deck)
		return;
	pCmdUI->SetText(_deck->IsOpaque() ? _T("Disabled") : _T("Enabled")) ;
}

void CAircraftLayoutTreeView::OnCtxFloorthicknessValue()
{
	// TODO: Add your command handler code here
	CDeck* _deck = GetCurrentSelDeck() ;
	if(!_deck)
		return;
	// get floor thickness
	CDlgFloorThickness dlg(_deck->Thickness(),(CTermPlanDoc*)GetDocument());
	if(IDOK == dlg.DoModal()) {
		// TRACE("Set floor thickness: %.2f\n", dlg.GetThickness());
		_deck->Thickness(dlg.GetThickness());
		_deck->SaveDeckProperies();
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_DECK_CHANGED, (CObject*)_deck);
	}
}

void CAircraftLayoutTreeView::OnDeckShowmap()
{
	// TODO: Add your command handler code here
	CDeck* _deck = GetCurrentSelDeck() ;
	if(!_deck)
		return;
	_deck->IsShowMap(!_deck->IsShowMap());
	_deck->SaveDeckProperies();
	GetDocument()->UpdateAllViews(this, VM_ONBOARD_DECK_CHANGED, (CObject*)_deck);
}

void CAircraftLayoutTreeView::OnUpdateDeckShowmap(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CDeck* _deck = GetCurrentSelDeck() ;
	if(!_deck)
		return;

	if(_deck->IsShowMap())
		pCmdUI->SetText(_T("Map Off")) ;
	else
		pCmdUI->SetText(_T("Map On")) ;
}

void CAircraftLayoutTreeView::OnUpdateOperations( CAircraftLayoutEditCommand* pOP )
{
	if(!pOP)
		return;
	ACLayoutEditOp emOp = pOP->getOpType();
	if( emOp == OP_addDeck || emOp == OP_delDeck )
	{
		ReloadDeck();
	}
	if( emOp == OP_addElments)
	{	
		CAddAircraftElementsCmd* pCmd = (CAddAircraftElementsCmd*)pOP;
		if(pOP->mLastOp == Command::_do)
		{
			AddPlacements( pCmd->GetElmList() );
		}
		else
		{
			DelPlacements( pCmd->GetElmList() ); 
		}	
	}
	if( emOp == OP_delElments)
	{
		CDelAircraftElementsCmd * pCmd = (CDelAircraftElementsCmd*)pOP;
		if(pCmd->mLastOp == Command::_do)
		{
			DelPlacements( pCmd->GetElmList() ); 
		}
		else{
			AddPlacements( pCmd->GetElmList() ); 
		}
	}
	if( emOp == OP_editElement)
	{
		CModifyElementsPropCmd* pCmd = (CModifyElementsPropCmd*)pOP;
		if(pCmd->GetModifyType() != CModifyElementsPropCmd::_DisplayProp)
		{
			UpdatePlacement( pCmd->GetElmList().GetItem(0) );
		}
	}
	if( CEditSeatRowCmd* pCmd = pOP->ToSeatRowCmd())
	{
		DelPlacements( pCmd->GetCurrentElements() ); 
		AddPlacements(pCmd->GetCurrentElements());
	}
	if(CSeatGroupEditCmd* pCmd = pOP->ToSeatArrayCmd())
	{
		//HTREEITEM hFindItem = FindPlacementGroupItem(CSeat::TypeString,pCmd->GetGroupName());
		//if(hFindItem)
		//{
		//	GetTreeCtrl().DeleteItem(hFindItem);
		//}
		DelPlacements(pCmd->GetCurrentElements());
		AddPlacements(pCmd->GetCurrentElements());
	}
	
}

void CAircraftLayoutTreeView::AddPlacements( const CAircraftElmentShapeDisplayList& elmlist)
{
	for(int i=0;i<elmlist.Count();i++)
		AddPlacement(elmlist.GetItem(i));
}

void CAircraftLayoutTreeView::DelPlacements( const CAircraftElmentShapeDisplayList& elmlist)
{
	for(int i=0;i<elmlist.Count();i++)
		DeletePlacement(elmlist.GetItem(i));
}

void CAircraftLayoutTreeView::NotifyOperations( CAircraftLayoutEditCommand* pOP, bool toSelf /*= true*/ )
{
	GetDocument()->UpdateAllViews(toSelf?NULL:this,VM_ONBOARD_ACLAYOUT_EDIT_OPERATION,(CObject*)pOP);
}
void CAircraftLayoutTreeView::OnMenuShAc()
{
	// TODO: Add your command handler code here
	CAircaftLayOut* playout = GetEditor()->GetEditLayOut();
	playout->IsShowAC(!playout->IsShowAC());
	GetDocument()->UpdateAllViews(this, VM_ONBOARD_ACLAYOUT_GRID_MODEL_CHANGE);
}

void CAircraftLayoutTreeView::OnUpdateMenuShAc(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CAircaftLayOut* playout = GetEditor()->GetEditLayOut();
	if(playout->IsShowAC())
		pCmdUI->SetText(_T("Hide Aircraft"));
	else
		pCmdUI->SetText(_T("Show Aircraft"));
}

void CAircraftLayoutTreeView::OnMenuShGrid()
{
	// TODO: Add your command handler code here
	CAircaftLayOut* playout = GetEditor()->GetEditLayOut();
	playout->IsShowGrid(!playout->IsShowGrid());
	GetDocument()->UpdateAllViews(this, VM_ONBOARD_ACLAYOUT_GRID_MODEL_CHANGE);
}

void CAircraftLayoutTreeView::OnUpdateMenuShGrid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CAircaftLayOut* playout = GetEditor()->GetEditLayOut();
	if(playout->IsShowGrid())
		pCmdUI->SetText("Hide Grid");
	else
		pCmdUI->SetText("Show Grid");
}

void CAircraftLayoutTreeView::LoadUnderConstructs( HTREEITEM hRootItem )
{
	CString str  = _T("Under Construction Placements");
	HTREEITEM hUndefinedShapeNdoe = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = m_NodeData.newItem() ;
	NodeData->m_Ty = TY_NODE_UNDEFINED_SHAPE ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hUndefinedShapeNdoe,(DWORD)NodeData);
	m_wndTreeCtrl.Expand(hUndefinedShapeNdoe,TVE_EXPAND);
	m_mapPlacementRoot[TY_NODE_UNDEFINED_SHAPE] = hUndefinedShapeNdoe;	
	//m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}
LRESULT CAircraftLayoutTreeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case ENDLABELEDIT:
		{
			OnEndLabelEdit(wParam, lParam);
		}
		break;
	case BEGINLABELEDIT:
		{
			OnBeginLabelEdit(wParam,lParam);
		}
		break;
	case SELCHANGED:
		{
			OnSelChanged(wParam,lParam);
		}
		break;
	default:
		{
// 			ASSERT(FALSE);
		}
		break;
	}

	return CView::WindowProc(message, wParam, lParam);
}

LRESULT CAircraftLayoutTreeView::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
	if (pNMTreeView->itemNew.hItem)
	{
		CLayoutNodeData* nodedata = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
		
		if(nodedata->m_Ty == TY_NODE_PLACEMENTS_DATA)//object
		{
			GetEditor()->OnSelectElement((CAircraftElmentShapeDisplay*)nodedata->m_wData);
		}
	}

	return 0;
}

LRESULT CAircraftLayoutTreeView::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	//	*pResult = TRUE;
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;

	CDeck* pDeck = GetCurrentSelDeck();
	*pResult = 0 == pDeck ? TRUE : FALSE;
	return FALSE;
}


LRESULT CAircraftLayoutTreeView::OnEndLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;
	//return immediately if the name is null (no rename).
	if(pDispInfo->item.pszText == NULL)
		return 0;

	CDeck* pDeck = GetCurrentSelDeck();
	pDeck->SetName(pDispInfo->item.pszText);

	m_layout->GetDeckManager()->WriteDeckToDataBase();

	*pResult = TRUE;  // return FALSE to cancel edit, TRUE to accept it.
	return 0;
}

CAircraftElmentShapeDisplayList CAircraftLayoutTreeView::GetItemElementsList( HTREEITEM hItem )
{
	CAircraftElmentShapeDisplayList vlist;
	if(!hItem)
		return vlist;
	CLayoutNodeData* pNodedata = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(hItem) ;
	if(!pNodedata)
		return vlist;
	if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_DATA)
	{		
		vlist.Add((CAircraftElmentShapeDisplay*)pNodedata->m_wData);
		return vlist;
	}
	if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_GROUP)
	{
		HTREEITEM hChild = m_wndTreeCtrl.GetChildItem(hItem);
		while(hChild)
		{
			vlist.Add(GetItemElementsList(hChild));
			hChild = m_wndTreeCtrl.GetNextSiblingItem(hChild);
		}
	}
	return vlist;

}




HTREEITEM CAircraftLayoutTreeView::FindPlacementGroupItem( CString placeType,const ALTObjectID& grpName )
{
	HTREEITEM hRootItem = GetPlacementRoot(placeType);
	if(hRootItem)
	{		
		HTREEITEM findItem = hRootItem;
		for(int i=0;i<OBJECT_STRING_LEVEL;i++)
		{
			CString strText=  grpName.at(i).c_str();
			if(strText.IsEmpty())
				return findItem;
			findItem  = FindChildNode( findItem,strText );
			if(findItem == NULL)
				return findItem;

		}
	}
	return NULL;
}
void CAircraftLayoutTreeView::OnOnboardElementAddCorridor()
{
	//// TODO: Add your command handler code here
	if(GetEditor())
		GetEditor()->OnNewCorridor(this);
}
void CAircraftLayoutTreeView::OnOnboardElementAddStair()
{
	//// TODO: Add your command handler code here
	if(GetEditor())
		GetEditor()->OnNewStair(this);
}
void CAircraftLayoutTreeView::OnOnboardElementAddEscalator()
{
	//// TODO: Add your command handler code here
	if(GetEditor())
		GetEditor()->OnNewEscalator(this);
}
void CAircraftLayoutTreeView::OnOnboardElementAddElevator()
{
	//// TODO: Add your command handler code here
	if(GetEditor())
		GetEditor()->OnNewElevator(this);
}
void CAircraftLayoutTreeView::OnOnboardElementAddWall()
{
	// TODO: Add your command handler code here
	if(GetEditor())
		GetEditor()->OnNewWall(this);
}

void CAircraftLayoutTreeView::OnOnboardElementAddSurface()
{	
	if(GetEditor())
		GetEditor()->OnNewSurface(this);
}

void CAircraftLayoutTreeView::OnOnboardElementAddPortal()
{
	if(GetEditor())
		GetEditor()->OnNewPortal(this);

}

HTREEITEM CAircraftLayoutTreeView::FindRootNodeType( Nodetype ndType ) const
{
	std::map<Nodetype, HTREEITEM>::const_iterator iter = m_mapPlacementRoot.find(ndType);
	if(iter!= m_mapPlacementRoot.end())
	{
		return (*iter).second;
	}
	return NULL;
}

CAircraftLayoutTreeView::Nodetype CAircraftLayoutTreeView::GetElementTypeNode( CString typeStr )
{
	if(typeStr == CSeat::TypeString) //seat
	{
		return (TY_NODE_PLACEMENTS_SEATS);		
	}
	else if(typeStr == CCabinDivider::TypeString)
	{
		return (TY_NODE_PLACEMENTS_CABINDIVIDER);		
	}
	else if(typeStr == CStorage::TypeString)
	{
		return (TY_NODE_PLACEMENTS_STORAGE);		
	}
	else if(typeStr == CDoor::TypeString)
	{
		return (TY_NODE_PLACEMENTS_DOORS);		
	}
	else if(typeStr == COnBoardStair::TypeString)
	{
		return (TY_NODE_VERTICALDEVICES_STAIRS);		
	}
	else if(typeStr == COnboardGalley::TypeString)
	{
		return TY_NODE_PLACEMENTS_GALLEYS;
	}
	else if(typeStr == CEmergencyExit::TypeString)
	{
		return TY_NODE_PLACEMENTS_EMERGENCYEXITS;
	}
	else if(typeStr == COnBoardEscalator::TypeString)
	{
		return (TY_NODE_VERTICALDEVICES_ESCALATOR);		
	}
	else if(typeStr == COnBoardElevator::TypeString)
	{
		return (TY_NODE_VERTICALDEVICES_ELEVATOR);		
	}
	else if(typeStr == CAircraftElmentShapeDisplay::UndefinedType)
	{
		return (TY_NODE_UNDEFINED_SHAPE);		
	}
	else if(typeStr == COnboardSurface::TypeString)
	{
		return TY_NODE_SURFACEAREAS_AREAS;
	}
	else if(typeStr == COnBoardWall::TypeString)
	{
		return TY_NODE_SURFACEAREAS_WALLS;
	}
	else if(typeStr == COnboardPortal::TypeString)
	{
		return TY_NODE_SURFACEAREAS_PORTALS;
	}
	else if(typeStr == COnboardCorridor::TypeString)
	{
		return TY_NODE_PLACEMENTS_CORRIDOR;
	}	
	else
	{
		ASSERT(0);
	}
	return TY_NODE_UNKNOW;
}
void CAircraftLayoutTreeView::OnOnboardobjectLock()
{
	// TODO: Add your command handler code here
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CAircraftElmentShapeDisplayList vlist = GetItemElementsList(hItemSelected);
	if(vlist.Count())
	{
		CAircraftElmentShapeDisplay* pFirst  = vlist.GetItem(0);
		BOOL bLock = pFirst->IsLocked();
		for(int i=0;i<vlist.Count();i++)
			vlist.GetItem(i)->Lock(!bLock);
	}
}

void CAircraftLayoutTreeView::OnUpdateOnboardobjectLock(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code hereHTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CAircraftElmentShapeDisplayList vlist = GetItemElementsList(hItemSelected);
	if(vlist.Count())
	{
		CAircraftElmentShapeDisplay* pFirst  = vlist.GetItem(0);
		BOOL bLock = pFirst->IsLocked();
		if(bLock)
		{
			pCmdUI->SetText("UnLock");
		}
		else
		{
			pCmdUI->SetText("Lock");
		}
	}
}
