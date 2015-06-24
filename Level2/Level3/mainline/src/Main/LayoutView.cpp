// LayoutView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LayoutView.h"
#include "../InputOnBoard/Deck.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "XPStyle/NewMenu.h"
// CLayoutView
#include "TermPlanDoc.h"
#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/CInputOnboard.h"
#include "../InputOnBoard/AircraftLayoutEditContext.h"
#include "DlgDeckProperties.h"
#include "DlgVisibleRegions.h"
#include "OnBoard/OnboardViewMsg.h"

#include "../InputOnboard/Seat.h"
#include "../InputOnboard/AircaftLayOut.h"
#include "../InputOnboard/AircraftPlacements.h"
#include ".\layoutview.h"

#include "OnBoardCabinDividerDlg.h"
#include "OnBoardStorageDlg.h"
#include "OnBoardDoorDlg.h"
#include "OnBoardSeatDlg.h"
#include "DlgAircraftConfigurationSpecView.h"
#include "OnBoardVerticalDeviceDlg.h"
#include "../InputOnboard/OnBoardBaseVerticalDevice.h"
#include "onboard/AircraftLayoutEditor.h"

IMPLEMENT_DYNCREATE(CLayoutView, CView)

CLayoutView::CLayoutView():m_layout(NULL)
{
}

CLayoutView::~CLayoutView()
{
	for (int i = 0 ; i < (int)m_NodeData.size() ; i++)
	{
		delete m_NodeData[i] ;
	}
	m_NodeData.clear() ;
}
CARCBaseTree& CLayoutView::GetTreeCtrl()
{
	return m_wndTreeCtrl;
}
BEGIN_MESSAGE_MAP(CLayoutView, CView)
	ON_WM_CREATE() 
	ON_WM_SIZE() 
	//ON_MESSAGE(ON_REFRESH_LAYOUTTREE,OnReFeshTree)
	ON_WM_CONTEXTMENU() 
	ON_COMMAND(ID_EDITDECKS,OnEditDeck)
	ON_COMMAND(ID_DECK_COMMENT,OnDeckComment)
	ON_COMMAND(ID_DECK_HELP,OnDeckHelp)
	ON_COMMAND(ID_DECK_PROPERTIES,OnDeckProperies)

	ON_COMMAND(ID_DECK_ACTIVATE,OnDeckActive)
	ON_COMMAND(ID_DECK_ISOLATE,OnIsolate)
	ON_COMMAND(ID_DECK_HIDE,OnDeckHideOrShow)
	ON_COMMAND(ID_DECK_GIRDON,OnDeckGridOn)
	ON_COMMAND(ID_DECK_COMPOSITION_THICKNESSON,OnIfThickness)
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
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()
void CLayoutView::OnEditDeck()
{
	if(m_layout == NULL)
	{
		MessageBox(_T("Please Select one AircraftLayout!"),NULL,MB_OK) ;
		return ;
	}
    CDlgAircraftConfigurationSpecView DlgEdit(m_layout ,&((CTermPlanDoc *)GetDocument())->GetTerminal(),(CTermPlanDoc *)GetDocument(),this) ;
	DlgEdit.DoModal() ;
}
void CLayoutView::OnDeckComment()
{

}
void CLayoutView::OnDeckHelp()
{

}
CDeck* CLayoutView::GetCurrentSelDeck()
{
	if(m_hRightClkItem == NULL)
		return NULL;
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return NULL;
	CDeck* _deck = (CDeck*)nodeData->m_wData ; 
	return _deck ;
}
void CLayoutView::OnDeckActive()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
	{
		CDeckManager* deckManger = m_layout->GetDeckManager() ;
		deckManger->ActiveDeck(_deck);
		//_deck->IsActive(_deck->IsActive()?FALSE:TRUE) ;
		GetDocument()->UpdateAllViews(this , VM_ONBOARD_ACTIVE_DECK_CHANGE);
	}
}
void CLayoutView::OnIsolate()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
		_deck->IsVisible(TRUE) ;
	CDeck* otherdeck = NULL ;
	CDeckManager* pManger =  m_layout->GetDeckManager() ;
   for(int i = 0 ; i < pManger->getCount() ;i++)
   {
	   otherdeck = pManger->getItem(i) ;
	   if(otherdeck != _deck)
		   otherdeck->IsVisible(FALSE) ;
   }
	
}
void CLayoutView::OnDeckHideOrShow()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL){
		_deck->IsVisible(_deck->IsVisible()?FALSE:TRUE) ;
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_DECK_HIDESHOW);
	}
}
void CLayoutView::OnDeckGridOn()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
		_deck->GetGrid()->bVisibility = (_deck->GetGrid()->bVisibility?FALSE:TRUE) ;
}
void CLayoutView::OnIfThickness()
{
	CDeck* _deck = GetCurrentSelDeck() ;
	if(_deck != NULL)
		_deck->IsOpaque(_deck->IsOpaque()?FALSE:TRUE) ;
}
void CLayoutView::OnVisibLeregions()
{
   CDlgDeckVisibleRegions dlg(GetCurrentSelDeck(),TRUE,(CTermPlanDoc *)GetDocument(),this) ;
   dlg.DoModal() ;
}
void CLayoutView::OnInvisibLeregion()
{
	CDlgDeckVisibleRegions dlg(GetCurrentSelDeck(),FALSE,(CTermPlanDoc *)GetDocument(),this) ;
	dlg.DoModal() ;
}
void CLayoutView::OnComment()
{

}
void CLayoutView::OnReFeshTree()
{
   InitLayoutTree() ;
}
// CLayoutView drawing

void CLayoutView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

void CLayoutView::OnInitialUpdate()
{
	CView::OnInitialUpdate() ;
	m_layout = ((CTermPlanDoc *)GetDocument())->GetAircraftLayoutEditor()->GetEditLayOut();
	InitLayoutTree() ;
}
void CLayoutView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType,cx,cy) ;
	if(m_wndTreeCtrl.GetSafeHwnd() != NULL)
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}
int CLayoutView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CView::OnCreate(lpcs) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS ;

	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0),
		this, IDC_LAYOUTVIEW_TREE);

	return (bResult ? 0 : -1);
}
void CLayoutView::InitLayoutTree()
{
	m_mapPlacementNode.clear();
	m_mapPlacementRoot.clear();

	for (int i = 0 ; i < (int)m_NodeData.size() ; i++)
	{
		delete m_NodeData[i] ;
	}
	m_NodeData.clear() ;

	m_wndTreeCtrl.DeleteAllItems();


	if(m_layout == NULL)
		return ;
	m_wndTreeCtrl.DeleteAllItems() ;
	CLayoutNodeData* pLayoutNodeData = new CLayoutNodeData ;
	pLayoutNodeData->m_Ty = TY_NODE_LAYOUT ;
	pLayoutNodeData->m_wData = m_layout ;
	m_NodeData.push_back(pLayoutNodeData) ;
    
	HTREEITEM hLayoutNode = AddItem(_T("Layout")) ;
	m_wndTreeCtrl.SetItemData(hLayoutNode,(DWORD_PTR)pLayoutNodeData) ;
	CDeckManager* deckManger = m_layout->GetDeckManager() ;
    LoadDecks(deckManger,hLayoutNode) ;
	

	//

	LoadVerticalDevices(hLayoutNode);
	LoadSurfaceAreas(hLayoutNode);
    GetTreeCtrl().Expand(hLayoutNode,TVE_EXPAND) ;
	LoadProcessorBehavior(TVI_ROOT);
	LoadPlacement(hLayoutNode);
}
void CLayoutView::LoadDecks(CDeckManager* _manger , HTREEITEM _item)
{
	if(_manger == NULL)
		return ;
	CLayoutNodeData* pDeckNodeData = new CLayoutNodeData ;
	pDeckNodeData->m_Ty = TY_NODE_DECKS ;
	pDeckNodeData->m_wData = NULL ;
	pDeckNodeData->m_resouceID = IDR_MENU_ONBOARD_LAYOUT_DECKEDIT ; 
	m_NodeData.push_back(pDeckNodeData) ;

	HTREEITEM hDecksNode = AddItem(_T("Decks"),_item,TVI_FIRST) ;
	m_wndTreeCtrl.SetItemData(hDecksNode,(DWORD_PTR)pDeckNodeData) ;
	m_mapPlacementRoot[TY_NODE_DECKS] = hDecksNode;
	for (int i = 0 ; i < _manger->getCount();i++)
	{
		AddDeckNodeTree(_manger->getItem(i),i+1,hDecksNode) ;
	}
	m_wndTreeCtrl.Expand(hDecksNode,TVE_EXPAND) ;
}
void CLayoutView::AddDeckNodeTree(CDeck* _deck,int index,HTREEITEM _item)
{
	if(_deck == NULL)
		return ;
   CString str ;
   str.Format(_T("Deck %d "),index) ;
   str.Append(_deck->GetName()) ;
   HTREEITEM deckNode = AddItem(str,_item) ;
   CLayoutNodeData* NodeData = new CLayoutNodeData ;
   NodeData->m_Ty = TY_NODE_DECK ;
   NodeData->m_wData = _deck ;
   NodeData->m_resouceID = IDR_MENU_ONBOARD_LAYOUT_DECK ;
   m_NodeData.push_back(NodeData) ;

   m_wndTreeCtrl.SetItemData(deckNode,(DWORD_PTR)NodeData) ;
}
HTREEITEM CLayoutView::AddItem(const CString& strNode,HTREEITEM hParent,HTREEITEM hInsertAfter,int nImage ,int nSelectImage )
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
void CLayoutView::OnContextMenu(CWnd* pWnd, CPoint point)
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
	  if(nodedata->m_resouceID != 0)
	  {
		  menu.LoadMenu(nodedata->m_resouceID);
		  pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
	  }
  }
  if(pCtxMenu != NULL)
	  pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , point.x, point.y, AfxGetMainWnd()) ;
}
void CLayoutView::OnUpdateBarShowShowHide(CCmdUI* pCmdUI) 
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
void CLayoutView::OnUpdateBarGridOnOff(CCmdUI* pCmdUI) 
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
void CLayoutView::OnUpdateActiveItem(CCmdUI* pCmdUI)
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
void CLayoutView::OnUpdateThicknessOnOff(CCmdUI* pCmdUI)
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
void CLayoutView::AssertValid() const
{
	CView::AssertValid();
}

void CLayoutView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG
void CLayoutView::OnDeckProperies()
{
	if(m_hRightClkItem == NULL)
		return ;
	CLayoutNodeData* nodeData = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(m_hRightClkItem) ;
	if(nodeData == NULL)
		return ;
	CDeck* _deck = (CDeck*)nodeData->m_wData ;
	if(_deck == NULL)
		return ;
	CDlgDeckProperties dlgDeckPro(_deck,(CTermPlanDoc*) (GetParentFrame()->GetActiveDocument())) ;
	dlgDeckPro.DoModal() ;
	GetDocument()->UpdateAllViews(this, VM_ONBOARD_DECKMAPCHANGE, (CObject*)_deck);
}


// CLayoutView message handlers
void CLayoutView::LoadPlacement( HTREEITEM hRootItem )
{
	CString str  = _T("Placements");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData) ;
	m_NodeData.push_back(NodeData);
	
	

	LoadSeat(hPlacementNode);
	LoadStorage(hPlacementNode);
	LoadCabinDivider(hPlacementNode);
	LoadGalleys(hPlacementNode);
	LoadDoors(hPlacementNode);
	LoadEmergencyExits(hPlacementNode);

	std::vector<CAircraftElement* > vElements = m_layout->GetPlacements()->GetAllElements();
	for (int nElement = 0; nElement < static_cast<int>(vElements.size()); ++ nElement)
	{
		AddPlacement(vElements[nElement]);
	}

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CLayoutView::LoadSeat( HTREEITEM hRootItem )
{
	//seat node root

	CString str  = _T("Seats");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_SEATS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_SEATS] = hPlacementNode;
	m_NodeData.push_back(NodeData);


	//CSea* deckManger = m_layout->GetPlacements()->GetSeatManager() ;
	//add seat into tree
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CLayoutView::LoadStorage( HTREEITEM hRootItem )
{
	CString str  = _T("Storage");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_STORAGE ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_STORAGE] = hPlacementNode;
	m_NodeData.push_back(NodeData);




	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);

}

void CLayoutView::LoadCabinDivider( HTREEITEM hRootItem )
{
	CString str  = _T("Cabin Dividers");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_CABINDIVIDER ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_CABINDIVIDER] = hPlacementNode;
	m_NodeData.push_back(NodeData);




	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);




}

void CLayoutView::LoadGalleys( HTREEITEM hRootItem )
{
	CString str  = _T("Galleys");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_GALLEYS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_GALLEYS] = hPlacementNode;
	m_NodeData.push_back(NodeData);



	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CLayoutView::LoadDoors( HTREEITEM hRootItem )
{
	CString str  = _T("Doors");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_DOORS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_DOORS] = hPlacementNode;
	m_NodeData.push_back(NodeData);






	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CLayoutView::LoadEmergencyExits( HTREEITEM hRootItem )
{
	CString str  = _T("Emergency Exits");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PLACEMENTS_EMERGENCYEXITS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_mapPlacementRoot[TY_NODE_PLACEMENTS_EMERGENCYEXITS] = hPlacementNode;
	m_NodeData.push_back(NodeData);









	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);

}

void CLayoutView::LoadVerticalDevices( HTREEITEM hRootItem )
{

	CString str  = _T("Vertical Devices");
	HTREEITEM hVerticalNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hVerticalNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);



	LoadStairs(hVerticalNode);
	LoadEscalator(hVerticalNode);
	LoadElevator(hVerticalNode);
	m_wndTreeCtrl.Expand(hVerticalNode,TVE_EXPAND);
}

void CLayoutView::LoadStairs( HTREEITEM hRootItem )
{
	CString str  = _T("Stairs");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES_STAIRS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);
	m_mapPlacementRoot[TY_NODE_VERTICALDEVICES_STAIRS] = hPlacementNode;




	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CLayoutView::LoadEscalator( HTREEITEM hRootItem )
{
	CString str  = _T("Escalator");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES_ESCALATOR ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);

	//
	m_mapPlacementRoot[TY_NODE_VERTICALDEVICES_ESCALATOR] = hPlacementNode;
	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
}

void CLayoutView::LoadElevator( HTREEITEM hRootItem )
{
	CString str  = _T("Elevator");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES_ELEVATOR ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);


	m_mapPlacementRoot[TY_NODE_VERTICALDEVICES_ELEVATOR] = hPlacementNode;

	m_wndTreeCtrl.Expand(hPlacementNode,TVE_EXPAND);
	m_NodeData.push_back(NodeData);
}

void CLayoutView::LoadSurfaceAreas( HTREEITEM hRootItem )
{
	CString str  = _T("Surface Areas");
	HTREEITEM hSurfaceNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_VERTICALDEVICES ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hSurfaceNode,(DWORD_PTR)NodeData);


	LoadWalls(hSurfaceNode);
	LoadAreas(hSurfaceNode);
	LoadPortals(hSurfaceNode);

	m_wndTreeCtrl.Expand(hSurfaceNode,TVE_EXPAND);
	m_NodeData.push_back(NodeData);
}

void CLayoutView::LoadWalls( HTREEITEM hRootItem )
{
	CString str  = _T("Walls");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS_WALLS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);


}

void CLayoutView::LoadAreas( HTREEITEM hRootItem )
{
	CString str  = _T("Areas");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS_AREAS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);


}

void CLayoutView::LoadPortals( HTREEITEM hRootItem )
{
	CString str  = _T("Portals");
	HTREEITEM hPlacementNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_SURFACEAREAS_PORTALS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hPlacementNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);


}

void CLayoutView::LoadProcessorBehavior( HTREEITEM hRootItem )
{
	CString str  = _T("Processor Behaviors");
	HTREEITEM hBehaviorsNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PROCESSORBEHAVIORS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hBehaviorsNode,(DWORD_PTR)NodeData);

	
	LoadBehaviorElevator(hBehaviorsNode);
	LoadBehaviorEscalator(hBehaviorsNode);
	m_wndTreeCtrl.Expand(hBehaviorsNode,TVE_EXPAND);
	m_NodeData.push_back(NodeData);
}

void CLayoutView::LoadBehaviorElevator( HTREEITEM hRootItem )
{
	CString str  = _T("Elevators");
	HTREEITEM hBehaviorNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* NodeData = new CLayoutNodeData ;
	NodeData->m_Ty = TY_NODE_PROCESSORBEHAVIORS_ELEVATORS ;
	NodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hBehaviorNode,(DWORD_PTR)NodeData);
	m_NodeData.push_back(NodeData);
}

void CLayoutView::LoadBehaviorEscalator( HTREEITEM hRootItem )
{
	CString str  = _T("Escalators");
	HTREEITEM hBehaviorNode = AddItem(str,hRootItem) ;
	CLayoutNodeData* pNodeData = new CLayoutNodeData ;
	pNodeData->m_Ty = TY_NODE_PROCESSORBEHAVIORS_ESCALATORS ;
	pNodeData->m_wData = NULL ;
	m_wndTreeCtrl.SetItemData(hBehaviorNode,(DWORD_PTR)pNodeData);
	m_NodeData.push_back(pNodeData);



}

void CLayoutView::AddPlacement( CAircraftElement *pElement )
{
	if (pElement == NULL)
	{
		ASSERT(pElement != NULL);
		return;
	}

	//get root
	HTREEITEM hRootItem = GetPlacementRoot(pElement);
	if(hRootItem == NULL)
		return;

	const ALTObjectID& objName = pElement->GetIDName();
	
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

				CLayoutNodeData* pNodeData = new CLayoutNodeData ;
				pNodeData->m_Ty = TY_NODE_PLACEMENTS_GROUP ;
				pNodeData->m_wData = NULL ;

				m_wndTreeCtrl.SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				hParentItem = hTreeItem;
				m_NodeData.push_back(pNodeData);
				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);

				CLayoutNodeData* pNodeData = new CLayoutNodeData ;
				pNodeData->m_Ty = TY_NODE_PLACEMENTS_DATA ;
				pNodeData->m_wData = pElement ;
				pNodeData->m_resouceID = IDR_MENU_ONBOARD_OBJECT;
				m_wndTreeCtrl.SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				m_wndTreeCtrl.SelectItem(hTreeItem);

				m_mapPlacementNode[(DWORD_PTR)pElement] = hTreeItem;
				m_NodeData.push_back(pNodeData);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);

			CLayoutNodeData* pNodeData = new CLayoutNodeData ;
			pNodeData->m_Ty = TY_NODE_PLACEMENTS_DATA ;
			pNodeData->m_wData = pElement ;
			m_wndTreeCtrl.SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
			m_wndTreeCtrl.SelectItem(hTreeItem);
			pNodeData->m_resouceID = IDR_MENU_ONBOARD_OBJECT;
			m_mapPlacementNode[(DWORD_PTR)pElement] = hTreeItem;
			m_NodeData.push_back(pNodeData);
			break;
		}
	}
}

HTREEITEM CLayoutView::FindChildNode(HTREEITEM hParentItem,const CString& strNodeText)
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
HTREEITEM CLayoutView::GetPlacementRoot( CAircraftElement *pElement)
{
	HTREEITEM hRootItem = NULL;

	if (pElement == NULL)
	{
		ASSERT(pElement != NULL);
		return hRootItem;
	}

	if(pElement->GetType() == CSeat::TypeString) //seat
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_PLACEMENTS_SEATS);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}
	else if(pElement->GetType() == CCabinDivider::TypeString)
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_PLACEMENTS_CABINDIVIDER);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}
	else if(pElement->GetType() == CStorage::TypeString)
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_PLACEMENTS_STORAGE);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}
	else if(pElement->GetType() == CDoor::TypeString)
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_PLACEMENTS_DOORS);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}
	else if(pElement->GetType() == COnBoardStair::TypeString)
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_VERTICALDEVICES_STAIRS);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}
	else if(pElement->GetType() == COnBoardEscalator::TypeString)
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_VERTICALDEVICES_ESCALATOR);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}
	else if(pElement->GetType() == COnBoardElevator::TypeString)
	{
		std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_VERTICALDEVICES_ELEVATOR);
		if(iter!= m_mapPlacementRoot.end())
		{
			hRootItem = (*iter).second;
		}
	}

	else
	{
		ASSERT(0);
	}
	
	return hRootItem;
}

void CLayoutView::UpdatePlacement( CAircraftElement *pElement )
{
	DeletePlacement(pElement);
	AddPlacement(pElement);
}

void CLayoutView::DeletePlacement( CAircraftElement *pElement )
{
	std::map<DWORD_PTR, HTREEITEM>::iterator iter = m_mapPlacementNode.find((DWORD_PTR)pElement);
	if(iter!= m_mapPlacementNode.end())
	{
		HTREEITEM hDeleteNode = (*iter).second;
		
		HTREEITEM hChildItem = hDeleteNode;

		for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
		{

			//if(GetTreeCtrl().GetNextSiblingItem(hChildItem) )
			{
				CLayoutNodeData *pNodeData = (CLayoutNodeData*)GetTreeCtrl().GetItemData(hChildItem);
				if(pNodeData->m_Ty == TY_NODE_PLACEMENTS_DATA  )
				{
					
					HTREEITEM hItem = GetTreeCtrl().GetParentItem(hChildItem);
					GetTreeCtrl().DeleteItem(hChildItem);
					hChildItem = hItem;
				}
				else if(pNodeData->m_Ty == TY_NODE_PLACEMENTS_GROUP)
				{
					HTREEITEM hItem = GetTreeCtrl().GetChildItem(hChildItem);
					if(hItem)
					{
						break;
					}
					hItem = GetTreeCtrl().GetParentItem(hChildItem);
					GetTreeCtrl().DeleteItem(hChildItem);
					hChildItem = hItem;
				}
				else
                    break;
			}
		//	else
		//	{
		//		CLayoutNodeData *pNodeData = (CLayoutNodeData*)GetTreeCtrl().GetItemData(hChildItem);
		//		if(pNodeData->m_Ty == TY_NODE_PLACEMENTS_DATA || pNodeData->m_Ty == TY_NODE_PLACEMENTS_GROUP)
		//		{
		//			HTREEITEM hItem = GetTreeCtrl().GetParentItem(hChildItem);
		//			GetTreeCtrl().DeleteItem(hChildItem);
		//			hChildItem = hItem;	
		//		}
		//		else if(pNodeData->m_Ty == TY_NODE_PLACEMENTS_GROUP)
		//		{
		//			HTREEITEM hItem = GetTreeCtrl().GetChildItem(hChildItem);
		//			if(hItem)
		//			{
		//				break;
		//			}
		//			GetTreeCtrl().DeleteItem(hChildItem);
		//			hChildItem = hItem;
		//		}
		//	}
		}
	}
}

void CLayoutView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class

	if(this == pSender)
		return;
	
	switch(lHint)
	{
	case VM_ONBOARD_ADD_AIRCRAFTELEMENT:
		{
			CAircraftElement *pElement = (CAircraftElement *)pHint;
			AddPlacement(pElement );
		}
		break;
	case VM_ONBOARD_DEL_AIRCRAFTELEMENT:
		{
			CAircraftElement *pElement = (CAircraftElement *)pHint;
			DeletePlacement(pElement );
		}
		break;

	case VM_ONBOARD_UPDATE_AIRCRAFTELEMENT:
		{
			CAircraftElement *pElement = (CAircraftElement *)pHint;
			UpdatePlacement(pElement );
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
	default:
		{

		}
		break;
	}

	CView::OnUpdate(pSender, lHint, pHint);
}
void CLayoutView::ReloadDeck()
{
   std::map<Nodetype, HTREEITEM>::iterator iter = m_mapPlacementRoot.find(TY_NODE_DECKS);
   HTREEITEM item = (*iter).second ;
   if(item == NULL)
	   return ;
   HTREEITEM PLayoutItem = GetTreeCtrl().GetParentItem(item) ;
   GetTreeCtrl().DeleteItem(item) ;
    LoadDecks(m_layout->GetDeckManager(),PLayoutItem) ;
}
void CLayoutView::OnOnBoardObjectProperties()
{
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CLayoutNodeData *pNodeData = (CLayoutNodeData*)GetTreeCtrl().GetItemData(hItemSelected);
	if(pNodeData->m_Ty != TY_NODE_PLACEMENTS_DATA  )
		return;

	CAircraftElement *pElement = (CAircraftElement *)pNodeData->m_wData;
	if(pElement == NULL)
		return;
	
	INT_PTR bOperation;
	if(pElement->GetType() == CSeat::TypeString)
	{
		COnBoardSeatDlg dlg((CSeat *)pElement,pElement->GetLayOut(),((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();
	}
	else if(pElement->GetType() == CCabinDivider::TypeString)
	{
		COnBoardCabinDividerDlg dlg((CCabinDivider *)pElement,pElement->GetLayOut(),((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();
	}
	else if (pElement->GetType() == CStorage::TypeString)
	{
		COnBoardStorageDlg dlg((CStorage *)pElement,pElement->GetLayOut(),((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();
	}
	else if (pElement->GetType() == CDoor::TypeString)
	{	
		COnBoardDoorDlg dlg((CDoor *)pElement,pElement->GetLayOut(),((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();
	}
	else if(pElement->GetType() == COnBoardStair::TypeString)
	{
		COnBoardVerticalDeviceDlg dlg((COnBoardStair *)pElement,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();

	}
	else if (pElement->GetType() == COnBoardElevator::TypeString)
	{
		COnBoardVerticalDeviceDlg dlg((COnBoardElevator *)pElement,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();
	}
	else if(pElement->GetType() == COnBoardEscalator::TypeString)
	{
		COnBoardVerticalDeviceDlg dlg((COnBoardElevator *)pElement,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
		bOperation = dlg.DoModal();

	}
	else
	{
		ASSERT(0);
	}

	if (bOperation == IDOK)
	{
		GetDocument()->UpdateAllViews(this,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT,(CObject*)pElement);
	}

}

void CLayoutView::OnOnBoardObjectDisplayProperties()
{	
	// TODO: Add your command handler code here
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CLayoutNodeData *pNodeData = (CLayoutNodeData*)GetTreeCtrl().GetItemData(hItemSelected);
	if(pNodeData->m_Ty != TY_NODE_PLACEMENTS_DATA  )
		return;

	CAircraftElement *pElement = (CAircraftElement *)pNodeData->m_wData;
	if(pElement == NULL)
		return;
	GetDocument()->UpdateAllViews(this,VM_ONBOARD_AIRCRAFTELEMENT_DISPLAYPROPERTY,(CObject*)pElement);

}

void CLayoutView::OnOnBoardObjectDelete()
{
	// TODO: Add your command handler code here
	HTREEITEM hItemSelected = m_wndTreeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CLayoutNodeData *pNodeData = (CLayoutNodeData*)GetTreeCtrl().GetItemData(hItemSelected);
	if(pNodeData->m_Ty != TY_NODE_PLACEMENTS_DATA  )
		return;

	CAircraftElement *pElement = (CAircraftElement *)pNodeData->m_wData;
	if(pElement == NULL)
		return;
	GetDocument()->UpdateAllViews(this,VM_ONBOARD_DEL_AIRCRAFTELEMENT,(CObject*)pElement);


	DeletePlacement(pElement);
	m_layout->GetPlacements()->DeleteElement(pElement);
	pElement->DeleteData();

}

void CLayoutView::OnOnBoardBbjectComment()
{
	// TODO: Add your command handler code here
}

void CLayoutView::OnOnBoardBbjectHelp()
{
	// TODO: Add your command handler code here
}




void CLayoutView::OnLButtonDblClk(UINT nFlags, CPoint point)
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
			else if(pNodedata->m_Ty == TY_NODE_VERTICALDEVICES_STAIRS)
			{
				COnBoardStair *pStair = new COnBoardStair;
				COnBoardVerticalDeviceDlg dlg(pStair,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
				if(dlg.DoModal() == IDOK)
				{
					m_layout->GetPlacements()->GetVerticalDeviceList().AddNewItem(pStair);
					AddPlacement(pStair);
				}
				else
				{
					delete pStair;
				}
			}
			else if(pNodedata->m_Ty == TY_NODE_VERTICALDEVICES_ESCALATOR)
			{
				COnBoardEscalator *pEscalator = new COnBoardEscalator;
				COnBoardVerticalDeviceDlg dlg(pEscalator,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
				if(dlg.DoModal() == IDOK)
				{
					m_layout->GetPlacements()->GetVerticalDeviceList().AddNewItem(pEscalator);
					AddPlacement(pEscalator);

				}
				else
				{
					delete pEscalator;
				}
			}
			else if(pNodedata->m_Ty == TY_NODE_VERTICALDEVICES_ELEVATOR)
			{
				COnBoardElevator *pElevator = new COnBoardElevator;
				COnBoardVerticalDeviceDlg dlg(pElevator,m_layout,((CTermPlanDoc *)GetDocument())->GetProjectID(),this);
				if(dlg.DoModal() == IDOK)
				{
					m_layout->GetPlacements()->GetVerticalDeviceList().AddNewItem(pElevator);
					AddPlacement(pElevator);
				}
				else
				{
					delete pElevator;
				}
			}


		}


	}
	CView::OnLButtonDblClk(nFlags, point);
}
void CLayoutView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	UINT uFlags;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(cliPt, &uFlags);
	if(hItem)
	{
		CLayoutNodeData* pNodedata = (CLayoutNodeData*)m_wndTreeCtrl.GetItemData(hItem) ;

		if(pNodedata != NULL)
		{

			if(pNodedata->m_Ty == TY_NODE_PLACEMENTS_DATA)//object
			{
				
				GetDocument()->UpdateAllViews(this,VM_ONBOARD_SELECT_AIRCRAFTELEMENT,(CObject *)pNodedata->m_wData);
			}
		}
	}
	CView::OnLButtonUp(nFlags, point);
}

void CLayoutView::ReloadSeat()
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

	std::vector<CAircraftElement* > vElements = m_layout->GetPlacements()->GetAllElements();
	for (int nElement = 0; nElement < static_cast<int>(vElements.size()); ++ nElement)
	{
		CAircraftElement *pElement = vElements[nElement];
		if(pElement->GetType() == CSeat::TypeString)
			AddPlacement(vElements[nElement]);
	}
}























