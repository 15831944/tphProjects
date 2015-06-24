// LandsideMSView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LandsideMSView.h"
#include "LandsideProcDlg.h"
#include "TermPlanDoc.h"
#include "LandsideProcDlg.h"
#include "DlgComments.h"
#include "SimEngSettingDlg.h"
#include "DlgVehicleLandSideSpecification.h"
#include "DlgVehicleOperation.h"
#include "SimEngineDlg.h"
#include "DlgGatArp.h"

#include "Landside/InputLandside.h"
#include "Landside/Landside.h"
#include "LandsideEditContext.h"
#include "ARCportLayoutEditor.h"
#include "MSVNodeData.h"
#include "ViewMsg.h"
#include "TreeCtrlItem.h"
#include "TreeItemLayoutObject.h"

#include "DlgLandSideDistribution.h"
#include "DlgCurbsideAssign.h"
#include "DlgParkinglotAssign.h"
#include "DlgODAssign.h"
#include "DlgVechiclProbDisp.h"
#include "FlightScheduleDlg.h"
#include "DlgLandsideVehicleTags.h"
//#include "DlgParkingBehaviors.h"
#include "DlgIntersectionTrafficCtrl.h"
#include "FltPaxDataDlg.h"
#include "landside/LandsidePortal.h"
#include "landside/LandsidePortals.h"
#include "DlgLandsideSpeedConstraint.h"
#include "DlgLayoutObjectDisplayProperties.h"
#include "DlgFlightDB.h"
#include "DlgDBAirports.h"
#include "DlgDBAirline.h"
#include "DlgDBAircraftTypes.h"
#include "DlgLandsideVehicleTypeDefine.h"
#include "DlgLandsideVehicleGroupProperty.h"
#include "DlgLandsideEntryOffset.h"
#include "DlgLaneChangeCriteria.h"
#include "DlgLandsideVehicleOperationPlan.h"
#include "DlgLandsideNonRelatedPlan.h"
#include "PaxTypeDefDlg.h"
#include "DlgLandsideVehicleAssignment.h"
#include "DlgParkingSpotSpec.h"
#include "PaxBulkListDlg.h"
#include "FacilityDataSheet.h"
#include "DlgSpeedControl.h"
#include "BmpBtnDropList.h"
#include "DlgPaxTags.h"
#include "PaxDispPropDlg.h"
#include "PaxDistDlg.h"
#include "DlgFloorPictureReference.h"
#include "../Renderer/RenderEngine/IRender3DView.h"
#include "DlgVehicleAssignmentNonPaxRelated.h"
#include "DlgInitCondition.h"
#include "DlgVehicleBehaviorDistritbution.h"
#include "DlgLaneAvailabilitySpecification.h"
#include "DlgVehicleOperationNonRelated.h"
#include "DlgNonPassengerRelatedVehicleAssignment.h"
#include "DlgCurbsideStrategy.h"
#include "DlgParkingLotStrategy.h"
#include "VehicleTypeDefDlg.h"
#include "DlgVehicleDistribution.h"
#include "DlgLandsideLayoutOptions.h"
#include "DlgLaneTollCriteria.h"

#define S_LAND_ENTRY_OFFSET _T("Land Entry Offset")
#define S_LAND_PAXRELATEDATA _T("Passenger related data")
#define S_LAND_SPEEDCONTROL _T("Speed Control")
#define S_SPEEDSIGN _T("Speed Sign")
#define S_TAXIQUEUE _T("Taxi Queues")
#define S_TAXIPOOL _T("Taxi Pools")
#define S_SERVICESTATIONS _T("Service Stations")

//#include "AirsideMSView.h"
using namespace MSV;

#define IDC_LANDSIDE_TREE 2
#define ID_NEW_ARP	(WM_USER + 0x0100)
#define ID_NEW_LNDSDPROC (ID_NEW_ARP+0x0100)
#define ID_LINKTO_TERMINALFLOOR 36000
// CLandsideMSView
///
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CLandsideMSView, CView)
CLandsideMSView::CLandsideMSView()
:m_pLandsideEntryOffsetDlg(NULL)
,m_pVehicleAssignmentNonPaxRelatedDlg(NULL)
,m_pSpeedControlDlg(NULL)
,m_pParkingSpotSpecDlg(NULL)
,m_pLaneAvailabilitySpecificationDlg(NULL)
,m_pLaneChangeCriteriaDlg(NULL)
,m_pIntersectionTrafficCtrlDlg(NULL)
,m_pCurbsideStrategyDlg(NULL)
,m_pParkingLotstrategyDlg(NULL)
,m_pLandsideVehicleOperationPlanDlg(NULL)
,m_pLandsideNonRelatedPlanDlg(NULL)
,m_pVehicleOperationNonRelatedDlg(NULL)
,m_FacilityDataSheet(NULL)
,m_pServiceTimeDlg(NULL)
,m_pInitConDitionDlg(NULL)
,m_pSimEngSettingDlg(NULL)
,m_pLaneTollGateDlg(NULL)
{
	m_hRightClkItem = NULL;
	m_hLevelRoot = NULL;
	m_hLayout = NULL;
	m_selItem = NULL;
	m_pNodeDataAllocator= new CNodeDataAllocator();
}


CLandsideMSView::~CLandsideMSView()
{
	if (m_pNodeDataAllocator != NULL)
	{
		delete m_pNodeDataAllocator;
	}
	if (m_pLandsideEntryOffsetDlg != NULL)
	{
		delete m_pLandsideEntryOffsetDlg;
	}
	if (m_pVehicleAssignmentNonPaxRelatedDlg != NULL)
	{
		delete m_pVehicleAssignmentNonPaxRelatedDlg;
	}
	if (m_pSpeedControlDlg != NULL)
	{
		delete m_pSpeedControlDlg;
	}
	if (m_pParkingSpotSpecDlg != NULL)
	{
		delete m_pParkingSpotSpecDlg;
	}		
	if (m_pLaneAvailabilitySpecificationDlg != NULL)
	{
		delete m_pLaneAvailabilitySpecificationDlg;
	}
	if (m_pLaneChangeCriteriaDlg != NULL)
	{
		delete m_pLaneChangeCriteriaDlg;
	}
	if (m_pIntersectionTrafficCtrlDlg != NULL)
	{
		delete m_pIntersectionTrafficCtrlDlg;
	}
	if (m_pCurbsideStrategyDlg != NULL)
	{
		delete m_pCurbsideStrategyDlg;
		m_pCurbsideStrategyDlg = NULL;
	}
	if (m_pParkingLotstrategyDlg != NULL)
	{
		delete m_pParkingLotstrategyDlg;
		m_pParkingLotstrategyDlg = NULL;
	}
	if (m_pLandsideVehicleOperationPlanDlg != NULL)
	{
		delete m_pLandsideVehicleOperationPlanDlg;
	}	
	if (m_FacilityDataSheet != NULL)
	{
		delete m_FacilityDataSheet;
	}
	if (m_pLandsideNonRelatedPlanDlg != NULL)
	{
		delete m_pLandsideNonRelatedPlanDlg;
	}
	if (m_pVehicleOperationNonRelatedDlg)
	{
		delete m_pVehicleOperationNonRelatedDlg;
		m_pVehicleOperationNonRelatedDlg = NULL;
	}
	if (m_pServiceTimeDlg != NULL)
	{
		delete m_pServiceTimeDlg;
	}
	if (m_pInitConDitionDlg != NULL)
	{
		delete m_pInitConDitionDlg;
	}
	if (m_pSimEngSettingDlg != NULL)
	{
		delete m_pSimEngSettingDlg;
	}
	if (m_pLaneTollGateDlg != NULL)
	{
		delete m_pLaneTollGateDlg;
		m_pLaneTollGateDlg = NULL;
	}
}

BEGIN_MESSAGE_MAP(CLandsideMSView, CView)
	ON_COMMAND(ID_CTX_RENAME, OnCtxRename)
	ON_MESSAGE(ITEMEXPANDING,OnItemExpanding)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(ID_NEW_LNDSDPROC + ALT_LSTRETCH, ID_NEW_LNDSDPROC + ALT_LTYPE_END -1, OnNewLandsideProc)
	ON_COMMAND(ID_CTX_LANDPROCPROPERTIES, OnCtxLandprocproperties)
	ON_COMMAND(ID_CTX_DELETEPROC,OnCtxDeleteGroupProc)
	ON_COMMAND(ID_CTX_LANDDELETEPROC, OnCtxLanddeleteproc)
	ON_COMMAND(ID_CTX_LAYOUTDISPPROPERTIES, OnCtxDisplayProp)
	ON_COMMAND(ID_CTX_LANDSIDE_NEWPORTAL,OnNewLandsidePortal)
	ON_COMMAND(ID_CTX_EDITPORTAL,OnEditLandsidePortal)
	ON_COMMAND(ID_CTX_DELETEPORTAL,OnDelLandsidePortal)	
	ON_COMMAND(ID_CTX_PORTALCOLOR,OnLandsidePortalColor)	

	ON_COMMAND(ID_CTX_LANDSIDE_NEW_AREA,OnNewLandsideArea)
	ON_COMMAND(ID_CTX_EDIT_LANDSIDE_AREA,OnEditLandsideArea)
	ON_COMMAND(ID_CTX_DELETE_LANDSIDE_AREA,OnDelLandsideArea)
	ON_COMMAND(ID_CTX_LANDSIDE_AREA_COLOR,OnLandsideAreaColor)

	ON_COMMAND(ID_CTX_FLOORPICTURE_INSERT, OnFloorPictureInsert)
	ON_COMMAND(ID_CTX_FLOORPICTURE_HIDE, OnFloorPictureHide)
	ON_COMMAND(ID_CTX_FLOORPICTURE_SHOW, OnFloorPictureShow)
	ON_COMMAND(ID_CTX_FLOORPICTURE_REMOVE,OnFloorPictureRemove)

	ON_COMMAND(ID_CTX_LAYOUT, OnSetLandsideLayoutOptions)

	ON_COMMAND_RANGE( ID_LINKTO_TERMINALFLOOR,ID_LINKTO_TERMINALFLOOR+10,OnLinkToTerminalFloors)
END_MESSAGE_MAP()


// CLandsideMSView diagnostics

#ifdef _DEBUG
void CLandsideMSView::AssertValid() const
{
	CView::AssertValid();
}

void CLandsideMSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTermPlanDoc* CLandsideMSView::GetDocument()  //non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*) m_pDocument;
}

#endif //_DEBUG

InputTerminal* CLandsideMSView::GetInputTerminal()
{
	return &GetDocument()->GetTerminal();
}

// CLandsideMSView message handlers

void CLandsideMSView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	if (m_imgList.m_hImageList == NULL)
	{
		m_imgList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
		CBitmap bmp;
		bmp.LoadBitmap(IDB_COOLTREE);
		m_imgList.Add(&bmp,RGB(255,0,255));
	}
	CBitmap bmL;

	bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
	m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_NODESMALLIMG);
	m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_FLOORSMALLIMG);
	m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_AREASSMALLIMG);
	m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_PORTALSSMALLIMG);
	m_imgList.Add(&bmL, RGB(255,0,255));

	GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
	GetTreeCtrl().SetImageList(&m_imgList,TVSIL_STATE);
  
	InitTree();
	SetScrollPos(SB_VERT,0);
}

int CLandsideMSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |TVS_EDITLABELS ;


	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0),
		this, IDC_LANDSIDE_TREE);

	return (bResult ? 0 : -1);

}

void CLandsideMSView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);

}


void CLandsideMSView::UpdatePopMenu(CCmdTarget* pThis, CMenu* pPopMenu)
{// added by aivin.
	if(pThis==NULL || pPopMenu==NULL)return;

	CCmdUI state;
	state.m_pMenu = pPopMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	state.m_nIndexMax = pPopMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pPopMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(pThis, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pThis, FALSE && state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pPopMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}

}

void CLandsideMSView::OnNewLandsideProc(UINT menuID)
{
	UINT nOffset = ID_NEW_LNDSDPROC;
	nOffset = menuID - nOffset;
	ALTOBJECT_TYPE enumProcType = (ALTOBJECT_TYPE) nOffset ;

	CTermPlanDoc* pDoc = GetDocument();	
	InputLandside* pInput = pDoc->getARCport()->m_pInputLandside;	
// 	if (enumProcType==ALT_LPORTAL)
// 	{
// 		GetEditContext()->OnCtxNewLandsidePortal();
// 	}else
// 	{
		pDoc->GetLayoutEditor()->OnNewLandsideProc(enumProcType,this);
// 	}
}
//
//void CLandsideMSView::AddPortals(CLandsidePortals &portals)
//{
//	for (int i=0;i<portals.GetItemCount();i++)
//	{
//		AddPortalItem(portals.GetItem(i));
//	}
//}
//void CLandsideMSView::AddPortalItem(CLandsidePortal *portal)
//{	
//	COOLTREE_NODE_INFO cni;		
//	CARCBaseTree::InitNodeInfo(GetTreeCtrl().GetParent(),cni);
//	cni.nImage = 0 + ID_IMAGE_COUNT;
//	cni.nImageSeled = 0 + ID_IMAGE_COUNT;
//	CNodeData* pData = m_pNodeDataAllocator->allocateNodeData(NodeType_Normal);
//	pData->nIDResource=IDR_CTX_PORTAL;
//	pData->dwData=(DWORD)portal;
//	HTREEITEM hPortal=GetTreeCtrl().InsertItem(portal->name,cni,FALSE,FALSE,m_hPortalRoot);
//	GetTreeCtrl().SetItemData(hPortal,(DWORD)pData);
//}

//void CLandsideMSView::EditPortalItem(CLandsidePortal *portal)
//{
//	HTREEITEM hPortal=GetTreeCtrl().GetChildItem(m_hPortalRoot);
//	while(hPortal)
//	{
//		CNodeData* pNode = (CNodeData*)GetTreeCtrl().GetItemData(hPortal);
//		if(pNode)
//		{
//			if (portal == (CLandsidePortal *)pNode->dwData)
//			{
//                GetTreeCtrl().SetItemText(hPortal,portal->name);
//				return;
//			}
//		}
//		hPortal=GetTreeCtrl().GetNextItem(hPortal,TVGN_NEXT);
//	}
//}

//void CLandsideMSView::DelPortalItem(CLandsidePortal *portal)
//{
//	HTREEITEM hPortal=GetTreeCtrl().GetChildItem(m_hPortalRoot);
//	while(hPortal)
//	{
//		CNodeData* pNode = (CNodeData*)GetTreeCtrl().GetItemData(hPortal);
//		if(pNode)
//		{
//			if (portal == (CLandsidePortal *)pNode->dwData)
//			{
//				GetTreeCtrl().DeleteItem(hPortal);
//				return;
//			}
//		}
//		hPortal=GetTreeCtrl().GetNextItem(hPortal,TVGN_NEXT);
//
//
//	}
//}

void CLandsideMSView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

BOOL CLandsideMSView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CView::PreCreateWindow(cs))
		return FALSE;

	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS|WS_CLIPCHILDREN;

	return TRUE;
}

void CLandsideMSView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (GetDocument()->m_systemMode==EnvMode_LandSide)
	{			
		switch(lHint)
		{
		case VM_NEW_ONE_FLOOE :
			NewLevelItem( GetDocument()->GetFloorByMode(EnvMode_LandSide) , (int)pHint );
			break;
		case VM_DELETE_ONE_FLOOR:
			DeleteLevelItem( GetDocument()->GetFloorByMode(EnvMode_LandSide) , (int)pHint );
			break;
		case VM_ACTIVE_ONE_FLOOR:		
			{
				CRenderFloor* pFloor = (CRenderFloor*)pHint;
				if(pFloor)			
					ActiveLevelItem(pFloor->Level());
			}
			break;
		case VM_UPDATE_COMMAND:

			{
				IEnvModeEditOperation* pCmd = (IEnvModeEditOperation*)pHint;
				if(ILandsideEditCommand* plandsideOp  = pCmd->toLandside())
				{
					OnCommandUpdate( (ILandsideEditCommand*)pHint);
				}
			}
			break;
		}
	}

	CView::OnUpdate(pSender, lHint, pHint);
	/////////////////////////////////////////////////
	//send message to tree ctrl to make h_scroolbar scroll to end_left 	
	GetTreeCtrl().SendMessage( WM_HSCROLL, MAKEWPARAM(SB_LEFT,0),(LPARAM)NULL );
}

LRESULT CLandsideMSView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		OnLButtonDblClk(wParam,lParam);
		break;
	case ENDLABELEDIT:
		OnEndLabelEdit(wParam,lParam);
		break;
	case BEGINLABELEDIT:
		OnBeginLabelEdit(wParam,lParam);
		break;
	case SELCHANGED:
		OnSelChanged(wParam,lParam);
		break;
	default:
		break;
	}
	return CView::WindowProc(message, wParam, lParam);	
}

void CLandsideMSView::PumpMessages()
{
	ASSERT(m_hWnd!=NULL);
	MSG msg;
	// Handle dialog messages
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
	}
}

bool CLandsideMSView::VersionCheck()
{
	return true;
}



void CLandsideMSView::RenameSelected()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if(hItem != NULL)
	{
		GetTreeCtrl().EditLabel(hItem);
	}
}

void CLandsideMSView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Send WM_CONTEXTMENU to self
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	// Mark message as handled and suppress default handling
	*pResult = 1;
}

void CLandsideMSView::OnCtxRename() 
{
	HTREEITEM hItem = m_hRightClkItem;
	if (hItem != NULL)
		GetTreeCtrl().EditLabel(hItem);
}


LRESULT CLandsideMSView::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
	HTREEITEM hItem = NULL;		
	hItem = pNMTreeView->itemNew.hItem;
	m_selItem = hItem;

	CTreeItemLayoutObject itemNode(&GetTreeCtrl(),hItem);
	
	if( itemNode.getNodeType() != NodeType_Object )
		return 0;	
	
	ILayoutObject* pobj=  itemNode.getObject();
	
	if(pobj)
	{
		GetEditContext()->selectObject(pobj);
		GetDocument()->UpdateAllViews(this, VM_SELECT_LAYOUTOBJECT,(CObject*) pobj);
	}	
	return 0;
}

LRESULT CLandsideMSView::OnItemExpanding(WPARAM wParam, LPARAM lParam) 
{	
	return 0;
}

void CLandsideMSView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);

	//InputTerminal* pInTerm = (InputTerminal*)&GetDocument()->GetTerminal();
	// ?was a node clicked?
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	if((hItem != NULL)&& (TVHT_ONITEM & uFlags))
	{
		CNodeData* pNode = (CNodeData*)GetTreeCtrl().GetItemData(hItem);
		SelectNode(pNode);
	}	
}

void CLandsideMSView::OnCtxComments()
{
	
}

void CLandsideMSView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0 );

	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );
}
void CLandsideMSView::OnNewLandsidePortal()
{
	GetEditContext()->OnCtxNewLandsidePortal();
}
void CLandsideMSView::OnEditLandsidePortal()
{
	CTreeItemLayoutObject node(&GetTreeCtrl(),m_selItem);
	if(!node.IsValid())
		return;
	if(LandsideLayoutObject* pObj = node.getObject())
	{
		if( pObj->GetType() == ALT_LPORTAL)
		{
			GetEditContext()->OnCtxEditLandsidePortal((CLandsidePortal*)pObj);
		}
	}
}
void CLandsideMSView::OnDelLandsidePortal()
{
	CTreeItemLayoutObject node(&GetTreeCtrl(),m_selItem);
	if(!node.IsValid())
		return;

	if(LandsideLayoutObject* pObj = node.getObject())
	{
		if( pObj->GetType() == ALT_LPORTAL)
		{
			GetEditContext()->OnCtxDelLandsidePortal((CLandsidePortal*)pObj);
		}
	}
}

void CLandsideMSView::OnLandsidePortalColor()
{
	CTreeItemLayoutObject node(&GetTreeCtrl(),m_selItem);
	if(!node.IsValid())
		return;

	if(LandsideLayoutObject* pObj = node.getObject())
	{
		if( pObj->GetType() == ALT_LPORTAL)
		{
			GetEditContext()->OnCtxLandsidePortalColor((CLandsidePortal*)pObj);
		}
	}
}

void CLandsideMSView::DeleteFirstChild(HTREEITEM hItem)
{
	HTREEITEM hChild;
	if((hChild = GetTreeCtrl().GetChildItem(hItem)) != NULL)
		GetTreeCtrl().DeleteItem(hChild);
}

void CLandsideMSView::DeleteAllChildren(HTREEITEM hItem)
{
	HTREEITEM hChild;
	if((hChild = GetTreeCtrl().GetChildItem(hItem)) == NULL)
		return;

	GetDocument()->m_bCanResposeEvent = false;	

	do
	{
		HTREEITEM hNext = GetTreeCtrl().GetNextSiblingItem(hChild);
		GetTreeCtrl().DeleteItem(hChild);
		hChild = hNext;
	} while(hChild != NULL);

	GetDocument()->m_bCanResposeEvent = true;
}

void CLandsideMSView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	InputTerminal* pInTerm = (InputTerminal*)&GetDocument()->GetTerminal();
	// ?was a node clicked?
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	CString strNodeName;
	// then expand the item
	//CView::OnLButtonDblClk(nFlags, point);
	//////////////////////////////////////////////////////////////////////////
	if((hItem != NULL)&& (TVHT_ONITEM & uFlags))
	{
		//CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( hItem);
		CTreeItemMSVNode msvNode(&GetTreeCtrl(),hItem);
        strNodeName= msvNode.GetText();
		CString s;	
		s.LoadString(IDS_TVNN_SIMSETTINGS);
		if(s.Compare(strNodeName)==0)
		{
			// run simulation setting
			if (!GetInputTerminal()->flightSchedule->getCount())
			{
				AfxMessageBox(_T("Please define flight schedule first."));
				return;
			}
			if (m_pSimEngSettingDlg != NULL)
			{
				delete m_pSimEngSettingDlg;
			}
			m_pSimEngSettingDlg = new CSimEngSettingDlg(GetInputTerminal()->m_pSimParam, this);
			m_pSimEngSettingDlg->Create(CSimEngSettingDlg::IDD);
			m_pSimEngSettingDlg->CenterWindow();
			m_pSimEngSettingDlg->ShowWindow(SW_SHOWNORMAL);
		}//end if compare

		s.LoadString(IDS_TVNN_RUNSIM);
		if(s.Compare(strNodeName)==0)
			theApp.GetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_RUNSIM);

		s.LoadString(IDS_TVNN_SCHEDULE);
		if (s == strNodeName)
		{	
			CFlightScheduleDlg dlg( this );
			dlg.DoModal();
			return;
		}


		if (strNodeName.Compare(S_LAND_SPEEDCONTROL)==0)
		{
			CTermPlanDoc* pDoc=(CTermPlanDoc*)GetDocument();
			if (m_pSpeedControlDlg != NULL)
			{
				delete m_pSpeedControlDlg;
			}
			m_pSpeedControlDlg = new CDlgSpeedControl(pDoc->GetInputLandside(),this);
			m_pSpeedControlDlg->Create(CDlgSpeedControl::IDD);
			m_pSpeedControlDlg->CenterWindow();
			m_pSpeedControlDlg->ShowWindow(SW_SHOWNORMAL);
		}
		

		if (strNodeName.Compare("Intersection Traffic management")==0)
		{
			CTermPlanDoc *pDoc=(CTermPlanDoc*)GetDocument();
			if (m_pIntersectionTrafficCtrlDlg != NULL)
			{
				delete m_pIntersectionTrafficCtrlDlg;
			}
			m_pIntersectionTrafficCtrlDlg = new CDlgIntersectionTrafficControlManagement(pDoc,this);
			m_pIntersectionTrafficCtrlDlg->Create(CDlgIntersectionTrafficControlManagement::IDD);
			m_pIntersectionTrafficCtrlDlg->CenterWindow();
			m_pIntersectionTrafficCtrlDlg->ShowWindow(SW_SHOWNORMAL);
		}

		if (strNodeName.Compare(_T("Curbside Strategies")) == 0)
		{
			if (m_pCurbsideStrategyDlg)
			{
				m_pCurbsideStrategyDlg->DestroyWindow();
				delete m_pCurbsideStrategyDlg;
				m_pCurbsideStrategyDlg = NULL;
			}

			m_pCurbsideStrategyDlg = new CDlgCurbsideStrategy(this);
			m_pCurbsideStrategyDlg->Create(CDlgLandsideStrategy::IDD);
			m_pCurbsideStrategyDlg->CenterWindow();
			m_pCurbsideStrategyDlg->ShowWindow(SW_SHOWNORMAL);
		}

		if (strNodeName.Compare(_T("Parking Lot Strategies")) == 0)
		{
			if (m_pParkingLotstrategyDlg)
			{
				delete m_pParkingLotstrategyDlg;
				m_pParkingLotstrategyDlg = NULL;
			}

			m_pParkingLotstrategyDlg = new CDlgParkingLotStrategy(this);
			m_pParkingLotstrategyDlg->Create(CDlgLandsideStrategy::IDD);
			m_pParkingLotstrategyDlg->CenterWindow();
			m_pParkingLotstrategyDlg->ShowWindow(SW_SHOWNORMAL);
		}

		if (strNodeName.Compare("Vehicle Display")==0)
		{
			CDlgVechiclProbDisp dlg(this);
			dlg.DoModal();
		}
		if (strNodeName.Compare("Vehicle Tags") == 0)
		{
			CDlgLandsideVehicleTags dlg(this);
			dlg.DoModal();
		}
		if (strNodeName.Compare("Parking constraints") == 0)
		{
			//CTermPlanDoc *pDoc=(CTermPlanDoc*)GetDocument();
			////CDlgParkingBehaviors dlg(pDoc,this);
			//dlg.DoModal();
		}
		s.LoadString(IDS_TVNN_LOADFACTORS);
		if(s.Compare(strNodeName)==0)
		{
			CFltPaxDataDlg dlg( FLIGHT_LOAD_FACTORS, this);
			dlg.DoModal();
		}
		s.LoadString(IDS_TVNN_ACCAPACITIES);
		if(s.Compare(strNodeName)==0)
		{
			CFltPaxDataDlg dlg(FLIGHT_AC_CAPACITIES, this);
			dlg.DoModal();
		}

		//Flight/Groups
		s.LoadString(IDS_TVNN_FLIGHT_GROUPS);
		if(s.Compare(strNodeName) == 0){
			CDlgFlightDB dlg(GetProjectID(),&GetDocument()->GetTerminal());
			dlg.DoModal();
		}
		//Airport/Sectors
		s.LoadString(IDS_TVNN_AIRPORTSECTORS);
		if(s.Compare(strNodeName) == 0){
			CDlgDBAirports dlg(FALSE,&GetDocument()->GetTerminal());
			dlg.setAirportMan( GetDocument()->GetTerminal().m_pAirportDB->getAirportMan() );
			dlg.DoModal();
		}		
		//Airline/Groups
		s.LoadString(IDS_TVNN_AIRLINEGROUPS);
		if(s.Compare(strNodeName) == 0){
			CDlgDBAirline dlg(FALSE,&GetDocument()->GetTerminal() );
			dlg.setAirlinesMan( GetDocument()->GetTerminal().m_pAirportDB->getAirlineMan() );
			dlg.DoModal();
		}
		//Aircraft/categories
		s.LoadString(IDS_TVNN_AIRCRAFTCATEGORIES);
		if(s.Compare(strNodeName) == 0){
			CDlgDBAircraftTypes dlg(FALSE, &GetDocument()->GetTerminal() );
			dlg.setAcManager( GetDocument()->GetTerminal().m_pAirportDB->getAcMan() );
			dlg.DoModal();
		}
		//vehicle data name
		s = CString(_T("Names"));
		if(s.Compare(strNodeName) == 0)
		{
			CString parentNodeText = msvNode.GetParent().GetText();
			if(parentNodeText.Compare(S_LAND_PAXRELATEDATA)==0){
				CPaxTypeDefDlg dlg( this );
				dlg.DoModal();
			}
		}

		s = CString(_T("Specific Vehicle Types"));
		if (s.Compare(strNodeName) == 0)
		{
			CDlgLandsideVehicleTypeDefine dlg;
			dlg.DoModal();
		}

		s = CString(_T("General Vehicle Characteristics"));
		if (s.Compare(strNodeName) == 0)
		{
			CVehicleTypeDefDlg dlg(this);
			dlg.DoModal();
		}
		
		s = CString(_T("Distribution of Characteristics"));
		if (s.Compare(strNodeName) == 0)
		{
			CDlgVehicleDistribution dlg(this);
			dlg.DoModal();
		}

		s.LoadString(IDS_TVNN_PAXDISTRIBUTIONS);
		if(s.Compare(strNodeName)==0)
		{
			CPaxDistDlg dlg( this );
			dlg.DoModal();
		}
	
	//	s = CString(_T("Properties"));
		s = CString(_T("Properties of Specific V Types"));
		if (s.Compare(strNodeName) == 0)
		{
			CDlgLandsideVehicleGroupProperty dlg(GetDocument()->GetInputLandside(), GetProjectID());
			dlg.DoModal();
		}

		//landside entry offset
		if(strNodeName.Compare(S_LAND_ENTRY_OFFSET)==0)
		{
			if (m_pLandsideEntryOffsetDlg != NULL)
			{
				delete m_pLandsideEntryOffsetDlg;
			}
			m_pLandsideEntryOffsetDlg = new CDlgLandsideEntryOffset(GetDocument()->GetInputLandside(),this);
			m_pLandsideEntryOffsetDlg->Create(CDlgLandsideEntryOffset::IDD);
			m_pLandsideEntryOffsetDlg->CenterWindow();
			m_pLandsideEntryOffsetDlg->ShowWindow(SW_SHOWNORMAL);
		}

		//Boarding Calls
		s.LoadString(IDS_TVNN_PAXBULK);
		if(strNodeName.Compare(s)==0)
		{
			///CFltPaxDataDlg dlg(PAX_LEAD_LAG, this );
			CPaxBulkListDlg dlg(this);
			dlg.DoModal();
		}		
		
	//	s = CString(_T("Lane Availability"));
		s = CString(_T("Lane / Toll Gate Allocation Criteria"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pLaneTollGateDlg != NULL)
			{
				delete m_pLaneTollGateDlg;
			}
		/*	m_pLaneAvailabilitySpecificationDlg = new CDlgLaneAvailabilitySpecification(GetDocument()->GetInputLandside(),this);
			m_pLaneAvailabilitySpecificationDlg->Create(CDlgLaneAvailabilitySpecification::IDD);*/
			m_pLaneTollGateDlg = new CDlgLaneTollCriteria(GetDocument(),this);
			m_pLaneTollGateDlg->Create(CDlgLaneTollCriteria::IDD);
			m_pLaneTollGateDlg->CenterWindow();
			m_pLaneTollGateDlg->ShowWindow(SW_SHOWNORMAL);
		}

		s = CString(_T("Lane Change Criteria"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pLaneChangeCriteriaDlg != NULL)
			{
				delete m_pLaneChangeCriteriaDlg;
			}
			m_pLaneChangeCriteriaDlg = new CDlgLaneChangeCriteria(GetDocument(),&GetDocument()->GetTerminal());
			m_pLaneChangeCriteriaDlg->Create(CDlgLaneChangeCriteria::IDD);
			m_pLaneChangeCriteriaDlg->CenterWindow();
			m_pLaneChangeCriteriaDlg->ShowWindow(SW_SHOWNORMAL);
		}

		s = CString(_T("Resident"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pLandsideVehicleOperationPlanDlg != NULL)
			{
				delete m_pLandsideVehicleOperationPlanDlg;
			}
			m_pLandsideVehicleOperationPlanDlg = new CDlgLandsideVehicleOperationPlan(&GetDocument()->GetTerminal(),GetDocument()->GetInputLandside(),this);
			m_pLandsideVehicleOperationPlanDlg->Create(CDlgLandsideVehicleOperationPlan::IDD);
			m_pLandsideVehicleOperationPlanDlg->CenterWindow();
			m_pLandsideVehicleOperationPlanDlg->ShowWindow(SW_SHOWNORMAL);
		}

		s = CString(_T("Non resident"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pLandsideNonRelatedPlanDlg != NULL)
			{
				delete m_pLandsideNonRelatedPlanDlg;
			}
			m_pLandsideNonRelatedPlanDlg = new CDlgLandsideNonRelatedPlan(&GetDocument()->GetTerminal(),GetDocument()->GetInputLandside(),this);
			m_pLandsideNonRelatedPlanDlg->Create(CDlgLandsideNonRelatedPlan::IDD);
			m_pLandsideNonRelatedPlanDlg->CenterWindow();
			m_pLandsideNonRelatedPlanDlg->ShowWindow(SW_SHOWNORMAL);
		}

		s = CString(_T("Non passenger related"));
		{
			if (strNodeName.Compare(s) == 0)
			{
				if (m_pVehicleOperationNonRelatedDlg != NULL)
				{
					delete m_pVehicleOperationNonRelatedDlg;
				}
				m_pVehicleOperationNonRelatedDlg = new CDlgVehicleOperationNonRelated(&GetDocument()->GetTerminal(),GetDocument()->GetInputLandside(),this);
				m_pVehicleOperationNonRelatedDlg->Create(CDlgVehicleOperationNonRelated::IDD);
				m_pVehicleOperationNonRelatedDlg->CenterWindow();
				m_pVehicleOperationNonRelatedDlg->ShowWindow(SW_SHOWNORMAL);
			}
		}

		//s = CString(_T("Vehicle Assignment"));
		//if (strNodeName.Compare(s) == 0)
		//{
		//	CDlgLandsideVehicleAssignment dlg(GetDocument()->GetInputLandside());
		//	dlg.DoModal();
		//}

		s = CString(_T("Parking spots designation"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pParkingSpotSpecDlg != NULL)
			{
				delete m_pParkingSpotSpecDlg;
			}
			m_pParkingSpotSpecDlg = new CDlgParkingSpotSpec(this);
			m_pParkingSpotSpecDlg->Create(CDlgParkingSpotSpec::IDD);
			m_pParkingSpotSpecDlg->CenterWindow();
			m_pParkingSpotSpecDlg->ShowWindow(SW_SHOWNORMAL);
		}

		s.LoadString(IDS_LANDSIDE_FACILITYBEHAVIOR);
		if(s.Compare(strNodeName)==0)
		{
			if (m_FacilityDataSheet != NULL)
			{
				delete m_FacilityDataSheet;
			}
			m_FacilityDataSheet = new CFacilityDataSheet(GetDocument()->GetInputLandside(), &(GetDocument()->GetTerminal()) ,s, this);
			m_FacilityDataSheet->Create(this);
			m_FacilityDataSheet->CenterWindow();
			m_FacilityDataSheet->ShowWindow(SW_SHOWNORMAL);
		}

		//
		s.LoadString(IDS_TVNN_MOBILEDISPLAY);
		if(s.Compare(strNodeName)==0)
		{
			CPaxDispPropDlg dlg( this );
			dlg.DoModal();
			GetDocument()->UpdateTrackersMenu();
		}//end if compare

		s.LoadString(IDS_TVNN_MOBILETAGS);
		if(s.Compare(strNodeName) == 0)
		{
			CDlgPaxTags dlg( this );
			dlg.DoModal();
			GetDocument()->UpdateTrackersMenu();
		}

		//Passenger Related	
		s = CString(_T("Passenger Related"));
		if (strNodeName.Compare(s) == 0)
		{
			CDlgLandsideVehicleAssignment dlg(GetDocument()->GetInputLandside(), this);
			dlg.DoModal();			
		}


		//Non-Passenger Related			
		s = CString(_T("Non-Passenger Related"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pVehicleAssignmentNonPaxRelatedDlg != NULL)
			{
				delete m_pVehicleAssignmentNonPaxRelatedDlg;
			}
			m_pVehicleAssignmentNonPaxRelatedDlg = new CDlgNonPassengerRelatedVehicleAssignment(GetDocument()->GetInputLandside(), this);
			m_pVehicleAssignmentNonPaxRelatedDlg->Create(CDlgNonPassengerRelatedVehicleAssignment::IDD);
			m_pVehicleAssignmentNonPaxRelatedDlg->CenterWindow();
			m_pVehicleAssignmentNonPaxRelatedDlg->ShowWindow(SW_SHOWNORMAL);		
		}

		//initcondition
		s = CString(_T("Initial Conditions"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pInitConDitionDlg != NULL)
			{
				delete m_pInitConDitionDlg;
			}
			m_pInitConDitionDlg = new CDlgInitCondition(GetDocument()->GetInputLandside(),this);
			m_pInitConDitionDlg->Create(CDlgInitCondition::IDD);
			m_pInitConDitionDlg->CenterWindow();
			m_pInitConDitionDlg->ShowWindow(SW_SHOWNORMAL);
		}
		s = CString(_T("Service Time"));
		if (strNodeName.Compare(s) == 0)
		{
			if (m_pServiceTimeDlg != NULL)
			{
				delete m_pServiceTimeDlg;
			}
			m_pServiceTimeDlg = new CDlgVehicleBehaviorDistritbution(GetDocument()->GetInputLandside(), this);
			m_pServiceTimeDlg->Create(CDlgVehicleBehaviorDistritbution::IDD);
			m_pServiceTimeDlg->CenterWindow();
			m_pServiceTimeDlg->ShowWindow(SW_SHOWNORMAL);
		}




	}
}

void CLandsideMSView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	CNewMenu menu, *pCtxMenu = NULL;
	UINT uFlags;
	m_hRightClkItem = GetTreeCtrl().HitTest(cliPt, &uFlags);
	if((m_hRightClkItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		CTreeItemMSVNode itemNode(&GetTreeCtrl(),m_hRightClkItem);
		CNodeData* pNode = itemNode.getNodeData();

		if(pNode != NULL)
		{
			SelectNode(pNode);
			if(pNode->nIDResource != NULL )
			{
				menu.LoadMenu(pNode->nIDResource);
				if (pNode->nIDResource == IDR_CTX_LEVEL)
				{
					CMenu* pLinkToFloorMenu = menu.GetSubMenu(0);
					pLinkToFloorMenu->GetSubMenu(15)->RemoveMenu(0,MF_BYPOSITION);
					const CFloors& TerminalFloors = GetDocument()->GetFloorByMode(EnvMode_Terminal);
					for(int i = 0; i < TerminalFloors.GetCount(); i++)
					{
						CRenderFloor* floor = (CRenderFloor*)TerminalFloors.GetFloor2(i);
						CString floorname = floor->FloorName();
 						pLinkToFloorMenu->GetSubMenu(15)->AppendMenu( MF_STRING|MF_ENABLED,ID_LINKTO_TERMINALFLOOR+i,floorname);
					}
					int nLevelID = pNode->dwData;
					CRenderFloor* pFloor = GetDocument()->GetFloorByMode(EnvMode_LandSide).GetFloor(nLevelID);
					if(!pFloor) return;
					int linkindex = pFloor->linkToTerminalFloor();
					pLinkToFloorMenu->GetSubMenu(15)->CheckMenuItem(linkindex,MF_BYPOSITION|MF_CHECKED);
				}
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

				UINT nMenuID = 0;
				CString strMenuCaption;

				if(itemNode.getNodeType()==NodeType_ObjectRoot )
				{
					CTreeItemLayoutObjectRoot objItemRoot = itemNode;
					if( objItemRoot.getObjectType()!=ALT_LAREA && objItemRoot.getObjectType()!= ALT_LPORTAL )
					{
						nMenuID = ID_NEW_LNDSDPROC + objItemRoot.getObjectType();
						strMenuCaption = objItemRoot.GetMenuAddCaption();
						pCtxMenu->InsertODMenu(0, strMenuCaption,MF_STRING | MF_BYPOSITION, nMenuID);
					}
				}
			}
		}
		//Landside Node
		if(pCtxMenu != NULL)
		{
			//UpdatePopMenu(this, pCtxMenu);
			pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, AfxGetMainWnd() );
		}
	}

}

void CLandsideMSView::OnCtxLandprocproperties()
{
	CTreeItemMSVNode treeItem(&GetTreeCtrl(),m_hRightClkItem);
	if(!treeItem)
		return;
	if(treeItem.getNodeType() != NodeType_Object)
		return;

	CTreeItemLayoutObject objItem = treeItem;
	LandsideFacilityLayoutObject* pObj  = (LandsideFacilityLayoutObject*) objItem.getObject();
	if(!pObj)
		return;


	CTermPlanDoc* pDoc = GetDocument();	
	pDoc->GetLayoutEditor()->OnCtxProcproperties(pObj,this);
	
}

void CLandsideMSView::OnCommandUpdate( ILandsideEditCommand* pCmd )
{
	CTreeItemLayout layoutItem(&GetTreeCtrl(),m_hLayout) ;
	if(LandsideCopyLayoutObjectCmd* pCopyCmd = pCmd->toCopyObjCmd())
	{
		if(pCopyCmd->mLastOp == Command::_do)
		{
			layoutItem.AddLayoutObject(pCopyCmd->getObject());
		}
		else
		{
			layoutItem.RemoveLayoutObject(pCopyCmd->getObject());
		}
	}
	if(LandsideAddLayoutObjectCmd* pAddCmd = pCmd->toAddObjCmd())
	{
		{
			if(pAddCmd->mLastOp == Command::_do)
			{
				layoutItem.AddLayoutObject(pAddCmd->getObject());
			}
			else
			{
				layoutItem.RemoveLayoutObject(pAddCmd->getObject());
			}
		}	
	}
	else if(LandsideDelLayoutObjectCmd* pDelCmd = pCmd->toDelObjCmd())
	{
		if(pDelCmd->mLastOp==Command::_do)
		{
			layoutItem.RemoveLayoutObject(pDelCmd->getObject());
		}
		else
		{
			layoutItem.AddLayoutObject(pDelCmd->getObject());
		}		
	}
	else if(LandsideModifyLayoutObjectCmd* pModCmd = pCmd->toModObjCmd())
	{	
		if(pModCmd->IsNameChanged())
			layoutItem.ReNameLayoutObject(pModCmd->getObject());		
	}
}

int CLandsideMSView::GetProjectID()
{
	return GetDocument()->GetProjectID();
}

void CLandsideMSView::InitTree()
{	
	
	HTREEITEM hTreeItem = NULL;
	CNodeData *pNodeData = NULL;
	int nProjID = -1;
	HTREEITEM hRoot = TVI_ROOT;
	m_hLevelRoot = NULL;
	CTreeCtrlItem rootItem(&GetTreeCtrl(),hRoot);
	InputLandside* pInputLandside = GetDocument()->getARCport()->m_pInputLandside;


	//Build Node structure
	CTreeItemLayout layoutRoot = rootItem.AddChild(IDS_TVNN_LAYOUT);
	m_hLayout = layoutRoot.GeHItem();
	layoutRoot.InitNodeData(NodeType_Normal,m_pNodeDataAllocator);
	layoutRoot.SetResourceID(IDR_CTX_LANDSIDELAYOUT);

	//add levels 
	//s.LoadString();
	CLevelRootTreeItem levelRoot = layoutRoot.AddChild(IDS_TVNN_LEVELS);
	m_hLevelRoot = levelRoot.GeHItem();
	levelRoot.Init(m_pNodeDataAllocator);

	//add each level to Node
	CFloors& levelList = GetDocument()->GetFloorByMode(EnvMode_LandSide);
	levelRoot.AddLevelList(levelList);
	levelRoot.Expand();
	
	
	HTREEITEM hObjItem = NULL;
	LandsideFacilityLayoutObjectList& layoutlist = pInputLandside->getObjectList();
	//add Rolling Surfaces	
	if( CTreeItemMSVNode rollRoot = layoutRoot.AddChild(IDS_TVNN_ROLLSURFACE) )
	{
		rollRoot.InitNodeData(NodeType_Normal,m_pNodeDataAllocator);	
		//HTREEITEM hRollSurface = rollRoot.m_hItem;
		//Stretches
		{
			CTreeItemLayoutObjectRoot stretchRoot = rollRoot.AddChild(IDS_TVNN_STRETCHS);
			stretchRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			stretchRoot.setObjectType(ALT_LSTRETCH);
			stretchRoot.AddObjectList(layoutlist);
		}
		//Intersections
		{			
			CTreeItemLayoutObjectRoot intersectionRoot = rollRoot.AddChild(IDS_TVNN_INTERSECTIONS);
			intersectionRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			intersectionRoot.setObjectType(ALT_LINTERSECTION);
			intersectionRoot.AddObjectList(layoutlist);			
		}	
		//RoundAbout
		{
			CTreeItemLayoutObjectRoot rdRoot = rollRoot.AddChild(IDS_TVNN_ROUNDABOUT);
			rdRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			rdRoot.setObjectType(ALT_LROUNDABOUT);
			rdRoot.AddObjectList(layoutlist);		
		}
		//s = "External Node";
		{			
			CTreeItemLayoutObjectRoot extRoot = rollRoot.AddChild(_T("Entry Exit Point"));
			extRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			extRoot.setObjectType(ALT_LEXT_NODE);
			extRoot.AddObjectList(layoutlist);			
		}
		//Turnoffs
		{			
			//CTreeItemMSVNode tfroot = rollRoot.AddChild(IDS_TVNN_TURNOFFS);
			//tfroot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			//tfroot.setObjectType(ALT);			
		}
		
		//stretch segment
		{
			
			CTreeItemLayoutObjectRoot stretchRoot = rollRoot.AddChild( _T("Named Stretch Segment"));
			stretchRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			stretchRoot.setObjectType(ALT_LSTRETCHSEGMENT);
			stretchRoot.AddObjectList(layoutlist);
		}

		////decision point
		//{
		//	CTreeItemLayoutObjectRoot strDecPoint = rollRoot.AddChild( _T("Decision Point"));
		//	strDecPoint.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		//	strDecPoint.setObjectType(ALT_LDECISIONPOINT);
		//	strDecPoint.AddObjectList(layoutlist);
		//}
		//rollRoot.Expand();//GetTreeCtrl().Expand(hRollSurface,TVE_EXPAND);
	}

	//add Control Devices
	if( CTreeItemMSVNode deviceItem =  layoutRoot.AddChild(IDS_TVNN_CONTROLDEVICE) )	
	{
		//Traffic lights
		{
			CTreeItemLayoutObjectRoot deviceRoot  =  deviceItem.AddChild(IDS_TVNN_TRAFFICLIGHT);
			deviceRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			deviceRoot.setObjectType(ALT_LTRAFFICLIGHT);
			deviceRoot.AddObjectList(layoutlist);
		}
		//Toll Gates
		{
			CTreeItemLayoutObjectRoot deviceRoot  =  deviceItem.AddChild(IDS_TVNN_TOLLGATE);
			deviceRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			deviceRoot.setObjectType(ALT_LTOLLGATE);
			deviceRoot.AddObjectList(layoutlist);
		}
	

		//Stop signs
		{
			CTreeItemLayoutObjectRoot deviceRoot  =  deviceItem.AddChild(IDS_TVNN_STOPSIGN);
			deviceRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			deviceRoot.setObjectType(ALT_LSTOPSIGN);
			deviceRoot.AddObjectList(layoutlist);
		}
		

		//Yield sign
		{
			CTreeItemLayoutObjectRoot deviceRoot  =  deviceItem.AddChild(IDS_TVNN_YIELDSIGN);
			deviceRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			deviceRoot.setObjectType(ALT_LYIELDSIGN);
			deviceRoot.AddObjectList(layoutlist);
		}
		//deviceItem.AddChild(IDS_TVNN_YIELDSIGN);
		//Speed Sign
		{
			CTreeItemLayoutObjectRoot deviceRoot  =  deviceItem.AddChild(S_SPEEDSIGN);
			deviceRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			deviceRoot.setObjectType(ALT_LSPEEDSIGN);
			deviceRoot.AddObjectList(layoutlist);
		}
		
		deviceItem.Expand();
	}

	//parking lot		
	{
		CTreeItemLayoutObjectRoot parkinglotroot = layoutRoot.AddChild(IDS_TVNN_PARKINGLOT);
		parkinglotroot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		parkinglotroot.setObjectType(ALT_LPARKINGLOT);
		parkinglotroot.AddObjectList(layoutlist);			

	}
	//Taxi Pool
	{
		CTreeItemLayoutObjectRoot taxipoolRoot = layoutRoot.AddChild(S_TAXIPOOL);
		taxipoolRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		taxipoolRoot.setObjectType(ALT_LTAXIPOOL);
		taxipoolRoot.AddObjectList(layoutlist);			
	}
	//service stations
	{
		CTreeItemLayoutObjectRoot servieStationRoot = layoutRoot.AddChild(S_SERVICESTATIONS);
		servieStationRoot.InitNodeData(NodeType_Normal,m_pNodeDataAllocator);
		//taxipoolRoot.setObjectType(ALT_LTAXIPOOL);
		//taxipoolRoot.AddObjectList(layoutlist);			
	}


	//add Terminal Interfaces	
	if( CTreeItemMSVNode termlFace = layoutRoot.AddChild(IDS_TVNN_TERMINAL_INTERFACE) )
	{
		termlFace.InitNodeData(NodeType_Normal,m_pNodeDataAllocator);
		//HTREEITEM hTermFace = termlFace.m_hItem;
		//Curbside
		CTreeItemLayoutObjectRoot curbsideRoot = termlFace.AddChild( _T("Curbsides") );
		curbsideRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		curbsideRoot.setObjectType(ALT_LCURBSIDE);
		curbsideRoot.AddObjectList(layoutlist);	

		//Bus Station		
		CTreeItemLayoutObjectRoot busStationRoot = termlFace.AddChild(_T("Bus Station"));
		busStationRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		busStationRoot.setObjectType(ALT_LBUSSTATION);
		busStationRoot.AddObjectList(layoutlist);	

		//Taxi Queue
		CTreeItemLayoutObjectRoot taxiQueueRoot = termlFace.AddChild(S_TAXIQUEUE);
		taxiQueueRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		taxiQueueRoot.setObjectType(ALT_LTAXIQUEUE);
		taxiQueueRoot.AddObjectList(layoutlist);	
		
		//Crosswalk		
		CTreeItemLayoutObjectRoot crosswalkRoot = termlFace.AddChild(_T("Crosswalk"));
		crosswalkRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		crosswalkRoot.setObjectType(ALT_LCROSSWALK);
		crosswalkRoot.AddObjectList(layoutlist);	

		//Walkway		
		CTreeItemLayoutObjectRoot walkwayRoot = termlFace.AddChild(_T("Walkway"));
		walkwayRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		walkwayRoot.setObjectType(ALT_LWALKWAY);
		walkwayRoot.AddObjectList(layoutlist);	

		termlFace.Expand();
	}
	
	//add Landside Portals
	{		
		CTreeItemLayoutObjectRoot portalRoot=layoutRoot.AddChild(IDS_TVNN_LANDSIDE_PORTALS);
		portalRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		portalRoot.SetResourceID(IDR_CTX_LANDSIDE_PORTALS);
		portalRoot.setObjectType(ALT_LPORTAL);
		m_hPortalRoot = portalRoot.GeHItem();	
		portalRoot.AddObjectList(pInputLandside->GetPortals());
	}

	//add Landside areas
	{
		CTreeItemLayoutObjectRoot LandsideAreaRoot = layoutRoot.AddChild(IDS_TVNN_LANDSIDE_AREAS);
		LandsideAreaRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
		LandsideAreaRoot.SetResourceID(IDR_CTX_LANDSIDE_AREAS);
		LandsideAreaRoot.setObjectType(ALT_LAREA);
		m_hLandsideAreaRoot = LandsideAreaRoot.GeHItem();
		LandsideAreaRoot.AddObjectList(pInputLandside->GetLandsideAreas());
		
	}

	layoutRoot.Expand();
	//////////////////////////////////////////////////////////////////////////
	if(CTreeItemMSVNode nodeDemandRoot  = rootItem.AddChild("Demand Data") )
	{
		HTREEITEM hDemand = nodeDemandRoot.GeHItem();
		if(CTreeItemMSVNode vehData = nodeDemandRoot.AddChild("Vehicle data"))
		{
			//Vehicle Names
		//	vehData.AddChild("Names");
			vehData.AddChild("Specific Vehicle Types");
			//Vehicle Properties
			vehData.AddChild("Properties of Specific V Types");
			//vehData.AddChild("Properties");
			vehData.AddChild("General Vehicle Characteristics");
			vehData.AddChild("Distribution of Characteristics");

			vehData.Expand();
		}

		//add Flight related data to root
		if(CTreeItemMSVNode nodeFlightRelate = nodeDemandRoot.AddChild(_T("Flight related Data")))
		{
			//Flight Schedule
			nodeFlightRelate.AddChild(IDS_TVNN_SCHEDULE);
			//Aircraft Capacity
			nodeFlightRelate.AddChild(IDS_TVNN_ACCAPACITIES);
			

			//Flight type specification
			if(CTreeItemMSVNode nodeflttype = nodeFlightRelate.AddChild(IDS_TVNN_FLIGHTTYPESPECIFICATION) )
			{
				
				//Flight/Groups
				CTreeItemMSVNode nodefltgroup = nodeflttype.AddChild(IDS_TVNN_FLIGHT_GROUPS);
				nodefltgroup.InitNodeData(NodeType_Dlg,m_pNodeDataAllocator);
				nodefltgroup.SetResourceID(IDR_MENU_FLIGHT_GROUPS);			

				//Airport/Sectors
				CTreeItemMSVNode nodeSector = nodeflttype.AddChild(IDS_TVNN_AIRPORTSECTORS);
				nodeSector.InitNodeData(NodeType_Dlg,m_pNodeDataAllocator);
				nodeSector.SetResourceID(IDR_MENU_AIRPORT_AND_SECTORS);
				

				//Airline/Groups
				CTreeItemMSVNode nodeAirline = nodeflttype.AddChild(IDS_TVNN_AIRLINEGROUPS);
				nodeAirline.InitNodeData(NodeType_Dlg,m_pNodeDataAllocator);
				nodeAirline.SetResourceID(IDR_MENU_AIRLINE_GROUP);
				

				//Aircraft/categories
				CTreeItemMSVNode nodeAcCat = nodeflttype.AddChild(IDS_TVNN_AIRCRAFTCATEGORIES);
				nodeAcCat.InitNodeData(NodeType_Dlg,m_pNodeDataAllocator);
				nodeAcCat.SetResourceID(IDR_MENU_FLITYPE_CATEGORY);			
			

				nodeflttype.Expand();
			}

			//Load factors
			nodeFlightRelate.AddChild(IDS_TVNN_LOADFACTORS);			

			nodeFlightRelate.Expand();
		}	

		//add Passenger related data to root
		if(CTreeItemMSVNode nodePaxRelate =  nodeDemandRoot.AddChild(S_LAND_PAXRELATEDATA))
		{
			//Names
			nodePaxRelate.AddChild(_T("Names"));		

			//Distributions
			nodePaxRelate.AddChild(IDS_TVNN_PAXDISTRIBUTIONS);		
						
			//Land Entry Offset	
			nodePaxRelate.AddChild(S_LAND_ENTRY_OFFSET);		

			//bulk profile
			//nodePaxRelate.AddChild(IDS_TVNN_PAXBULK);

			nodePaxRelate.Expand();
		}
		
		nodeDemandRoot.Expand();
	}



	//add Management data to root
	if(CTreeItemMSVNode nodeManagement = rootItem.AddChild(_T("Management data")) )
	{
		//vehicle assignment
		if(CTreeItemMSVNode VAItem  = nodeManagement.AddChild(_T("Vehicle Assignment")))
		{
			//			
			VAItem.AddChild(_T("Passenger Related"));
			//			
			VAItem.AddChild( _T("Non-Passenger Related") );

			VAItem.Expand();
		}


		//stretch segment
		{

			CTreeItemLayoutObjectRoot stretchRoot = nodeManagement.AddChild( _T("Named Stretch Segment"));
			stretchRoot.InitNodeData(NodeType_ObjectRoot,m_pNodeDataAllocator);
			stretchRoot.setObjectType(ALT_LSTRETCHSEGMENT);
			stretchRoot.AddObjectList(layoutlist);
		}
		//Speed control
		nodeManagement.AddChild(S_LAND_SPEEDCONTROL);
		
		
		//Parking constraints
		nodeManagement.AddChild(_T("Parking spots designation"));
		

		//Lane Availability
		//nodeManagement.AddChild(_T("Lane Availability"));
		nodeManagement.AddChild(_T("Lane / Toll Gate Allocation Criteria"));
		

		//Lane changes
		nodeManagement.AddChild(_T("Lane Change Criteria"));
	
	

		nodeManagement.AddChild(_T("Intersection Traffic management"));	

		//curbside strategies 
		nodeManagement.AddChild(_T("Curbside Strategies"));

		//parking lot strategies
		nodeManagement.AddChild(_T("Parking Lot Strategies"));

		if(CTreeItemMSVNode VOPItem  = nodeManagement.AddChild(_T("Route Operations Plan")))
		{
			if(CTreeItemMSVNode RouteItem  = VOPItem.AddChild(_T("Passenger related")))
			{
				//			
				RouteItem.AddChild(_T("Resident"));
				//			
				RouteItem.AddChild( _T("Non resident") );

				RouteItem.Expand();
			}
			VOPItem.AddChild(_T("Non passenger related"));
			VOPItem.Expand();
		}

		CTreeItemMSVNode nodeSector = nodeManagement.AddChild(IDS_LANDSIDE_FACILITYBEHAVIOR);
		nodeSector.InitNodeData(NodeType_Dlg,m_pNodeDataAllocator);


		nodeManagement.AddChild(_T("Service Time"));	



		nodeManagement.Expand();

	}
		// add Simulation to root
	if(CTreeItemMSVNode nodeSimulation = rootItem.AddChild(_T("Simulation")))
	{
		//Initial Conditions
		nodeSimulation.AddChild(_T("Initial Conditions"));
		
		//Settings
		nodeSimulation.AddChild(_T("Settings"));
		//Run
		nodeSimulation.AddChild(_T("Run"));

		nodeSimulation.Expand();	

		
	}


	//add Analysis parameters to root
	if(CTreeItemMSVNode nodeAnalysis = rootItem.AddChild(_T("Analysis parameters")))
	{
		//Vehicle display parameters
		nodeAnalysis.AddChild(_T("Vehicle Display"));
		//vehicle tags
		nodeAnalysis.AddChild(_T("Vehicle Tags"));
		//pax display
		nodeAnalysis.AddChild(IDS_TVNN_MOBILEDISPLAY);
		//pax tags;
		nodeAnalysis.AddChild(IDS_TVNN_MOBILETAGS);			
		//Report parameters
		nodeAnalysis.AddChild(_T("Report parameters"));
		//Callout Display Specification
		nodeAnalysis.AddChild(_T("Callout Display Specification"));	

		nodeAnalysis.Expand();
	}

	GetTreeCtrl().SelectItem(m_hLayout);	
}

//HTREEITEM CLandsideMSView::AddItem( const CString& strNode,HTREEITEM hParent,HTREEITEM hInsertAfter,int nImage ,int nSelectImage )
//{
//	COOLTREE_NODE_INFO cni;	
//	CARCBaseTree::InitNodeInfo(this,cni);
//	cni.nImage = nImage+ID_IMAGE_COUNT;
//	cni.nImageSeled = nSelectImage+ID_IMAGE_COUNT;
//	return GetTreeCtrl().InsertItem(strNode,cni,FALSE,FALSE,hParent,TVI_LAST);
//}

void CLandsideMSView::SelectNode( CNodeData* pNode )
{
	CTVNode* pTVNode= NULL;
	if(pNode != NULL && pNode->nodeType==NodeType_Level)
		pTVNode = ((CTVNode*)pNode->nOtherData);
	if(pNode!=NULL && pNode->nodeType == NodeType_LevelRoot)
		pTVNode = ((CTVNode*)pNode->nOtherData);
	GetDocument()->SelectNode(pTVNode);
}

void CLandsideMSView::NewLevelItem(CFloors& floor,int idx)
{
	CLevelRootTreeItem levelRoot( CTreeCtrlItem(&GetTreeCtrl(),m_hLevelRoot) );
	levelRoot.AddLevelItem(floor.GetFloor2(idx));
}

void CLandsideMSView::DeleteLevelItem( CFloors& floor,int idx )
{
	CLevelRootTreeItem levelRoot =  CTreeCtrlItem(&GetTreeCtrl(),m_hLevelRoot) ;
	levelRoot.RemoveLevelItem(idx);

	CLevelTreeItem child = levelRoot.GetFirstChild();
	int nFloorIdx = 0;
	while(child)
	{
		child.SetFloorIndex(nFloorIdx);
		nFloorIdx++;
		child = child.GetNextSibling();
	}	
}

void CLandsideMSView::ActiveLevelItem( int idx )
{	
	CLevelRootTreeItem levelRoot =  CTreeCtrlItem(&GetTreeCtrl(),m_hLevelRoot) ;

	CLevelTreeItem child = levelRoot.GetFirstChild();	
	while(child)
	{
		if(idx == child.GetFloorIndex() )
		{
			child.SetActive(TRUE);
		}
		else
			child.SetActive(FALSE);
		
		child = child.GetNextSibling();
	}	
}

void CLandsideMSView::OnCtxDeleteGroupProc()
{

}

void CLandsideMSView::OnCtxLanddeleteproc()
{
	CTreeItemMSVNode treeItem(&GetTreeCtrl(),m_hRightClkItem);
	if(!treeItem)
		return;
	if(treeItem.getNodeType() != NodeType_Object)
		return;

	CTreeItemLayoutObject objItem = treeItem;
	LandsideFacilityLayoutObject* pObj  = (LandsideFacilityLayoutObject*) objItem.getObject();
	if(!pObj)
		return;

	CTermPlanDoc* pDoc = GetDocument();	
	pDoc->GetLayoutEditor()->OnCtxDelLandsideProc(pObj);	
}

CLandsideEditContext* CLandsideMSView::GetEditContext()
{
	return GetDocument()->GetLayoutEditor()->m_landsideEdit;
}

LRESULT CLandsideMSView::OnEndLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;
	//return immediately if the name is null (no rename).
	if(pDispInfo->item.pszText == NULL)
		return 0;
	//LandsideFacilityLayoutObjectList List;
	CTreeCtrlItem CurctrlItem(&GetTreeCtrl(),m_selItem);
	int count = 0;
	while (CurctrlItem)
	{
		CurctrlItem = CurctrlItem.GetFirstChild();
		count++;
	}
	CTreeCtrlItem ctrlItem(&GetTreeCtrl(),m_selItem);
	if(CLevelTreeItem levelItem  = ctrlItem)
	{
		int nFloorIdx = levelItem.GetFloorIndex();
		CRenderFloor* pSelFloor = GetDocument()->GetCurModeFloor().GetFloor(nFloorIdx);
		if(pSelFloor)
		{
			pSelFloor->FloorName(pDispInfo->item.pszText);
			levelItem.SetText(pDispInfo->item.pszText);
			GetDocument()->GetCurModeFloor().saveDataSet(GetDocument()->m_ProjInfo.path, true);
			*pResult = TRUE;
		}
		
	}
	if (CTreeItemLayoutObjectGroup groupItem = ctrlItem)
	{
		if (groupItem.getNodeType() == NodeType_ObjectGroup)
		{
			LandsideFacilityLayoutObjectList groupObjectList;
			groupItem.GetLayoutObjectList(groupObjectList);
			for (int i =0; i < groupObjectList.getCount(); i++)
			{
				ALTObjectID* ID = &(groupObjectList.getObject(i)->getName());
				CString str = groupItem.GetText();
				int num = ID->idLength();
				ID->m_val[num-count] = pDispInfo->item.pszText;
				groupObjectList.getObject(i)->SaveObject(groupObjectList.getObject(i)->getID());
			}
			groupItem.SetText(pDispInfo->item.pszText);
			*pResult = TRUE;
		}

	}

	if (CTreeItemLayoutObject objectItem = ctrlItem)
	{
		if (objectItem.getNodeType() == NodeType_Object)
		{
			LandsideFacilityLayoutObjectList ObjectList;
			objectItem.GetLayoutObjectList(ObjectList);
			for (int i =0; i < ObjectList.getCount(); i++)
			{
				ALTObjectID* ID = &(ObjectList.getObject(i)->getName());
				CString str = objectItem.GetText();
				int num = ID->idLength();
				ID->m_val[num-count] = pDispInfo->item.pszText;
				ObjectList.getObject(i)->SaveObject(ObjectList.getObject(i)->getID());
			}
			objectItem.SetText(pDispInfo->item.pszText);
			*pResult = TRUE;
		}
	}
	*pResult = FALSE;  // return FALSE to cancel edit, TRUE to accept it.
	return 0;
}
LRESULT CLandsideMSView::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;
	//only allow editing of the following nodes:
	//CTVNode with IDR_CTX_FLOOR as resID
	//CTVNode with IDR_CTX_PROCESSOR as resID
	//CTVNode with IDR_CTX_PROCESSORGROUP as resID
	CTreeCtrlItem ctrlItem(&GetTreeCtrl(),m_selItem);
	if (CLevelTreeItem levelItem  = ctrlItem)
	{
		*pResult = FALSE;
		return 0;
	}

	if (CTreeItemLayoutObjectGroup objectItem = ctrlItem)
	{
		if (objectItem.getNodeType() == NodeType_ObjectGroup)
		{
			*pResult = FALSE;
			return 0;
		}
		
	}

	if (CTreeItemLayoutObject groupItem = ctrlItem)
	{
		if (groupItem.getNodeType() == NodeType_Object)
		{
			*pResult = FALSE;
			return 0;
		}
	}

	*pResult = TRUE;
	return 0;
}

void CLandsideMSView::OnCtxDisplayProp()
{
	CTreeItemMSVNode treeItem(&GetTreeCtrl(),m_hRightClkItem);
	if(!treeItem)
		return;

	LandsideFacilityLayoutObjectList layoutObjectList;
	if(treeItem.getNodeType()==NodeType_Object)
	{
		CTreeItemLayoutObject layoutObjectNode = (CTreeItemLayoutObject)treeItem;
		layoutObjectNode.GetLayoutObjectList(layoutObjectList);
	}
	else if(treeItem.getNodeType()==NodeType_ObjectGroup )
	{
		CTreeItemLayoutObjectGroup layoutGroupNode = (CTreeItemLayoutObjectGroup)treeItem;
		layoutGroupNode.GetLayoutObjectList(layoutObjectList);
	}
	else
	{
		return;
	}

	if(layoutObjectList.getCount() == 0)
		return;

	CDlgLayoutObjectDisplayProperties dlgProp(&layoutObjectList,this);
	if( dlgProp.DoModal() ==IDOK)
	{
		for (int i = 0; i < layoutObjectList.getCount(); i++)
		{
			LandsideFacilityLayoutObject* pObj = layoutObjectList.getObject(i);
			if (pObj == NULL)
				continue;
			
			pObj->SaveDisplayProp();
			//update all views
			GetDocument()->UpdateAllViews(NULL, VM_UPDATE_ALTOBJECT3D,(CObject*)pObj);
		}
	}
	
		
//	GetDocument()->GetLayoutEditor()->OnCtxLayoutObjectDisplayProp(this);

}

void CLandsideMSView::OnFloorPictureInsert()
{
	if (m_hRightClkItem == NULL)
		return;

	CLevelTreeItem levelItem(&GetTreeCtrl(),m_hRightClkItem);
	if(!levelItem)
		return;

	int nLevelID = levelItem.GetFloorIndex();
	CRenderFloor* pFloor = GetDocument()->GetFloorByMode(EnvMode_LandSide).GetFloor(nLevelID);
	if(!pFloor) return;

	CPictureInfo oldInof =  pFloor->m_picInfo;
	
	CDlgFloorPictureReference dlg(pFloor->m_picInfo.sPathName, nLevelID,pFloor, GetDocument()->GetIRenderView(), this);
	INT_PTR retCode = dlg.DoModal(); 
	if(IDOK == retCode)
	{
		GetDocument()->GetFloorByMode(EnvMode_LandSide).SaveData( GetDocument()->m_ProjInfo.path, true );
	}
	else if(IDCANCEL == retCode)
	{
		pFloor->m_picInfo = oldInof;
	}
}

void CLandsideMSView::OnFloorPictureHide()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	CRenderFloor* pFloor = GetDocument()->GetFloorByMode(EnvMode_LandSide).GetFloor(nLevelID);
	if(!pFloor) return;
	
	pFloor->m_picInfo.bShow = false;

	if( IRender3DView* pView =  GetDocument()->GetIRenderView() )
	{
		pView->UpdateFloorOverlay(nLevelID,pFloor->m_picInfo);
	}
	GetDocument()->GetFloorByMode(EnvMode_LandSide).SaveData( GetDocument()->m_ProjInfo.path, true );
}

void CLandsideMSView::OnFloorPictureShow()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	CRenderFloor* pFloor = GetDocument()->GetFloorByMode(EnvMode_LandSide).GetFloor(nLevelID);
	if(!pFloor) return;

	pFloor->m_picInfo.bShow = true;

	if( IRender3DView* pView =  GetDocument()->GetIRenderView() )
	{
		pView->UpdateFloorOverlay(nLevelID,pFloor->m_picInfo);
	}
	GetDocument()->GetFloorByMode(EnvMode_LandSide).SaveData( GetDocument()->m_ProjInfo.path, true );
}

void CLandsideMSView::OnFloorPictureRemove()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	CRenderFloor* pFloor = GetDocument()->GetFloorByMode(EnvMode_LandSide).GetFloor(nLevelID);
	if(!pFloor) return;

	pFloor->m_picInfo.sFileName = "";
	pFloor->m_picInfo.sPathName = "";

	if( IRender3DView* pView =  GetDocument()->GetIRenderView() )
	{
		pView->UpdateFloorOverlay(nLevelID,pFloor->m_picInfo);
	}
	GetDocument()->GetFloorByMode(EnvMode_LandSide).SaveData( GetDocument()->m_ProjInfo.path, true );
}

void CLandsideMSView::OnLinkToTerminalFloors( UINT nID )
{
	//get the terminal floor
	int TerminalFloorindex = nID - ID_LINKTO_TERMINALFLOOR;
	const CFloors& pTerminalFloors = GetDocument()->GetFloorByMode(EnvMode_Terminal);
	CRenderFloor* terminalfloor = (CRenderFloor*)pTerminalFloors.GetFloor2(TerminalFloorindex);
	
	//get the landside floor
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	CRenderFloor* pFloor = GetDocument()->GetFloorByMode(EnvMode_LandSide).GetFloor(nLevelID);
	if(!pFloor) return;
	if (pFloor->linkToTerminalFloor() == -1 || pFloor->linkToTerminalFloor() != TerminalFloorindex)
	{
		pFloor->linkToTerminalFloor(TerminalFloorindex);
		terminalfloor->RealAltitude(pFloor->RealAltitude());
		terminalfloor->Altitude(pFloor->Altitude());
	} 
	else
	{
		pFloor->linkToTerminalFloor(-1);
	}
	GetDocument()->GetFloorByMode(EnvMode_Terminal).SaveData( GetDocument()->m_ProjInfo.path, true );
	GetDocument()->GetFloorByMode(EnvMode_LandSide).SaveData( GetDocument()->m_ProjInfo.path, true );
	if(!GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
		return ;
	GetDocument()->UpdateAllViews(NULL,VM_UPDATE_ONE_FLOOR,(CObject*)pFloor);
	GetDocument()->UpdateAllViews(NULL,VM_UPDATE_ONE_FLOOR,(CObject*)terminalfloor);
}


//void CLandsideMSView::AddLandsideAreaItem( CLandsideArea *LandsideArea )
//{
//	
//	rootItem.addobj
//
//	COOLTREE_NODE_INFO cni;		
//	CARCBaseTree::InitNodeInfo(GetTreeCtrl().GetParent(),cni);
//	cni.nImage = 0 + ID_IMAGE_COUNT;
//	cni.nImageSeled = 0 + ID_IMAGE_COUNT;
//	CNodeData* pData = m_pNodeDataAllocator->allocateNodeData(NodeType_Normal);
//	pData->nIDResource = IDR_CTX_LANDSIDE_AREA;
//	pData->dwData=(DWORD)LandsideArea;
//
//	HTREEITEM hLandsideArea=GetTreeCtrl().InsertItem(LandsideArea->name,cni,FALSE,FALSE,m_hLandsideAreaRoot);
//	GetTreeCtrl().SetItemData(hLandsideArea,(DWORD)pData);	
//}

//void CLandsideMSView::EditLandsideAreaItem( CLandsideArea *LandsideArea )
//{
//	HTREEITEM hLandsideArea=GetTreeCtrl().GetChildItem(m_hLandsideAreaRoot);
//	while(hLandsideArea)
//	{
//		CNodeData* pNode = (CNodeData*)GetTreeCtrl().GetItemData(hLandsideArea);
//		if(pNode)
//		{
//			if (LandsideArea == (CLandsideArea *)pNode->dwData)
//			{
//				GetTreeCtrl().SetItemText(hLandsideArea,LandsideArea->name);
//				return;
//			}
//		}
//		hLandsideArea=GetTreeCtrl().GetNextItem(hLandsideArea,TVGN_NEXT);
//	}
//}

//void CLandsideMSView::DelLandsideAreaItem( CLandsideArea *LandsideArea )
//{
//	HTREEITEM hLandsideArea=GetTreeCtrl().GetChildItem(m_hLandsideAreaRoot);
//	while(hLandsideArea)
//	{
//		CNodeData* pNode = (CNodeData*)GetTreeCtrl().GetItemData(hLandsideArea);
//		if(pNode)
//		{
//			if (LandsideArea == (CLandsideArea *)pNode->dwData)
//			{
//				GetTreeCtrl().DeleteItem(hLandsideArea);
//				return;
//			}
//		}
//		hLandsideArea=GetTreeCtrl().GetNextItem(hLandsideArea,TVGN_NEXT);
//
//
//	}
//}

void CLandsideMSView::OnNewLandsideArea()
{
	GetEditContext()->OnCtxNewLandsideArea();
}

void CLandsideMSView::OnEditLandsideArea()
{
	CTreeItemLayoutObject node(&GetTreeCtrl(),m_selItem);
	if(!node.IsValid())
		return;

	if(LandsideLayoutObject* pObj = node.getObject())
	{
		if( pObj->GetType() == ALT_LAREA)
		{
			GetEditContext()->OnCtxEditLandsideArea((CLandsideArea*)pObj);
		}
	}
}

void CLandsideMSView::OnDelLandsideArea()
{
	CTreeItemLayoutObject node(&GetTreeCtrl(),m_selItem);
	if(!node.IsValid())
		return;

	if(LandsideLayoutObject* pObj = node.getObject())
	{
		if( pObj->GetType() == ALT_LAREA)
		{
			GetEditContext()->OnCtxDelLandsideArea((CLandsideArea*)pObj);
		}
	}
}

void CLandsideMSView::OnLandsideAreaColor()
{
	CTreeItemLayoutObject node(&GetTreeCtrl(),m_selItem);
	if(!node.IsValid())
		return;

	if(LandsideLayoutObject* pObj = node.getObject())
	{
		if( pObj->GetType() == ALT_LAREA)
		{
			GetEditContext()->OnCtxLandsideAreaColor((CLandsideArea*)pObj);
		}
	}
}

void CLandsideMSView::OnSetLandsideLayoutOptions()
{
	CDlgLandsideLayoutOptions dlg;
	if( dlg.DoModal() == IDOK )
	{
		InputLandside* pInput = GetDocument()->getARCport()->m_pInputLandside;
		pInput->getLayoutOption()->ReadData(-1);
	}
}
