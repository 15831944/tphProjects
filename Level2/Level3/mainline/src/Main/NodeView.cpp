// NodeView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include ".\MFCExControl\ARCBaseTree.h"
#include "NodeView.h"  
#include "3DCamera.h"
#include "3DGridOptionsDlg.h"
#include "3DView.h"
#include "ACCatPropertiesDlg.h"
#include "AnnualActivityDeductionDlg.h"
#include "BagDevDlg.h"
#include "BoardingCallDlg.h"
#include "CapitalBDDecisionDlg.h"
#include "CarScheduleSheet.h"
#include "ChildFrameSplit.h"
#include "CoolTreeEx.h"    
#include "DlgActivityDensity.h"
#include "DlgComments.h"
#include "DlgGatArp.h"
#include "DlgPaxTags.h"
#include "DlgProcCongArea.h" 
#include "DlgProcessorTags.h"
#include "DlgRetailPackFee.h"
#include "DlgStandAssignment.h"
#include "FlightDialog.h"
#include "Economic2Dialog.h"
#include "EconomicaDataTypeDlg.h"
#include "EconomicDialog.h"
#include <CommonData/Fallback.h>
#include "FlightScheduleDlg.h"
#include "Floor2.h"
#include "FltPaxDataDlg.h"
//#include "GateAssignDlg.h"
#include "Htmlhelp.h"
#include "HubDataDlg.h"
#include "InterestOnCapitalDlg.h"
#include "LeadlagTimeDlg.h"
#include "MainFrm.h"
#include "MaintenanceDialog.h"
#include "MobileCountDlg.h"
#include "MoveSideWalkSheet.h"
#include "NameListMappingDlg.h"
#include "PaxBulkListDlg.h"
#include "PaxDispPropDlg.h"
#include "PaxDistDlg.h"
#include "PaxFlowByProcDlg.h"
#include "PaxFlowDlg.h"
#include "PaxTypeDefDlg.h"
#include "PipeDefinitionDlg.h"
#include "ProbDistDlg.h"
#include "ProcAssignDlg.h"
#include "ProcDataSheet.h"
#include "ProcessDefineDlg.h"
#include "ProcPropDlg.h"
#include "ProcToResPoolDlg.h"
#include "reportparamdlg.h"
#include "ResDispPropDlg.h"
#include "DlgPro2Name.h"
#include "resourcepooldlg.h"
#include "ServiceTimeDlg.h"
#include "ShapesManager.h"
#include "SimEngSettingDlg.h"
#include "StationLinkerDlg.h"
#include "TermPlanDoc.h"
#include "TVN.h"
#include "../common/UnitsManager.h"
#include "UsedProcInfo.h"
#include "UsedProcInfoBox.h"
#include "UtilitiesDialog.h"
#include "DlgAirportProperties.h"
#include "DlgStructureProp.h"
#include "AircraftDispPropDlg.h"
#include "DlgAircraftTags.h"

#include "DlgProcDisplayProperties.h"
#include "DlgWallShapeProp.h"
#include "LandProcessor2.h"
#include "LandsideProcDlg.h"
#include "LandsideDocument.h"

#include "../Common/FlightManager.h"
#include "DlgFlightDB.h"
#include "DlgDBAirline.h"
#include "DlgDBAircraftTypes.h"
#include "DlgDBAirports.h"
#include "DlgScheduleAndRostContent.h"


#include "../common/template.h"
//#include "../common/infozip.h"
#include "../Common/ZipInfo.h"
#include "../common/winmsg.h"
#include "../common/ACTypesManager.h"
#include "../common/AirlineManager.h"
#include "../Common/AirportDatabase.h"
#include "../engine/AirfieldProcessor.h"
#include "../engine/terminal.h"
#include "../engine/proclist.h"
#include "../engine/dep_srce.h"
#include "../engine/dep_sink.h"
#include "../engine/gate.h"
#include "../engine/floorchg.h"
#include "../engine/lineproc.h"
#include "../engine/baggage.h"
#include "../engine/hold.h"
#include "../engine/MovingSideWalk.h"
#include "../engine/Barrier.h"
#include "../engine/ElevatorProc.h"
#include "../engine/Conveyor.h"
#include "../engine/IntegratedStation.h"
#include "../engine/Stair.h"
#include "../engine/Escalator.h"
#include "../engine/ContourTree.h"
#include "../engine/RunwayProc.h"
#include "../engine/Airside/Simulation.h"
#include "../Engine/Airside/Flight.h"
#include "../Engine/Airside/Event.h"

#include "../inputs/flight.h"
#include "../inputs/schedule.h"
#include "../inputs/fltdata.h"
#include "../inputs/paxdata.h"
#include "../inputs/RailWayData.h"
#include "../inputs/AllCarSchedule.h"
#include "../inputs/AirsideInput.h"
#include "../Common/LatLong.h"
#include "../Inputs/AirportInfo.h"
#include "../Common/LatLong.h"
#include "../inputs/AirportInfo.h"
#include "../inputs/SimParameter.h"
#include "../reports/ReportParameter.h"
#include "../Inputs/FlightPlans.h"
#include "../InputAirside/CParkingStandBuffer.h"

#include "../AirsideGUI/NodeViewDbClickHandler.h"
#include "../AirsideGUI/ArrivalInitPara.h"

#include "DlgFlightPlan.h"
#include "AirsideRunwayDlg.h"
#include "AirsideTaxiwayDlg.h"
#include "AirsideGateDlg.h"
#include "AirsideDeicePadDlg.h"
#include "AirsideWaypointDlg.h"
#include "AirsideSectorDlg.h"

#include "AirsideContourDlg.h"
#include "AirsideHoldDlg.h"
#include "AirsideSurfaceDlg.h"
#include "AirsideStructureDlg.h"
#include "DlgVehicleTags.h" 
#include "DlgArrivalGateAssignment.h"
#include "DlgDepartureGateAssignment.h"
#include "Placement.h"
#include "Resource.h"
#include "DlgOperatingDoorSpecification.h"
#include "CalloutDispSpecDlg.h"
#include "DlgAffinityGroup.h"
#include "DlgRetailProductsStocking.h"
#include "DlgRetailFees.h"
#include "DlgExportNames.h"

#include <deque>
#include <map>
#include "Common/FloorChangeMap.h"
#include "Inputs/Procdata.h"
#include "DlgAircraftEntrySpec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DECLARE_FLIGHTTYPE_SELECT_CALLBACK()

#define ID_NEW_ARP	(WM_USER + 0x0100)
#define ID_ENABLECONTOUREDIT 300
#define IDC_BUTTON_MOVEUPFLOOR 301
#define IDC_BUTTON_MOVEDOWNFLOOR 302
#define IDC_TERMINAL_TREE 1
using namespace ns_AirsideInput;
using namespace airside_engine;
/////////////////////////////////////////////////////////////////////////////
// CNodeView

void GetDefaultStationDisplayProperties(CProcessor2::CProcDispProperties* pPDP);

static void GetDefaultProcDispProperties(CDefaultDisplayProperties* pDDP, CProcessor2::CProcDispProperties* pPDP)
{
	pPDP->bDisplay[PDP_DISP_SHAPE] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcShapeOn);
	pPDP->bDisplay[PDP_DISP_SERVLOC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcServLocOn);
	pPDP->bDisplay[PDP_DISP_QUEUE] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcQueueOn);
	pPDP->bDisplay[PDP_DISP_INC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcInConstraintOn);
	pPDP->bDisplay[PDP_DISP_OUTC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcOutConstraintOn);
	pPDP->bDisplay[PDP_DISP_PROCNAME] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcNameOn);
	pPDP->color[PDP_DISP_SHAPE] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcShapeColor);
	pPDP->color[PDP_DISP_SERVLOC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcServLocColor);
	pPDP->color[PDP_DISP_QUEUE] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcQueueColor);
	pPDP->color[PDP_DISP_INC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcInConstraintColor);
	pPDP->color[PDP_DISP_OUTC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcOutConstraintColor);
	pPDP->color[PDP_DISP_PROCNAME] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcNameColor);
}


IMPLEMENT_DYNCREATE(CNodeView, CView)

CNodeView::CNodeView()
:m_pDragNode(NULL)
,m_pDlgFlow(NULL)
,m_pDlgProcessDefine(NULL)
,m_pDlgScheduleRosterContent(NULL)
,m_pCopyReportPara(NULL),
callback_ver(CALLBACK_PICKXYFROMMAP)
{
}

CNodeView::~CNodeView()
{
	delete m_pDlgFlow; m_pDlgFlow = NULL;

	delete m_pDlgProcessDefine; m_pDlgProcessDefine = NULL;

	if (m_pDlgScheduleRosterContent)
	{
		delete m_pDlgScheduleRosterContent;
		m_pDlgScheduleRosterContent = NULL;
	}
//	delete m_pTree; m_pTree = NULL;
}


BEGIN_MESSAGE_MAP(CNodeView, CView)
	ON_MESSAGE(ITEMEXPANDING,OnItemExpanding)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()

	ON_COMMAND(ID_CTX_RENAME, OnCtxRename)
	ON_COMMAND(ID_SUBMENU_RENAME,OnRenameProc2)
	ON_COMMAND(ID_CTX_NEWPROCESSOR, OnNewProcessor)
	ON_COMMAND(ID_CTX_PROCPROPERTIES, OnCtxProcproperties)
	ON_COMMAND(ID_CTX_DELETEPROC, OnCtxDeleteProc)
	ON_COMMAND(ID_CTX_DELETEFLOOR, OnCtxDeleteFloor)
	ON_COMMAND(ID_CTX_COMMENTS, OnCtxComments)
	ON_COMMAND(ID_RAILWAYLAYOUT_DEFINERAILWAYLAYOUT, OnRailwaylayoutDefine)
	ON_COMMAND(ID_DEFINE_PIPE_PROPORITY,OnPipeDefine)
	ON_COMMAND(IDC_CTX_CHANGE_TO_POINT, OnCtxChangeToPoint)
	ON_COMMAND(IDC_CTX_CHANGE_TO_FLOORCHANGE, OnCtxChangeToFloorchange)
	ON_COMMAND(IDC_CTX_CHANGE_TO_GATE, OnCtxChangeToGate)
	ON_COMMAND(IDC_CTX_CHANGE_TO_SINK, OnCtxChangeToSink)
	ON_COMMAND(IDC_CTX_CHANGE_TO_SOURCE, OnCtxChangeToSource)
	ON_COMMAND(ID_LAYOUTEXPORT, OnLayoutexport)
	ON_COMMAND(ID_LAYOUTIMPORT_APPEND, OnLayoutimportAppend)
	ON_COMMAND(ID_LAYOUTIMPORT_REPLACE, OnLayoutimportReplace)
	ON_COMMAND(ID_CTX_ARP, OnCtxArp)
	ON_COMMAND(ID_CTX_PICKFROMMAP, OnCtxPickXYFromMap)
	ON_COMMAND(ID_CONTOUR_ADDNEXTCONTOUR, OnContourAddNextContour)
	ON_COMMAND(ID_CTX_ADDSTRUCTURE, OnAddStructure)
	ON_COMMAND(ID_CTX_ADDSURFACEAREA, OnAddSurfaceArea)
	ON_COMMAND(ID_CTX_PROCDISPPROPERTIES, OnCtxProcdispproperties)
	ON_COMMAND(ID_ENABLECONTOUREDIT, OnEnableCoutourEdit)
	ON_COMMAND_RANGE(ID_NEW_ARP,ID_NEW_ARP ,OnMenuNewProc)
	
	ON_UPDATE_COMMAND_UI(ID_CTX_FLOORVISIBLE, OnUpdateFloorVisible)
	ON_UPDATE_COMMAND_UI(ID_CTX_GRIDVISIBLE, OnUpdateGridVisible)
	ON_UPDATE_COMMAND_UI(ID_CTX_MONOCHROME, OnUpdateFloorMonochrome)
	ON_UPDATE_COMMAND_UI(ID_CTX_SHOWHIDEMAP, OnUpdateShowHideMap)
	ON_UPDATE_COMMAND_UI(ID_CTX_OPAQUEFLOOR, OnUpdateOpaqueFloor)
	ON_UPDATE_COMMAND_UI(ID_CTX_DELETEFLOOR, OnUpdateFloorsDelete)
	ON_UPDATE_COMMAND_UI(ID_ENABLECONTOUREDIT, OnUpdateEnableCoutourEdit)
	ON_UPDATE_COMMAND_UI(ID_CTX_MOVEUPFLOOR, OnUpdateMoveUpFloor)
	ON_UPDATE_COMMAND_UI(ID_CTX_MOVEDOWNFLOOR, OnUpdateMoveDownFloor)
	ON_COMMAND(ID_GATEMENU_ADDGATE, OnCtxAddGate)
	ON_COMMAND(ID_GATE_PROCDISPPROPERTIES, OnGateProcdispproperties)
	ON_COMMAND(ID_WALLSHAPE_ADDWALLSHAPE, OnWallshapeAddwallshape)
	ON_COMMAND(IDC_BUTTON_MOVEUPFLOOR, OnBtnMoveUpFloor)
	ON_COMMAND(IDC_BUTTON_MOVEDOWNFLOOR, OnBtnMoveDownFloor)
	ON_COMMAND(ID_CTX_MOVEUPFLOOR, OnBtnMoveUpFloor)
	ON_COMMAND(ID_CTX_MOVEDOWNFLOOR, OnBtnMoveDownFloor)

	ON_COMMAND(ID_SUBMENU_PLACEALIGNMENTLINE,OnPlaceMarkerLine)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodeView drawing

void CNodeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CNodeView diagnostics

#ifdef _DEBUG
void CNodeView::AssertValid() const
{
	CView::AssertValid();
}

void CNodeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTermPlanDoc* CNodeView::GetDocument()  //non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*) m_pDocument;
}
#endif //_DEBUG

void CNodeView::RenameSelected()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if(hItem != NULL)
	{
		GetTreeCtrl().EditLabel(hItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNodeView message handlers

BOOL CNodeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CView::PreCreateWindow(cs))
		return FALSE;
	
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS|WS_CLIPCHILDREN;
	
	return TRUE;
}

void CNodeView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	//initialize image list
	//m_ilNodes.Create(If_NODEIMAGES, 16, 1, RGB(255,0,255));
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

	if(GetDocument()->m_systemMode==EnvMode_Terminal)
	{
		//Add Nodes from Document
		for (int i = 0; i < GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildCount(); i++)
			AddItem((CTVNode*) GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildByIdx(i));

		if (GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildCount() > 0)
			GetTreeCtrl().SelectSetFirstVisible(((CTVNode*)GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildByIdx(0))->m_hItem);
	}

	m_btnMoveFloorUp.LoadBitmaps(IDB_MOVE_UP_FLOOR, IDB_MOVE_UP_FLOOR1, IDB_MOVE_UP_FLOOR, IDB_MOVE_UP_FLOOR2);
	m_btnMoveFloorDown.LoadBitmaps(IDB_MOVE_DOWN_FLOOR, IDB_MOVE_DOWN_FLOOR1, IDB_MOVE_DOWN_FLOOR, IDB_MOVE_DOWN_FLOOR2);
}

void CNodeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(GetDocument()->m_systemMode==EnvMode_Terminal)
	{
		if(lHint == NODE_HINT_NEWNODE)
		{
			CTVNode* pNode = (CTVNode*) pHint;
			CTVNode* pAfterNode = NULL;
			if (pNode->Parent() == GetDocument()->LayoutNode() && GetDocument()->UnderConstructionNode() == pNode)
			{
				pAfterNode = GetDocument()->FloorsNode();
			}
			if(pNode != NULL && !pNode->IsLeaf())
			{
				if (pNode->Parent() == GetDocument()->FloorsNode())
				{
					AddSortItem(pNode,TVI_FIRST);
				}
				else
				{
					AddItem(pNode,pAfterNode);
				}
				
			}
		}
		else if(lHint == NODE_HINT_REFRESHNODE)//delete node
		{
			CTVNode* pNode = (CTVNode*) pHint;
			if(pNode != NULL && pNode->m_hItem != NULL)
			{
				RefreshItem(pNode);
  				//DeleteAllChildren(pNode->m_hItem);
  				//for(int i=0; i<pNode->GetChildCount(); i++)
  				//{
  				//	CTVNode* pChild = (CTVNode*) pNode->GetChildByIdx(i);
  				//	if(!pChild->IsLeaf())
  				//	AddItem(pChild);
  				//}
  				//GetTreeCtrl().EnsureVisible(pNode->m_hItem);
			}
		}

		if (GetDocument()->m_simType == SimType_MontCarlo)
		{
			if (lHint == UPDATETREE_TERMINAL)
			{
				if (!GetTreeCtrl().GetCount())
				{ 				
					for (int i=0; i<GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildCount(); i++)
					AddItem((CTVNode*) GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildByIdx(i));

					if(GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildCount() > 0)
						GetTreeCtrl().SelectSetFirstVisible(((CTVNode*)GetDocument()->m_msManager.GetTerminalRootTVNode()->GetChildByIdx(0))->m_hItem);
				}
			}
		}
	}
	
	CView::OnUpdate(pSender, lHint, pHint);
	/////////////////////////////////////////////////
	//send message to tree ctrl to make h_scroolbar scroll to end_left 	
	GetTreeCtrl().SendMessage( WM_HSCROLL, MAKEWPARAM(SB_LEFT,0),(LPARAM)NULL );
}

LRESULT CNodeView::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;
	//only allow editing of the following nodes:
	//CTVNode with IDR_CTX_FLOOR as resID
	//CTVNode with IDR_CTX_PROCESSOR as resID
	//CTVNode with IDR_CTX_PROCESSORGROUP as resID
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	if (pNode->m_nIDResource == IDR_CTX_FLOOR || 
		pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP ||
		pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE ||
		pNode->m_nIDResource == IDR_CTX_PROCESSOR ||
		pNode->m_nIDResource==IDR_CTX_AIRPORTMENU || pNode->m_nIDResource == IDR_CTX_LEVEL)
	{
		*pResult = FALSE;
		return 0;
	}
	*pResult = TRUE;
	return 0;
}

LRESULT CNodeView::OnEndLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;
	//return immediately if the name is null (no rename).
	if(pDispInfo->item.pszText == NULL)
		return 0;
	
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	BOOL bNameOK = FALSE;
	if(pNode->m_nIDResource == IDR_CTX_FLOOR || pNode->m_nIDResource == IDR_CTX_LEVEL) {
		//get "floors" node to check if name already exists
		CTVNode* pParentNode=GetDocument()->FloorsNode();

		ASSERT(pParentNode != NULL);
		if(pParentNode->GetChildByName(pDispInfo->item.pszText) != NULL) {
			//name already exists
			*pResult = FALSE;
			return 0;
		}
		else {
			pNode->Name(pDispInfo->item.pszText);
			//set floor name
			GetDocument()->GetCurModeFloor().m_vFloors[(int)pNode->m_dwData]->FloorName(pDispInfo->item.pszText);
			GetDocument()->GetCurModeFloor().saveDataSet(GetDocument()->m_ProjInfo.path, true);
			
		}
	}
	else if(pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP
			|| pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE 
			||pNode->m_nIDResource == IDR_CTX_PROCESSOR) 
	{
		CTVNode* pParentNode = (CTVNode*) pNode->Parent();
		ASSERT(pParentNode != NULL);
		if(pParentNode->GetChildByName(pDispInfo->item.pszText) != NULL) {
			//name already exists
			*pResult = FALSE;
			return 0;
		}
		//build proc group name string by moving up the tree
		int nLevel = 0;
		CString sOldGroupFullName = pNode->Name();
		CString strOldName(sOldGroupFullName);
		CString sNewName =pDispInfo->item.pszText;
		//get "placements" node 
		if(GetDocument()->m_systemMode == EnvMode_Terminal)
		{
			CTVNode* pPlacementsNode = GetDocument()->m_msManager.FindNodeByName(IDS_TVNN_PLACEMENTS);
			ASSERT(pPlacementsNode != NULL);
			while(pParentNode != pPlacementsNode)
			{
				nLevel++;
				sOldGroupFullName = pParentNode->Name() + "-" + sOldGroupFullName;
				sNewName = pParentNode->Name() + "-" + sNewName;
				pParentNode = (CTVNode*) pParentNode->Parent();
			}
		}
		else if(GetDocument()->m_systemMode == EnvMode_AirSide|| 
			GetDocument()->m_systemMode ==EnvMode_LandSide)
		{
			while(!GetDocument()->m_msManager.FindNodeByName(pParentNode->Name()))
			{
				nLevel++;
				sOldGroupFullName = pParentNode->Name() + "-" + sOldGroupFullName;
				sNewName = pParentNode->Name() + "-" + sNewName;
				pParentNode = (CTVNode*) pParentNode->Parent();
			}
		}
		
		ProcessorID id;
		id.SetStrDict(GetInputTerminal()->inStrDict);
		id.setID(sOldGroupFullName);

		CUsedProcInfoBox *box = new CUsedProcInfoBox();
		CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),&id);
		box->AddInfo(usedProcInfo);
		if(!box->IsNoInfo())
		{
			if(MessageBox("Rename processors,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
			{
				VERIFY(GetDocument()->RenameProcessorGroup(id, nLevel, sNewName));
				delete box;
				pNode->Name(pDispInfo->item.pszText);
				GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path,true);
				*pResult = TRUE;
				GetDocument()->UpdateAllViews(this);
				return 0;
			}
			else
			{
				delete box;
				*pResult = FALSE;
				return 0;
			}
		}
		else
		{
			int nPos = sOldGroupFullName.ReverseFind('-');
			CString strParentName = sOldGroupFullName.Left(nPos);
			ProcessorID idName;
			idName.SetStrDict(GetInputTerminal()->inStrDict);
			idName.setID(strParentName);

			ProcessorID newID;
			newID.SetStrDict(GetInputTerminal()->inStrDict);
			newID.setID(sNewName);

			bool bRelation = false;
			CUsedProcInfoBox *newbox = new CUsedProcInfoBox();
			CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),&newID);
			newbox->AddInfo(usedProcInfo);
			bRelation = newbox->IsNoInfo();
			delete newbox;

			GroupIndex gIndex = GetInputTerminal()->GetProcessorList()->getGroupIndex(idName);
			if (bRelation && (gIndex.start != gIndex.end || idName.idFits(newID)))
			{
				if(MessageBox("Rename processors,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
				{
					VERIFY(GetDocument()->RenameProcessorGroup(id, nLevel, sNewName));
					delete box;
					pNode->Name(pDispInfo->item.pszText);
					GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path,true);
					*pResult = TRUE;
					GetDocument()->UpdateAllViews(this);
					return 0;
				}
				else
				{
					delete box;
					*pResult = FALSE;
					return 0;
				}
			}
			if(box->DoModal()==IDOK)	
			{
				GetDocument()->UpdateAllViews(this);
				*pResult = FALSE;
				return 0;
			}
			else
			{
				delete box;
				*pResult = FALSE;
				return 0;
			}
		}
	}		
	else if(pNode->m_nIDResource==IDR_CTX_AIRPORTMENU)//if the Node is airport
	{
		CTVNode* pParentNode = GetDocument()->m_msManager.FindNodeByName(IDS_TVNN_AIRPORTS);
		ASSERT(pParentNode != NULL);
		if(pParentNode->GetChildByName(pDispInfo->item.pszText) != NULL) 
		{
			//name already exists
			*pResult = FALSE;
			return 0;
		}
		else 
		{
			CString strItemText= pDispInfo->item.pszText;
			if (GetDocument()->m_msManager.m_msImplAirSide.GetProcNode(strItemText)!=NULL)
			{
				*pResult=FALSE;
				return 0;
			}else
			{

				CString strOldName=pNode->Name();
				pNode->Name(pDispInfo->item.pszText);
//				GetDocument()->m_AirsideInput.m_csAirportName=pDispInfo->item.pszText;	
//					GetDocument()->m_AirsideInput.
				CTVNode *pSelectedNode=GetDocument()->GetSelectedNode();
				GetDocument()->GetTerminal().GetAirsideInput()->RenameAirportName(GetDocument()->m_ProjInfo.path,strOldName,pDispInfo->item.pszText,false);
			}
		}
	}

	GetDocument()->UpdateAllViews(this);
	*pResult = TRUE;  // return FALSE to cancel edit, TRUE to accept it.
	return 0;
}

void CNodeView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CView::OnLButtonUp(nFlags, point);
	//// TRACE("OnLButtonUp called from CNodeView\n");
}


void CNodeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
	
	InputTerminal* pInTerm = (InputTerminal*)&GetDocument()->GetTerminal();
	// ?was a node clicked?
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	if((hItem != NULL)&& (TVHT_ONITEM & uFlags))
	{
		CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData(hItem);
		if(pNode != NULL)
			GetDocument()->SelectNode(pNode);
	}	
}


void CNodeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	CNewMenu menu, *pCtxMenu = NULL;
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(cliPt, &uFlags);
	if((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( hItem );
		
		if(pNode != NULL)
		{
			GetDocument()->SelectNode(pNode);
			if(pNode->m_nIDResource != NULL )
			{
				menu.LoadMenu(pNode->m_nIDResource);

				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

				UINT nMenuID = 0;
				CString strMenuCaption;

				if (pNode->Name() == _T("Runways"))
				{
					nMenuID = ID_NEW_ARP + 1;
					strMenuCaption = _T("Add Runway");
				}
				else if (pNode->Name() == _T("Taxiways"))
				{
					nMenuID = ID_NEW_ARP + 2;
					strMenuCaption = _T("Add Taxiway");
				}
				else if (pNode->Name() == _T("Gates(Parking Stands)"))
				{
//					nMenuID = ID_NEW_ARP + 3;
//					strMenuCaption = _T("Add Gates");
				}
				else if (pNode->Name() == _T("Aircraft Stands"))
				{
					nMenuID = ID_NEW_ARP + 3;
					strMenuCaption = _T("Add Aircraft Stand");
				}
				else if (pNode->Name() == _T("Deice Pads"))
				{
					nMenuID = ID_NEW_ARP + 4;
					strMenuCaption = _T("Add Deice Pad");
				}
				else if (pNode->Name() == _T("Airfield Network Nodes"))
				{
					nMenuID = ID_NEW_ARP + 5;
					strMenuCaption = _T("Add Airfield Netword Node");
				}
				else if (pNode->Name() == _T("Waypoints"))
				{
					nMenuID = ID_NEW_ARP + 6;
					strMenuCaption = _T("Waypoint");
				}
				else if(pNode->Name() == _T("Contours"))
				{
					nMenuID = ID_NEW_ARP + 7;
					strMenuCaption = _T("Add Contour");
				}
				else if(pNode->Name()==_T("Aprons"))
				{
					nMenuID=ID_NEW_ARP+8;
					strMenuCaption=_T("Add Apron");
				}
				else if(pNode->Name() == _T("Holds"))
				{
					nMenuID = ID_NEW_ARP +9;
					strMenuCaption = _T("Add Hold");
				}
				else if(pNode->Name() == _T("Airspace Sectors")){
					nMenuID = ID_NEW_ARP + 10;
					strMenuCaption = _T("Add Sector");
				}
				
				if (nMenuID && !strMenuCaption.IsEmpty())
					pCtxMenu->InsertODMenu(0, strMenuCaption,MF_STRING | MF_BYPOSITION, nMenuID,IDB_BITMAP_DLG_DLBUTTON);

				if(pNode->Name() == _T("Contours"))
				{
					pCtxMenu->InsertMenu(0, MF_STRING | MF_BYPOSITION, ID_ENABLECONTOUREDIT, _T("Chang To Render Mode"));
				}

			}
		}
		//Landside Node
		
		if(pCtxMenu != NULL)
		{
			UpdatePopMenu(this, pCtxMenu);
			pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, AfxGetMainWnd() );
		}
	}

}

void CNodeView::UpdatePopMenu(CCmdTarget* pThis, CMenu* pPopMenu)
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

void CNodeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnRButtonDown(nFlags, point);
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	if((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( hItem );
		if(pNode != NULL)
			GetDocument()->SelectNode(pNode);
		GetTreeCtrl().SelectItem(hItem);
	}
}

void CNodeView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Send WM_CONTEXTMENU to self
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	// Mark message as handled and suppress default handling
	*pResult = 1;
}

void CNodeView::OnCtxRename() 
{
	if(GetDocument()->m_systemMode!=EnvMode_Terminal)
		return;

	HTREEITEM hItem = GetDocument()->GetSelectedNode()->m_hItem;
	if (hItem != NULL)
		GetTreeCtrl().EditLabel(hItem);
}

LRESULT CNodeView::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
	if (GetDocument()->m_bCanResposeEvent && pNMTreeView->itemNew.hItem)
	{
		//CTVNode* pNode = FindNode(pNMTreeView->itemNew.hItem);
		CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( pNMTreeView->itemNew.hItem);
		if(pNode != NULL)
			GetDocument()->UpdateAllViews(this, NODE_HINT_SELCHANGED, (CObject*) pNode);
		if(pNode != NULL && pNode != GetDocument()->GetSelectedNode())
			GetDocument()->SelectNode(pNode);
	}

	m_btnMoveFloorUp.ShowWindow(SW_HIDE);
	m_btnMoveFloorDown.ShowWindow(SW_HIDE);
	HTREEITEM hParent = m_wndTreeCtrl.GetParentItem(pNMTreeView->itemNew.hItem);
	CString strParent = m_wndTreeCtrl.GetItemText(hParent);
	CString strSel = m_wndTreeCtrl.GetItemText(pNMTreeView->itemNew.hItem);
	if(strParent == "Floors") // selected tree node data is floor.
	{
		CRect rect1;
		if(m_wndTreeCtrl.GetItemRect(pNMTreeView->itemNew.hItem,&rect1,TRUE))
		{
			if(rect1.Width()<180)
			{
				m_btnMoveFloorUp.SetWindowPos(&wndTop, 200, rect1.top-1, 18, 9, SWP_DRAWFRAME);
				m_btnMoveFloorDown.SetWindowPos(&wndTop, 200, rect1.top+8, 18, 9, SWP_DRAWFRAME);
			}
			else
			{
				m_btnMoveFloorUp.SetWindowPos(&wndTop, rect1.right+10, rect1.top-1, 18, 9, SWP_DRAWFRAME);
				m_btnMoveFloorDown.SetWindowPos(&wndTop, rect1.right+10, rect1.top+8, 18, 9, SWP_DRAWFRAME);
			}

// 			m_btnMoveFloorUp.ShowWindow(SW_SHOW);
// 			m_btnMoveFloorUp.EnableWindow(FALSE);
// 			m_btnMoveFloorDown.ShowWindow(SW_SHOW);
// 			m_btnMoveFloorDown.EnableWindow(FALSE);
// 			if(m_wndTreeCtrl.GetPrevSiblingItem(pNMTreeView->itemNew.hItem) != NULL)
// 				m_btnMoveFloorUp.EnableWindow(TRUE);
// 			if(m_wndTreeCtrl.GetNextSiblingItem(pNMTreeView->itemNew.hItem) != NULL)
// 				m_btnMoveFloorDown.EnableWindow(TRUE);
		}	
	}
	return 0;
//	*pResult = 0;
}



void CNodeView::RefreshProcessorItem(CTVNode* pNode,CTVNode* pRootNode,const ProcessorID& procID)
{
	ASSERT(pNode);
 	ProcessorID id = procID;

	CTVNode* pTempNode = pNode;
	std::deque<CTVNode*> vList;
	std::deque<CString> vName;
	std::stack<CTVNode*> vNodeList;
	std::vector<CTVNode*> vNode;
	while(id.idLength())
	{
		vName.push_front(id.GetLeafName());
		id.clearLevel(id.idLength() - 1);
	}

 	while (pNode && pNode != pRootNode )
 	{
		vNode.push_back(pNode);
 		vNodeList.push(pNode);
 		pNode = (CTVNode*)pNode->Parent();
 	}

	vList.push_front(pRootNode);
	
	CTVNode* pDeleteNode = NULL;
	while(vList.size())
	{
		CTVNode* tvNode = vList[0];
		if (vName.size() == 0)
		{
			while(vNodeList.size())
			{
				if (vNodeList.top()->GetLeafCount() == 1)
				{
					pDeleteNode = vNodeList.top();
					break;
				}
				vNodeList.pop();
			}
			break;
		}
		
		CTVNode* pNodeItem = NULL;
		CString sNodeName = vName[0];
		if (vNodeList.size())
		{ 
			pNodeItem = vNodeList.top();
		}
		
		
		
		vName.pop_front();
		vList.pop_front();
		vNodeList.pop();
		
		CTVNode* pChildNode = (CTVNode*)tvNode->GetChildByName(sNodeName);
	
		if (pChildNode)
		{
			vList.push_front(pChildNode);
			if (pNodeItem && pChildNode != pNodeItem && pNodeItem->GetLeafCount() == 1 && !pDeleteNode)
			{
				pDeleteNode = pNodeItem;
			}
		}
		else
		{
			if (pNodeItem && pNodeItem->GetLeafCount() == 1 && std::find(vNode.begin(),vNode.end(),tvNode) != vNode.end())
			{
				pNodeItem->Name(sNodeName);
				vList.push_front(pNodeItem);
			}
			else if(pNodeItem)
			{
				CTVNode* pInsertNode = NULL;
				if (vName.size() == 0)
				{
					pInsertNode = new CTVNode(sNodeName, pNodeItem->m_nIDResource);
					pInsertNode->m_dwData = pNodeItem->m_dwData;
				}
				else
				{
					pInsertNode = new CTVNode(sNodeName);
					pInsertNode->m_nIDResource = pNodeItem->m_nIDResource;
					
				}

				tvNode->AddChild(pInsertNode);
				if (pNodeItem->GetLeafCount() == 1 && !pDeleteNode)
				{
					pDeleteNode = pNodeItem;
				}

				vList.push_front(pInsertNode);
			}
			else
			{
				CTVNode* pInsertNode = NULL;
				if (vName.size() == 0)
				{
					pInsertNode = new CTVNode(sNodeName, pTempNode->m_nIDResource);
					pInsertNode->m_dwData = pTempNode->m_dwData;
				}
				else
				{
					pInsertNode = new CTVNode(sNodeName);
					pInsertNode->m_nIDResource = tvNode->m_nIDResource;
				}

				tvNode->AddChild(pInsertNode);
				vList.push_front(pInsertNode);
			}
		}
	}
	if (pDeleteNode && pDeleteNode->Parent())
	{
		pDeleteNode->Parent()->RemoveChild(pDeleteNode);
	}
	GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)pRootNode);
}

void CNodeView::AddSortItem(CTVNode* pNode,HTREEITEM hInsertAfter)
{
	HTREEITEM hParent = NULL;
	if(pNode->Parent() != NULL)
		hParent = ((CTVNode*) pNode->Parent())->m_hItem;

	if(hParent == NULL)
		hParent = TVI_ROOT;

	// add the node
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nImage = pNode->m_iImageIdx+ID_IMAGE_COUNT;
	cni.nImageSeled = pNode->m_iImageIdx+ID_IMAGE_COUNT;
	pNode->m_hItem = GetTreeCtrl().InsertItem(pNode->Name(),cni,FALSE,FALSE,hParent,hInsertAfter);
	GetTreeCtrl().SetItemData( pNode->m_hItem, (long)pNode );
	if(pNode->m_eState == CTVNode::expanded)
	{
		// add all children and expand the node
		for(int i=0; i<pNode->GetChildCount(); i++)
		{
			AddItem((CTVNode*) pNode->GetChildByIdx(i));
		}
		GetTreeCtrl().Expand(pNode->m_hItem, TVE_EXPAND);
	}
	else
	{
		// add a dummy child if the node has children
		cni.nImage = 0+ID_IMAGE_COUNT;
		cni.nImageSeled = 0+ID_IMAGE_COUNT;
		if(pNode->GetChildCount() != 0)
			//	GetTreeCtrl().InsertItem("error", 0+ID_IMAGE_COUNT, 0+ID_IMAGE_COUNT, pNode->m_hItem, TVI_LAST);
			GetTreeCtrl().InsertItem("error",cni,FALSE,FALSE,pNode->m_hItem,TVI_LAST);
	}
}

void CNodeView::RefreshItem(CTVNode* pNode)
{
	CTVNode* tvNode = NULL;
	std::deque<CTVNode*> vList;
	vList.push_front(pNode);
	
	while(!vList.empty()) 
	{
		CTVNode* tvNode = vList.front();

		vList.pop_front();

		//if tvnode is layout node, parent is root node.
		if (tvNode != GetDocument()->LayoutNode())
		{
			CTVNode* pParentNode = (CTVNode*)tvNode->Parent();

			if (pParentNode == NULL)
				continue;

			if (pParentNode->m_eState == CTVNode::collapsed)
				continue;
		}
		
		//update node item 
		if(tvNode->m_hItem)
		{
			GetTreeCtrl().SetItemText(tvNode->m_hItem,tvNode->Name());
			GetTreeCtrl().SetItemImage(tvNode->m_hItem,tvNode->m_iImageIdx + ID_IMAGE_COUNT,tvNode->m_iImageIdx + ID_IMAGE_COUNT);
		}
		else // insert node item
		{
			AddSortItem(tvNode,TVI_SORT);
		//	GetDocument()->SelectNode(tvNode);
		}
		
		if (tvNode->m_eState == CTVNode::collapsed)
			continue;
		
		//delete tree item
		for (int i = 0; i < (int)tvNode->m_vItem.size(); i++)
		{
			GetTreeCtrl().DeleteItem(tvNode->m_vItem.at(i));
		}
		tvNode->clearNode();
		
		//push node
		for(int i=0; i<tvNode->GetChildCount(); i++) 
		{
			vList.push_front((CTVNode*)tvNode->GetChildByIdx(i));
		}
	}
}

HTREEITEM CNodeView::AddItem(CTVNode* pNode, CTVNode* pAfterNode)
{
	HTREEITEM hParent = NULL;
	if(pNode->Parent() != NULL)
		hParent = ((CTVNode*) pNode->Parent())->m_hItem;
	
	if(hParent == NULL)
		hParent = TVI_ROOT;

	HTREEITEM hInsertAfter = TVI_LAST;
	if(pAfterNode != NULL) 
	{
		hInsertAfter = pAfterNode->m_hItem;
		//ensure that this item is a child of hParent
		BOOL bFound = FALSE;
		if(GetTreeCtrl().ItemHasChildren(hParent))
		{
			HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hParent);
			while (hChildItem != NULL) 
			{
				if(hChildItem == hInsertAfter)
				{
					bFound = TRUE;
					if (pAfterNode->m_hItem)
					{
						hInsertAfter = pAfterNode->m_hItem;
					}
					else
					{
						hInsertAfter = TVI_LAST;
					}
					break;
				}
				hChildItem = GetTreeCtrl().GetNextSiblingItem(hChildItem);
			}
		}
		if(!bFound)
			hInsertAfter = TVI_LAST;
	}

	// add the node
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nImage = pNode->m_iImageIdx+ID_IMAGE_COUNT;
	cni.nImageSeled = pNode->m_iImageIdx+ID_IMAGE_COUNT;
	pNode->m_hItem = GetTreeCtrl().InsertItem(pNode->Name(),cni,FALSE,FALSE,hParent,hInsertAfter);
	GetTreeCtrl().SetItemData( pNode->m_hItem, (long)pNode );
	if(pNode->m_eState == CTVNode::expanded)
	{
		// add all children and expand the node
		for(int i=0; i<pNode->GetChildCount(); i++)
		{
			AddItem((CTVNode*) pNode->GetChildByIdx(i));
		}
		GetTreeCtrl().Expand(pNode->m_hItem, TVE_EXPAND);
	}
	else
	{
		// add a dummy child if the node has children
		cni.nImage = 0+ID_IMAGE_COUNT;
		cni.nImageSeled = 0+ID_IMAGE_COUNT;
		if(pNode->GetChildCount() != 0)
		//	GetTreeCtrl().InsertItem("error", 0+ID_IMAGE_COUNT, 0+ID_IMAGE_COUNT, pNode->m_hItem, TVI_LAST);
		GetTreeCtrl().InsertItem("error",cni,FALSE,FALSE,pNode->m_hItem,TVI_LAST);
	}
	
	
	return pNode->m_hItem;
	
}

CTVNode* CNodeView::FindNode(HTREEITEM hItem)
{
	std::deque<CTVNode*> vList;
	//put top level nodes into list..
	vList.push_front(GetDocument()->m_msManager.GetTerminalRootTVNode());
	while(vList.size() != 0) {
		CTVNode* pNode = vList[0];
		vList.pop_front();
		if(pNode->m_hItem == hItem)
			return pNode;
		for(int i=0; i<pNode->GetChildCount(); i++) {
			vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
		}
	}
	return NULL;
}

LRESULT CNodeView::OnItemExpanding(WPARAM wParam, LPARAM lParam) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( hItem );
//	*pResult = FALSE;
	
	if(pNMTreeView->action == TVE_EXPAND)
	{
		HTREEITEM hItem = GetTreeCtrl().GetFirstVisibleItem();
		DeleteAllChildren(pNode->m_hItem);
		// add the real children (only if they themselves have children
	
		for(int i=0; i<pNode->GetChildCount(); i++)
		{
			CTVNode* pChild = (CTVNode*) pNode->GetChildByIdx(i);
			if(!pChild->IsLeaf())
				AddItem(pChild);
		}
		pNode->m_eState = CTVNode::expanded;
		GetTreeCtrl().SelectSetFirstVisible(hItem);
		
	}
	else // pNMTreeView->action = TVE_COLLAPSE
	{
		DeleteAllChildren(pNode->m_hItem);
		// set the node's childrens' m_hItem to NULL
		for(int i=0; i<pNode->GetChildCount(); i++)
		{
			((CTVNode*) pNode->GetChildByIdx(i))->m_hItem = NULL;
		}
		pNode->m_eState = CTVNode::collapsed;
		// add a dummy child if the node has children
		COOLTREE_NODE_INFO cni;
		CARCBaseTree::InitNodeInfo(this,cni);
		cni.nImage = 0+ID_IMAGE_COUNT;
		cni.nImageSeled = 0+ID_IMAGE_COUNT;
		if(pNode->GetChildCount() != 0)
			GetTreeCtrl().InsertItem("error",cni,FALSE,FALSE,pNode->m_hItem,TVI_LAST);
	}
	return 0;
}

void CNodeView::DeleteFirstChild(HTREEITEM hItem)
{
	HTREEITEM hChild;
	if((hChild = GetTreeCtrl().GetChildItem(hItem)) != NULL)
		GetTreeCtrl().DeleteItem(hChild);
}

void CNodeView::DeleteAllChildren(HTREEITEM hItem)
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

void CNodeView::OnNewProcessor() 
{
	if(GetDocument()->GetSelectedNode() != NULL)
	{
		GetDocument()->OnNewProcessor(GetDocument()->GetSelectedNode());
	}
	
}

void CNodeView::OnUpdateFloorVisible(CCmdUI* pCmdUI) 
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	if(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/)
	{
		CFloor2* pFloor = GetDocument()->GetCurModeFloor().m_vFloors[(int)pNode->m_dwData];
		if(pFloor->IsVisible()) {
			pCmdUI->SetText(_T("Hide Floor"));
		}
		else {
			pCmdUI->SetText(_T("Show Floor"));
		}
	}
}

void CNodeView::OnUpdateGridVisible(CCmdUI* pCmdUI) 
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetDocument()->GetCurModeFloor().m_vFloors[(int)pNode->m_dwData];
	if(pFloor->IsGridVisible()) {
		pCmdUI->SetText(_T("Hide Grid"));
	}
	else {
		pCmdUI->SetText(_T("Show Grid"));
	}
	if(pFloor->IsVisible())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	
}

void CNodeView::OnUpdateFloorMonochrome(CCmdUI* pCmdUI)
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetDocument()->GetCurModeFloor().m_vFloors[(int)pNode->m_dwData];
	pCmdUI->SetCheck(pFloor->IsMonochrome()?1:0);
	pCmdUI->Enable(pFloor->IsVisible());
}

void CNodeView::OnUpdateOpaqueFloor(CCmdUI* pCmdUI)
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetDocument()->GetCurModeFloor().m_vFloors[(int)pNode->m_dwData];
	pCmdUI->SetCheck(pFloor->IsOpaque()?1:0);
	pCmdUI->Enable(TRUE);
}

void CNodeView::OnUpdateShowHideMap(CCmdUI* pCmdUI)
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	ASSERT(pNode != NULL  /*&&pNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetDocument()->GetCurModeFloor().m_vFloors[(int)pNode->m_dwData];
	if(pFloor->IsShowMap()) {
		pCmdUI->SetText(_T("Hide Map"));
	}
	else {
		pCmdUI->SetText(_T("Show Map"));
	}
	if(pFloor->IsMapValid())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CNodeView::PumpMessages()
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

InputTerminal* CNodeView::GetInputTerminal()const
{
    return (InputTerminal*)& ( const_cast<CNodeView*>( this ) )->GetDocument()->GetTerminal();
}
CString CNodeView::GetProjectPath()const
{
	return ( const_cast<CNodeView*>( this ) )->GetDocument()->m_ProjInfo.path;
}

CPlacement& CNodeView::GetPlaceMent()const
{
	return *(( const_cast<CNodeView*>( this ) )->GetDocument()->GetCurrentPlacement());
}

void CNodeView::OnCtxProcproperties() 
{
	CTVNode* pProc2Node = GetDocument()->GetSelectedNode();
	CProcessor2* pProc2 = (CProcessor2*) pProc2Node->m_dwData;
	ASSERT(pProc2 != NULL);
	
	ARCVector3 _location=pProc2->GetLocation();
	//_location[Z3D] = pProc2->GetFloor()+0.5;
	_location[VZ] = GetDocument()->m_nActiveFloor * SCALE_FACTOR ;

//	CProcPropDlg dlg( pProc2->GetProcessor(), C3DMath::CVector2D(_location[X3D],_location[Y3D]),this );
	CProcPropDlg dlg( pProc2->GetProcessor(), _location ,this );
	if(dlg.DoModal() == IDOK) {
		Processor* pProc = dlg.GetProcessor();
		if(pProc2->GetProcessor() == NULL) {  //we have a new Processor
			pProc2->SetProcessor(pProc);
			if(pProc->getProcessorType() == IntegratedStationProc) {
				GetDefaultStationDisplayProperties(&(pProc2->m_dispProperties));
				ARCVector3 oldloc;
				oldloc  = pProc2->GetLocation();
				oldloc[VZ] = 0.0;
				pProc2->SetLocation(oldloc);
			}
			GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, true);
			//remove from undefined processor list and put in defined processor list
			CPROCESSOR2LIST* pUndefList = &(GetDocument()->GetCurrentPlacement()->m_vUndefined);
			CPROCESSOR2LIST* pDefList = &(GetDocument()->GetCurrentPlacement()->m_vDefined);
			int size = pUndefList->size();
			for(int i=0; i<static_cast<int>(pUndefList->size()); i++) {
				if(pUndefList->at(i) == pProc2) {
					pUndefList->removePro2(pProc2);
					pUndefList->erase(pUndefList->begin()+i);
					break;
				}
			}
			//ASSERT(i < size);
			pDefList->push_back(pProc2);
			//remove from undefined nodes and put in defined nodes
			GetDocument()->MoveFromUCToDefined(pProc2);
		}
		else { //existing processor
			pProc2->SetProcessor(pProc);
			GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, true);
			RefreshProcessorItem(pProc2Node,GetDocument()->ProcessorsNode(),*pProc->getID());
		//	GetDocument()->RefreshProcessorTree();
		}
		int enumProcType = pProc->getProcessorType();
		if(enumProcType ==TaxiwayProcessor ||enumProcType == RunwayProcessor || enumProcType== StandProcessor)
		{
			((AirfieldProcessor *) pProc)->autoSnap(GetDocument()->GetTerminal().GetProcessorList());
		}
	}
}

void CNodeView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	InputTerminal* pInTerm = (InputTerminal*)&GetDocument()->GetTerminal();
	// ?was a node clicked?
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	// then expand the item
	//CView::OnLButtonDblClk(nFlags, point);
	//////////////////////////////////////////////////////////////////////////
	if((hItem != NULL)&& (TVHT_ONITEM & uFlags))
	{
		CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( hItem);
		//CString strNodeName = pNode->Name();
		if(pNode != NULL)
		{
			//:xb
			CString s;
			// Have created NodeViewDbClickHandler class in the AirsideGUI project to handle the DbClick command
			if (GetDocument()->m_systemMode == EnvMode_AirSide)
			{
				// Run Simulation
				s.LoadString(IDS_TVNN_RUNSIM);
				if(s.Compare(pNode->Name())==0)
					theApp.GetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_RUNSIM);

				s.LoadString(IDS_TVNN_AIRSPACEDISPLAY);
				if (s == pNode->Name())
				{
					CAircraftDispPropDlg dlg(this);
					dlg.DoModal();
					return ;
				}

				s.LoadString(IDS_TVNN_AIRCRAFTTAGS);
				if (s == pNode->Name())
				{
					CDlgAircraftTags dlg(this);
					dlg.DoModal();
					return ;
				}

				s.LoadString(IDS_TVNN_VEHICLETAGS);
				if (s == pNode->Name())
				{
					CDlgVehicleTags dlg(this);
					dlg.DoModal();
					return ;
				}

				s.LoadString(IDS_TVNN_SCHEDULE);
				if (s == pNode->Name())
				{	
					CFlightScheduleDlg dlg( this );
					dlg.DoModal();
					return;
				}

				HTREEITEM hParent = NULL;
				if(pNode->Parent() != NULL)
				{
					hParent = ((CTVNode*) pNode->Parent())->m_hItem;
					s.LoadString(IDS_TVNN_STAND_GATE_ASSIGNMENT);
					if (m_wndTreeCtrl.GetItemText(hParent) == s )
					{		
						for (int i=0;i<(int)GetDocument()->GetTerminal().m_AirsideInput->m_csAirportNameList.size();i++)
						{
							s = GetDocument()->GetTerminal().m_AirsideInput->m_csAirportNameList.at(i);

							if (s == pNode->Name())
							{	
								CDlgArrivalGateAssignment dlg(GetDocument()->GetProjectID(), GetDocument()->GetInputAirside().m_pAirportDB, this );
								dlg.DoModal();

								return;
							}
						}
					}

					s.LoadString(IDS_TVNN_ARRIVAL_GATE_ASSIGNMENT);
					if (m_wndTreeCtrl.GetItemText(hParent) == s )
					{		
						for (int i=0;i<(int)GetDocument()->GetTerminal().m_AirsideInput->m_csAirportNameList.size();i++)
						{
							s = GetDocument()->GetTerminal().m_AirsideInput->m_csAirportNameList.at(i);

							if (s == pNode->Name())
							{	
								CDlgDepartureGateAssignment dlg(GetDocument()->GetProjectID(), GetDocument()->GetInputAirside().m_pAirportDB, this );
								dlg.DoModal();

								return;
							}
						}
					}

					s.LoadString(IDS_TVNN_DEPARTURE_GATE_ASSIGNMENT);
					if (m_wndTreeCtrl.GetItemText(hParent) == s )
					{		
						for (int i=0;i<(int)GetDocument()->GetTerminal().m_AirsideInput->m_csAirportNameList.size();i++)
						{
							s = GetDocument()->GetTerminal().m_AirsideInput->m_csAirportNameList.at(i);

							if (s == pNode->Name())
							{	
								CDlgStandAssignment dlg(GetDocument()->GetProjectID(), GetDocument()->GetInputAirside().m_pAirportDB, this );
								dlg.DoModal();

								return;
							}
						}
					}
				}
				//s.LoadString(IDS_TVNN_GATEASSIGNMENTS);


				AirsideGUI::NodeViewDbClickHandler AirsideNodeHandler( GetDocument()->GetTerminal().GetProcessorList(),&GetDocument()->GetInputAirside());
				AirsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);

				CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
				AirsideNodeHandler.SetACCategoryList(pvACCategory);

				CAirlinesManager* pManager = GetDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
				AirsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());
				
				//if (pNode->Name().CompareNoCase(_T("Taxiway usage criteria")) == 0) 
				//{
				//	std::vector<CString> vSegmentName;
				//	std::vector<ASLayout::Segment> *pSegments = &(GetDocument()->GetTerminal().GetAirsideInput()->GetAirsideLayout()->GetSegments());
				//	std::vector<ASLayout::Segment>::iterator iter = pSegments->begin();
				//	std::vector<ASLayout::Segment>::iterator iterEnd = pSegments->end();
				//	for (;iter != iterEnd;++iter)
				//	{
				//		CString strName = (*iter).GetName();
				//		vSegmentName.push_back(strName);
				//	}
				//	AirsideNodeHandler.SetSegmentName(vSegmentName);

				//}

				//if (pNode->Name().CompareNoCase(_T("Runway Exit Performance")) == 0) 
				//{
				//	std::map<CString,int> mapRunwayExit;
				//	mapRunwayExit = GetDocument()->GetTerminal().GetAirsideInput()->GetAirsideLayout()->GetRunwayExitCount();

				//	AirsideNodeHandler.SetmapRunwayExit(mapRunwayExit);

				//}

				//if(pNode->Name().CompareNoCase(_T("STAR-Runway assignment and criteria")) == 0
				//	|| pNode->Name().CompareNoCase(_T("Runway-SID assignment and criteria")) == 0
				//	)
				//{
				//	ProcessorList* procList = GetDocument()->GetTerminal().GetProcessorList();
				//	std::vector<BaseProcessor*> vProcList;
				//	procList->GetProcessorsByType(vProcList, RunwayProcessor);
				//	std::vector<CString> vProcNameList;
				//	for(int nSize = 0; nSize < (int)vProcList.size(); nSize++)
				//	{
				//		CString strName = vProcList[nSize]->getIDName();
				//		vProcNameList.push_back(strName);
				//	}
				//	AirsideNodeHandler.Handle(pNode->Name(), vProcNameList);
				//}
				
				//if(pNode->Name().CompareNoCase(_T("Intersection blocking criteria")) == 0)
				//{
				//	std::vector<ASLayout::Intersection>& vIntersections = 
				//		GetDocument()->GetTerminal().m_AirsideInput->GetAirsideLayout()->GetIntersections();
				//	std::vector<CString> vIntersectionsNameList;
				//	for (std::vector<ASLayout::Intersection>::iterator iter = vIntersections.begin();
				//		iter != vIntersections.end();
				//		iter++)
				//		vIntersectionsNameList.push_back(iter->GetName());
				//	AirsideNodeHandler.SetIntersectionsName(vIntersectionsNameList);

				//}

//				if(pNode->Name().CompareNoCase(_T("Arrival Initialization")) == 0)
//				{
//					airside_engine::Simulation *pSimulation =  new airside_engine::Simulation();
//					if (pSimulation->Init(GetDocument()->GetTerminal().GetAirsideInput(),&GetDocument()->GetAirsieInput()))
//					{
//						FlightList* _flightlist = pSimulation->getFlightList();
//						//ns_AirsideInput::CFlightPlans* pFlightplans = (GetDocument()->GetAirsieInput()).GetFlightPlans();
//						std::vector<FlightArrivalInit> vArrivalInitParaList;
//						FlightArrivalInit _flightarrivalinit;
//						char pchar[512];
//						Flight*  _pflight = NULL;
//						for (int i =0;i<int(_flightlist->size());i++)
//						{
//							_pflight = _flightlist->at(i)->GetCFlight();
//							_pflight->getAirline(pchar); 
//							_flightarrivalinit.strAirline = pchar;
//
//							_pflight->getArrID(pchar);
//							_flightarrivalinit.strArrID = pchar;
//
//							_flightarrivalinit.strArrGate = _pflight->getArrGate().GetIDString();
//
//							FlightDescStruct& fltdesc = _pflight->getLogEntry();
///*							for (int j =0;j < int(pFlightplans->size());j++)
//							{
//								FlightConstraint& fltcont = (pFlightplans->at(j)).GetFltType();
//								if( fltcont.fits(fltdesc) )
//								{
//									AirRoute* pAirRoute = (pFlightplans->at(j)).GetArrEnRoute();
//									if (pAirRoute != NULL)
//									{
//										_flightarrivalinit.strEnroute = pAirRoute->GetName();
//									}
//									else
//									{
//										_flightarrivalinit.strEnroute = "\0";
//									}
//									pAirRoute = (pFlightplans->at(j)).GetSTAR();
//									if (pAirRoute != NULL)
//									{
//										_flightarrivalinit.strSTAR = pAirRoute->GetName();									
//										_flightarrivalinit.strRunway = pAirRoute->m_strRunwayName;
//									}
//									else
//									{
//										_flightarrivalinit.strSTAR = "\0";									
//										_flightarrivalinit.strRunway = "\0";
//									}
//									break;
//								}
//							}*/			
//							_flightarrivalinit.dScheduleTime = _pflight->getArrTime();
//							_flightarrivalinit.dSimTime = (_flightlist->at(i))->getFEL()->getEventInType(airside_engine::FlightEvent::StopInGate)->getEventTime();
//							_flightarrivalinit.dOffsetTime = _flightarrivalinit.dScheduleTime - _flightarrivalinit.dSimTime;
//							vArrivalInitParaList.push_back(_flightarrivalinit);
//						}
//						AirsideNodeHandler.SetFlightArrivalInitList(vArrivalInitParaList);
//					}
//				}
				
				s.LoadString(IDS_TVNN_FLIGHTPLANS);
				if (s == pNode->Name())
				{
					CDlgFlightPlan dlg(GetDocument()->GetProjectID(),GetInputTerminal()->m_pAirportDB, this);
					dlg.DoModal();
					return;
				}

				PMSVDbClickFunction pFun = GetDocument()->m_msManager.m_msImplAirSide.GetItemDbClickFunction(pNode);
				if (pFun != NULL)
					(AirsideNodeHandler.*pFun)();

				return;
			}
			
			s.LoadString(IDS_TVNN_MANUAL);
			if (s.Compare(pNode->Name()) == 0)
			{

				if(m_pDlgScheduleRosterContent && m_pDlgScheduleRosterContent->IsWindowVisible()) 
				{
					//hide (close) window
					m_pDlgScheduleRosterContent->DestroyWindow();
					delete m_pDlgScheduleRosterContent;
					m_pDlgScheduleRosterContent = NULL;
				}
				else
				{
					m_pDlgScheduleRosterContent = new CDlgScheduleAndRostContent(GetInputTerminal(),GetDocument()->GetProjectID(),this);
					m_pDlgScheduleRosterContent->Create(CDlgScheduleAndRostContent::IDD, this);
					m_pDlgScheduleRosterContent->CenterWindow();
					m_pDlgScheduleRosterContent->ShowWindow(SW_SHOW);
				}
			}

			// Name List Mapping
			s.LoadString( IDS_TVNN_NAMELIST );
			if( s.Compare(pNode->Name())==0 )
			{
				//CNameListMappingDlg dlg( this );
				//dlg.DoModal();
				
				CDlgExportNames dlg(GetDocument()->getARCport(),GetDocument()->GetProjectID(),this);
				dlg.DoModal();
			}

			//Flight Schedule
			s.LoadString( IDS_TVNN_SCHEDULE );
			if(s.Compare(pNode->Name())==0)
			{
				if (GetDocument()->m_systemMode == EnvMode_Terminal)
				{
					CFlightScheduleDlg dlg( this );
					dlg.DoModal();
				}
			}
			
			s.LoadString(IDS_TVNN_FLIGHT_GROUPS);
			if (s == pNode->Name())
			{
				CDlgFlightDB dlg(GetDocument()->GetProjectID(),&GetDocument()->GetTerminal());
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_AIRPORTSECTORS);
			if (s == pNode->Name())
			{
				CDlgDBAirports dlg(FALSE,&GetDocument()->GetTerminal());
				dlg.setAirportMan( GetDocument()->GetTerminal().m_pAirportDB->getAirportMan() );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_AIRLINEGROUPS);
			if (s == pNode->Name())
			{
				CDlgDBAirline dlg(FALSE,&GetDocument()->GetTerminal() );
				dlg.setAirlinesMan( GetDocument()->GetTerminal().m_pAirportDB->getAirlineMan() );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_AIRCRAFTCATEGORIES);
			if (s == pNode->Name())
			{
				CDlgDBAircraftTypes dlg(FALSE, &GetDocument()->GetTerminal() );
				dlg.setAcManager( GetDocument()->GetTerminal().m_pAirportDB->getAcMan() );
				dlg.DoModal();
			}

			//Flight Delays
			s.LoadString(IDS_TVNN_DELAYS);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg( FLIGHT_DELAYS, this );
				dlg.DoModal();
			}

			if (pNode->Name() == "Visitor STA Trigger")
			{
				CFltPaxDataDlg dlg( VISITOR_STA_TRIGGER, this );
				dlg.DoModal();
			}
			////Texi Routes
			//s.LoadString(IDS_TVNN_TAXIROUTES);
			//if(s.Compare(pNode->Name())==0)
			//{
			//	CDlgGroundRoutes dlg(GetDocument()->GetProjectID(),GetDocument()->GetCurrentAirport(),this);
			//	dlg.DoModal();
			//}

			//Passenger Loads
			s.LoadString(IDS_TVNN_LOADFACTORS);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg( FLIGHT_LOAD_FACTORS, this);
				dlg.DoModal();
			}

			//Aircraft Capacity
			s.LoadString(IDS_TVNN_ACCAPACITIES);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg(FLIGHT_AC_CAPACITIES, this);
				dlg.DoModal();
			}

			//Boarding Calls
			s.LoadString(IDS_TVNN_BOARDINGCALLS);
			if(s.Compare(pNode->Name())==0)
			{
				CBoardingCallDlg dlg(this);
				//CFltPaxDataDlg dlg(FLIGHT_BOARDING_CALLS, this);
				dlg.DoModal();
			}

			//passenger / name
			s.LoadString(IDS_TVNN_PAXNAMES);
			if(s.Compare(pNode->Name())==0)
			{
				CPaxTypeDefDlg dlg( this );
				dlg.DoModal();
			}

			//passenger / dist
			s.LoadString(IDS_TVNN_PAXDISTRIBUTIONS);
			if(s.Compare(pNode->Name())==0)
			{
				CPaxDistDlg dlg( this );
				dlg.DoModal();
			}

			//Mobile Element
			s.LoadString(IDS_TVNN_MOBILECOUNT);
			if(s.Compare(pNode->Name())==0)
			{
				CMobileCountDlg dlg(this);
				dlg.DoModal();
			}
			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXGROUPSIZE);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg(PAX_GROUP_SIZE, this );
				dlg.DoModal();
				//CDlgAffinityGroup dlg(&(GetDocument()->GetTerminal()) ,this) ;
				//dlg.DoModal() ;				
			}
		
			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXDISTPROFILE);
			if(s.Compare(pNode->Name())==0)
			{
				///CFltPaxDataDlg dlg(PAX_LEAD_LAG, this );
				CLeadLagTimeDlg dlg(this);
				dlg.DoModal();
			}

			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXBULK);
			if(s.Compare(pNode->Name())==0)
			{
				///CFltPaxDataDlg dlg(PAX_LEAD_LAG, this );
				CPaxBulkListDlg dlg(this);
				dlg.DoModal();
			}			
			
			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXISDIST);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg(PAX_IN_STEP, this );
				dlg.DoModal();
			}
		
			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXSSDIST);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg(PAX_SIDE_STEP, this );
				dlg.DoModal();
			}
		
			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXSPEED);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg(PAX_SPEED, this );
				dlg.DoModal();
			}
		
			//Boarding Calls
			s.LoadString(IDS_TVNN_PAXVISTIME);
			if(s.Compare(pNode->Name())==0)
			{
				CFltPaxDataDlg dlg(PAX_VISIT_TIME, this );
				dlg.DoModal();
			}

			//Response Time
			s.LoadString(IDS_TVNN_INITRESPTIME );
			if( s.Compare(pNode->Name()) == 0)
			{
				CFltPaxDataDlg dlg(PAX_RESPONSE_TIME, this );
				dlg.DoModal();
			}

			//Speed Impact
			s.LoadString( IDS_TVNN_INITSPDIMP );
			if( s.Compare(pNode->Name()) == 0)
			{
				CFltPaxDataDlg dlg(PAX_SPEED_IMPACT, this );
				dlg.DoModal();
			}
			
			// push propensity  
			s.LoadString( IDS_TVNN_PROPENTOPUSH );
			if( s.Compare( pNode->Name()) == 0)
			{
				CFltPaxDataDlg dlg(PAX_PUSH_PROPENSITY, this );
				dlg.DoModal();
			}
			
			// passenger entry flight time distribution
			s.LoadString(IDS_TVNN_ACENTRYPROCS);
			if( s.Compare( pNode->Name()) == 0)
			{
				CAircraftEntryProcessorDlg dlg(GetInputTerminal(), this);
				dlg.DoModal();
			}

			//Gate Assignment
			s.LoadString(IDS_TVNN_STAND_GATE_ASSIGNMENT);
			if(s.Compare(pNode->Name())==0)
			{

				CDlgStandAssignment dlg(GetDocument()->GetProjectID(), GetDocument()->GetInputAirside().m_pAirportDB, this );
				dlg.DoModal();
				
			}//end if compare	

			s.LoadString(IDS_TVNN_ARRIVAL_GATE_ASSIGNMENT);
			if(s.Compare(pNode->Name())==0)
			{

				CDlgArrivalGateAssignment dlg(GetDocument()->GetProjectID(), GetDocument()->GetInputAirside().m_pAirportDB, this );
				dlg.DoModal();

			}//end if compare

			s.LoadString(IDS_TVNN_DEPARTURE_GATE_ASSIGNMENT);
			if(s.Compare(pNode->Name())==0)
			{

				CDlgDepartureGateAssignment dlg(GetDocument()->GetProjectID(), GetDocument()->GetInputAirside().m_pAirportDB, this );
				dlg.DoModal();

			}//end if compare

			//Pax Flow by Pax Type
			s.LoadString(IDS_TVNN_FLOWBYPAXTYPE);
			if(s.Compare(pNode->Name())==0)
			{
				//if( FindWindow(NULL,"Mobile Element Flow") == NULL )
				if(m_pDlgFlow==NULL)
				{
					m_pDlgFlow = new CPaxFlowDlg ( GetInputTerminal(), this );
					m_pDlgFlow->Create( CPaxFlowDlg::IDD, this );
					HICON hIcon = LoadIcon( AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME) );
					m_pDlgFlow->SetIcon( hIcon,TRUE);
 				}
				AfxGetApp()->BeginWaitCursor();
				if( !m_pDlgFlow->InitFlow() )
					m_pDlgFlow->ShowWindow( SW_HIDE );
				else
					m_pDlgFlow->ShowWindow( SW_SHOWNORMAL );
				AfxGetApp()->EndWaitCursor();
			}

			//Pax Flow by Proc
			s.LoadString(IDS_TVNN_FLOWBYPROC);
			if(s.Compare(pNode->Name())==0)
			{
				CPaxFlowByProcDlg dlg( this );
				dlg.DoModal();
			}
			
			// resource pool
			s.LoadString( IDS_TVNN_RESOURCEPOOL );
			if( s.Compare(pNode->Name())==0 )
			{
				CResourcePoolDlg dlg( GetDocument(), this );
				dlg.DoModal();
			}

			// processor and resource pool
			s.LoadString( IDS_TVNN_PROCTORESPOOL );
			if( s.Compare(pNode->Name())==0 )
			{
				CProcToResPoolDlg dlg( GetDocument(), this );
				dlg.DoModal();
			}
			// process congestion area
			s.LoadString( IDS_TVNN_PROCCONGAREA );
			if( s.Compare(pNode->Name())==0 )
			{	
				CTermPlanDoc* pDoc = GetDocument();
				CDlgProcCongArea dlg(&(pDoc->GetTerminal()),pDoc);
				dlg.DoModal();
			}
			// Process definition
			s.LoadString(IDS_TVNN_PROCPROCESS);
			if( s.Compare(pNode->Name())==0 )
			{
// 				CMainFrame * pMainWnd=(CMainFrame *)AfxGetApp()->m_pMainWnd;
// 				pMainWnd->OnShowProcessDefine();
				if(m_pDlgProcessDefine==NULL)
				{
					CTermPlanDoc* pDoc = GetDocument();
					m_pDlgProcessDefine = new CProcessDefineDlg ( pDoc, this );
					m_pDlgProcessDefine->Create( CProcessDefineDlg::IDD, this );
					HICON hIcon = LoadIcon( AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME) );
					m_pDlgProcessDefine->SetIcon( hIcon,TRUE);
				}
				else
					m_pDlgProcessDefine->InitDialog();
				AfxGetApp()->BeginWaitCursor();
				m_pDlgProcessDefine->ShowWindow( SW_SHOWNORMAL );
				m_pDlgProcessDefine->CenterWindow();
				AfxGetApp()->EndWaitCursor();
			}

			// proc assignment
			s.LoadString(IDS_TVNN_PROCROSTER);
			if(s.Compare(pNode->Name())==0)
			{
				CProcAssignDlg dlg( this );
				dlg.DoModal();
			}//end if compare	


			s.LoadString(IDS_TVNN_RETAIL_PRODUCTS);
			if(s.Compare(pNode->Name())==0)
			{
				CDlgRetailProductsStocking dlg( this );
				dlg.DoModal();
			}//end if compare	

			s.LoadString(IDS_TVNN_TERMINALSERVICETIME);
			if(s.Compare(pNode->Name())==0)
			{
				CServiceTimeDlg dlg( this );
				dlg.DoModal();
			}//end if compare	

			s.LoadString(IDS_TVNN_HUBBINGDATA);
			if(s.Compare(pNode->Name())==0)
			{
				CHubDataDlg dlg( this );
				dlg.DoModal();
			}//end if compare

			s.LoadString(IDS_TVNN_OPERATINGDOOR);
			if(s.Compare(pNode->Name())==0)
			{
				DlgOperatingDoorSpecification dlg(GetInputTerminal()->m_pAirportDB, this );
				dlg.DoModal();
			}//end if compare	

			s.LoadString(IDS_TVNN_PROCBAGDEVASSIGN);
			if(s.Compare(pNode->Name())==0)
			{
				int nAptID = InputAirside::GetAirportID(GetDocument()->GetProjectID());
				CBagDevDlg dlg(nAptID, this );
				dlg.DoModal();
			}//end if compare

			s.LoadString(IDS_TVNN_PROCLINKAGES);
			if(s.Compare(pNode->Name())==0)
			{
				CProcDataSheet sheet( "Processor Behaviors", this );
				sheet.DoModal();
			}//end if compare
			
			s.LoadString(IDS_TVNN_MOVINGSIDEWALK);
			if( s.Compare(pNode->Name())==0)
			{
				//CCarScheduleDlg dlg( &(GetDocument()->m_trainTraffic), GetDocument()->GetTerminal().pAllCarSchedule,this );
				CMoveSideWalkSheet dlg( " SideWalk  Property" ,this );
				dlg.DoModal();
			}
			//all bus schedule
			s.LoadString( IDS_ALLCAR_SCHEDULE );
			if( s.Compare(pNode->Name())==0)
			{
				//CCarScheduleDlg dlg( &(GetDocument()->m_trainTraffic), GetDocument()->GetTerminal().pAllCarSchedule,this );
				CCarScheduleSheet dlg( " Schedule Information" ,&(GetDocument()->m_trainTraffic), GetDocument()->GetTerminal().m_pAllCarSchedule,this );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_SIMSETTINGS);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				if (!GetInputTerminal()->flightSchedule->getCount())
				{
					AfxMessageBox(_T("Please define flight schedule first."));
					return;
				}
				CSimEngSettingDlg dlg( GetInputTerminal()->m_pSimParam, this );
				dlg.DoModal();
			}//end if compare
			
			s.LoadString(IDS_TVNN_RUNSIM);
			if(s.Compare(pNode->Name())==0)
				theApp.GetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_RUNSIM);

			s.LoadString(IDS_TVNN_MOBILEDISPLAY);
			if(s.Compare(pNode->Name())==0)
			{
				CPaxDispPropDlg dlg( this );
				dlg.DoModal();
				GetDocument()->UpdateTrackersMenu();
			}//end if compare

			s.LoadString(IDS_TVNN_MOBILETAGS);
			if(s.Compare(pNode->Name()) == 0)
			{
				CDlgPaxTags dlg( this );
				dlg.DoModal();
				GetDocument()->UpdateTrackersMenu();
			}

			s.LoadString(IDS_TVNN_RESOURCEDISPLAY);
			if(s.Compare(pNode->Name()) == 0)
			{
				CResDispPropDlg dlg( this, GetDocument() );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_PROCESSORTAGS);
			if(s.Compare(pNode->Name()) == 0)
			{
				CDlgProcessorTags dlg( GetDocument(), this );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_DYNAMICLOADDISPLAY);
			if(s.Compare(pNode->Name()) == 0)
			{
				CDlgActivityDensity dlg( this );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_CALLOUT_DISP_SPEC);
			if(s.Compare(pNode->Name()) == 0)
			{
				CCalloutDispSpecDlg dlg( FuncSelectFlightType, GetDocument()->GetProjectID(), GetDocument(), this );
				dlg.DoModal();
			}
	// --------- Terminal end ??? --------- //

			
	// --------- Economic begin --------- //
			
			// Building Depreciation
			s.LoadString(IDS_TVNN_BUILDINGDEP);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulation setting
				CEconomicDialog dlg( this, BuildingDeprecCost );
				dlg.DoModal();
			}//end if compare

			// Intersest on Captial
			s.LoadString(IDS_TVNN_INTERESTONCAPITAL);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CInterestOnCapitalDlg dlg( this );
				dlg.DoModal();
			}//end if compare

			// Insurance
			s.LoadString(IDS_TVNN_INSURANCE);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomicDialog dlg( this, InsuranceCost );
				dlg.DoModal();
			}//end if compare

			// Contacts
			s.LoadString(IDS_TVNN_CONTRACTS);
			CNode *pTempNode = pNode->Parent();
			if (pTempNode)
			{
				CString strTemp;
				strTemp.LoadString(IDS_TVNN_FIXEDCOSTS);
			
				if(s.Compare(pNode->Name())==0 &&
					strTemp.Compare(pTempNode->Name())==0
					)
				{
					// run simulationsetting
					CEconomicDialog dlg( this, ContractsCost );
					dlg.DoModal();
				}//end if compare
			}

			// Paking Lots Depreciation
			s.LoadString(IDS_TVNN_PARKINGLOTSDEP);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomicDialog dlg( this, ParkingLotsDeprecCost );
				dlg.DoModal();
			}//end if compare

			// Landside Facilities
			s.LoadString(IDS_TVNN_LANDSIDEFACILITIES);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomicDialog dlg( this, LandsideFacilitiesCost );
				dlg.DoModal();
			}//end if compare

			// Airside Facilities
			s.LoadString(IDS_TVNN_AIRSIZEFACILITIES);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomicDialog dlg( this, AirsideFacilitiesCost );
				dlg.DoModal();
			}//end if compare

			// Leases
			s.LoadString(IDS_TVNN_LEASES);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomicDialog dlg( this, LeasesRevenue );
				dlg.DoModal();
			}//end if compare

			// Advertising
			s.LoadString(IDS_TVNN_ADVERTISING);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomicDialog dlg( this, AdvertisingRevenue );

				dlg.DoModal();
			}//end if compare

			// Contracts
			s.LoadString(IDS_TVNN_REVENUESCONTRACTS);
			pTempNode = pNode->Parent();
			if (pTempNode)
			{
				CString strTemp;
				strTemp.LoadString(IDS_TVNN_FIXEDREVENUES);
			
				if(s.Compare(pNode->Name())==0 &&
					strTemp.Compare(pTempNode->Name())==0
					)
				{
					// run simulationsetting
					CEconomicDialog dlg( this, ContractsRevenue );

					dlg.DoModal();
				}//end if compare
			}

			// Passenger
			s.LoadString(IDS_TVNN_PASSENGER);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomic2Dialog dlg( PASSENGER, this );

				//
				// Init it
				//
				
				// Dialog title
				dlg.SetTitle("Passenger Fixed and Variable Costs");

				// Column name
				CStringArray arColName;
				arColName.Add("Passenger Type");
				arColName.Add("Fixed/pax( $ )");
				arColName.Add("Var/hr( $ )");
				arColName.Add("Var/proc( $ )");
				arColName.Add("Var/cart( $ )");
				arColName.Add("Var/bag( $ )");
				dlg.SetColNameArray(arColName);
				
				// Show it
				dlg.DoModal();
			}//end if compare

			// Aircraft
			s.LoadString(IDS_TVNN_AIRCRAFT);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomic2Dialog dlg( AIRCRAFT, this );

				//
				// Init it
				//
				
				// Dialog title
				dlg.SetTitle("Aircraft Fixed and Variable Costs");

				// Column name
				CStringArray arColName;
				arColName.Add("Flight Type");
				arColName.Add("Service/ac ($)");
				arColName.Add("Service/hr ($)");
				dlg.SetColNameArray(arColName);
				
				// Show it
				dlg.DoModal();
			}//end if compare

			// Passenger Airport Fees
			s.LoadString(IDS_TVNN_PASSENGERAIRPORTFEES);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomic2Dialog dlg( PASSENGER_AIRPORT_FEES, this );

				//
				// Init it
				//
				
				// Dialog title
				dlg.SetTitle("Passenger Airport Fees");

				// Column name
				CStringArray arColName;
				arColName.Add("Passenger Type");
				arColName.Add("$/pax");
				arColName.Add("$/hr");
				dlg.SetColNameArray(arColName);
				
				// Show it
				dlg.DoModal();
			}//end if compare

			// Landing Fees
			s.LoadString(IDS_TVNN_LANDINGFEES);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomic2Dialog dlg( LANDING_FEES, this );

				//
				// Init it
				//
				
				// Dialog title
				dlg.SetTitle("Landing Fees");

				// Column name
				CStringArray arColName;
				arColName.Add("Flight Type");
				arColName.Add("$/landing");
				arColName.Add("$/1,000 lb.MLW");
				dlg.SetColNameArray(arColName);
				
				// Show it
				dlg.DoModal();
			}//end if compare

			s.LoadString(IDS_TVNN_CAPITALBDDECISIONS);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CCapitalBDDecisionDlg dlg( this );
				dlg.DoModal();
			}


			// Waiting Cost Factors
			s.LoadString(IDS_TVNN_WAITINGCOST);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulationsetting
				CEconomic2Dialog dlg( WAITING_COST_FACTORS, this );

				//
				// Init it
				//
				
				// Dialog title
				dlg.SetTitle("Waiting Cost Factors");

				// Column name
				CStringArray arColName;
				arColName.Add("Passenger Type");
				arColName.Add("$/hr");
				dlg.SetColNameArray(arColName);
				
				// Show it
				dlg.DoModal();
			}//end if compare
			
			s.LoadString(IDS_TVNN_MAINTENANCE);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulation
				CMaintenanceDialog dlg( this );

				//
				// Init it
				//
				
				// Dialog title
				dlg.SetTitle("Maintenance Fixed and Variable Costs");

				// Column name
				CStringArray arColName;
				arColName.Add("Component");
				arColName.Add("$/landing");
				arColName.Add("$/yr");
				arColName.Add("$/flight");
				arColName.Add("$/day");
				dlg.SetColNameArray(arColName);
				
				// Show it
				dlg.DoModal();
			}//end if compare

			s.LoadString(IDS_TVNN_UTILITIES);
			if(s.Compare(pNode->Name())==0)
			{
				// run simulation
				CUtilitiesDialog dlg( this );

				// Show it
				dlg.DoModal();
			}//end if compare

			s.LoadString(IDS_TVNN_LABOUR);
			if(s.Compare(pNode->Name())==0)
			{
				CEconomicaDataTypeDlg dlg( ECONOMIC_LABOUR,this );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_PROCESSOR);
			if(s.Compare(pNode->Name())==0)
			{
				CEconomicaDataTypeDlg dlg( ECONOMIC_PROCESSOR,this );
				dlg.DoModal();
			}
			s.LoadString(IDS_TVNN_GATEUSAGE);
			if(s.Compare(pNode->Name())==0)
			{
				CEconomicaDataTypeDlg dlg( ECONOMIC_GATEUSAGE,this );
				dlg.DoModal();
			}
			s.LoadString(IDS_TVNN_PROCESSORUSAGE);
			if(s.Compare(pNode->Name())==0)
			{
				CEconomicaDataTypeDlg dlg( ECONOMIC_PROCESSORUSAGE,this );
				dlg.DoModal();
			}

			s.LoadString(IDS_TVNN_RETAILPERCENTAGE);
			if(s.Compare(pNode->Name())==0)
			{
				CDlgRetailFees dlg( this );
				dlg.DoModal();
			}
			s.LoadString(IDS_TVNN_PAKINGFEES);
			if(s.Compare(pNode->Name())==0)
			{
				CDlgRetailPackFee dlg( PACKINGFEES,this );
				dlg.DoModal();
			}
			s.LoadString(IDS_TVNN_LANDSIDETRANS);
			if(s.Compare(pNode->Name())==0)
			{
				CDlgRetailPackFee dlg( LANDSIDETRANSFEE,this );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_ANNUALACTIVITY );
			if(s.Compare(pNode->Name())==0)
			{
				CAnnualActivityDeductionDlg dlg( this );
				dlg.DoModal();
			}
	// --------- Economic end--------- //
			//------------Report Param Begin ----------//

			s.LoadString( IDS_TVNN_REP_PAX_LOG );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_PAXLOG_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PAX_DISTANCE );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_DISTANCE_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PAX_TIMEINQ );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_QUEUETIME_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PAX_TIMEINT );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_DURATION_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PAX_TIMEINS );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_SERVICETIME_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PAX_BREAKDOWN );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_ACTIVEBKDOWN_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_FIREEVACUTION );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_FIREEVACUTION_REP, GetInputTerminal(), this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PROC_UTILISATION );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_UTILIZATION_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PROC_THROUGHPUT );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_THROUGHPUT_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_PROC_QLENGTH );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_QUEUELENGTH_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_PROC_QUEUEPARAM );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_AVGQUEUELENGTH_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_SPACE_OCCUPANCY );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_PAXCOUNT_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_SPACE_DENSITY );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_PAXDENS_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_SPACE_THROUGHPUT );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_SPACETHROUGHPUT_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_SPACE_COLLISIONS );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_COLLISIONS_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_AIRCRAFT );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_ACOPERATION_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_BAG_COUNT );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_BAGCOUNT_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_BAG_WAITTIME );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_BAGWAITTIME_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_BAG_DISTRIBUTION );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_BAGDISTRIBUTION_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_BAG_DELIVERYTIME );
			if( (s == pNode->Name()) && (pNode->GetChildCount() == 0) )
			{
				CReportParamDlg dlg( ENUM_BAGDELIVTIME_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}
			
			s.LoadString( IDS_TVNN_REP_ECONOMIC );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_ECONOMIC_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_CONVEYOR_WAIT_LENGTH_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			s.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
			if( s == pNode->Name() )
			{
				CReportParamDlg dlg( ENUM_CONVEYOR_WAIT_TIME_REP, GetInputTerminal(),this );
				dlg.setDocument( GetDocument() );
				dlg.DoModal();
			}

			GetDocument()->SelectNode(pNode);
		}
	}	
}

void CNodeView::OnCtxDeleteProc() 
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	if (pNode->m_nIDResource == IDR_CTX_PROCESSOR ||
		pNode->m_nIDResource == IDR_CTX_UCPROCESSOR||
		pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP||
		pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE ||
		pNode->m_nIDResource == IDR_MENU_CONTOUR ) 
	{
		
		int nSize = GetDocument()->GetSelectProcessorsCount();
		ASSERT(nSize > 0);
		if(nSize == 1 )
		{	
			//CObjectDisplayList__
			CObjectDisplay* pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
			if(pObjDisplay == NULL)
			{
				TRACE("Error occur in function void CNodeView::OnCtxDeleteProc() ");
				return;
			}
			if(pObjDisplay->GetType() == ObjectDisplayType_OtherObject)
				return;
			CProcessor2* pProc2 = (CProcessor2*)pObjDisplay;			
			ASSERT(pProc2 != NULL);

			//delete the contour process 
			//call the Contour Tree to get the processes that need to be deleted
			if(pNode->m_nIDResource == IDR_MENU_CONTOUR)
			{

				//CProcessor2* pProc2 = (CProcessor2*)pObjDisplay;			
				//ASSERT(pProc2 != NULL);

				CProcessor2* pProc2 = (CProcessor2*)pObjDisplay;//GetDocument()->m_vSelectedProcessors[0];
				ASSERT(pProc2 != NULL);

				if( pProc2->GetProcessor() == NULL )
				{
					GetDocument()->DeleteProcessor(pProc2);
					return;
				}

				CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),(ProcessorID *)pProc2->GetProcessor()->getID());
				CUsedProcInfoBox box;
				box.AddInfo(usedProcInfo);
				if(!box.IsNoInfo())
				{
					if(MessageBox("Delete processor ,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
					{					
//						GetDocument()->DeleteProcessor(pProc2);
						//Get the count of the process that need to delete
						std::vector<CString> ContourProcList;
						CString strProc = pProc2->GetProcessor()->getID()->GetIDString();
						GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->DeleteAnyNode(ContourProcList,strProc);
						//					GetDocument()->DeleteProcessor(pProc2);
						for (int i = 0 ; i < static_cast<int>(ContourProcList.size()) ; i++ )
						{
							strProc = ContourProcList[i];
							pProc2 = GetDocument()->GetProcessor2FromName(strProc,EnvMode_AirSide);
							GetDocument()->DeleteProcessor(pProc2);
						}
					}
//					GetDocument()->m_msManager.m_msImplAirSide.AddProcessorGroup(ContourProcessor,NULL);
					return;
				}
				if(box.DoModal()==IDOK)
				{
					//Get the count of the process that need to delete
// 					std::vector<CString> ContourProcList;
// 					CString strProc = pProc2->GetProcessor()->getID()->GetIDString();
// 					GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->DeleteAnyNode(ContourProcList,strProc);
// //					GetDocument()->DeleteProcessor(pProc2);
// 					for (int i = 0 ; i < static_cast<int>(ContourProcList.size()) ; i++ )
// 					{
// 						strProc = ContourProcList[i];
// 						pProc2 = GetDocument()->GetProcessor2FromName(strProc,EnvMode_AirSide);
// 						GetDocument()->DeleteProcessor(pProc2);
// 					}
	//				::AfxMessageBox("Cannot delete processor:the processor is in use in other places");
				}
//				GetDocument()->m_msManager.m_msImplAirSide.AddProcessorGroup(ContourProcessor,NULL);
				return;
			}
			else
			{
				if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
				{
					CProcessor2* pProc2 = (CProcessor2*)pObjDisplay;
					ASSERT(pProc2 != NULL);
					if( pProc2->GetProcessor() == NULL )
					{
						GetDocument()->DeleteProcessor(pProc2);
						return;
					}

					CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),(ProcessorID *)pProc2->GetProcessor()->getID());
					CUsedProcInfoBox box;
					box.AddInfo(usedProcInfo);
					if(!box.IsNoInfo())
					{
						if(MessageBox("Delete processor ,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
							GetDocument()->DeleteProcessor(pProc2);
						return;
					}
					if(box.DoModal()==IDOK)	
		//				::AfxMessageBox("Cannot delete processor:the processor is in use in other places");
					//	GetDocument()->DeleteProcessor(pProc2);
					return;
				}
			}
		}
		else
		{
			CUsedProcInfoBox *box = new CUsedProcInfoBox();

			CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),&GetDocument()->m_selectProID);
			box->AddInfo(usedProcInfo);
			if(!box->IsNoInfo())
			{
				if(MessageBox("Delete processors,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
					GetDocument()->DeleteProcessors(GetDocument()->GetSelectProcessors());
			}
			else
			{
				if(box->DoModal()==IDOK)	
				{	
	//				::AfxMessageBox(_T("Cannot delete processor:the processor is in use in other places"));
					//GetDocument()->DeleteProcessors(GetDocument()->m_vSelectedProcessors);	
				}
			}
			delete box;
			
		}
	}
	//----------------------------------Lanside Node-----------------------------------------------------------------
	
}

void CNodeView::OnCtxDeleteFloor() 
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);
	GetDocument()->DeleteFloor((int)pNode->m_dwData);
}

void CNodeView::OnCtxComments()
{
	if (GetDocument()->m_systemMode == EnvMode_AirSide 
		|| GetDocument()->m_systemMode == EnvMode_OnBoard
		|| GetDocument()->m_systemMode == EnvMode_LandSide)
		return;
	TCHAR buffer[4097];
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	ASSERT(pNode != NULL);
	CString sCommentsPath = PROJMANAGER->GetPath( GetDocument()->m_ProjInfo.path, CommentsDir );
	FileManager fileMan;
	if(!fileMan.IsDirectory(sCommentsPath)) {
		if(!fileMan.MakePath(sCommentsPath)) {
			AfxMessageBox("Error: could not create comments directory");
			return;
		}
	}
	//filename = node name (if processor, whole processor name) (if proc group, group name)
	CString sFileName;
	if(pNode->m_nIDResource == IDR_CTX_PROCESSOR) {
		((CProcessor2*)pNode->m_dwData)->GetProcessor()->getID()->printID(buffer);
		sFileName = buffer;
	}
	else if(pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP
			|| pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE ) {
		//build proc group name string by moving up the tree
		CTVNode* pPlacementsNode = GetDocument()->m_msManager.FindNodeByName(IDS_TVNN_PLACEMENTS);
		CTVNode* pParentNode = (CTVNode*) pNode->Parent();
		int nLevel = 0;
		CString sGroupFullName = pNode->Name();
		while(pParentNode != pPlacementsNode) {
			nLevel++;
			sGroupFullName = pParentNode->Name() + "-" + sGroupFullName;
			pParentNode = (CTVNode*) pParentNode->Parent();
		}
		sFileName = sGroupFullName;
	}
	else {
		sFileName = pNode->Name();
	}
	sFileName = sCommentsPath + "\\" + sFileName + ".txt";
	FILE* f = NULL;
	f = fopen(sFileName, "r");
	if(f != NULL) { //file exists
		//fill buffer with file contents
		int c = fread(buffer, sizeof(TCHAR), 4096, f);
		buffer[c] = '\0';
		fclose(f);
	}
	else {
		buffer[0] = '\0';
	}
	CDlgComments dlg(buffer);
	dlg.DoModal();
	if(buffer[0] == '\0') {
		//no comments, delete file if it exists
		CFileStatus rStatus;
		if(CFile::GetStatus(sFileName, rStatus))
			CFile::Remove(sFileName);
	}
	else {
		//create or overwrite file
		f = fopen(sFileName, "w");
		fwrite(buffer, sizeof(TCHAR), _tcslen(buffer), f);
		fclose(f);
	}
}

void CNodeView::OnRailwaylayoutDefine() 
{
	CTermPlanDoc* pDoc = GetDocument();
	CStationLinkerDlg dlg(&(pDoc->m_trainTraffic),pDoc);	
	dlg.DoModal();
}


void CNodeView::OnPipeDefine()
{
	CTermPlanDoc* pDoc = GetDocument();
	CPipeDefinitionDlg dlg(&(pDoc->GetTerminal()),pDoc);	
	dlg.DoModal();	
}

// BOOL CNodeView::OnEraseBkgnd(CDC* pDC) 
// {
// 	// TODO: Add your message handler code here and/or call default
// 	
// 	return TRUE;
// }

void CNodeView::OnPaint() 
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

void CNodeView::OnCtxChangeToPoint() 
{
	CTVNode* pProc2Node = GetDocument()->GetSelectedNode();
	ASSERT( pProc2Node);

	if( IfLinkageOrGateAssignUsingThisProc( pProc2Node->Name() ) )
	{
		return ;
	}

	if( !IfProcTypeCanBeChanged( pProc2Node->Name() ) )
	{
		AfxMessageBox(" Can not change to Point processor from this kind of processor! ");
		return;
	}
	//pProc2->GetProcessor()->getID()

	
	ChangeProcType( pProc2Node->Name(), PointProc );
	
	GetPlaceMent().saveDataSet(GetProjectPath(), false);	
	GetInputTerminal()->saveInputs( GetProjectPath() );
	
	//CTVNode* pPlacements = GetDocument()->ProcessorsNode();
	//GetDocument()->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pPlacements);
}


void CNodeView::OnCtxChangeToFloorchange() 
{
	CTVNode* pProc2Node = GetDocument()->GetSelectedNode();
	ASSERT( pProc2Node);

	if( IfLinkageOrGateAssignUsingThisProc( pProc2Node->Name() ) )
	{
		return ;
	}

	if( !IfProcTypeCanBeChanged( pProc2Node->Name() ) )
	{
		AfxMessageBox(" Can not change to Floor Change processor from this kind of processor! ");
		return;
	}
	ChangeProcType( pProc2Node->Name(), FloorChangeProc );

	
	GetPlaceMent().saveDataSet(GetProjectPath(), false);			
	GetInputTerminal()->saveInputs( GetProjectPath() );
}

void CNodeView::OnCtxChangeToGate() 
{
	CTVNode* pProc2Node = GetDocument()->GetSelectedNode();
	ASSERT( pProc2Node);

	if( IfLinkageOrGateAssignUsingThisProc( pProc2Node->Name() ) )
	{
		return ;
	}

	if( !IfProcTypeCanBeChanged( pProc2Node->Name() ) )
	{
		AfxMessageBox(" Can not change to Gate processor from this kind of processor! ");
		return;
	}	

	ChangeProcType( pProc2Node->Name(), GateProc );

	GetPlaceMent().saveDataSet(GetProjectPath(), false);			
	GetInputTerminal()->saveInputs( GetProjectPath() );
}

void CNodeView::OnCtxChangeToSink() 
{
	CTVNode* pProc2Node = GetDocument()->GetSelectedNode();
	ASSERT( pProc2Node);

	if( IfLinkageOrGateAssignUsingThisProc( pProc2Node->Name() ) )
	{
		return ;
	}

	if( !IfProcTypeCanBeChanged( pProc2Node->Name() ) )
	{
		AfxMessageBox(" Can not change to Dependent Sink processor from this kind of processor! ");
		return;
	}	
	ChangeProcType( pProc2Node->Name(), DepSinkProc );
	GetPlaceMent().saveDataSet(GetProjectPath(), false);			
	GetInputTerminal()->saveInputs( GetProjectPath() );
}

void CNodeView::OnCtxChangeToSource() 
{
	CTVNode* pProc2Node = GetDocument()->GetSelectedNode();
	ASSERT( pProc2Node);

	if( IfLinkageOrGateAssignUsingThisProc( pProc2Node->Name() ) )
	{
		return ;
	}

	if( !IfProcTypeCanBeChanged( pProc2Node->Name() ) )
	{
		AfxMessageBox(" Can not change to Dependent Source  processor from this kind of processor! ");
		return;
	}		

	ChangeProcType( pProc2Node->Name(), DepSourceProc );
	GetPlaceMent().saveDataSet(GetProjectPath(), false);			
	GetInputTerminal()->saveInputs( GetProjectPath() );
}

bool CNodeView::IfProcTypeCanBeChanged( CString _strProcName )const
{
	InputTerminal* pTerm = GetInputTerminal();
	ASSERT( pTerm );

	ProcessorID testID;
	testID.SetStrDict( pTerm->inStrDict );
	testID.setID( _strProcName );

	GroupIndex groupIdx =  pTerm->GetProcessorList()->getGroupIndex( testID );
	if( groupIdx.start < 0 )
	{
		return false;
	}

	int iProcType = pTerm->GetProcessorList()->getProcessor( groupIdx.start )->getProcessorType();

	return iProcType == PointProc || iProcType == GateProc || iProcType == FloorChangeProc 
		 || iProcType == DepSourceProc || iProcType == DepSinkProc ;
}
bool CNodeView::IfLinkageOrGateAssignUsingThisProc( CString _strProcName )const
{
	InputTerminal* pTerm = GetInputTerminal();
	ASSERT( pTerm );

	ProcessorID testID;
	testID.SetStrDict( pTerm->inStrDict );
	testID.setID( _strProcName );
	vector<CString> _strVec;
	if( pTerm->miscData->IfUsedThisProcssor( testID,pTerm,_strVec) )
	{
		AfxMessageBox(" Linkage data is using this Processor ! Can not change processor type !");
		return true;
	}

	if( pTerm->flightSchedule->IfUsedThisGate( testID ) )
	{
		AfxMessageBox(" Gate assignment is using this Processor ! Can not change processor type !");
		return true;
	}

	return false;
	
}

void CNodeView::ChangeProcType( CString _strProcName, int _iChangeToType )
{
	
	
	InputTerminal* pTerm = GetInputTerminal();
	ASSERT( pTerm );

	ProcessorID testID;
	testID.SetStrDict( pTerm->inStrDict );
	testID.setID( _strProcName );

	GroupIndex groupIdx =  pTerm->GetProcessorList()->getGroupIndex( testID );
	if( groupIdx.start < 0 )
	{
		return;
	}

	int iProcType = pTerm->GetProcessorList()->getProcessor( groupIdx.start )->getProcessorType();
	if( iProcType == _iChangeToType )
	{
		return;
	}

	

	Point clearPath;
	std::vector<Processor*>vProc;
	for( int i=groupIdx.start; i <= groupIdx.end; ++i )
	{
		vProc.push_back( pTerm->GetProcessorList()->getProcessor( i ) );
	}
	
	Processor* pNewProc =NULL;
	int vSize = vProc.size();
	for(int i=0; i<vSize; ++i )
	{
		Processor* pProc = vProc[ i ];
		switch( _iChangeToType )
		{
			case PointProc:
				pNewProc = new Processor;
				break;
			case GateProc:
				pNewProc = new GateProcessor;
				break;
			case FloorChangeProc:
				pNewProc = new FloorChangeProcessor;
				break;
			case DepSourceProc:
				pNewProc = new DependentSource; 
				break;

			case DepSinkProc:
				pNewProc = new DependentSink;
				break;

			case LineProc:
				pNewProc = new LineProcessor;
				break;
			case BaggageProc:
				pNewProc = new BaggageProcessor;
				break;
			case HoldAreaProc:
				pNewProc = new HoldingArea;
				break;
			case MovingSideWalkProc:
				pNewProc=new MovingSideWalk;
				break;
			case BarrierProc:
				pNewProc = new Barrier;
				break;
			case Elevator:
				pNewProc = new ElevatorProc;
				break;
			case ConveyorProc:
				pNewProc = new Conveyor;
				break;			
			case IntegratedStationProc:
				pNewProc = new IntegratedStation;
				break;
			case StairProc:
				pNewProc = new Stair;
				break;
			case EscalatorProc:
				pNewProc = new Escalator;
				break;					
		}

		if( !pNewProc )
		{
			return;
		}
		pNewProc->init( *pProc->getID() );
		Path* pPath = pProc->serviceLocationPath();
		if( pPath )
		{
			pNewProc->initServiceLocation( pPath->getCount(), pPath->getPointList() );
		}

		pPath = pProc->inConstraint();
		if( pPath )
		{
			pNewProc->initInConstraint( pPath->getCount(), pPath->getPointList() );
		}
		else
		{
			pNewProc->initInConstraint( 0, &clearPath );
		}

		pPath = pProc->outConstraint();
		if( pPath && DepSourceProc != _iChangeToType)// depsouce proc don not has out constraint
		{
			pNewProc->initOutConstraint( pPath->getCount(), pPath->getPointList() );
		}
		else
		{
			pNewProc->initOutConstraint( 0, &clearPath );
		}

		pPath = pProc->queuePath();
		if( pPath && DepSinkProc != _iChangeToType )// depsink proc don not has queue
		{
			pNewProc->initQueue( pProc->queueIsFixed(), pPath->getCount(), pPath->getPointList() );
		}
		else
		{
			pNewProc->initQueue( 0, 0, &clearPath );
		}

		pTerm->replaceProcessor( pProc, pNewProc , GetDocument()->m_ProjInfo.path );
		GetPlaceMent().ReplaceProcssor( pProc, pNewProc );
		
	}

}


LRESULT CNodeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==TP_DATA_BACK)
	{
		if(callback_ver == CALLBACK_PICKXYFROMMAP)
		  OnTempFallbackFinished(wParam,lParam);
		if(CALLBACK_ALIGNLINE == callback_ver)
			SetActiveFloorMarkerLine(wParam,lParam) ;
	}
	else if (message == WM_LBUTTONDBLCLK)
	{
		OnLButtonDblClk(wParam,lParam);
	}
	else if (message == BEGINLABELEDIT)
	{
		OnBeginLabelEdit(wParam,lParam);
	}
	else if (message == ENDLABELEDIT)
	{
		OnEndLabelEdit(wParam,lParam);
	}
	else if (message == SELCHANGED)
	{
		OnSelChanged(wParam,lParam);
	}
	return CView::WindowProc(message, wParam, lParam);	
}

BOOL CNodeView::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CView::OnChildNotify(message, wParam, lParam, pLResult);
	//if (CTreeView::OnChildNotify(message, wParam, lParam, pLResult))
	//	return TRUE;

	//return FALSE;
	//return m_pTree && m_pTree->OnChildNotify(message, wParam, lParam, pLResult);
}

BOOL CNodeView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	//if (CTreeView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	//return FALSE;
	//return m_pTree && m_pTree->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//////////////////////////////////////////////////////////////////////////
// add by bird 2003/6/23
// for report parameter copy/paste in node view
void CNodeView::setCopyReortPara( CReportParameter* _pPara )
{
	m_pCopyReportPara = _pPara;
}

CReportParameter* CNodeView::getCopyReportPara( void ) const
{
	return m_pCopyReportPara;
}

void CNodeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_DELETE:
		OnCtxDeleteProc();
		break;

	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CNodeView::OnLayoutexport() 
{
	// TODO: Add your command handler code here
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	
	CFileDialog dlgFile( FALSE, ".zip","LayoutExport.zip",OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||" , NULL, 0, FALSE );
	CString sZipFileName;
	
	if (dlgFile.DoModal() == IDOK)
	{
		sZipFileName = dlgFile.GetPathName();
	}
	else
	{
		return;
	}
	DeleteFile( sZipFileName );
	BeginWaitCursor();
	
	CString sProjectPath = pDoc->m_ProjInfo.path;
	sProjectPath += "\\INPUT";

	int iFileCount = 5;//vAllInputFileName.size();		
	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], sProjectPath.GetBuffer(0) );
	}
	strcat(pFiles[0],"\\Placements.txt");
	strcat(pFiles[1],"\\LAYOUT.LAY");
	strcat(pFiles[2],"\\MOVERS.MVR");
	strcat(pFiles[3], "\\PeopleMove.MVR");
	strcat(pFiles[4],"\\StationLaout.IN");
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return;
// 	}
	
	if (!InfoZip.AddFiles(sZipFileName, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return;
	}
	
	
// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return;
// 	}
	EndWaitCursor();
	
	AfxMessageBox("Export Layout successfully !");
	
}

void CNodeView::OnLayoutimportAppend() 
{
	// TODO: Add your command handler code here

	///////////////UnZip files

	CFileDialog dlgFile( TRUE, ".zip","*.zip",OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||", NULL, 0, FALSE );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else return;

	CString sTempPath = "C:\\WINDOWS\\Temp";
	if(!UnZipLayoutFile(sZipFileName,sTempPath))
		AfxMessageBox("UnZip Failed!");
	CFileFind f;
	bool bRet = f.FindFile("C:\\WINDOWS\\Temp\\Placements.txt") ? true : false;
	if(!bRet) AfxMessageBox("File not Fild!");


	//Version Check
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	bool bFailStation = false;
	CPlacement *pNewPlacement=new CPlacement();
	ProcessorList *pNewProcList = new ProcessorList(pDoc->GetTerminal().inStrDict);
	CRailWayData *pNewRailWay=new CRailWayData();
	PeopleMoverDataSet* pNewPeopleMove = new PeopleMoverDataSet();
	CStationLayout *pStationLayout=new CStationLayout();
	try
	{
		pNewProcList->SetInputTerminal(&pDoc->GetTerminal());
		pNewProcList->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\LAYOUT.LAY");
		
		
		pNewRailWay->SetInputTerminal(&pDoc->GetTerminal());
		pNewRailWay->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\MOVERS.MVR");

		pNewPeopleMove->SetInputTerminal(&pDoc->GetTerminal());
		pNewPeopleMove->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\PeopleMove.MVR");
		
		pStationLayout->SetInputTerminal(&pDoc->GetTerminal());
		pStationLayout->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\StationLaout.IN");
  
	    pNewPlacement->SetInputTerminal(&pDoc->GetTerminal());
		ProcessorList *pProcList = pDoc->GetTerminal().procList;
		pDoc->GetTerminal().procList = pNewProcList;
	    pNewPlacement->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\Placements.txt");
		pDoc->GetTerminal().procList = pProcList;
	}
	catch( FileVersionTooNewError* _pError )
	{
		AfxMessageBox( "File Version conflict!");
		delete _pError;
		return;
	}
    
	///////////////Floor and Name Check

	std::vector<CString> addGroup;
	std::vector<CString> failsGroup;
	std::vector<int> reasonGroup; // 1 means floor,2means name
	int nCount = pNewPlacement->m_vDefined.size();
	int nMaxFloorIndex =pDoc->GetCurModeFloor().m_vFloors.size();
    for(int i = 0; i < nCount; i++ )
    {
		CProcessor2* pProc2 = pNewPlacement->m_vDefined[i];
		int nFloor =pProc2->GetFloor();
		Processor * pProc = pProc2->GetProcessor();
		ASSERT(pProc!=NULL);
		CString proName = pProc->getID()->GetIDString();
		if(nFloor>=0 && nFloor< nMaxFloorIndex)
		{
			int nProcCount =pDoc->GetTerminal().procList->getProcessorCount();
			Processor *pTermp =pDoc->GetTerminal().procList->getProcessor(proName.GetBuffer(0));
			if(pTermp!=NULL)
			{
				failsGroup.push_back(proName);
				reasonGroup.push_back(2);
				if(pProc->getProcessorType()==IntegratedStationProc)
					bFailStation = true;
			}
			else 
			{
				addGroup.push_back(proName);
			}
		}
		else
		{
			failsGroup.push_back(proName);
			reasonGroup.push_back(1);
			if(pProc->getProcessorType()==IntegratedStationProc)
                bFailStation = true;
		}
	}

	/////////////// list Box to User

	vector<CString>::iterator iter=addGroup.begin();
	vector<CString>::iterator ite=failsGroup.begin();
	vector<int>::iterator it = reasonGroup.begin();
	CString msg="";
	if(addGroup.size()>0)
	{
		msg += "You will be add processor :";
		msg +="\r\n";

		for(;iter!=addGroup.end();iter++)
		{
		   msg+=(*iter);
		   msg += " ";
		}
	}
	msg  += "\r\n";

	if(failsGroup.size()>0)
	{
		msg += "You will be failed add processor below:";
		msg +="\r\n";
		for(;ite!=failsGroup.end();ite++,it++)
		{
			msg+=(*ite);
			msg += " ";
			if((*it)==1)
			  msg += "floor not exist";
			else
			  msg += "processorName already exist";
			msg +="\r\n";
		}
	}
	
	////////////////////// Import layout

	if(MessageBox(msg.GetBuffer(0),NULL,MB_OKCANCEL)==IDOK)
	{
		//add prolist
		for(int ii=0;ii<pNewProcList->getProcessorCount();ii++)
		{
			Processor *aProc = pNewProcList->getProcessor(ii);
			iter=addGroup.begin();
			for(;iter!=addGroup.end();iter++)
			{
				if(aProc->getID()->GetIDString()==(*iter))
				{
					pDoc->GetTerminal().procList->addProcessor (aProc,false);
				}
			}
		}
		pDoc->GetTerminal().procList->BuildProcGroupIdxTree();
		pDoc->GetTerminal().procList->setIndexes();
        pDoc->GetTerminal().procList->saveDataSet(pDoc->m_ProjInfo.path,false);
		//add placement

		for(int i = 0; i < static_cast<int>(pNewPlacement->m_vUndefined.size()); i++ )
		{
			CProcessor2* pProc2 = pNewPlacement->m_vUndefined[i];
			pDoc->GetCurrentPlacement()->m_vUndefined.push_back(pProc2);
		}
		for(int i = 0; i < static_cast<int>(pNewPlacement->m_vDefined.size()); i++ )
		{
			CProcessor2* pProc2 = pNewPlacement->m_vDefined[i];
			iter=addGroup.begin();
			for(;iter!=addGroup.end();iter++)
			{
				if(pProc2->GetProcessor()->getID()->GetIDString()==(*iter))
				{
					pDoc->GetCurrentPlacement()->m_vDefined.push_back(pProc2);
				}
			}
		}
        pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path,false);
		
		////if(!bFailStation)
		////{
		////	int carCount =pDoc->GetTerminal().pStationLayout->GetCarCount();
		////	//add StationLayout
		////	for(i=0;i<pStationLayout->GetCarCount();i++)
		////	{
		////		CCar* pCar =pStationLayout->GetCar(i);
		////		pDoc->GetTerminal().pStationLayout->AddNewCar(pCar);
		////	}
		////	if( carCount==0&& pStationLayout->GetCarCount()>0)
		////	{
		////		pDoc->GetTerminal().pStationLayout->SetAdjustPoint(pStationLayout->GetAdjustPoint());
		////		pDoc->GetTerminal().pStationLayout->SetCarCapacity(pStationLayout->GetCarCapacity());
		////		pDoc->GetTerminal().pStationLayout->SetCarNumber(pStationLayout->GetCarNumber());
		////		pDoc->GetTerminal().pStationLayout->SetCarLength(pStationLayout->GetCarLength());
		////		pDoc->GetTerminal().pStationLayout->SetCarWidth(pStationLayout->GetCarWidth());
		////		pDoc->GetTerminal().pStationLayout->SetEntryDoorNumber(pStationLayout->GetEntryDoorNumber());
		////		pDoc->GetTerminal().pStationLayout->SetExitDoorNumber(pStationLayout->GetExitDoorNumber());
		////		pDoc->GetTerminal().pStationLayout->SetExitDoorType(pStationLayout->GetExitDoorType());
		////		pDoc->GetTerminal().pStationLayout->SetPlatformLength(pStationLayout->GetPlatformLength());
		////		pDoc->GetTerminal().pStationLayout->SetPlatformWidth(pStationLayout->GetPlatformWidth());
		////		pDoc->GetTerminal().pStationLayout->SetPort1Path(pStationLayout->GetPort1Path().getCount(),pStationLayout->GetPort1Path().getPointList());
		////		pDoc->GetTerminal().pStationLayout->SetPort2Path(pStationLayout->GetPort2Path().getCount(),pStationLayout->GetPort2Path().getPointList());
		////		pDoc->GetTerminal().pStationLayout->SetPreBoardingArea(pStationLayout->GetPreBoardingArea());
		////	}
		////	pDoc->GetTerminal().pStationLayout->saveDataSet(pDoc->m_ProjInfo.path,false);

		////	//add railWay 
		////	vector<RailwayInfo*>::const_iterator iter=pNewRailWay->GetAllRailWayInfo().begin();
		////	vector<RailwayInfo*>::const_iterator iterLast=pNewRailWay->GetAllRailWayInfo().end();
		////	for(;iter!=iterLast;++iter)
		////	{
		////		((std::vector<RailwayInfo*>)pDoc->GetTerminal().pRailWay->GetAllRailWayInfo()).push_back(*iter);
		////	}
		////	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
		////	{
		////		CString strLog = _T("");
		////		strLog = _T("Import Append;");
		////		ECHOLOG->Log(RailWayLog,strLog);
		////	}
		////	pDoc->GetTerminal().pRailWay->saveDataSet(pDoc->m_ProjInfo.path,false);
		////}
	} //import layout to this project

	pDoc->ReloadInputData();
	pDoc->UpdateAllViews(this);
   AfxMessageBox("Import Succcess");
}

void CNodeView::OnLayoutimportReplace() 
{
	// TODO: Add your command handler code here

		///////////////UnZip files

	CFileDialog dlgFile( TRUE, ".zip","*.zip",OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||", NULL, 0, FALSE  );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else return;

	CString sTempPath = "C:\\WINDOWS\\Temp";
	if(!UnZipLayoutFile(sZipFileName,sTempPath))
		AfxMessageBox("UnZip Failed!");
	CFileFind f;
	bool bRet = f.FindFile("C:\\WINDOWS\\Temp\\Placements.txt") ? true : false;
	if(!bRet) AfxMessageBox("File not Fild!");


	//Version Check
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	bool bFailStation = false;
	CPlacement *pNewPlacement= new CPlacement;
	ProcessorList *pNewProcList = new ProcessorList(pDoc->GetTerminal().inStrDict);
	CRailWayData *pNewRailWay=new CRailWayData();
	PeopleMoverDataSet* pNewPeopleMove = new PeopleMoverDataSet();
	CStationLayout *pStationLayout=new CStationLayout();
	try
	{
		pNewProcList->SetInputTerminal(&pDoc->GetTerminal());
		pNewProcList->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\LAYOUT.LAY");
		
		
		pNewRailWay->SetInputTerminal(&pDoc->GetTerminal());
		pNewRailWay->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\MOVERS.MVR");

		pNewPeopleMove->SetInputTerminal(&pDoc->GetTerminal());
		pNewPeopleMove->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\PeopleMove.MVR");
		
		pStationLayout->SetInputTerminal(&pDoc->GetTerminal());
		pStationLayout->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\StationLaout.IN");

	    pNewPlacement->SetInputTerminal(&pDoc->GetTerminal());
		ProcessorList *pProcList = pDoc->GetTerminal().procList;
		pDoc->GetTerminal().procList = pNewProcList;
		
	    pNewPlacement->loadDataSetFromOtherFile("C:\\WINDOWS\\Temp\\Placements.txt");
		pDoc->GetTerminal().procList = pProcList;
	}
	catch( FileVersionTooNewError* _pError )
	{
		AfxMessageBox( "File Version conflict!");
		delete _pError;
		return;
	}
    
	///////////////Floor Check
	
	std::vector<CString> addGroup;
	std::vector<CString> failsGroup;
	int nCount = pNewPlacement->m_vDefined.size();
	int nMaxFloorIndex =pDoc->GetCurModeFloor().m_vFloors.size();
    for(int i = 0; i < nCount; i++ )
    {
		CProcessor2* pProc2 = pNewPlacement->m_vDefined[i];
		int nFloor =pProc2->GetFloor();
		Processor * pProc = pProc2->GetProcessor();
		ASSERT(pProc!=NULL);
		CString proName = pProc->getID()->GetIDString();
		if(nFloor>=0 && nFloor< nMaxFloorIndex)
		{
			addGroup.push_back(proName);
		}
		else
		{
			AfxMessageBox( "floor conflict!");
			return;
			//failsGroup.push_back(proName);
			//if(pProc->getProcessorType()==IntegratedStationProc)
            //    bFailStation = true;
		}
	}

	// list box to user
	vector<CString>::iterator iter=addGroup.begin();
	vector<CString>::iterator ite=failsGroup.begin();
	CString msg="";
	if(addGroup.size()>0)
	{
		msg += "You will  add processor below:";
		msg +="\r\n";
		
		for(;iter!=addGroup.end();iter++)
		{
			msg+=(*iter);
			msg += " ";
		}
	}
	msg  += "\r\n";
	if(pDoc->GetTerminal().procList->getProcessorCount()>0)
	{
	msg += "You will delete or  replace processor below:";
	msg +="\r\n";
	}
	for(int i=0;i<pDoc->GetTerminal().procList->getProcessorCount();i++)
	{
	   CString sName = pDoc->GetTerminal().procList->getProcessor(i)->getID()->GetIDString();
	   msg+=sName;
	   msg += " ";
	}

	////////////////////// Import layout
	
	if(MessageBox(msg.GetBuffer(0),NULL,MB_OKCANCEL)==IDOK)
	{
		//replace prolist
		for(int i=0;i<pDoc->GetTerminal().procList->getProcessorCount();i++)
		   pDoc->GetTerminal().procList->removeProcessor(i,false);
		for(int ii=0;ii<pNewProcList->getProcessorCount();ii++)
		{
			Processor *aProc = pNewProcList->getProcessor(ii);
			iter=addGroup.begin();
			for(;iter!=addGroup.end();iter++)
			{
				if(aProc->getID()->GetIDString()==(*iter))
				{
					pDoc->GetTerminal().procList->addProcessor (aProc,false);
				}
			}
		}
		pDoc->GetTerminal().procList->BuildProcGroupIdxTree();
		pDoc->GetTerminal().procList->setIndexes();
        pDoc->GetTerminal().procList->saveDataSet(pDoc->m_ProjInfo.path,false);

		//add placement
		pDoc->GetCurrentPlacement()->m_vUndefined.clear();
		pDoc->GetCurrentPlacement()->m_vDefined.clear();
		for(int i = 0; i < static_cast<int>(pNewPlacement->m_vUndefined.size()); i++ )
		{
			CProcessor2* pProc2 = pNewPlacement->m_vUndefined[i];
			pDoc->GetCurrentPlacement()->m_vUndefined.push_back(pProc2);
		}
		for(int i = 0; i < static_cast<int>(pNewPlacement->m_vDefined.size()); i++ )
		{
			CProcessor2* pProc2 = pNewPlacement->m_vDefined[i];
			pDoc->GetCurrentPlacement()->m_vDefined.push_back(pProc2);
		}
        pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path,false);

		//if(!bFailStation)
		//{
		//	//add StationLayout
		//	for(i=0;i<pDoc->GetTerminal().pStationLayout->GetCarCount();i++)
		//	    pDoc->GetTerminal().pStationLayout->DeleteCar(i);

		//	for(i=0;i<pStationLayout->GetCarCount();i++)
		//	{
		//		CCar* pCar =pStationLayout->GetCar(i);
		//		pDoc->GetTerminal().pStationLayout->AddNewCar(pCar);	
		//	}
		//	pDoc->GetTerminal().pStationLayout->SetAdjustPoint(pStationLayout->GetAdjustPoint());
		//	pDoc->GetTerminal().pStationLayout->SetCarCapacity(pStationLayout->GetCarCapacity());
		//	pDoc->GetTerminal().pStationLayout->SetCarNumber(pStationLayout->GetCarNumber());
		//	pDoc->GetTerminal().pStationLayout->SetCarLength(pStationLayout->GetCarLength());
		//	pDoc->GetTerminal().pStationLayout->SetCarWidth(pStationLayout->GetCarWidth());
		//	pDoc->GetTerminal().pStationLayout->SetEntryDoorNumber(pStationLayout->GetEntryDoorNumber());
		//	pDoc->GetTerminal().pStationLayout->SetExitDoorNumber(pStationLayout->GetExitDoorNumber());
		//	pDoc->GetTerminal().pStationLayout->SetExitDoorType(pStationLayout->GetExitDoorType());
		//	pDoc->GetTerminal().pStationLayout->SetPlatformLength(pStationLayout->GetPlatformLength());
		//	pDoc->GetTerminal().pStationLayout->SetPlatformWidth(pStationLayout->GetPlatformWidth());
		//	pDoc->GetTerminal().pStationLayout->SetPort1Path(pStationLayout->GetPort1Path().getCount(),pStationLayout->GetPort1Path().getPointList());
		//	pDoc->GetTerminal().pStationLayout->SetPort2Path(pStationLayout->GetPort2Path().getCount(),pStationLayout->GetPort2Path().getPointList());
  //          pDoc->GetTerminal().pStationLayout->SetPreBoardingArea(pStationLayout->GetPreBoardingArea());

		//	pDoc->GetTerminal().pStationLayout->saveDataSet(pDoc->m_ProjInfo.path,false);
		////	pDoc->GetTerminal().pStationLayout->SetInputTerminal(&pDoc->GetTerminal());
		////	pDoc->GetTerminal().pStationLayout->loadDataSet(pDoc->m_ProjInfo.path);
		//	//add railWay 
		//	((std::vector<RailwayInfo*>)pDoc->GetTerminal().pRailWay->GetAllRailWayInfo()).clear();
		//	vector<RailwayInfo*>::const_iterator iter=pNewRailWay->GetAllRailWayInfo().begin();
		//	vector<RailwayInfo*>::const_iterator iterLast=pNewRailWay->GetAllRailWayInfo().end();
		//	for(;iter!=iterLast;++iter)
		//	{
		//		((std::vector<RailwayInfo*>)pDoc->GetTerminal().pRailWay->GetAllRailWayInfo()).push_back(*iter);
		//	}
		//	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
		//	{
		//		CString strLog = _T("");
		//		strLog = _T("Import Replace;");
		//		ECHOLOG->Log(RailWayLog,strLog);
		//	}
		//	pDoc->GetTerminal().pRailWay->saveDataSet(pDoc->m_ProjInfo.path,false);
		////	pDoc->GetTerminal().pRailWay->SetInputTerminal(&pDoc->GetTerminal());
  //      //    pDoc->GetTerminal().pRailWay->loadDataSet(pDoc->m_ProjInfo.path);
		//}
	} //import layout to this project
	
	pDoc->ReloadInputData();
	pDoc->UpdateAllViews(this);
	AfxMessageBox("Import Success");
}
bool CNodeView::UnZipLayoutFile( const CString& _strZip, const CString& _strPath )
{
	BeginWaitCursor();
	
	// extract zip to temp path
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeUnzip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Must be initialized", MB_OK);
// 		return false;
// 	}
	
	if (!InfoZip.ExtractFiles(_strZip, CString(_strPath + "\\")))
	{
		EndWaitCursor();
		AfxMessageBox("Unzip file failed! file: " + _strZip, MB_OK);
		return false;
	}
	
// 	if (!InfoZip.FinalizeUnzip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Cannot finalize", MB_OK);
// 		return false;
// 	}
	
	EndWaitCursor();
	return true;
}

bool CNodeView::VersionCheck()
{
	return true;
}

//void CNodeView::OnNewAirsideObject(UINT nID)
//{
//	UINT nOffset = ID_NEW_ARP;
//	nOffset = nID - nOffset;
//	ProcessorClassList enumProcType;
//	switch (nOffset)
//	{
//	case 0:
////		enumProcType = ArpProcessor;
//		break;
//	case 1:
//		{
////			enumProcType = RunwayProcessor;
//			CAirsideRunwayDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 2:
//		{
//			CAirsideTaxiwayDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 3:
//		//enumProcType = StandProcessor;
//		{
//			CAirsideGateDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 4:
//		//enumProcType = DeiceBayProcessor;
//		{
//			CAirsideDeicePadDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 5:
//		enumProcType = NodeProcessor;
//		break;
//	case 6:
//		enumProcType = FixProcessor;
//		{
//			CAirsideWaypointDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 7:
////		enumProcType = ContourProcessor;
//		{
//			CAirsideContourDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 8:
//		enumProcType=ApronProcessor;
//		break;
//	case 9:
//		//enumProcType = HoldProcessor;
//		{
//			CAirsideHoldDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	case 10:
//		//enumProcType = SectorProcessor;
//		{
//			CAirsideSectorDlg dlg(-1,this);
//			dlg.DoModal();
//		}
//		break;
//	default:
//		assert(false);
//		break;
//	}
//
//}
void CNodeView::OnMenuNewProc(UINT nID)
{
//	UINT nOffset = ID_NEW_ARP;
//	nOffset = nID - nOffset;
//	ProcessorClassList enumProcType;
	
	//	Create Processor2
	CShape* pShape = SHAPESMANAGER->GetShapeByName(_T("Default"));
	ASSERT(pShape);

	//ARCVector3 mpos = Get3DMousePos(point);
	ARCVector3 mpos(0.0f, 0.0f, 0.0f);
	//// TRACE("Droped shape: %s at point:(%.2f,%.2f) %s\n", pShape->Name(), UNITSMANAGER->ConvertLength(mpos[VX]),UNITSMANAGER->ConvertLength(mpos[VY]), UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()));
	ARCVector3 mpos2D(mpos[VX], mpos[VY],0);

	CTermPlanDoc* pDoc = GetDocument();
	CProcessor2* pProc2 = new CProcessor2();
	ASSERT(pProc2 != NULL);
	pProc2->SetFloor(pDoc->m_nActiveFloor);
	pProc2->SetLocation(mpos2D);
	pProc2->SetShape(pShape);
	pProc2->SetSelectName(pDoc->GetUniqueNumber());
	GetDefaultProcDispProperties(&(pDoc->m_defDispProp), &(pProc2->m_dispProperties));
	pDoc->GetCurrentPlacement()->m_vUndefined.push_back(pProc2);
	pDoc->UnSelectAllProcs();
	pDoc->SelectProc(pProc2, FALSE);
	//add a node to the under construction node
	//pDoc->AddToUnderConstructionNode(pProc2);

	ARCVector3 _location=pProc2->GetLocation();
	//_location[Z3D] = pProc2->GetFloor()+0.5;
	_location[VZ] = GetDocument()->m_nActiveFloor * SCALE_FACTOR ;


	CProcPropDlg dlg( pProc2->GetProcessor(), _location ,this );
//	dlg.m_nForceProcType = static_cast<int>(enumProcType);
	if(dlg.DoModal() == IDOK)
	{
		Processor* pProc = dlg.GetProcessor();
		if(pProc2->GetProcessor() == NULL) {  //we have a new Processor
			pProc2->SetProcessor(pProc);
			if(pProc->getProcessorType() == IntegratedStationProc) {
				GetDefaultStationDisplayProperties(&(pProc2->m_dispProperties));
				ARCVector3 oldloc;
				oldloc = pProc2->GetLocation();
				oldloc[VZ] = 0.0;
				pProc2->SetLocation(oldloc);
			}
			if (pProc->getProcessorType() == StandProcessor
				|| pProc->getProcessorType() == FixProcessor)
			{
				ARCVector3 oldloc;
				oldloc = pProc2->GetLocation();
				Point& pt = pProc->GetServiceLocation();
				oldloc[VX] = pt.getX();
				oldloc[VY] = pt.getY();
				pProc2->SetLocation(oldloc);
			}

			//remove from undefined processor list and put in defined processor list
			pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
			CPROCESSOR2LIST* pUndefList = &(GetDocument()->GetCurrentPlacement()->m_vUndefined);
			CPROCESSOR2LIST* pDefList = &(GetDocument()->GetCurrentPlacement()->m_vDefined);
			int size = pUndefList->size();
			for(int i=0; i<static_cast<int>(pUndefList->size()); i++) {
				if(pUndefList->at(i) == pProc2) {
					pUndefList->erase(pUndefList->begin()+i);
					break;
				}
			}
			//ASSERT(i < size);
			pDefList->push_back(pProc2);
			//remove from undefined nodes and put in defined nodes
//			if (enumProcType == ContourProcessor)
//			{
//				CTVNode *pNode = GetDocument()->GetSelectedNode();
//				
////				CString strName=((CProcessor2 *)pNode->m_dwData)->GetProcessor()->getID()->GetIDString();
//
//				CString strNodeName=pProc->getID()->GetIDString();
//				CTVNode *pSubNode =new CTVNode(strNodeName,IDR_MENU_CONTOUR);
//				pSubNode->m_dwData=(DWORD)pProc2;
//				pNode->AddChild(pSubNode);
//
//				GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->AddChildNode(CONTOURROOTNODENAME,strNodeName);
//				GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->saveDataSet(GetDocument()->m_ProjInfo.path,false);			
//				
//				pDoc->UpdateAllViews(NULL, UPDATETREE_AIRSIDE);
//			}
//			else
				pDoc->MoveFromUCToDefined(pProc2);
		}
		else { //existing processor
			pProc2->SetProcessor(pProc);
			pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
		}
		//autoSnap
		//if(enumProcType ==TaxiwayProcessor ||enumProcType == RunwayProcessor || enumProcType== StandProcessor)
		//{
		//	((AirfieldProcessor *) pProc)->autoSnap(GetDocument()->GetTerminal().GetProcessorList());
		//}
		pDoc->UpdateAllViews(NULL, UPDATETREE_AIRSIDE);
	}
	else
	{
		pDoc->UnSelectAllProcs();
		CPROCESSOR2LIST* pvUndef = &(pDoc->GetCurrentPlacement()->m_vUndefined);
		for (UINT i = 0; i < pvUndef->size(); i++)
		{
			if (pvUndef->at(i) == pProc2)
			{
				pvUndef->erase(pvUndef->begin() + i);
				break;
			}
		}

		delete pProc2;
		pProc2 = NULL;
	}
}

//get ARP
void CNodeView::OnCtxArp()
{
//	CDlgGatArp dlg(this);
//	dlg.DoModal();
/*	CString strLongitude;
	CString strLatidute;
	GetDocument()->GetTerminal().m_AirsideInput->GetLL(GetDocument()->m_ProjInfo.path,strLongitude,strLatidute);
	dlgGetArp.strLongitude=strLongitude;
	dlgGetArp.strLatitude=strLatidute;

	if(dlgGetArp.DoModal()==IDOK)
	{
		strLongitude=dlgGetArp.strLongitude;
		strLatidute=dlgGetArp.strLatitude;
		GetDocument()->GetTerminal().m_AirsideInput->SetLL(GetDocument()->m_ProjInfo.path,strLongitude,strLatidute,FALSE);
	}
*/


//	CDlgAirportProperty dlg(this);
//	dlg.DoModal();

}

//void CNodeView::OnCtxIputLatlong()
//{
//	CDlgEditARP dlg(this);
//	if(dlg.DoModal()==IDOK)
//	{
//		//Update view 
//		CTVNode *pRefNode=GetDocument()->m_msManager.FindNodeByName(IDS_TVNN_AIRPORTREFERENCEPOINTS);
////		GetDocument()->RefreshProcessorTree();
//		GetDocument()->m_msManager.m_msImplAirSide.AddProcessorARPGroup(pRefNode);
//		this->OnUpdate(NULL,NODE_HINT_REFRESHNODE,(CObject *)pRefNode);
//	}
//	
//}

void CNodeView::OnCtxPickXYFromMap()
{
	IRender3DView* p3DView = GetDocument()->Get3DView();
	if( p3DView == NULL )
	{
		GetDocument()->GetMainFrame()->CreateOrActive3DView();
		p3DView = GetDocument()->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_ONEPOINT;

	//set active 
	CWnd* parentWnd=GetDocument()->GetMainFrame();

	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();

    callback_ver = CALLBACK_PICKXYFROMMAP ;
	if( !GetDocument()->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
		{
			parentWnd->ShowWindow(SW_SHOW);
			parentWnd->EnableWindow(FALSE);
			parentWnd = parentWnd->GetParent();
		}
		parentWnd->EnableWindow(FALSE);
		EnableWindow();

		return;
	}	
}
LRESULT CNodeView::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd *parentWnd=GetDocument()->GetMainFrame();
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(TRUE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	EnableWindow();
	
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	if(pData->size()>0)
	{
		CString strItemText;
		double dx,dy;
		Point ptList[MAX_POINTS];
		ARCVector3 v3D = pData->at(0);
		CString strTemp;
		dx= UNITSMANAGER->ConvertLength(v3D[VX]);
		dy=UNITSMANAGER->ConvertLength(v3D[VY]);
		strItemText.Format( "x = %.2f; y = %.2f",dx,dy);
		GetDocument()->GetTerminal().GetAirsideInput()->m_pAirportInfo->m_dx=dx;
		GetDocument()->GetTerminal().GetAirsideInput()->m_pAirportInfo->m_dy=dy;
		GetDocument()->GetTerminal().GetAirsideInput()->m_pAirportInfo->saveDataSet(GetDocument()->m_ProjInfo.path,0,"",false);
		
		//Update view 
		CTVNode *pRefNode=GetDocument()->m_msManager.FindNodeByName(IDS_TVNN_AIRPORTREFERENCEPOINTS);
		if (pRefNode->GetChildCount()>1)
		{
			CNode* pNode=pRefNode->GetChildByIdx(1);
			pNode->Name(strItemText);
			OnUpdate(NULL,NODE_HINT_REFRESHNODE,(CObject *)pRefNode);
		}
	}
	return TRUE;
}

void CNodeView::OnUpdateFloorsDelete(CCmdUI* pCmdUI) 
{
	if( EnvMode_AirSide == GetDocument()->m_systemMode )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}


void CNodeView::OnUpdateMoveUpFloor(CCmdUI* pCmdUI)
{
	CTVNode* _pSelNode = GetDocument()->GetSelectedNode();
	if(!_pSelNode)
		return;
	CTVNode* pParent = (CTVNode*)_pSelNode->Parent();
	if(!pParent)
		return;
	if(_pSelNode == pParent->GetChildByIdx(0))
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void CNodeView::OnUpdateMoveDownFloor(CCmdUI* pCmdUI)
{
	CTVNode* _pSelNode = GetDocument()->GetSelectedNode();
	if(!_pSelNode)
		return;
	CTVNode* pParent = (CTVNode*)_pSelNode->Parent();
	if(!pParent)
		return;
	if(_pSelNode == pParent->GetChildByIdx(pParent->GetChildCount()-1))
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}


void CNodeView::OnContourAddNextContour()
{

	ProcessorClassList enumProcType;

	enumProcType = ContourProcessor;


	//	Create Processor2
	CShape* pShape = SHAPESMANAGER->GetShapeByName(_T("Default"));
	ASSERT(pShape);

	//ARCVector3 mpos = Get3DMousePos(point);
	ARCVector3 mpos(0.0f, 0.0f, 0.0f);
	//// TRACE("Droped shape: %s at point:(%.2f,%.2f) %s\n", pShape->Name(), UNITSMANAGER->ConvertLength(mpos[VX]),UNITSMANAGER->ConvertLength(mpos[VY]), UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()));
	ARCVector3 mpos2D(mpos[VX], mpos[VY],0);

	CTermPlanDoc* pDoc = GetDocument();
	CProcessor2* pProc2 = new CProcessor2();
	ASSERT(pProc2 != NULL);
	pProc2->SetFloor(pDoc->m_nActiveFloor);
	pProc2->SetLocation(mpos2D);
	pProc2->SetShape(pShape);
	pProc2->SetSelectName(pDoc->GetUniqueNumber());
	GetDefaultProcDispProperties(&(pDoc->m_defDispProp), &(pProc2->m_dispProperties));
	pDoc->GetCurrentPlacement()->m_vUndefined.push_back(pProc2);
	pDoc->UnSelectAllProcs();
	pDoc->SelectProc(pProc2, FALSE);
	//add a node to the under construction node
	//pDoc->AddToUnderConstructionNode(pProc2);

	ARCVector3 _location;
	_location = pProc2->GetLocation();
	//_location[Z3D] = pProc2->GetFloor()+0.5;
	_location[VZ] = GetDocument()->m_nActiveFloor * SCALE_FACTOR ;


	CProcPropDlg dlg( pProc2->GetProcessor(), _location ,this );
	dlg.m_nForceProcType = static_cast<int>(enumProcType);
	if(dlg.DoModal() == IDOK)
	{
		Processor* pProc = dlg.GetProcessor();
		if(pProc2->GetProcessor() == NULL) {  //we have a new Processor
			pProc2->SetProcessor(pProc);
			if(pProc->getProcessorType() == IntegratedStationProc) {
				GetDefaultStationDisplayProperties(&(pProc2->m_dispProperties));
				ARCVector3 oldloc;
				oldloc = pProc2->GetLocation();
				oldloc[VZ] = 0.0;
				pProc2->SetLocation(oldloc);
			}

			//remove from undefined processor list and put in defined processor list
			pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
			CPROCESSOR2LIST* pUndefList = &(GetDocument()->GetCurrentPlacement()->m_vUndefined);
			CPROCESSOR2LIST* pDefList = &(GetDocument()->GetCurrentPlacement()->m_vDefined);
			int size = pUndefList->size();
			for(int i=0; i<static_cast<int>(pUndefList->size()); i++) {
				if(pUndefList->at(i) == pProc2) {
					pUndefList->erase(pUndefList->begin()+i);
					break;
				}
			}
			//ASSERT(i < size);
			pDefList->push_back(pProc2);
			//remove from undefined nodes and put in defined nodes
			CTVNode *pNode = GetDocument()->GetSelectedNode();
			
			CString strName=((CProcessor2 *)pNode->m_dwData)->GetProcessor()->getID()->GetIDString();

			CString strNodeName=pProc->getID()->GetIDString();
			CTVNode *pSubNode =new CTVNode(strNodeName,IDR_MENU_CONTOUR);
			pSubNode->m_dwData=(DWORD)pProc2;
			pNode->AddChild(pSubNode);

			GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->AddChildNode(strName,strNodeName);
			GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->saveDataSet(GetDocument()->m_ProjInfo.path,false);			
			pDoc->UpdateAllViews(NULL, UPDATETREE_AIRSIDE);
//			pDoc->MoveFromUCToDefined(pProc2);
		}
		else { //existing processor
			pProc2->SetProcessor(pProc);
			pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
		}
		
		pDoc->UpdateAllViews(NULL, UPDATETREE_AIRSIDE);
	}
	else
	{
		pDoc->UnSelectAllProcs();
		CPROCESSOR2LIST* pvUndef = &(pDoc->GetCurrentPlacement()->m_vUndefined);
		for (UINT i = 0; i < pvUndef->size(); i++)
		{
			if (pvUndef->at(i) == pProc2)
			{
				pvUndef->erase(pvUndef->begin() + i);
				break;
			}
		}

		delete pProc2;
		pProc2 = NULL;
	}
	
}

void CNodeView::OnCtxProcdispproperties()
{
	GetDocument()->OnCtxProcDisplayProperties();
/*
	ProcessorArray procarray;
	CPROCESSOR2LIST proc2list;
	GetDocument()->GetTerminal().GetProcessorList(EnvMode_AirSide)->getProcessorsOfType(StandProcessor,procarray);
	for (int i = 0 ;i < procarray.getCount() ; i++)
	{
		CProcessor2 *proc2 = GetDocument()->getProcessor2ByProcessor(procarray.getItem(i));
		if (proc2)
		{
			proc2list.push_back(proc2);
		}
	}
	if ( proc2list.size() > 0 )
	{	
		CDlgProcDisplayProperties dlg(&proc2list);
		
		if(dlg.DoModal() == IDOK)
		{
			GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, false);
		}
	}
*/

//	OnCtxProcproperties();
//	ASSERT(GetDocument()->m_vSelectedProcessors.size() > 0);

	//CProcessor2* pProc = m_vSelectedProcessors[m_vSelectedProcessors.size()-1];
	// show processor display properties dialog
		/*
		CProcessor2::CProcDispProperties dispProp;
		for(int i=PDP_MIN; i<=PDP_MAX; i++) {
		dispProp.bDisplay[i] = dlg.m_bDisplay[i];
		dispProp.color[i] = dlg.m_cColor[i];
		}
		pProc->m_dispProperties = dispProp;
		*/
	
}


void CNodeView::OnAddStructure()
{
}

void CNodeView::OnAddSurfaceArea()
{
	//CAirsideSurfaceDlg dlg(-1,this);
	//dlg.DoModal();
	CStructure *tstrc=new CStructure();
	tstrc->SetTerminal(&(GetDocument()->GetTerminal()));
	CDlgStructureProp dlg(tstrc,this);
	if (IDOK == dlg.DoFakeModal())
	{
		//should new a surface here
		CTermPlanDoc* pDoc	= GetDocument();
//		CStructure tstrc;

		CString strProcName =dlg.getcsLevel1()+ "-" + dlg.getcsLevel2() + "-" + dlg.getcsLevel3() + "-" +dlg.getcsLevel4();
		std::vector<ARCVector3> & PolygonPath=dlg.getServiceLocation();
		if( PolygonPath.size() > 0 )
		{
//			tstrc.SetNameStr( strProcName );	
			tstrc->setTexture( dlg.getText() ); 

			for( int i=0; i<static_cast<int>(PolygonPath.size()); i++ )
			{
				ARCVector3 v3D = PolygonPath[i];	
				Point pt(v3D[VX], v3D[VY], 0.0);
				tstrc->addPoint(pt);
			}
			
			tstrc->setFloorIndex(pDoc->m_nActiveFloor);
			pDoc->GetCurStructurelist().addStructure(tstrc);

			ProcessorID id;
			id.SetStrDict(GetDocument()->GetTerminal().inStrDict);
			id.setID(strProcName);

			std::stack<CString> vList;
			int nLength = id.idLength();
			for (int i = 0; i < nLength; i++)
			{
				vList.push(id.GetLeafName());
				id.clearLevel(id.idLength() - 1);
			}

			int nIndex = 0;
			CTVNode* pNode = NULL;
			CTVNode* pTempNode = NULL;
			int idLength = id.idLength();
			CString strNodeName = _T("");

			pNode = GetDocument()->m_msManager.GetSurfaceAreasNode();
			pNode->m_eState = CTVNode::expanded;
			while(vList.size())
			{
				pTempNode = pNode;
				strNodeName = vList.top();
				vList.pop();
				pNode = (CTVNode*)pNode->GetChildByName(strNodeName);
				if (pNode == NULL)
				{
					if (vList.size() == 0)
					{
						CTVNode* pChildNode = new CTVNode(strNodeName,IDR_CTX_STRUCTURE);
						pTempNode->AddChild(pChildNode);
						pChildNode->m_dwData = (DWORD) tstrc;
					}
					else
					{
						pNode = new CTVNode(strNodeName);
						pTempNode->AddChild(pNode);
					}
					
				}
			}
		}
		pDoc->GetCurStructurelist().saveDataSet(pDoc->m_ProjInfo.path, false);
		GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)GetDocument()->m_msManager.GetSurfaceAreasNode());
	}
	else
	{
		if (tstrc)
			delete tstrc;
	}
}


void CNodeView::OnEnableCoutourEdit()
{	
	BOOL bCoutourEditMode= GetDocument()->Get3DView()->m_bCoutourEditMode;
	GetDocument()->Get3DView()->m_bCoutourEditMode=!bCoutourEditMode;
	GetDocument()->Get3DView()->m_bHillDirtflag=TRUE;	
	GetDocument()->Get3DView()->Invalidate(FALSE);
}

void CNodeView::OnUpdateEnableCoutourEdit(CCmdUI* pCmdUI)
{
	//BOOL bCoutourEditMode= GetDocument()->Get3DView()->m_bCoutourEditMode;
	//pCmdUI->Enable(TRUE);
	//if(bCoutourEditMode) {
	//	pCmdUI->SetText("Chang To Render Mode");
	//}
	//else {
	//	pCmdUI->SetText("Change To Edit Mode");
	//}
}
void CNodeView::UpdateSurfaceNode()
{
	GetDocument()->m_msManager.RefreshSurface(GetDocument()->m_systemMode);
	//CTVNode* pSurfaceArea = GetDocument()->m_msManager.GetSurfaceAreasNode();
	//if (pSurfaceArea == NULL)
	//	return;

	//pSurfaceArea->DeleteAllChildren();

	//CStructureList* pStructureList = NULL;

	//pStructureList= & GetDocument()->GetCurStructurelist();

	//if (pStructureList == NULL)
	//	return;

	//for (UINT i = 0; i < pStructureList->getStructureNum(); i++)
	//{
	//	CString strName = pStructureList->getStructureAt(i).GetNameStr();
	//	GetDocument()->m_msManager.AddSurfaceAreaNode(strName,i,GetDocument()->m_systemMode);
	//}
	LPARAM lHint = 0L;
	if (GetDocument()->m_systemMode == EnvMode_Terminal)
		lHint = UPDATETREE_TERMINAL;
	else if (GetDocument()->m_systemMode == EnvMode_AirSide)
		lHint = UPDATETREE_AIRSIDE;
	else
		ASSERT(0);
//	m_pDocument->UpdateAllViews(NULL, lHint);
	
}
void CNodeView::OnCtxAddGate()
{
	//OnNewProc(StandProcessor);
	//CAirsideGateDlg dlg(-1,this);
	//dlg.DoModal();
}

void CNodeView::OnNewProc(ProcessorClassList procType)
{
	ProcessorClassList enumProcType = procType;
	//	Create Processor2
	CShape* pShape = SHAPESMANAGER->GetShapeByName(_T("Default"));
	
	ASSERT(pShape);

	//ARCVector3 mpos = Get3DMousePos(point);
	ARCVector3 mpos(0.0f, 0.0f, 0.0f);
	//// TRACE("Droped shape: %s at point:(%.2f,%.2f) %s\n", pShape->Name(), UNITSMANAGER->ConvertLength(mpos[VX]),UNITSMANAGER->ConvertLength(mpos[VY]), UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()));
	ARCVector3 mpos2D(mpos[VX], mpos[VY],0);

	CTermPlanDoc* pDoc = GetDocument();
	CProcessor2* pProc2 = new CProcessor2();
	ASSERT(pProc2 != NULL);
	pProc2->SetFloor(pDoc->m_nActiveFloor);
	pProc2->SetLocation(mpos2D);
	pProc2->SetShape(pShape);
	pProc2->SetSelectName(pDoc->GetUniqueNumber());
	
	GetDefaultProcDispProperties(&(pDoc->m_defDispProp), &(pProc2->m_dispProperties));
	pDoc->GetCurrentPlacement()->m_vUndefined.push_back(pProc2);
	pDoc->UnSelectAllProcs();
	pDoc->SelectProc(pProc2, FALSE);
	//add a node to the under construction node
	//pDoc->AddToUnderConstructionNode(pProc2);

	ARCVector3 _location;
	_location= pProc2->GetLocation();
	//_location[Z3D] = pProc2->GetFloor()+0.5;
	_location[VZ] = GetDocument()->m_nActiveFloor * SCALE_FACTOR ;


	CProcPropDlg dlg( pProc2->GetProcessor(), _location ,this );
	dlg.m_nForceProcType = static_cast<int>(enumProcType);
	if(dlg.DoModal() == IDOK)
	{
		Processor* pProc = dlg.GetProcessor();
		if(pProc2->GetProcessor() == NULL) {  //we have a new Processor
			pProc2->SetProcessor(pProc);
			if(pProc->getProcessorType() == IntegratedStationProc) {
				GetDefaultStationDisplayProperties(&(pProc2->m_dispProperties));
				ARCVector3 oldloc;
				oldloc= pProc2->GetLocation();
				oldloc[VZ] = 0.0;
				pProc2->SetLocation(oldloc);
			}

			if (pProc->getProcessorType() == StandProcessor
				|| pProc->getProcessorType() == FixProcessor)
			{
				ARCVector3 oldloc;
				oldloc = pProc2->GetLocation();
				Point& pt = pProc->GetServiceLocation();
				oldloc[VX] = pt.getX();
				oldloc[VY] = pt.getY();
				pProc2->SetLocation(oldloc);
			}

			//remove from undefined processor list and put in defined processor list
			pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
			CPROCESSOR2LIST* pUndefList = &(GetDocument()->GetCurrentPlacement()->m_vUndefined);
			CPROCESSOR2LIST* pDefList = &(GetDocument()->GetCurrentPlacement()->m_vDefined);
			int size = pUndefList->size();
			for(int i=0; i<static_cast<int>(pUndefList->size()); i++) {
				if(pUndefList->at(i) == pProc2) {
					pUndefList->erase(pUndefList->begin()+i);
					break;
				}
			}
			//ASSERT(i < size);
			pDefList->push_back(pProc2);
			//remove from undefined nodes and put in defined nodes
			if (enumProcType == ContourProcessor)
			{
				CTVNode *pNode = GetDocument()->GetSelectedNode();

				//				CString strName=((CProcessor2 *)pNode->m_dwData)->GetProcessor()->getID()->GetIDString();

				CString strNodeName=pProc->getID()->GetIDString();
				CTVNode *pSubNode =new CTVNode(strNodeName,IDR_MENU_CONTOUR);
				pSubNode->m_dwData=(DWORD)pProc2;
				pNode->AddChild(pSubNode);

				GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->AddChildNode(CONTOURROOTNODENAME,strNodeName);
				GetDocument()->GetTerminal().GetAirsideInput()->pContourTree->saveDataSet(GetDocument()->m_ProjInfo.path,false);			
//				pDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pNode);

//				UpdateAllViews(NULL, UPDATETREE_AIRSIDE);
			}
			else
				pDoc->MoveFromUCToDefined(pProc2);
		}
		else { //existing processor
			pProc2->SetProcessor(pProc);
			pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
		}
		//autoSnap the 
		if(enumProcType ==TaxiwayProcessor ||enumProcType == RunwayProcessor || enumProcType== StandProcessor)
		{
			((AirfieldProcessor *) pProc)->autoSnap(GetDocument()->GetTerminal().GetProcessorList());
		}
//		pDoc->UpdateAllViews(NULL, UPDATETREE_AIRSIDE);
	}
	else
	{
		pDoc->UnSelectAllProcs();
		CPROCESSOR2LIST* pvUndef = &(pDoc->GetCurrentPlacement()->m_vUndefined);
		for (UINT i = 0; i < pvUndef->size(); i++)
		{
			if (pvUndef->at(i) == pProc2)
			{
				pvUndef->erase(pvUndef->begin() + i);
				break;
			}
		}

		delete pProc2;
		pProc2 = NULL;
	}
}

void CNodeView::OnGateProcdispproperties()
{
	ProcessorArray procarray;
	CPROCESSOR2LIST proc2list;
	GetDocument()->GetTerminal().GetAirsideProcessorList()->getProcessorsOfType(StandProcessor,procarray);
	for (int i = 0 ;i < procarray.getCount() ; i++)
	{
		CProcessor2 *proc2 = GetDocument()->getProcessor2ByProcessor(procarray.getItem(i));
		if (proc2)
		{
			proc2list.push_back(proc2);
		}
	}
	if ( proc2list.size() > 0 )
	{	
		CDlgProcDisplayProperties dlg(&proc2list);

		if(dlg.DoModal() == IDOK)
		{
			GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, false);
		}
	}
}

void CNodeView::OnWallshapeAddwallshape()
{
	// TODO: Add your command handler code here
	WallShape *pNewwallsh=new WallShape();
	pNewwallsh->SetTerminal(&(GetDocument()->GetTerminal()));
	DlgWallShapeProp dlg(pNewwallsh,this);
	if (IDOK == dlg.DoFakeModal())
	{
		//should new a surface here
		CTermPlanDoc* pDoc	= GetDocument();
		

		CString strProcName =dlg.m_csLevel1+ "-" + dlg.m_csLevel2 + "-" + dlg.m_csLevel3 + "-" +dlg.m_csLevel4;
		pNewwallsh->SetFloorIndex(pDoc->m_nActiveFloor);
		pNewwallsh->SetPath(dlg.m_Wallpath);
		pDoc->GetCurWallShapeList().addShape( pNewwallsh);
		
		pDoc->GetCurWallShapeList().saveDataSet(pDoc->m_ProjInfo.path, false);
		
		ProcessorID id;
		id.SetStrDict(GetDocument()->GetTerminal().inStrDict);
		id.setID(strProcName);

		std::stack<CString> vList;
		int nLength = id.idLength();
		for (int i = 0; i < nLength; i++)
		{
			vList.push(id.GetLeafName());
			id.clearLevel(id.idLength() - 1);
		}

		int nIndex = 0;
		CTVNode* pNode = NULL;
		CTVNode* pTempNode = NULL;
		int idLength = id.idLength();
		CString strNodeName = _T("");

		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_WALLSHAPE))
		{
			CTVNode* pWallNode = GetDocument()->m_msManager.FindNodeByName(strProcTypeName);
			if (pWallNode == NULL)
				return;
			
			pWallNode->m_eState = CTVNode::expanded;
			pNode = pWallNode;
			while(vList.size())
			{
				pTempNode = pNode;
				strNodeName = vList.top();
				vList.pop();
				pNode = (CTVNode*)pNode->GetChildByName(strNodeName);
				if (pNode == NULL)
				{
					if (vList.size() == 0)
					{
						CTVNode* pChildNode = new CTVNode(strNodeName,IDR_CTX_WALLSHAPE);
						pTempNode->AddChild(pChildNode);
						pChildNode->m_dwData = (DWORD) pNewwallsh;
					}
					else
					{
						pNode = new CTVNode(strNodeName);
						pTempNode->AddChild(pNode);
					}
				}	
			}
			GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)pWallNode);
		}
	}
	else
	{
		if(pNewwallsh)
			delete pNewwallsh;	
	}


}
void CNodeView::UpdateWallShapeNode()
{
	GetDocument()->m_msManager.RefreshWallShape(GetDocument()->m_systemMode);
	
	LPARAM lHint = 0L;
	if (GetDocument()->m_systemMode == EnvMode_Terminal)
		lHint = UPDATETREE_TERMINAL;
	else if (GetDocument()->m_systemMode == EnvMode_AirSide)
		lHint = UPDATETREE_AIRSIDE;
	else
		ASSERT(0);
//	m_pDocument->UpdateAllViews(NULL, lHint);
}

void CNodeView::OnPlaceMarkerLine()
{
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	callback_ver = CALLBACK_ALIGNLINE ;
    CTermPlanDoc* pTermDoc = GetDocument() ; 
	CFloor2* pFloor = pTermDoc->GetCurModeFloor().m_vFloors[(int)pTermDoc->GetSelectedNode()->m_dwData];
	pTermDoc->ActivateFloor(pFloor->Level());
	if( !pTermDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)PICK_TWOPOINTS ) )
	{
		//MessageBox( "Error" );
		return;
	}
}
LRESULT CNodeView::SetActiveFloorMarkerLine(WPARAM wParam, LPARAM lParam)
{
	CTermPlanDoc* doc = GetDocument() ;
	doc->ActivateFloor(doc->m_nLastActiveFloor);
	CFloor2* pFloor = doc->GetCurModeFloor().m_vFloors[(int)doc->GetSelectedNode()->m_dwData];
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	DWORD item = GetTreeCtrl().GetItemData(	GetTreeCtrl() .GetSelectedItem()) ;
	if(pData->size()>1)
	{
		if(pFloor != NULL)
		{
            pFloor->UseAlignLine(TRUE) ;
			pFloor->UseMarker(FALSE) ;
			pFloor->SetAlignLine(ARCVector2(pData->at(0)[0],pData->at(0)[1]),ARCVector2(pData->at(1)[0],pData->at(1)[1])) ;
			doc->UpdateFloorInRender3DView(pFloor);
			CFloor2::SaveAlignLineFromDB(GetDocument()->GetCurrentMode(),pFloor->Level(),pFloor->GetAlignLine()) ;
		}
	}
	return true ;
}

int CNodeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | 
	TVS_SHOWSELALWAYS |TVS_EDITLABELS |WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;

	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0), this, IDC_TERMINAL_TREE);
	m_btnMoveFloorUp.Create("", WS_CHILD | BS_OWNERDRAW , CRect(0,0,0,0), this, IDC_BUTTON_MOVEUPFLOOR);
	m_btnMoveFloorDown.Create("", WS_CHILD | BS_OWNERDRAW , CRect(0,0,0,0), this, IDC_BUTTON_MOVEDOWNFLOOR);
	return (bResult ? 0 : -1);

}

void CNodeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}

void CNodeView::OnRenameProc2()
{
	CTVNode* pNode = GetDocument()->GetSelectedNode();
	CObjectDisplay* pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if(pObjDisplay == NULL)
	{
		TRACE("Error occur in function void CNodeView::OnRenameProc2() ");
		return;
	}
	if(pObjDisplay->GetType() == ObjectDisplayType_OtherObject)
		return;
	CProcessor2* pProc2 = (CProcessor2*)pObjDisplay;
	CString shapeName = pProc2->ShapeName().GetIDString();
	ASSERT(pProc2 != NULL);
	CDlgPro2Name dlg(pProc2,this);
	if (IDOK == dlg.DoModal())
	{
		CString newShapeName = pProc2->ShapeName().GetIDString();
		pProc2->ShapeName(shapeName);
		GetDocument()->GetCurrentPlacement()->m_vUndefined.removePro2(pProc2);
		pProc2->ShapeName(newShapeName);
		GetDocument()->GetCurrentPlacement()->m_vUndefined.setNode(pProc2->ShapeName());
		GetDocument()->GetCurrentPlacement()->m_vUndefined.InsertUnDefinePro2(std::make_pair(pProc2->ShapeName().GetIDString(),pProc2));
		GetDocument()->GetCurrentPlacement()->m_vUndefined.InsertShapeName(newShapeName);
		GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, true);
		ProcessorID id;
		id.SetStrDict(GetDocument()->GetTerminal().inStrDict);
		id.setID(newShapeName);
		RefreshProcessorItem(pNode,GetDocument()->UnderConstructionNode(),id);
	}
}


void CNodeView::OnBtnMoveUpFloor()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hPrevItem = m_wndTreeCtrl.GetPrevSiblingItem(hSelItem);
	if(hPrevItem)
	{
		CTVNode* pSelNode = (CTVNode*)m_wndTreeCtrl.GetItemData(hSelItem);
		int selFloor = (int)pSelNode->m_dwData;

		CTermPlanDoc* pDoc = GetDocument();
		CFloors& floors = pDoc->GetFloorByMode(EnvMode_Terminal);
		CFloor2* pSelectedFloor = floors.m_vFloors[selFloor];
		CString strMsg;
		strMsg.Format("Are you sure you want to move up the floor '%s'?", pSelectedFloor->FloorName());
		if(MessageBox(strMsg, NULL, MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			return;
		}
		else
		{
			CTVNode* pPrevNode = (CTVNode*)m_wndTreeCtrl.GetItemData(hPrevItem);
			int prevFloor = (int)pPrevNode->m_dwData;
			ASSERT(selFloor == (prevFloor - 1));
			SwapTwoFloor(selFloor, prevFloor);

			HTREEITEM hFloorsItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			ReloadFloorsItem(hFloorsItem, pSelNode->Name());
		}
	}
	return;
}

void CNodeView::OnBtnMoveDownFloor()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hNextItem = m_wndTreeCtrl.GetNextSiblingItem(hSelItem);
	if(hNextItem)
	{
		CTVNode* pSelNode = (CTVNode*)m_wndTreeCtrl.GetItemData(hSelItem);
		int selFloor = (int)pSelNode->m_dwData;

		CTermPlanDoc* pDoc = GetDocument();
		CFloors& floors = pDoc->GetFloorByMode(EnvMode_Terminal);
		CFloor2* pSelectedFloor = floors.m_vFloors[selFloor];
		CString strMsg;
		strMsg.Format("Are you sure you want to move down the floor '%s'?", pSelectedFloor->FloorName());
		if(MessageBox(strMsg, NULL, MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			return;
		}
		else
		{
			CTVNode* pNextNode = (CTVNode*)m_wndTreeCtrl.GetItemData(hNextItem);
			int nextFloor = (int)pNextNode->m_dwData;
			ASSERT(selFloor == (nextFloor + 1));
			SwapTwoFloor(selFloor, nextFloor);

			HTREEITEM hFloorsItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			ReloadFloorsItem(hFloorsItem, pSelNode->Name());
		}
	}
	return;
}

void CNodeView::SwapTwoFloor(int iSelFloor, int iTarFloor)
{
	CTermPlanDoc* pDoc = GetDocument();
	CFloors& floors = pDoc->GetFloorByMode(EnvMode_Terminal);
	CFloor2* pSelectedFloor = floors.m_vFloors[iSelFloor];
	CFloor2* pTargetFloor = floors.m_vFloors[iTarFloor];

	floors.m_vFloors[iSelFloor] = pTargetFloor;
	floors.m_vFloors[iTarFloor] = pSelectedFloor;

	double newVisualHeight = pTargetFloor->Altitude();
	double newRealHeight = pTargetFloor->RealAltitude();

	if(iTarFloor+(iTarFloor-iSelFloor)<0 || iTarFloor+(iTarFloor-iSelFloor)>floors.GetCount()-1)
	{
		newVisualHeight = newVisualHeight + (iTarFloor-iSelFloor)*CUnitsManager::GetInstance()->UnConvertLength(20.0f);
		newRealHeight = newRealHeight + (iTarFloor-iSelFloor)*CUnitsManager::GetInstance()->UnConvertLength(5.0f);
	}
	else
	{
		newVisualHeight = (newVisualHeight+floors.m_vFloors[iTarFloor+(iTarFloor-iSelFloor)]->Altitude())/2;
		newRealHeight = (newRealHeight+floors.m_vFloors[iTarFloor+(iTarFloor-iSelFloor)]->RealAltitude())/2;
	}

	pSelectedFloor->Altitude(newVisualHeight);
	pSelectedFloor->RealAltitude(newRealHeight);
	if(pDoc->GetActiveFloor() == pSelectedFloor)
	{
		pDoc->ActivateFloor(iTarFloor);
	}
	if(pDoc->GetActiveFloor() == pTargetFloor)
	{
		pDoc->ActivateFloor(iSelFloor);
	}
	ResetFloorIndexToAll();
}

void CNodeView::ResetFloorIndexToAll()
{
	CTermPlanDoc* pDoc = GetDocument();

	//sort floor
	CFloors& floors = pDoc->GetFloorByMode(EnvMode_Terminal);
	FloorChangeMap floorIndexChangeMap;
	floorIndexChangeMap.nNewFloorIndex.resize(floors.GetCount());
	for(int i=0;i< floors.GetCount();i++)
	{
		floorIndexChangeMap.nNewFloorIndex[i] = floors.GetFloor2(i)->Level();
		floors.GetFloor2(i)->Level(i);
	}

	floors.saveDataSet(pDoc->m_ProjInfo.path, false);

	//modify placements
	CPlacement* pPlacement = pDoc->GetTerminalPlacement();
	//m_vUndefined
	for(size_t i=0;i<pPlacement->m_vUndefined.size();i++)
	{
		CProcessor2* proc2 = pPlacement->m_vUndefined.at(i);
		int nNewFloor = floorIndexChangeMap.getNewFloor(proc2->GetFloor());
		proc2->SetFloor(nNewFloor);
	}
	//vDefined
	for(size_t i=0;i<pPlacement->m_vDefined.size();i++)
	{
		CProcessor2* proc2 = pPlacement->m_vDefined.at(i);
		int nNewFloor = floorIndexChangeMap.getNewFloor(proc2->GetFloor());
		proc2->SetFloor(nNewFloor);
	}
	pPlacement->saveDataSet(pDoc->m_ProjInfo.path, false);
	//proclist
	ProcessorList* procList = pDoc->GetTerminal().procList;
	for(int i=0;i<procList->getProcessorCount();i++)
	{
		Processor* proc = procList->getProcessor(i);
		proc->UpdateFloorIndex(floorIndexChangeMap);
	}
	procList->saveDataSet(pDoc->m_ProjInfo.path, false);

	//pipe
	CPipeDataSet* pipes = pDoc->GetTerminal().m_pPipeDataSet;
	for(int i=0;i<pipes->GetPipeCount();i++)
	{
		CPipe* pipe = pipes->GetPipeAt(i);
		pipe->UpdateFloorIndex(floorIndexChangeMap);
	}
	pipes->saveDataSet(pDoc->m_ProjInfo.path, false);
	//structure
	CStructureList* structlist =  pDoc->GetTerminal().m_pStructureList;
	for(size_t i=0;i<structlist->getStructureNum();i++)
	{
		CStructure* structure = structlist->getStructureAt(i);
		int nNewFloor = floorIndexChangeMap.getNewFloor(structure->GetFloorIndex());
		structure->setFloorIndex(nNewFloor);
	}
	structlist->saveDataSet(pDoc->m_ProjInfo.path, false);
	//
	WallShapeList* wallshapelist = pDoc->GetTerminal().m_pWallShapeList;
	for(size_t i=0;i<wallshapelist->getShapeNum();i++)
	{
		WallShape* wall = wallshapelist->getShapeAt(i);
		int nNewFloor = floorIndexChangeMap.getNewFloor(wall->GetFloorIndex());
		wall->SetFloorIndex(nNewFloor);
	}
	wallshapelist->saveDataSet(pDoc->m_ProjInfo.path, false);
	//
	CAreas*	  arealist = pDoc->GetTerminal().m_pAreas;
	for(int i=0;i<arealist->getCount();i++)
	{
		CArea* area = arealist->getAreaByIndex(i);
		int nNewFloor = floorIndexChangeMap.getNewFloor(area->GetFloorIndex());
		area->SetFloor(nNewFloor);
	}
	arealist->saveDataSet(pDoc->m_ProjInfo.path, false);
	//
	CPortals& portals  = *pDoc->GetTerminal().m_pPortals;
	for(int i=0;i<portals.getCount();i++)
	{
		CPortal* portal = portals.getPortal(i);
		int nNewFloor = floorIndexChangeMap.getNewFloor(portal->GetFloorIndex());
		portal->SetFloor(nNewFloor);
	}
	portals.saveDataSet(pDoc->m_ProjInfo.path, false);



	// MiscElevatorData
	InputTerminal* pInTerm = GetInputTerminal();
	MiscProcessorData* pMiscDB = pInTerm->miscData->getDatabase(Elevator);
	int nMiscDataCount = pMiscDB->getCount();
	int nMaxStopFloor, nMinStopFloor;
	for(int i = 0; i < nMiscDataCount; ++i)
	{
		MiscDataElement* pMiscDataElem = (MiscDataElement*)pMiscDB->getItem(i);
		MiscElevatorData* pMiscElevData = (MiscElevatorData*)(pMiscDataElem->getData());
		int floorCount = pMiscElevData->getStopAfFloorCount();
		ASSERT(floorCount == floors.GetCount());
		std::vector<BOOL> vOldStopAtFloor;
		for(int i=0; i<floorCount; i++)
		{
			vOldStopAtFloor.push_back(pMiscElevData->getStopAtFloor(i));
			pMiscElevData->setStopAtFloor(i, FALSE);
		}

		const ProcessorID* pProcessorID  =  pMiscDataElem->getID();
		ElevProcessorFamily* tempProcessorFamily=NULL;
		tempProcessorFamily=(ElevProcessorFamily*)(GetInputTerminal()->procList->getFamilyIndex( *pProcessorID ));
		tempProcessorFamily->GetMaxMinFloorOfFamily(GetInputTerminal()->procList,nMaxStopFloor,nMinStopFloor);

		for(int i=0; i<floorCount; i++)
		{
			int newFloorIndex = floorIndexChangeMap.getNewFloor(i);
			if(nMinStopFloor<=newFloorIndex && newFloorIndex<=nMaxStopFloor)
				pMiscElevData->setStopAtFloor(newFloorIndex, vOldStopAtFloor[i]);
		}
	}
	pInTerm->miscData->saveDataSet(pDoc->m_ProjInfo.path, false);
}


void CNodeView::ReloadFloorsItem(HTREEITEM hFloorsItem, CString strSelFlrName)
{
	CTVNode* pFloorsNode = (CTVNode*)m_wndTreeCtrl.GetItemData(hFloorsItem);
	pFloorsNode->DeleteAllChildren();
	DeleteAllChildren(hFloorsItem);

	CTermPlanDoc* pDoc = GetDocument();
	for (int i=pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors.size()-1; i>=0; i--)
	{
		CString strFloor = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->FloorName();
		CTVNode* pFloorNode = new CTVNode(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->FloorName(), IDR_CTX_FLOOR);
		pFloorNode->m_iImageIdx = (i==pDoc->m_nActiveFloor?ID_NODEIMG_FLOORACTIVE:ID_NODEIMG_FLOORNOTACTIVE);
		pFloorNode->m_dwData = (DWORD) i;
		pFloorsNode->AddChild(pFloorNode);
	}
	// add the real children (only if they themselves have children)
	int floorCount = pFloorsNode->GetChildCount();
	HTREEITEM selItem = NULL;
	for(int i=0; i<floorCount; i++)
	{
		CTVNode* pChild = (CTVNode*) pFloorsNode->GetChildByIdx(i);
		if(!pChild->IsLeaf())
		{
			if(strSelFlrName == pChild->Name())
			{
				selItem = AddItem(pChild);
			}
			else
			{
				AddItem(pChild);
			}
		}
	}
	if(selItem != NULL)
	{
		m_wndTreeCtrl.SelectItem(selItem);
	}
	pFloorsNode->m_eState = CTVNode::expanded;
	m_wndTreeCtrl.SetFocus();
}
