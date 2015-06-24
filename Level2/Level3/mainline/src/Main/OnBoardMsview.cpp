// OnBoardMsview.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "./MFCExControl/ARCBaseTree.h"
#include ".\airsidemsview.h"
#include "./XPStyle/NewMenu.h"
#include "OnBoardMsview.h"
#include ".\onboardmsview.h"
#include ".\AirsideGUI\NodeViewDbClickHandler.h"
//#include "OnBoradInput.h"
//#include "../on_borad_impl/view_cmd.h"
#include "./MFCExControl/CoolTreeEditSpin.h"
#include "OnBoardLayoutView.h"
#include "ViewMsg.h"

#include "Inputs/probab.h"
#include "Common/ProbDistManager.h"
#include "../common/ACTypesManager.h"
#include "../common/AirportDatabase.h"
#include "../common/AirlineManager.h"
#include "../Common/FLT_CNST.H"
#include "../InputOnBoard/CInputOnboard.h"
#include "FlightScheduleDlg.h"
#include "DlgFlightDB.h"
#include "DlgDBAirports.h"
#include "DlgDBAirline.h" 
#include "DlgDBAircraftTypes.h"
#include "FlightDialog.h"

#include "DlgStandAssignment.h"
//#include "GateAssignDlg.h"
#include "DlgArrivalGateAssignment.h"
#include "DlgDepartureGateAssignment.h"

#include "BoardingCallDlg.h"

#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard/Deck.h"
#include "../InputOnBoard/AircraftModelConfigLoadManager.h"
#include "../Main/DlgACTypeModelDB.h"
#include "../Main/PaxTypeDefDlg.h"
#include "../Main/PaxDistDlg.h"
#include "../InputOnBoard/KineticsAndSpace.h"
#include "DlgKineticsAndSpace.h"
#include "../Main/DlgPaxCharaDisp.h"
#include "../Main/DlgAffinityGroup.h"
#include "../Main/DlgDocDisplayTime.h"
#include "DlgUnimpededBodyStateTransTime.h"
#include "DlgWayFindingIntervention.h"
#include "DlgTargetLocations.h"
#include "../Main/DlgAircraftConfiguration.h"
#include "DlgCarryonOwnership.h"
#include "DlgCarryonVolume.h"
#include "DlgUnexpectedBehaviorVariables.h"
#include "DlgSeatingPreference.h"
#include "DlgCabinCrewGeneration.h"
#include "DlgSeatAssignment.h"
#include "DlgTransitionTimeModifier.h"
#include "DlgCrewPaxInteractionCapability.h"
#include "SimEngSettingDlg.h"
#include "../Inputs/schedule.h"
#include "FltPaxDataDlg.h"
#include "DlgBoardingCallOnBoard.h"
#include "DlgCarryonStoragePriorities.h"
#include "DlgDeplanementSequenceSpec.h"
#include "DlgCrewAssignmentSpec.h"
#include "DlgDoorOperationSpecification.h"
#include "../InputOnBoard/AircraftLayoutEditContext.h"
#include "../InputOnBoard/AircraftPlacements.h"
#include "DlgSectBlockLogTime.h"
#include "LayOutFrame.h"
#include "LayoutView.h"
#include "MainFrm.h"
#include "OnBoard/AircraftLayout3DView.h"
#include "OnBoard/OnboardViewMsg.h"
#include "CalloutDispSpecDlg.h"
#include "DlgProbDist.h"
#include "OnBoard/DlgOnboardFlightSelection.h"
#include "OnBoard/AircraftLayout3DView.h"
#include "Onboard/AircraftLayOutFrame.h"
#include "onboard/AircraftLayoutEditor.h"
#include "DlgOnboardPaxCabinAssignment.h"
#include "DlgOnboardSeatTypeDefine.h"
#include "DlgSeatBlockDefine.h"
using namespace MSV;
// COnBoardMsview

DECLARE_FLIGHTTYPE_SELECT_CALLBACK()

LPCSTR strEdit[] = {_T("Number of seats:"),_T("Seat Width:"),_T("Seat Depth:"),_T("Seat Height:"),_T("Armrest Height:"),\
_T("Armrest Width Single:"),_T("Armrest Width Shared:"),_T("Backrest Hight:"),_T("Backrest thickness:"),\
_T("Underseat space clear width:"),_T("Underseat space clear depth:"),_T("Underseat space clear height:"),\
_T("Coat hook on seat back:"),_T("Passengers seat:")};
#define ACInfoNode_ID 1
#define IDC_ONBOARD_TREE 3
class OnBoradACInfoNodeData : public MSV::COnBoardNodeData
{
public:
	OnBoradACInfoNodeData() : COnBoardNodeData((emOnBoardNodeType)-1) {}
	OnBoradACInfoNodeData(emOnBoardNodeType nodetype) : COnBoardNodeData(nodetype) {}
	int get_type() { return ACInfoNode_ID; }
	std::string m_info_name;
};
#define AC3dModelNode_ID 2
class AC3dModelNodeData : public MSV::COnBoardNodeData
{
public:
	AC3dModelNodeData() : COnBoardNodeData((emOnBoardNodeType)-1) {}
	AC3dModelNodeData(emOnBoardNodeType nodetype) : COnBoardNodeData(nodetype) {}
	int get_type() { return AC3dModelNode_ID; }
};


IMPLEMENT_DYNCREATE(COnBoardMSView, CView)

COnBoardMSView::COnBoardMSView()
:m_emEditType(NoEditType)
{
	m_cur_select_item = 0;
}

COnBoardMSView::~COnBoardMSView()
{
}

//chloe
BEGIN_MESSAGE_MAP(COnBoardMSView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
//	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
//	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_COMMAND(ID_FLIGHT_SCHEDULE,OnDefineSchedule)
	ON_COMMAND(ID_FLIGHT_GROUPS_DB,OnDefineFlightDB)
	ON_COMMAND(ID_AIRLINE_GROUP_DB,OnDefineAirlineGroup)
	ON_COMMAND(ID_AIRPORT_AND_SECTORS_DB,OnDefineAirportSector)
	ON_COMMAND(ID_FLITYPE_CATEGORY,OnDefineACType)
	//ON_COMMAND(ID_POPUPMENUACINFO_LOADCONFIGURATION,OnLoadACInfoConfiguretion)
	//ON_COMMAND(ID_POPUPMENUACINFO_DEFINECONFIGURATION, OnDefineACInfoConfiguretion)
	//ON_COMMAND(ID_POPUPMENUACINFO_SAVECONFIGURATIONAS, OnSaveACInfoConfiguretionAs)
	//ON_COMMAND(ID_POPUPMENUAC3DMODEL_DEFINESHELLMODEL, OnDefineAC3DModelShell)
	//ON_COMMAND(ID_POPUPMENUAC3DMODEL_LOADSHELLMODEL, OnLoadAC3DModelShell)
	//ON_COMMAND(ID_POPUPMENUAC3DMODEL_SAVESHELLMODELAS, OnSaveAC3DModelShellAs)
	ON_COMMAND(ID_STAND_ASSIGNMENT,OnDefineStandAssignment)
	ON_COMMAND(ID_ADD_FLIGHTTYPE,OnAddFlightTypeCandidate)
	ON_COMMAND(ID_DEL_FLIGHTTYPE,OnDefineFltRemove)
	ON_COMMAND(ID_ADDCABINCLASS,OnDefineCabin)
	ON_COMMAND(ID_COPYCABINCLASSES,OnDefineCabinCopy)
	ON_COMMAND(ID_PASTECABINCLASSES,OnDefineCabinPaste)
	ON_COMMAND(ID_ASSIGNTOALLFLTTYPE,OnDefineCabinInFlt)
	ON_COMMAND(ID_ASSIGNTOTHESAMEACTYPE,OnDefineCabinFltACType)
	ON_COMMAND(ID_CHANGENAME,OnDefineCabinChangeName)
	ON_COMMAND(ID_DELETE_CLASS,OnDefineCabinRemove)
	ON_COMMAND(ID_SPECIFYSEATS,OnDefinCabinSpecify)
	ON_COMMAND(ID_DELETE_FLIGHT,OnRemoveFlight)
	ON_COMMAND(ID_FLTCANDIDATE_ADDFLIGHT,OnAddFlightToFltTypeCandidate)
	//ON_UPDATE_COMMAND_UI(ID_COPYCABINCLASSES,OnUpdateCopyCain)
	ON_UPDATE_COMMAND_UI(ID_PASTECABINCLASSES,OnUpdatePasteCabin)

//	//aircraft layout
////	ON_COMMAND(ID_LOAD_AIRCRAFTLAYOUT,OnLoadAirCraftLayOut)
//	ON_COMMAND(ID_IMPORT_AIRCRAFTLAYOUT,OnImportAircraftLayOut)
//	ON_COMMAND(ID_EXPORT_AIRCRAFTLAYOUT,OnExportAircraftLayOut)
//
//	//////////////////////////////
//	ON_COMMAND(ID_LAYOUT_RENAME,OnLayOutReName)
//	//ON_COMMAND(ID_LAYOUT_REDEFINE_ACTYPE ,OnLayOutReDefineActype)
//	ON_COMMAND(ID_LAYOUT_ADDDECK,OnLayoutAddDeck)
//	//ON_COMMAND(ID_LAYOUT_DELETEDECK,OnLayOutDelDeck)
//
//	//////////////////////////////////////////////////////////////////////////
//	ON_COMMAND(ID_DECK_ADDNEWSECTION,OnDeckNewSection)
//	ON_COMMAND(ID_DECK_DELETESECTION,OnDeckDelSection)
//    ON_COMMAND(ID_DECK_RENAME,OnDeckReName) 
//	ON_COMMAND(ID_DECK_ACTIVATE,OnDeckActive)
//	ON_COMMAND(ID_DECK_PROPERTIES,OnDeckProperty)
//	ON_COMMAND(ID_DECK_ISOLATE,OnDeckIsolater)
//	ON_COMMAND(ID_DECK_HIDE,OnDeckHide)
//	ON_COMMAND(ID_DECK_GIRDON,OnDeckGirdOn)
//	ON_COMMAND(ID_DECK_COMPOSITION_THICKNESSON,OnDeckThicknesson)
//	ON_COMMAND(ID_DECK_COMPOSITION_VISIBLEREGIONS,OnDeckVisibleRegion)
//	ON_COMMAND(ID_DECK_COMPOSITION_INVISIBLEREGIONS,OnDeckInVisibleRegion)
//	ON_COMMAND(ID_DECK_COLORMODE_MONOCHROME,OnDeckColorForMonochrome)
//    ON_COMMAND(ID_DECK_COLORMODE_VIVID,OnDeckColor_Vivid)
//	ON_COMMAND(ID_DECK_COLORMODE_DESATURATED,OnDeckColor_Desaturated)
//	ON_COMMAND(ID_DECK_DELETE,OnDeckDelete)
//	ON_COMMAND(ID_DECK_COMMENT,OnDeckComment)
//	ON_COMMAND(ID_DECK_HELP,OnDeckHelp)
//
//	//////////////////////////////////////////////////////////////////////////
//	ON_COMMAND(ID_SECTION_RENAME,OnSectionRename)
//	ON_COMMAND(ID_SECTION_POSITION,OnSectionPosition)
//	ON_COMMAND(ID_SECTION_DEFINE,OnSectionDefine)
//	ON_COMMAND(ID_SECTION_DELETE,OnSectionDelete)
//    ON_COMMAND(ID_SECTION_GRID,OnSectionGrid)

    //////////////////////////////////////////////////////////////////////////
	ON_COMMAND(ID_CONFIGURE_INTERIOR,OnConfigureInteror)
	ON_COMMAND(ID_CONFIGURE_SELECTINTERIOR,OnSelectConfigureInteror)
	ON_COMMAND(ID_UPDATE_SEAT_CLASSES,OnUpdateSeatClasses)
END_MESSAGE_MAP()


// COnBoardMsview diagnostics

#ifdef _DEBUG
void COnBoardMSView::AssertValid() const
{
	CView::AssertValid();
}

void COnBoardMSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}


#endif //_DEBUG


// COnBoardMsview message handlers
void COnBoardMSView::InitTree()
{	
	CString strLabel;
	COnBoardNodeData *pNodeData = NULL;
	HTREEITEM hTreeItem = NULL;
	int nProjID = -1;

	std::vector<ALTAirport> vAirport;
	try
	{
		nProjID = ALTAirport::GetProjectIDByName(GetARCDocument()->m_ProjInfo.name);
		ALTAirport::GetAirportList(nProjID,vAirport);
		m_nProjID = nProjID;
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the OnBoard information."));
		return;
	}
	//flights
	strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS);
	pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
	HTREEITEM hItemFlights = AddItem(strLabel);
	GetTreeCtrl().SetItemData(hItemFlights,(DWORD_PTR)pNodeData);
	
	{
		//schedule
		strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_SCHEDULE);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardFlightSchedule;
		pNodeData->nIDResource = IDR_MENU_FLIGHT_SCHEDULE;
		HTREEITEM hSchedule = AddItem(strLabel,hItemFlights);
		GetTreeCtrl().SetItemData(hSchedule,(DWORD_PTR)pNodeData);

		//flight type specification
		strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_FLIGHTTYPE);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
		HTREEITEM hItemFlightType = AddItem(strLabel,hItemFlights);
		GetTreeCtrl().SetItemData(hItemFlightType,(DWORD_PTR)pNodeData);
		{
			//Flight Groups
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_FLIGHTTYPE_FLIGHTGROUPS);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardDatabaseFlight;
			pNodeData->nIDResource = IDR_MENU_FLIGHT_GROUPS;
			pNodeData->bHasExpand = false;
			hTreeItem = AddItem(strLabel,hItemFlightType);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
			
			//Airports/Sectors	
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_FLIGHTTYPE_AIRPORTS);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardDatabaseAirportSector;
			pNodeData->nIDResource = IDR_MENU_AIRPORT_AND_SECTORS;
			pNodeData->bHasExpand = false;
			hTreeItem  = AddItem(strLabel,hItemFlightType);	
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//Airlines/Groups	
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_FLIGHTTYPE_AIRLINES);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardDatabaseAirline;
			pNodeData->nIDResource = IDR_MENU_AIRLINE_GROUP;
			pNodeData->bHasExpand = false;
			hTreeItem  = AddItem(strLabel,hItemFlightType);	
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//Aircraft/Categories	
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_FLIGHTTYPE_AIRCRAFT);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardDatabaseAircraft;
			pNodeData->nIDResource = IDR_MENU_FLITYPE_CATEGORY;
			pNodeData->bHasExpand = false;
			hTreeItem  = AddItem(strLabel,hItemFlightType);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			GetTreeCtrl().Expand(hItemFlightType,TVE_EXPAND);
		}
		
		//OnBoard Analysis Candidates
		strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_ANALYSISCANDIDATES);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
		pNodeData->nIDResource = IDR_MENU_ONBOARD_ANALYSIS;
		HTREEITEM hOnBoardAnalysis = AddItem(strLabel,hItemFlights);
		GetTreeCtrl().SetItemData(hOnBoardAnalysis,(DWORD_PTR)pNodeData);
		m_hRefTreeItems[Item_OnboardAnalysisCandidates] = hOnBoardAnalysis;
		OnInitOnBoardAnalysis(hOnBoardAnalysis);

		//load factor specification - no more now
// 		strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_LOADFACTORSPECIFICATION);
// 		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
// 		pNodeData->nSubType = Dlg_OnBoardLoadFactor;
// 		pNodeData->bHasExpand = false;
// 		HTREEITEM hLoadFactor = AddItem(strLabel,hItemFlights);
// 		GetTreeCtrl().SetItemData(hLoadFactor,(DWORD_PTR)pNodeData);


		//gate assignment
		strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_GATEASSIGNMENT);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
		HTREEITEM hItemGateAssignment = AddItem(strLabel,hItemFlights);
		GetTreeCtrl().SetItemData(hItemGateAssignment,(DWORD_PTR)pNodeData);
		{
			//stands
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_GATEASSIGNMENT_STANDS);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardStandAssignment;
			pNodeData->nIDResource = IDR_MENU_STAND_ASSIGNMENT;
			pNodeData->bHasExpand = false;
			hTreeItem  = AddItem(strLabel,hItemGateAssignment);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//arrival gates
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_GATEASSIGNMENT_ARRIVALGATES);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardArrGateAssignment;
			pNodeData->bHasExpand = false;
			hTreeItem  = AddItem(strLabel,hItemGateAssignment);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//departure gates
			strLabel.LoadString(ID_MSV_ONBOARD_FLIGHTS_GATEASSIGNMENT_DEPARTUREGATES);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardDepGateAssignment;
			pNodeData->bHasExpand = false;
			hTreeItem  = AddItem(strLabel,hItemGateAssignment);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			GetTreeCtrl().Expand(hItemGateAssignment,TVE_EXPAND);

		}

	}

	GetTreeCtrl().Expand(hItemFlights,TVE_EXPAND);

	//passenger
	strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS);
	HTREEITEM hItemPassenger = AddItem(strLabel);
	{
		//names
		strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_NAMES);
		HTREEITEM NameItem = AddItem(strLabel,hItemPassenger);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardPaxTypeDefine;
		pNodeData->bHasExpand = false;
		GetTreeCtrl().SetItemData(NameItem,(DWORD_PTR)pNodeData) ;
         
		//distributions
		strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_DISTRIBUTIONS);
		HTREEITEM DestribItem = AddItem(strLabel,hItemPassenger);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardPaxDestr ;
		pNodeData->bHasExpand = FALSE ;
		GetTreeCtrl().SetItemData(DestribItem,(DWORD_PTR)pNodeData) ;
		

		HTREEITEM hPaxCharaItem = AddItem("Physical Characteristics",hItemPassenger, TRUE) ;
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardPaxDisp;
		pNodeData->bHasExpand = false;
		GetTreeCtrl().SetItemData(hPaxCharaItem,(DWORD_PTR)pNodeData);

		HTREEITEM GroupItem = AddItem("Affinity Groups",hItemPassenger, TRUE) ;
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_AffinityGroup;
		pNodeData->bHasExpand = false;
		GetTreeCtrl().SetItemData(GroupItem,(DWORD_PTR)pNodeData);


		//Kinetics and Space
		HTREEITEM hKineticsAndSpace = AddItem("Kinetics and Space",hItemPassenger, TRUE);
		{
		
			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_SPEED);
			HTREEITEM hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_Speed;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_FREEINSTEP);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_FreeInStep;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_CONGESTIONINSTEP);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_CongestionInStep;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_QUEUEINSTEP);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_QueueInStep;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_FREESIDESTEP);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_FreeSideStep;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_CONGESTIONSIDESTEP);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_CongestionSideStep;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_QUEUESIDESTEP);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_QueueSideStep;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_INTERACTIONDISTANCE);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_InteractionDistance;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_AFFINITYFREEDISTANCE);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_AffinityFreeDistance;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_AFFINITYCONGESTIONDISTANCE);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_AffinityCongestionDistance;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_KINETICS_SQEEZABILITY);
			hKinieticsSubItem = AddItem(strLabel,hKineticsAndSpace, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardKineticsAndSpace_Sqeezability;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hKinieticsSubItem,(DWORD_PTR)pNodeData) ;

			//GetTreeCtrl().Expand(hKineticsAndSpace,TVE_EXPAND);
		}

		HTREEITEM hCarryonItem = AddItem("CarryOn Ownership",hItemPassenger, TRUE) ;
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardCarryonOwnership;
		pNodeData->bHasExpand = FALSE;
		GetTreeCtrl().SetItemData(hCarryonItem,(DWORD_PTR)pNodeData);

		HTREEITEM hCarryonVolumeItem = AddItem("CarryOn Volume",hItemPassenger) ;
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardCarryonVolumn;
		pNodeData->bHasExpand = FALSE;
		GetTreeCtrl().SetItemData(hCarryonVolumeItem,(DWORD_PTR)pNodeData);


		strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURALPROPERTIES);
		HTREEITEM hItemPaxBehavior = AddItem(strLabel, hItemPassenger) ;
		{

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_DOCDISPLAYTIME);
			HTREEITEM timeentry = AddItem(strLabel, hItemPaxBehavior, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_DocDisplayTime;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(timeentry,(DWORD_PTR)pNodeData) ;


			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_WAYFINDING);
			HTREEITEM hFindItem = AddItem(strLabel, hItemPaxBehavior, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_WayFindingIntervention;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hFindItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_UNTRANSITTIME);
			HTREEITEM hItemUnimpeded = AddItem(strLabel, hItemPaxBehavior, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_UnimpededStateTransTime;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hItemUnimpeded,(DWORD_PTR)pNodeData);


			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_TRANSITTIMEMODIFIER);
			HTREEITEM hItemTransTimeModifier = AddItem(strLabel, hItemPaxBehavior, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_TransitionTimeModifier;
			pNodeData->bHasExpand = FALSE ;
			GetTreeCtrl().SetItemData(hItemTransTimeModifier,(DWORD_PTR)pNodeData);


			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_TARGETLOCVAR);
			HTREEITEM hTargetItem = AddItem(strLabel, hItemPaxBehavior);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_TargetLocations;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hTargetItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_UNEXPECTEDPREF);
			HTREEITEM hUnexpectedItem =AddItem(strLabel, hItemPaxBehavior, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_UnexpectedBehavior;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hUnexpectedItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_SEATINGPREF);
			HTREEITEM hSeatItem = AddItem(strLabel, hItemPaxBehavior);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_SeatingPreference;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hSeatItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_PASSENGERS_BEHAVIOURAL_CARRYONPRIORITIES);
			HTREEITEM hPriorityItem = AddItem(strLabel, hItemPaxBehavior);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_CarryonStoragePriorities;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hPriorityItem,(DWORD_PTR)pNodeData);

			GetTreeCtrl().Expand(hItemPaxBehavior,TVE_EXPAND);
		}
		
		
		GetTreeCtrl().Expand(hItemPassenger,TVE_EXPAND);
	}

	//cabin crew
	strLabel.LoadString(ID_MSV_ONBOARD_CABINCREW);
	HTREEITEM hItemCabinCrew = AddItem(strLabel,TVI_ROOT, TRUE);
	{
		strLabel.LoadString(ID_MSV_ONBOARD_CABINCREW_GENERATION);
		HTREEITEM hCrewSubItem = AddItem(strLabel,hItemCabinCrew, TRUE);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardCabinCrewGeneration;
		pNodeData->bHasExpand = FALSE ;
		GetTreeCtrl().SetItemData(hCrewSubItem,(DWORD_PTR)pNodeData) ;

		strLabel.LoadString(ID_MSV_ONBOARD_CABINCREW_INTERACTIONCAPABILITY);
		hCrewSubItem = AddItem(strLabel,hItemCabinCrew, TRUE);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnBoardCrewPaxInteraction;
		pNodeData->bHasExpand = FALSE ;
		GetTreeCtrl().SetItemData(hCrewSubItem,(DWORD_PTR)pNodeData) ;

		//GetTreeCtrl().Expand(hItemCabinCrew,TVE_EXPAND);
	}

	//management strategies
	strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIES);
	HTREEITEM hItemManagementStrategies =	AddItem(strLabel);
	{
		strLabel.LoadString(IDS_MSV_ONBOARD_MANAGEMENTSTRATEGIE_PAX_CABIN_ASSIGN);
		HTREEITEM hItemPaxCabinAssign=AddItem(strLabel,hItemManagementStrategies);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_PaxCabinAssignment;
		pNodeData->bHasExpand = FALSE;
		GetTreeCtrl().SetItemData(hItemPaxCabinAssign,(DWORD_PTR)pNodeData);

		strLabel.LoadString(IDS_MSV_ONBOARD_MANAGEMENTSTRATEGIE_SEATTYPE_SPECIFICATION);
		HTREEITEM hItemSeatTypeSpecif=AddItem(strLabel,hItemManagementStrategies);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_SeatTypeDefine;
		pNodeData->bHasExpand = FALSE;
		GetTreeCtrl().SetItemData(hItemSeatTypeSpecif,(DWORD_PTR)pNodeData);


		//seat block definition
		HTREEITEM hSeatBlockItem = AddItem(_T("Seat Block"),hItemManagementStrategies);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_SeatBlockDefine;
		pNodeData->bHasExpand = FALSE;
		GetTreeCtrl().SetItemData(hSeatBlockItem,(DWORD_PTR)pNodeData);

		//boarding process
		strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_BOARDING);
		HTREEITEM hItemBoardingProcess = AddItem(strLabel,hItemManagementStrategies);
		{
			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_BOARDING_SEATASSIGN);
			HTREEITEM hSeatAssItem = AddItem(strLabel, hItemBoardingProcess);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_DepSeatAssignment;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hSeatAssItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_BOARDING_CREWASSIGN);
			hSeatAssItem = AddItem(strLabel, hItemBoardingProcess, TRUE);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_CrewAssignment;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hSeatAssItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_BOARDING_ENTRYDOORS);
			HTREEITEM hEntryDoorItem = AddItem(strLabel, hItemBoardingProcess);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_EntryDoorOperationSpec;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hEntryDoorItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_BOARDING_BOARDINGCALLS);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_OnBoardBoardingCall;
			pNodeData->bHasExpand = false;
			HTREEITEM hBoardCall = AddItem(strLabel, hItemBoardingProcess, TRUE);
			GetTreeCtrl().SetItemData(hBoardCall,(DWORD_PTR)pNodeData);


			GetTreeCtrl().Expand(hItemBoardingProcess,TVE_EXPAND);
		}

		//deplanement process
		strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIES_DEPLANEMENT);
		HTREEITEM hItemDeplaneProcess = AddItem(strLabel,hItemManagementStrategies);
		{
			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_DEPLANEMENT_SEATASSIGN);
			//AddItem(strLabel, hItemDeplaneProcess);
			HTREEITEM hSeatAssItem = AddItem(strLabel, hItemDeplaneProcess);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_ArrSeatAssignment;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hSeatAssItem,(DWORD_PTR)pNodeData);

			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_DEPLANEMENT_CREWASSING);
			AddItem(strLabel, hItemDeplaneProcess, TRUE);

			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_DEPLANEMENT_EXITDOORS);
			HTREEITEM hExitDoorItem = AddItem(strLabel, hItemDeplaneProcess);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_ExitDoorOperationSpec;
			pNodeData->bHasExpand = FALSE;
			GetTreeCtrl().SetItemData(hExitDoorItem,(DWORD_PTR)pNodeData);            

			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_LAGTIME;
			pNodeData->bHasExpand = false;
			HTREEITEM item = AddItem("Lag Time",hItemDeplaneProcess, TRUE) ;
			GetTreeCtrl().SetItemData(item,(DWORD_PTR)pNodeData) ;


			strLabel.LoadString(ID_MSV_ONBOARD_MANAGEMENTSTRATEGIE_DEPLANEMENT_DEPSEQUENCE);
			item = AddItem(strLabel, hItemDeplaneProcess, TRUE);
            
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
			pNodeData->nSubType = Dlg_DeplanementSequ;
			pNodeData->bHasExpand = false;
			GetTreeCtrl().SetItemData(item,(DWORD_PTR)pNodeData) ;


			GetTreeCtrl().Expand(hItemDeplaneProcess,TVE_EXPAND);
		}


		GetTreeCtrl().Expand(hItemManagementStrategies,TVE_EXPAND);
	}

	//simulation
	strLabel.LoadString(ID_MSV_ONBOARD_SIMULATION);
	HTREEITEM hItemSimulation =	AddItem(strLabel);
	{
		//settings
		strLabel.LoadString(ID_MSV_ONBOARD_SIMULATION_SETTINGS);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnboardSimSetting;
		HTREEITEM hSimSetting = AddItem(strLabel,hItemSimulation);
		GetTreeCtrl().SetItemData(hSimSetting, (DWORD_PTR) pNodeData);

		//run
		strLabel.LoadString(ID_MSV_ONBOARD_SIMULATION_RUN);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_RunSimulation;
		HTREEITEM hSimDialog = AddItem(strLabel,hItemSimulation);
		GetTreeCtrl().SetItemData(hSimDialog,(DWORD_PTR)pNodeData);

		
		GetTreeCtrl().Expand(hItemSimulation,TVE_EXPAND);
	}

	//analysis parameter
	strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS);
	HTREEITEM hItemAnalysisParameter =	AddItem(strLabel);
	{
		//mobile element display
		strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_MOBILEELEMENTDISPLAY);
		AddItem(strLabel,hItemAnalysisParameter);
		
		//mobile element tags
		strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_MOBILEELEMENTTAGS);
		AddItem(strLabel,hItemAnalysisParameter);
		
		strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_FURNISHINGINFODISP);
		AddItem(strLabel,hItemAnalysisParameter);

		strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_STORAGEDISPPARAS);
		AddItem(strLabel,hItemAnalysisParameter);

		strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_DYNCLOADDISP);
		AddItem(strLabel,hItemAnalysisParameter);

		//auto reports
		strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS);
		HTREEITEM hItemAutoReport = AddItem(strLabel,hItemAnalysisParameter);
		{
			//seating time
			strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_SEATINGTIME);
			AddItem(strLabel,hItemAutoReport);

			//interventions
			strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_INTERVENTIONS);
			AddItem(strLabel,hItemAutoReport);

			//crew-pax interactions
			strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_CREWPAXINTERACTIONS);
			AddItem(strLabel,hItemAutoReport);

			//pax-crew bumding
			strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_PAXCREWBUMPING);
			AddItem(strLabel,hItemAutoReport);
			
			//pax-pax bumding
			strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_PAXPAXBUMPING);
			AddItem(strLabel,hItemAutoReport);

			//utilization
			strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION);
			HTREEITEM hItemUtilization = AddItem(strLabel,hItemAutoReport);
			
			{
				//seat
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_SEAT);
				AddItem(strLabel,hItemUtilization);

				//Aisle
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_AISLE);
				AddItem(strLabel,hItemUtilization);


				//toilet
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_TOILET);
				AddItem(strLabel,hItemUtilization);

				//storage
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_STORAGE);
				AddItem(strLabel,hItemUtilization);
				

				//stairs
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_STAIRS);
				AddItem(strLabel,hItemUtilization);

				//elevator
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_ELEVATORS);
				AddItem(strLabel,hItemUtilization);

				//escalators
				strLabel.LoadString(ID_MSV_ONBOARD_ANALYSISPARAMETERS_AUTOREPORTS_UTILIZATION_ESCALATORS);
				AddItem(strLabel,hItemUtilization);
		

			}

			//GetTreeCtrl().Expand(hItemAutoReport,TVE_EXPAND);

		}

		//run
		strLabel.LoadString(IDS_TVNN_CALLOUT_DISP_SPEC);
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnboardCalloutDispSpec;
		HTREEITEM hCallDispSpec = AddItem(strLabel,hItemAnalysisParameter);
		GetTreeCtrl().SetItemData(hCallDispSpec,(DWORD_PTR)pNodeData);


		//GetTreeCtrl().Expand(hItemAnalysisParameter,TVE_EXPAND);
		GetTreeCtrl().SetItemGray(hItemAnalysisParameter,TRUE);
	}
	GetTreeCtrl().SelectItem(hItemFlights);

}
HTREEITEM COnBoardMSView::AddItem(const CString& strNode,HTREEITEM hParent,BOOL bGray ,HTREEITEM hInsertAfter,int nImage ,int nSelectImage )
{
	COOLTREE_NODE_INFO cni;
	ALTObjectList vObject;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_NORMAL;
	cni.nImage = nImage;
	cni.nImageSeled = nSelectImage;
	if(bGray)
		cni.clrItem = RGB(192,192,192);

	return GetTreeCtrl().InsertItem(strNode,cni,FALSE,FALSE,hParent,TVI_LAST);
//	return GetTreeCtrl().InsertItem(strNode,nImage,nSelectImage,hParent,hInsertAfter);
}
void COnBoardMSView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	if (m_imgList.m_hImageList == NULL)
	{
		m_imgList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
		//CBitmap bmp;
		//bmp.LoadBitmap(IDB_COOLTREE);
		//m_imgList.Add(&bmp,RGB(255,0,255));
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

	GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
	GetTreeCtrl().SetImageList(&m_imgList,TVSIL_STATE);

	m_pOnBoardList = GetARCDocument()->getARCport()->GetInputOnboard()->GetOnBoardFlightCandiates();
	InitTree();
	SetScrollPos(SB_VERT,0);

	// TODO: Add your specialized code here and/or call the base class
}

BOOL COnBoardMSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

void MSV::COnBoardMSView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	m_strData = GetTreeCtrl().GetItemText(hItem);
	int nPos = m_strData.Find(':');
	m_strData = m_strData.Left(nPos + 1);
	for (int i = 0; i < 14; i++)
	{
		if (m_strData.CompareNoCase(strEdit[i]) == 0)
		{
			m_emEditType = (ItemEditType)i;
			return;
		}
	}
	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData( hItem );
	if(pNodeData != NULL)
	{
		//if(pNodeData->nOtherData == TYPE_3DMODEL)
		//{
		//	//new message 
		//	//to be continue ,
		//	CDlgACTypeModelDB DBDlg ;
		//	DBDlg.setAcManager(GetARCDocument()->GetTerminal().m_pAirportDB->getAcMan()) ;
		//	DBDlg.DoModal();
		//	GetTreeCtrl().SetItemText(hItem,FormatAircraftModelNodeName(pNodeData->strDesc)) ;
		//	GetTreeCtrl().Expand(hItem,TVE_EXPAND);
		//	return ;
		//}
	}
	
	if (pNodeData == NULL || pNodeData->nodeType != OnBoardNodeType_Dlg)
	{
		return;
	}
	switch(pNodeData->nSubType)
	{
	case Dlg_OnBoardFlightSchedule:
		{
			CFlightScheduleDlg dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardDatabaseFlight:
		{
			CDlgFlightDB dlg(m_nProjID,&GetARCDocument()->GetTerminal());
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardDatabaseAirportSector:
		{
			CDlgDBAirports dlg(FALSE,&GetARCDocument()->GetTerminal());
			dlg.setAirportMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirportMan() );
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardDatabaseAirline:
		{
			CDlgDBAirline dlg(FALSE,&GetARCDocument()->GetTerminal() );
			dlg.setAirlinesMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan() );
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardDatabaseAircraft:
		{
			CDlgDBAircraftTypes dlg(FALSE, &GetARCDocument()->GetTerminal() );
			dlg.setAcManager( GetARCDocument()->GetTerminal().m_pAirportDB->getAcMan() );
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardStandAssignment:
		{
			CDlgStandAssignment dlg(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB, this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardArrGateAssignment:
		{
			CDlgArrivalGateAssignment dlg(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB, this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardDepGateAssignment:
		{
			CDlgDepartureGateAssignment dlg(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB, this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardBoardingCall:
		{
			CDlgBoardingCallOnBoard dlg(m_nProjID,&(GetARCDocument()->GetTerminal()),GetARCDocument()->GetTerminal().m_pAirportDB,m_pOnBoardList,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardPaxTypeDefine:
		{
			CPaxTypeDefDlg PaxType(this) ;
			PaxType.DoModal() ;
		}
		break ;
	case Dlg_OnBoardPaxDestr:
		{
			CPaxDistDlg dlg( this );
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_Speed:
		{
			DlgKineticsAndSpace dlg(m_nProjID,Speed,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_FreeInStep:
		{
			DlgKineticsAndSpace dlg(m_nProjID,FreeInStep,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_CongestionInStep:
		{
			DlgKineticsAndSpace dlg(m_nProjID,CongestionInStep,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_QueueInStep:
		{
			DlgKineticsAndSpace dlg(m_nProjID,QueueInStep,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_FreeSideStep:
		{
			DlgKineticsAndSpace dlg(m_nProjID,FreeSideStep,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_CongestionSideStep:
		{
			DlgKineticsAndSpace dlg(m_nProjID,CongestionSideStep,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_QueueSideStep:
		{
			DlgKineticsAndSpace dlg(m_nProjID,QueueSideStep,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_InteractionDistance:
		{
			DlgKineticsAndSpace dlg(m_nProjID,InteractionDistance,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_AffinityFreeDistance:
		{
			DlgKineticsAndSpace dlg(m_nProjID,AffinityFreeDistance,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_AffinityCongestionDistance:
		{
			DlgKineticsAndSpace dlg(m_nProjID,AffinityCongestionDistance,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardKineticsAndSpace_Sqeezability:
		{
			DlgKineticsAndSpace dlg(m_nProjID,Squeezability,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardPaxDisp:
		{
			CDlgPaxCharaDisp dlg(&(GetARCDocument()->GetTerminal()),m_nProjID,this);
			dlg.DoModal();
		}
		break;
	case Dlg_AffinityGroup:
		{
			CDlgAffinityGroup dlg(&(GetARCDocument()->GetTerminal()) ,this) ;
			dlg.DoModal() ;
		}
		break;
	case Dlg_DocDisplayTime:
		{
			CDlgDocDisplayTime dlg(&(GetARCDocument()->GetTerminal()),this) ;
			dlg.DoModal() ;
		}
		break;
	case Dlg_WayFindingIntervention:
		{
			CDlgWayFindingIntervention dlg(&(GetARCDocument()->GetTerminal()),m_nProjID,this);
			dlg.DoModal();
		}
		break;
	case Dlg_TargetLocations:
		{
			CDlgTargetLocations dlg(&(GetARCDocument()->GetTerminal()),m_nProjID,this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardCarryonOwnership:
		{
			DlgCarryonOwnership dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardCarryonVolumn:
		{
			CDlgCarryonVolume dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_UnexpectedBehavior:
		{
			DlgUnexpectedBehaviorVariables dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_SeatingPreference:
		{
			CDlgSeatingPreference dlg(&(GetARCDocument()->GetTerminal()),m_nProjID,this);
			dlg.DoModal();
		}
		break;
	case Dlg_CarryonStoragePriorities:
		{
			CTermPlanDoc* pDoc = GetARCDocument();
			if (pDoc->GetTerminal().flightSchedule->getCount() <=0)
			{
				AfxMessageBox(_T("Please define flights and storages in flight first!"));
				return;
			}

			DlgCarryonStoragePriorities dlg(&(GetARCDocument()->GetTerminal()),GetARCDocument()->GetInputOnboard(),this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardCabinCrewGeneration:
		{
			DlgCabinCrewGeneration dlg(&(GetARCDocument()->GetTerminal()),GetARCDocument(),this);
			dlg.DoModal();
		}
		break;
	case Dlg_UnimpededStateTransTime:
		{
			CDlgUnimpededBodyStateTransTime dlg(&(GetARCDocument()->GetTerminal()),this) ;
			dlg.DoModal() ;
		}
		break;
	case Dlg_DepSeatAssignment:
		{
			CDlgSeatAssignment dlg(GetARCDocument()->GetInputOnboard(),&(GetARCDocument()->GetTerminal()),m_nProjID,DepFlight,this);
			dlg.DoModal();
		}
		break;
	case Dlg_ArrSeatAssignment:
		{
			CDlgSeatAssignment dlg(GetARCDocument()->GetInputOnboard(),&(GetARCDocument()->GetTerminal()),m_nProjID,ArrFlight,this);
			dlg.DoModal();
		}
		break;

	case Dlg_TransitionTimeModifier:
		{
			CDlgTransitionTimeModifier dlg(&(GetARCDocument()->GetTerminal()),this) ;
			dlg.DoModal();
		}
		break;
	case Dlg_OnBoardCrewPaxInteraction:
		{
			CDlgCrewPaxInteractionCapability dlg(&(GetARCDocument()->GetTerminal()),this);
			dlg.DoModal();
		}
		break;

	case Dlg_OnboardSimSetting:
		{
			CTermPlanDoc* pDoc = GetARCDocument();
			if (pDoc->GetTerminal().flightSchedule->getCount() <=0)
			{
				AfxMessageBox(_T("Please define flight schedule first."));
				return;
			}

			CSimEngSettingDlg dlg(pDoc->GetTerminal().m_pSimParam, this );
			dlg.DoModal();
		}
		break;

	case Dlg_RunSimulation:
		{
			theApp.GetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_RUNSIM);
		}
		break;

	case Dlg_OnboardCalloutDispSpec:
		{
			CCalloutDispSpecDlg dlg( FuncSelectFlightType, m_nProjID, GetARCDocument(), this);
			dlg.DoModal();
		}
		break;
	case Dlg_OnboardSeatClassLoadFactor:
		{
			FlightAnalysisLoadFactor* pLoadFactor = (FlightAnalysisLoadFactor*)pNodeData->dwData;
			if (pLoadFactor)
			{
				CDlgProbDist dlg(GetARCDocument()->GetTerminal().m_pAirportDB,  TRUE);
				if(dlg.DoModal()==IDOK)
				{
					int idxPD = dlg.GetSelectedPDIdx();
					CProbDistEntry* pde = _g_GetActiveProbMan(&GetARCDocument()->GetTerminal())->getItem(idxPD);
					CString strLoadFactor;
					strLoadFactor.Format(_T("%s class (%s)"), pLoadFactor->GetSeatClassName(), pde->m_csName);
					GetTreeCtrl().SetItemText(hItem, strLoadFactor);
					pLoadFactor->SetProbDist(ProbabilityDistribution::CopyProbDistribution(pde->m_pProbDist));
					pLoadFactor->SaveData(-1/*ignore*/);
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		break;

// 	case Dlg_OnBoardLoadFactor:
// 		{
// 			CFltPaxDataDlg dlg( FLIGHT_LOAD_FACTORS, this);
// 			dlg.DoModal();
// 		}
// 		break;
	case Dlg_CrewAssignment:
		{

          CDlgCrewAssignmentSpec dlg(NULL,NULL) ;
		    dlg.DoModal() ;
		}
		break ;
	case Dlg_LAGTIME:
		{
              CDlgSectBlockLogTime dlg(m_nProjID,&(GetARCDocument()->GetTerminal()),GetARCDocument()->GetTerminal().m_pAirportDB,m_pOnBoardList,this) ;
			  dlg.DoModal() ;
		}
		break ;
	case Dlg_DeplanementSequ:
		{
			CDlgDeplanementSequenceSpec dlg(m_pOnBoardList,&(GetARCDocument()->GetTerminal()),this);
			dlg.DoModal() ;
		}
		break ;
	case Dlg_EntryDoorOperationSpec:
		{
			DlgDoorOperationSpecification dlg(&(GetARCDocument()->GetTerminal()),GetARCDocument()->GetInputOnboard(),true,this);
			dlg.DoModal();
		}
		break;
	case Dlg_ExitDoorOperationSpec:
		{
			DlgDoorOperationSpecification dlg(&(GetARCDocument()->GetTerminal()),GetARCDocument()->GetInputOnboard(),false,this);
			dlg.DoModal();
		}
		break;
	case Dlg_PaxCabinAssignment:
		{
			DlgOnboardPaxCabinAssignment dlg(GetARCDocument()->getARCport()->GetInputOnboard(),m_nProjID,&(GetARCDocument()->GetTerminal()),GetARCDocument()->getARCport()->getProjectCommon());
			dlg.DoModal();
		}
		break;
	case  Dlg_SeatTypeDefine:
		{
			DlgOnboardSeatTypeDefine dlg(GetARCDocument()->getARCport()->GetInputOnboard(),/*m_nProjID,*/&(GetARCDocument()->GetTerminal()));
			dlg.DoModal();
		}
		break;
	case Dlg_SeatBlockDefine:
		{
			CDlgSeatBlockDefine dlg(GetARCDocument()->getARCport()->GetInputOnboard(),&(GetARCDocument()->GetTerminal()));
			dlg.DoModal();
		}
		break;
	default:
		break;
	}
}

LRESULT MSV::COnBoardMSView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{

	case WM_LBUTTONDBLCLK:
		{
			OnLButtonDblClk(wParam,lParam);
			return 0;
		}
	case ENDLABELEDIT:
		OnEndLabelEdit(wParam,lParam);
		break;
	case BEGINLABELEDIT:
		OnBeginLabelEdit(wParam,lParam);
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;
			m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			if(pCB->GetCount()!=0)
			{
				pCB->ResetContent();
			}
			pCB->AddString(_T("YES"));
			pCB->AddString(_T("NO"));
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			CString strTemp = _T("");
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue=*((CString*)lParam);
			strTemp.Format(_T("%s %s"),m_strData ,strValue);
			GetTreeCtrl().SetItemText(hItem,strTemp);
			if (m_emEditType == CoatHookonSeatback)
			{
				COnBoardNodeData* pNodeData = NULL;
				HTREEITEM Level4Item = getLevelItem(hItem,4);
				pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
				CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
				HTREEITEM Level3Item = getLevelItem(hItem,3);
				pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
				COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
				HTREEITEM Level2Item = getLevelItem(hItem,2);
				pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
				FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
				if (strValue.CompareNoCase(_T("YES")) == 0)
				{
					pCabin->bCoatHook = TRUE;
				}
				else
				{
					pCabin->bCoatHook = FALSE;
				}
				GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)pCabin->bCoatHook);
				pCabin->UpdateData();
			}
		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			CString strTemp = _T("");
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue=*((CString*)lParam);
// 			strTemp.Format(_T("%s %s"),m_strData ,strValue);
 			GetTreeCtrl().SetItemText(hItem,strTemp);

			COnBoardNodeData* pNodeData = NULL;
			//HTREEITEM Level4Item = getLevelItem(hItem,3);
			//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
			//CFlightTypeCandidate* pFltData =(CFlightTypeCandidate*)pNodeData->dwData/* m_pOnBoardList->findItem(pNodeData->nOtherData)*/;

			HTREEITEM Level3Item = getLevelItem(hItem,2);
			pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
			COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
			if (pFlightData == NULL)
				return 0;

			HTREEITEM Level2Item = getLevelItem(hItem,1);
			pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
			FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
			if(pCabin == NULL)
				return 0;

			switch(m_emEditType)
			{
			case SeatNum:
				{
					pCabin->nSeats= atoi(strValue);
					strTemp.Format(_T("%s %d"),m_strData,pCabin->nSeats);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->nSeats * 100));
					pCabin->UpdateData();
				}
				break;
			case SeatWidth:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
					pCabin->fSeatWdth = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fSeatWdth);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fSeatWdth * 100));
					pCabin->UpdateData();
				}
				break;
			case SeatHeight:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = /*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = /*pFltData->findItem(pNodeData->nOtherData)*/;
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
					pCabin->fSeatHeight= atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fSeatHeight);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fSeatHeight * 100));
					pCabin->UpdateData();
				}
				break;
			case SeatDepth:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData /*pFlightData->findItem(pNodeData->nOtherData)*/;
					pCabin->fSeatDepth = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fSeatDepth);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fSeatDepth * 100));
					pCabin->UpdateData();
				}
				break;
			case ArmrestHeight:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
					pCabin->fArmrestHeight = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fArmrestHeight);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fArmrestHeight * 100));
					pCabin->UpdateData();
				}
				break;
			case ArmrestWidthSingle:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
					pCabin->fArmrestWidthSingle =atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fArmrestWidthSingle);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fArmrestWidthSingle * 100));
					pCabin->UpdateData();
				}
				break;
			case ArmrestWidthShared:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = pFlightData->findItem(pNodeData->nOtherData);
					pCabin->fArmrestWidthShared = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fArmrestWidthShared);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fArmrestWidthShared * 100));
					pCabin->UpdateData();
				}
				break;
			case BackrestHight:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = pFlightData->findItem(pNodeData->nOtherData);
					pCabin->fBackrestHight = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fBackrestHight);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fBackrestHight * 100));
					pCabin->UpdateData();
				}
				break;
			case BackrestThickness:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = pFlightData->findItem(pNodeData->nOtherData);
					pCabin->fBackrestThickness = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fBackrestThickness);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fBackrestThickness * 100));
					pCabin->UpdateData();
				}
				break;
			case UnderseatSpaceClearWidth:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = pFlightData->findItem(pNodeData->nOtherData);
					pCabin->fUnderWidth = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fBackrestThickness);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fUnderWidth * 100));
					pCabin->UpdateData();
				}
				break;
			case UnderseatSpaceClearDepth:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = pFlightData->findItem(pNodeData->nOtherData);
					pCabin->fUnderDepth = atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fUnderDepth);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fUnderDepth * 100));
					pCabin->UpdateData();
				}
				break;
			case UnderseatSpaceClearHeight:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,4);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					//HTREEITEM Level2Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level2Item);
					//FlightCabinData* pCabin = pFlightData->findItem(pNodeData->nOtherData);
					pCabin->fUnderHeight =atof(strValue);
					strTemp.Format(_T("%s %.2f"),m_strData,pCabin->fUnderHeight);
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pCabin->fUnderHeight * 100));
					pCabin->UpdateData();
				}
				break;
			case PassengersSeat:
				{
					//HTREEITEM Level4Item = getLevelItem(hItem,3);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level4Item);
					//CFlightTypeCandidate* pFltData = m_pOnBoardList->findItem(pNodeData->nOtherData);
					//HTREEITEM Level3Item = getLevelItem(hItem,2);
					//pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(Level3Item);
					//COnboardFlight* pFlightData = pFltData->findItem(pNodeData->nOtherData);
					pFlightData->setSeats( atoi(strValue));
					strTemp.Format(_T("%s %d"),m_strData,pFlightData->getSeats());
					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(pFlightData->getSeats() * 100));
					pFlightData->UpdateData();
				}
				break;
			default:
				break;
			}
			GetTreeCtrl().SetItemText(hItem,strTemp);
			m_emEditType = NoEditType;
		}
		break;
	default:
		break;
	}
	return CView::WindowProc(message,wParam,lParam);
}
void MSV::COnBoardMSView::OnDefineACType()
{
	CDlgDBAircraftTypes dlg(FALSE, &GetARCDocument()->GetTerminal() );
	dlg.setAcManager( GetARCDocument()->GetTerminal().m_pAirportDB->getAcMan() );
	dlg.DoModal();
}

void MSV::COnBoardMSView::OnDefineAirlineGroup()
{
	CDlgDBAirline dlg(FALSE,&GetARCDocument()->GetTerminal() );
	dlg.setAirlinesMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan() );
	dlg.DoModal();
}

void MSV::COnBoardMSView::OnDefineStandAssignment()
{
	CDlgStandAssignment dlg(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB, this);
	dlg.DoModal();
}

void MSV::COnBoardMSView::OnDefineSchedule()
{
	CFlightScheduleDlg dlg(this);
	dlg.DoModal();
}

void MSV::COnBoardMSView::OnDefineAirportSector()
{
	CDlgDBAirports dlg(FALSE,&GetARCDocument()->GetTerminal());
	dlg.setAirportMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirportMan() );
	dlg.DoModal();
}

void MSV::COnBoardMSView::OnDefineFlightDB()
{
	CDlgFlightDB dlg(m_nProjID,&GetARCDocument()->GetTerminal());
	dlg.DoModal();
}

void MSV::COnBoardMSView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	*pResult = 1;
}

void MSV::COnBoardMSView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	CNewMenu menu;
	CMenu *pCtxMenu = NULL;
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(cliPt, &uFlags);
	m_hRightClkItem = hItem;
	m_strData = GetTreeCtrl().GetItemText(hItem);
	int nPos = m_strData.Find(':');
	m_strData = m_strData.Left(nPos + 1);
	for (int i = 0; i < 14; i++)
	{
		if (m_strData.CompareNoCase(strEdit[i]) == 0)
		{
			return;
		}
	}
	if((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		COnBoardNodeData* node_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData( hItem );

		if(node_data != NULL)
		{
			if(node_data->nIDResource != NULL )
			{				
				menu.LoadMenu(node_data->nIDResource);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
			}
			{
				if(node_data->get_type() == ACInfoNode_ID)
				{
					CMenu menu;
					menu.LoadMenu(IDR_MENU_POPUP_ACINFO);
					menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
					return;
				}
				else if(node_data->get_type() == AC3dModelNode_ID)
				{
					CMenu menu;
					menu.LoadMenu(IDR_MENU_POPUP_SHELL_MODEL);
					menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
					return;
				}
				else m_cur_select_item = 0;
			}
		}
	}
	if(pCtxMenu != NULL)
	{
		//UpdatePopMenu(this, pCtxMenu);
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , point.x, point.y, AfxGetMainWnd());
	}
}

CTermPlanDoc *MSV::COnBoardMSView::GetARCDocument()
{
	return (CTermPlanDoc *)GetDocument();
}
//void MSV::COnBoardMSView::add_actype_node(const char* name)
//{
//	if(!name) return;
//	((::CTermPlanDoc*)GetDocument())->UpdateAllViewEx(0, WM_ADD_ACTYPE, (WPARAM)name, 0);
//}

void MSV::COnBoardMSView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (lHint == VM_ONBOARD_LAYOUT_EDITFINISH || lHint == VM_ONBOARD_LAYOUT_SEATS_CHANGE)
	{
		CAircaftLayOut* _layout = (CAircaftLayOut*)pHint;
		if (_layout)
		{
			// ...
		}
		UpdateFlightSeatNum(_layout);
	}
// 	if(this == pSender) return;
// 	if(!is_onborad_define_msg(lHint)) return;
// 	int* data = (int*)pHint;
// 	OnUpdateEx((UINT)lHint, (WPARAM)data[0], (LPARAM)data[1]);
}
void MSV::COnBoardMSView::OnUpdateEx(UINT message, WPARAM wParam, LPARAM lParam)
{
}

void MSV::COnBoardMSView::AddAC3DModelNode()
{
	if(!m_cur_select_item) return;
	HTREEITEM item_first = GetTreeCtrl().GetChildItem(m_cur_select_item);
	while(item_first)
	{
		COnBoardNodeData* node_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(item_first);
		if(node_data && node_data->get_type() == AC3dModelNode_ID) return;
		item_first = GetTreeCtrl().GetNextSiblingItem(item_first);
	}
	HTREEITEM item = AddItem("AirCraft 3D Model", m_cur_select_item);
	AC3dModelNodeData* node_data = new AC3dModelNodeData;
	GetTreeCtrl().SetItemData(item, (DWORD_PTR)node_data);
	GetTreeCtrl().Expand(m_cur_select_item, TVE_EXPAND);
}
void MSV::COnBoardMSView::Create3dModelNode(const std::string& name)
{
	if(!m_cur_select_item) return;
	OnBoradACInfoNodeData* node_data = (OnBoradACInfoNodeData*)GetTreeCtrl().GetItemData(m_cur_select_item);
	if(!node_data) return;
	if(node_data->m_info_name.length() == 0) AddAC3DModelNode();
	node_data->m_info_name = name;
	/*OnBoradInput* input = ((::CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input;
	OnBoradACInfo* info = input->get_cur_info();
	if(!info)
	{
		info = input->add_acinfo(name);
		input->set_cur_info_name(name);
	}*/
}


//void MSV::COnBoardMSView::OnLoadACInfoConfiguretion()
//{
//	if(!m_cur_select_item) return;
//	CString name = GetTreeCtrl().GetItemText(m_cur_select_item);
//	CFileDialog dlg(TRUE,_T("onborad_actypeinfo"),("*.onborad_actypeinfo"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("onborad_actypeinfo(*.onborad_actypeinfo)|*.onborad_actypeinfo|"));
//	if(dlg.DoModal() == IDCANCEL) return;
//	CString path = dlg.GetPathName();
//	force_cur_dir();
//	if(!is_file_exsit(path.GetBuffer())) return;
//	OnBoradACInfo info2;
//	info2.load(path);
//	Create3dModelNode(name.GetBuffer());
//	//OnBoradInput* input = ((::CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input;
//	/*OnBoradACInfo* info = input->get_cur_info();
//	if(info) 
//	{
//		info->m_3d_model_path = info2.m_3d_model_path;
//		input->save_cur_actype_info_to_database();
//		((::CTermPlanDoc*)GetDocument())->UpdateAllViewEx(this, WM_LOAD_ACTYPE_CONFIG, (WPARAM)info->m_3d_model_path.c_str(), 0);
//	}*/
//}
//void MSV::COnBoardMSView::OnDefineACInfoConfiguretion()
//{
//	std::string name;
//	OnBoradACInfoNodeData* node_data = (OnBoradACInfoNodeData*)GetTreeCtrl().GetItemData(m_cur_select_item);
//	if(!node_data) return;
//	if(node_data->m_info_name.length() == 0)
//	{
//		CString node_name = GetTreeCtrl().GetItemText(m_cur_select_item);
//		name = node_name.GetBuffer();
//	}
//	else
//		name = node_data->m_info_name;
//	Create3dModelNode(name.c_str());
//	/*OnBoradInput* input = ((::CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input;
//	OnBoradACInfo* info = input->get_cur_info();
//	if(info)
//	{
//		if(info->m_3d_model_path.length() == 0) info->m_3d_model_path = name + std::string(".onborad_model");
//		input->save_cur_actype_info_to_database();
//		((::CTermPlanDoc*)GetDocument())->UpdateAllViewEx(this, WM_DEFINE_ONBORAD_ACTYPE, (WPARAM)info->m_3d_model_path.c_str(), 0);
//	}*/
//}
//void MSV::COnBoardMSView::OnSaveACInfoConfiguretionAs()
//{
//	CFileDialog dlg(FALSE,_T("onborad_actypeinfo"),("*.onborad_actypeinfo"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("onborad_actypeinfo(*.onborad_actypeinfo)|*.onborad_actypeinfo|"));
//	if(dlg.DoModal() == IDCANCEL) return;
//	CString path = dlg.GetPathName();
//	force_cur_dir();
//	/*OnBoradInput* input = ((::CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input;
//	OnBoradACInfo* info = input->get_cur_info();
//	if(info) info->save(path);*/
//}
//void MSV::COnBoardMSView::OnLoadAC3DModelShell()
//{
//	CFileDialog dlg(TRUE,_T("onborad_model"),("*.onborad_model"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("onborad_model(*.onborad_model)|*.onborad_model|"));
//	if(dlg.DoModal() == IDCANCEL) return;
//	CString name = dlg.GetPathName();
//	force_cur_dir();
//	if(!is_file_exsit(name.GetBuffer())) return;
//	/*OnBoradInput* input = ((::CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input;
//	OnBoradACInfo* info = input->get_cur_info();
//	if(info)
//	{
//		info->m_3d_model_path = name.GetBuffer();
//		input->save_cur_actype_info_to_database();
//		((::CTermPlanDoc*)GetDocument())->UpdateAllViewEx(this, WM_LOAD_ACTYPE_CONFIG, (WPARAM)info->m_3d_model_path.c_str(), 0);
//	}*/
//}
//void MSV::COnBoardMSView::OnDefineAC3DModelShell()
//{
//	/*OnBoradInput* input = ((::CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input;
//	OnBoradACInfo* info = input->get_cur_info();
//	if(info)
//	{
//		input->save_cur_actype_info_to_database();
//		((::CTermPlanDoc*)GetDocument())->UpdateAllViewEx(this, WM_DEFINE_ONBORAD_ACTYPE, (WPARAM)info->m_3d_model_path.c_str(), 0);
//	}*/
//}
//void MSV::COnBoardMSView::OnSaveAC3DModelShellAs()
//{
//	CFileDialog dlg(FALSE,_T("onborad_model"),("*.onborad_model"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("onborad_model(*.onborad_model)|*.onborad_model|"));
//	if(dlg.DoModal() == IDCANCEL) return;
//	CString name = dlg.GetPathName();
//	force_cur_dir();
//	((::CTermPlanDoc*)GetDocument())->UpdateAllViewEx(this, WM_SAVE_AC_3D_MODEL, (WPARAM)name.GetBuffer(), 0);
//}

int MSV::COnBoardMSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |TVS_EDITLABELS;

	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,542,1024),
		this, IDC_ONBOARD_TREE);

	return (bResult ? 0 : -1);

}

void MSV::COnBoardMSView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);

}
void MSV::COnBoardMSView::OnDefineCabin()
{
	FlightCabinData* pCabinData = new FlightCabinData();
	HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	COnBoardNodeData* pData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
	CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pData->dwData/*m_pOnBoardList->findItem(pData->nOtherData)*/;

	pData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	COnboardFlight* pFlightData = (COnboardFlight*)pData->dwData/*pFltData->findItem(pData->nOtherData)*/;
	pFlightData->addItem(pCabinData);

	CString strCabinName = pCabinData->m_strCabinName;
	pCabinData->m_strCabinName.Format(_T("%s%d"),strCabinName,pFlightData->getCount());
	pCabinData->SaveData(pFlightData->getID());
	COnBoardNodeData* pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
	pNodeData->nIDResource = IDR_MENU_ONBOARD_CABIN;
	//pNodeData->nOtherData = pCabinData->getID();
	HTREEITEM hParent = AddARCBaseTreeItem(pCabinData->m_strCabinName,m_hRightClkItem);
	GetTreeCtrl().SetItemData(hParent,(DWORD_PTR)pNodeData);
	{
		CString strCabinData = _T("");
		strCabinData.Format(_T("Number of seats: %d"),pCabinData->nSeats);
		HTREEITEM hNumItem = AddARCBaseTreeItem(strCabinData,hParent,NET_EDIT_WITH_VALUE);
		GetTreeCtrl().SetItemData(hNumItem,(DWORD_PTR)pCabinData->nSeats * 100);
		HTREEITEM hItem = AddARCBaseTreeItem(_T("Seat parameters"),hParent);
		{
			strCabinData.Format(_T("Seat Width: %.2f"),pCabinData->fSeatWdth);
			HTREEITEM CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fSeatWdth * 100);

			strCabinData.Format(_T("Seat Depth: %.2f"),pCabinData->fSeatDepth);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fSeatDepth * 100);

			strCabinData.Format(_T("Seat Height: %.2f"),pCabinData->fSeatHeight);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fSeatHeight * 100);

			strCabinData.Format(_T("Armrest Height: %.2f"),pCabinData->fArmrestHeight);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fArmrestHeight * 100);

			strCabinData.Format(_T("Armrest Width Single: %.2f"),pCabinData->fArmrestWidthSingle);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fArmrestWidthSingle * 100);

			strCabinData.Format(_T("Armrest Width Shared: %.2f"),pCabinData->fArmrestWidthShared);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fArmrestWidthShared * 100);

			strCabinData.Format(_T("Backrest Hight: %.2f"),pCabinData->fBackrestHight);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fBackrestHight * 100);

			strCabinData.Format(_T("Backrest thickness: %.2f"),pCabinData->fBackrestThickness);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fBackrestThickness * 100);

			strCabinData.Format(_T("Underseat space clear width: %.2f"),pCabinData->fUnderWidth);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fUnderWidth * 100);

			strCabinData.Format(_T("Underseat space clear depth: %.2f"),pCabinData->fUnderDepth);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fUnderDepth * 100);

			strCabinData.Format(_T("Underseat space clear height: %.2f"),pCabinData->fUnderHeight);
			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fUnderHeight * 100);

			
			if (pCabinData->bCoatHook)
			{
				CabinItem = AddARCBaseTreeItem(_T("Coat hook on seat back: YES"),hItem,NET_COMBOBOX);
			}
			else
			{
				CabinItem = AddARCBaseTreeItem(_T("Coat hook on seat back: NO"),hItem,NET_COMBOBOX);
			}
			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->bCoatHook);

			GetTreeCtrl().Expand(hItem,TVE_EXPAND);
		}
		GetTreeCtrl().Expand(hParent,TVE_EXPAND);
	}
	GetTreeCtrl().Expand(m_hRightClkItem,TVE_EXPAND);
}
 void MSV::COnBoardMSView::OnDefineCabinCopy()
 {
 	HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
 	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
 	CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
 	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
 	COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
	COnboardFlight* pCopyFlight = new COnboardFlight(*pFlightData);
 	m_pOnBoardList->setCopyCabin(pCopyFlight);
 }

 void MSV::COnBoardMSView::OnDefineCabinPaste()
 {
 	HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	HTREEITEM hGranItem = getLevelItem(m_hRightClkItem,2);
 	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
 	CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
 	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
 	COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
 	pFlightData->getCabinList().insert(pFlightData->getCabinList().begin(),m_pOnBoardList->getCopyCabin()->getCabinList().begin(),\
 		m_pOnBoardList->getCopyCabin()->getCabinList().end());
	pFlightData->SaveData(pFltData->getID());

	while (GetTreeCtrl().GetChildItem(hGranItem))
	{
		GetTreeCtrl().DeleteItem(GetTreeCtrl().GetChildItem(hGranItem));
	}
	OnInitOnBoardAnalysis(hGranItem);
	m_pOnBoardList->setCopyCabin(NULL);
// 	CString strFlight = _T("");
// 	pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
// 	pNodeData->nIDResource = IDR_MENU_ONBOARD_FLIGHT;
// 	pNodeData->nOtherData = pFlightData->getID();
// 	if (pFlightData->getFlightType() == ArrFlight)
// 	{
// 		strFlight.Format(_T("ARR ID: %s ACTYPE: %s"),pFlightData->getFlightID(),pFlightData->getACType());
// 	}
// 	else
// 	{
// 		strFlight.Format(_T("DEP ID: %s ACTYPE: %s"),pFlightData->getFlightID(),pFlightData->getACType());
// 	}
// 	HTREEITEM hChildItem = AddARCBaseTreeItem(strFlight,hParentItem);
// 	GetTreeCtrl().SetItemData(hChildItem,(DWORD_PTR)pNodeData);
// 
// 	HTREEITEM hAcCapacity = AddARCBaseTreeItem(_T("AC capacity"),hChildItem);
// 	{
// 		CString strFlightSeats = _T("");
// 		strFlightSeats.Format(_T("Passengers seat: %d"),pFlightData->getSeats());
// 		HTREEITEM hPassenger = AddARCBaseTreeItem(strFlightSeats,hAcCapacity,NET_EDITSPIN_WITH_VALUE);
// 		GetTreeCtrl().SetItemData(hPassenger,(DWORD_PTR)(pFlightData->getSeats()*100));
// 		AddARCBaseTreeItem(_T("Hold Cargo(TBD)"),hAcCapacity);
// 		GetTreeCtrl().Expand(hAcCapacity,TVE_EXPAND);
// 	}
// 	for (int k = 0; k < pFlightData->getCount(); k++)
// 	{
// 		FlightCabinData* pCabinData = pFlightData->getItem(k);
// 		pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
// 		pNodeData->nIDResource = IDR_MENU_ONBOARD_CABIN;
// 		pNodeData->nOtherData = pCabinData->getID();
// 		HTREEITEM hCabin = AddARCBaseTreeItem(pCabinData->m_strCabinName,hChildItem);
// 		GetTreeCtrl().SetItemData(hCabin,(DWORD_PTR)pNodeData);
// 
// 
// 		CString strCabinData = _T("");
// 		strCabinData.Format(_T("Number of seats: %d"),pCabinData->nSeats);
// 		HTREEITEM hNumItem = AddARCBaseTreeItem(strCabinData,hCabin,NET_EDIT_WITH_VALUE);
// 		GetTreeCtrl().SetItemData(hNumItem,(DWORD_PTR)(pCabinData->nSeats * 100));
// 		HTREEITEM hItem = AddARCBaseTreeItem(_T("Seat parameters"),hCabin);
// 		{
// 			strCabinData.Format(_T("Seat Width: %.2f"),pCabinData->fSeatWdth);
// 			HTREEITEM CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fSeatWdth * 100));
// 
// 			strCabinData.Format(_T("Seat Depth: %.2f"),pCabinData->fSeatDepth);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fSeatDepth * 100));
// 
// 			strCabinData.Format(_T("Seat Height: %.2f"),pCabinData->fSeatHeight);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fSeatHeight * 100));
// 
// 			strCabinData.Format(_T("Armrest Height: %.2f"),pCabinData->fArmrestHeight);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fArmrestHeight * 100));
// 
// 			strCabinData.Format(_T("Armrest Width Single: %.2f"),pCabinData->fArmrestWidthSingle);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fArmrestWidthSingle * 100);
// 
// 			strCabinData.Format(_T("Armrest Width Shared: %.2f"),pCabinData->fArmrestWidthShared);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fArmrestWidthShared * 100));
// 
// 			strCabinData.Format(_T("Backrest Hight: %.2f"),pCabinData->fBackrestHight);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fBackrestHight * 100));
// 
// 			strCabinData.Format(_T("Backrest thickness: %.2f"),pCabinData->fBackrestThickness);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fBackrestThickness * 100));
// 
// 			strCabinData.Format(_T("Underseat space clear width: %.2f"),pCabinData->fUnderWidth);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fUnderWidth * 100));
// 
// 			strCabinData.Format(_T("Underseat space clear depth: %.2f"),pCabinData->fUnderDepth);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fUnderDepth * 100));
// 
// 			strCabinData.Format(_T("Underseat space clear height: %.2f"),pCabinData->fUnderHeight);
// 			CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fUnderHeight * 100));
// 
// 
// 			if (pCabinData->bCoatHook)
// 			{
// 				CabinItem = AddARCBaseTreeItem(_T("Coat hook on seat back: YES"),hItem,NET_COMBOBOX);
// 			}
// 			else
// 			{
// 				CabinItem = AddARCBaseTreeItem(_T("Coat hook on seat back: NO"),hItem,NET_COMBOBOX);
// 			}
// 			GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->bCoatHook);
// 			GetTreeCtrl().Expand(hItem,TVE_EXPAND);
// 		}
// 		GetTreeCtrl().Expand(hCabin,TVE_EXPAND);
// 	}
// 	GetTreeCtrl().Expand(hChildItem,TVE_EXPAND);
 }

 void MSV::COnBoardMSView::OnDefineCabinInFlt()
 {
	 HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	 HTREEITEM hGranItem = getLevelItem(m_hRightClkItem,2);
	 COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
	 CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
	 pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	 COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
	 for (int i = 0; i < m_pOnBoardList->getCount(); i++)
	 {
		 CFlightTypeCandidate* pData = m_pOnBoardList->getItem(i);
		 for (int j = 0; j < pData->getCount(); j++)
		 {
			 COnboardFlight* pCopyItem = new COnboardFlight(*pFlightData);
			 COnboardFlight* pFlightItem = pData->getItem(j);
			 pFlightItem->removeAllItem();
			 pFlightItem->getCabinList().insert(pFlightItem->getCabinList().begin(),pCopyItem->getCabinList().begin(),\
				 pCopyItem->getCabinList().end());
			 pFlightItem->SaveData(pData->getID());
		 }
	 }
	 while (GetTreeCtrl().GetChildItem(hGranItem))
	 {
		GetTreeCtrl().DeleteItem(GetTreeCtrl().GetChildItem(hGranItem));
	 }
	 OnInitOnBoardAnalysis(hGranItem);
 }

 void MSV::COnBoardMSView::OnDefineCabinFltACType()
 {
	 HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	 HTREEITEM hGranItem = getLevelItem(m_hRightClkItem,2);
	 COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
	 CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
	 pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	 COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
	 for (int i = 0; i < m_pOnBoardList->getCount(); i++)
	 {
		 CFlightTypeCandidate* pData = m_pOnBoardList->getItem(i);
		 for (int j = 0; j < pData->getCount(); j++)
		 {
			 COnboardFlight* pFlightItem = pData->getItem(j);
			 if (pFlightItem->getACType().CompareNoCase(pFlightData->getACType()) == 0)
			 {
				 COnboardFlight* pCopyItem = new COnboardFlight(*pFlightData);
				 pFlightItem->removeAllItem();
				 pFlightItem->getCabinList().insert(pFlightItem->getCabinList().begin(),pCopyItem->getCabinList().begin(),\
					 pCopyItem->getCabinList().end());
				 pFlightItem->SaveData(pData->getID());
			 }
		 }
	 }
	 while (GetTreeCtrl().GetChildItem(hGranItem))
	 {
		 GetTreeCtrl().DeleteItem(GetTreeCtrl().GetChildItem(hGranItem));
	 }
	 OnInitOnBoardAnalysis(hGranItem);
 }

void MSV::COnBoardMSView::OnDefineCabinChangeName()
{
	GetTreeCtrl().EditLabel(m_hRightClkItem);
}

void MSV::COnBoardMSView::OnDefineCabinRemove()
{
	COnBoardNodeData* pNodeData = NULL;
	HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	HTREEITEM hGrandItem = GetTreeCtrl().GetParentItem(hParentItem);
	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hGrandItem);
	CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
	COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	FlightCabinData* pCabingData = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;
	pCabingData->DelData();
	pFlightData->removeItem(pCabingData);
	GetTreeCtrl().DeleteItem(m_hRightClkItem);
}

 void MSV::COnBoardMSView::OnDefinCabinSpecify()
 {
// 	 CTermPlanDoc* pActiveDoc = GetARCDocument();
// 	 if (pActiveDoc == NULL)
// 		 return;
// 
// 	 if( EnvMode_OnBoard != pActiveDoc->m_systemMode )
// 		 return;
// 
// 	 CView* pView;
// 	 POSITION pos = pActiveDoc->GetFirstViewPosition();
// 	 while (pos != NULL)
// 	 {
// 		 pView = pActiveDoc->GetNextView(pos);
// 		 if (pView->IsKindOf(RUNTIME_CLASS(COnBoardLayoutView)))
// 		 {
// 			 pView->GetParentFrame()->ActivateFrame();
// 			 ASSERT(pView->GetParentFrame()->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
// 			 CMDIChildWnd* pChildFrm = (CMDIChildWnd*)pView->GetParentFrame();
// 			 pChildFrm->ShowWindow(SW_SHOWNORMAL);
// 		 }
// 	 }
// 	 CMDIChildWnd* pNewFrame
// 		 = (CMDIChildWnd*)(((CTermPlanApp*)AfxGetApp())->m_pOnBoardLayoutTemplate->CreateNewFrame(pActiveDoc, NULL));
// 	 if (pNewFrame == NULL)
// 		 return NULL;     // not created
// 	 ASSERT_KINDOF(CMDIChildWnd, pNewFrame);
// 
// 	 ((CTermPlanApp*)AfxGetApp())->m_pOnBoardLayoutTemplate->InitialUpdateFrame(pNewFrame, pActiveDoc);
// 	 if (((CTermPlanDoc*)pActiveDoc)->m_simType == SimType_MontCarlo)
// 	 {
// 		 SendMessage(UM_SHOW_CONTROL_BAR,2,1);
// 	 }
// 
// 	 //special show Windows for Process Define Formview  
// 	 CString strTemp;
// 	 pNewFrame->GetActiveFrame()->GetWindowText(strTemp);
// 	 if(strcmp("Process Define",strTemp.Right(14)) != 0)
// 		 OnWindowSidebyside();
// 	 return pNewFrame;
// 	 pActiveDoc->UpdateAllViews(NULL,VM_ONBOARD_LAYOUT_DEFAULT,NULL);
 }

void MSV::COnBoardMSView::OnAddFlightTypeCandidate()
{
	HTREEITEM hVisibleItem = GetTreeCtrl().GetFirstVisibleItem();
	FlightConstraint fltType;
	if(SelectFlightType(fltType,NULL) == false)
		return;

	int nFltType = m_pOnBoardList->getCount();
	for (int i = 0; i < nFltType; i++)
	{
		CFlightTypeCandidate* pData = m_pOnBoardList->getItem(i);
		if (fltType == pData->GetFltType())
		{
			::AfxMessageBox(_T("The Flight Type already exist!"));
			return;
		}
	}

	CFlightTypeCandidate* pFltTypeCandidate = new CFlightTypeCandidate(m_pOnBoardList, &GetARCDocument()->GetTerminal());
	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	pFltTypeCandidate->SetFltType(fltType);

	m_pOnBoardList->addItem(pFltTypeCandidate);

	COnBoardNodeData* pNodeData = NULL;
	pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
	pNodeData->nIDResource = IDR_MENU_ONBOARD_DELFLIGHTTYPE;
	pNodeData->dwData = (DWORD_PTR)pFltTypeCandidate ;
	//pNodeData->nOtherData = pFltTypeCandidate->getID();
	HTREEITEM hParent = AddARCBaseTreeItem(_T("Flight Type ")+CString(_T(szBuffer)),m_hRightClkItem);
	GetTreeCtrl().SetItemData(hParent,(DWORD_PTR)pNodeData);

	CString configname = _T("");
	int nCount = GetARCDocument()->GetTerminal().flightSchedule->getCount();
	for (int i = 0; i < nCount; i++)
	{
		Flight* pFlight = GetARCDocument()->GetTerminal().flightSchedule->getItem(i);
		if (fltType.fits(pFlight->getLogEntry()))
		{
			char szID[1024] = {0};
			char szACType[1024] = {0};
			CString strFlight = _T("");

			if(pFlight->isArriving() && fltType.GetFltConstraintMode() != ENUM_FLTCNSTR_MODE_DEP)
			{
				pFlight->getFullID(szID,'A');
				pFlight->getACType(szACType);
				int nDay = pFlight->getArrTime().GetDay();

				COnboardFlight* pExistFlight = m_pOnBoardList->GetExistOnboardFlight(szID,szACType,ArrFlight,nDay);
				if (pExistFlight != NULL)
				{
					bool IsRearrange = RearrangeFlightToNewCandidate(pExistFlight,pFltTypeCandidate);
					if (IsRearrange)
					{
						COnboardFlight* pFlightData = new COnboardFlight(*pExistFlight);
						RemoveOnboardFlight(pExistFlight,m_hRightClkItem);

						pFltTypeCandidate->addItem(pFlightData);
						strFlight = FormatFlightIdNodeForCandidate(configname,pFlightData) ;
						insertFlightOnTree(hParent,pFltTypeCandidate,pFlightData,strFlight);
					}
				}
				else
				{
					COnboardFlight* pFlightData = new COnboardFlight;
					pFlightData->setFlightType(ArrFlight);
					pFlightData->setACType(szACType);
					pFlightData->setFlightID(szID);
					pFlightData->setDay(nDay);
					pFltTypeCandidate->addItem(pFlightData);

					//CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID()) ;
					//if(_layout != NULL)
					//	configname = _layout->GetName() ;
					strFlight = FormatFlightIdNodeForCandidate(configname,pFlightData) ;
					insertFlightOnTree(hParent,pFltTypeCandidate,pFlightData,strFlight);
				}
			}

			if (pFlight->isDeparting() && fltType.GetFltConstraintMode() != ENUM_FLTCNSTR_MODE_ARR)
			{
				pFlight->getFullID(szID,'D');
				pFlight->getACType(szACType);
				int nDay = pFlight->getDepTime().GetDay();


				COnboardFlight* pExistFlight = m_pOnBoardList->GetExistOnboardFlight(szID,szACType,DepFlight,nDay);
				if (pExistFlight != NULL)
				{
					bool IsRearrange = RearrangeFlightToNewCandidate(pExistFlight,pFltTypeCandidate);
					if (IsRearrange)
					{
						COnboardFlight* pFlightData = new COnboardFlight(*pExistFlight);
						RemoveOnboardFlight(pExistFlight,m_hRightClkItem);

						pFltTypeCandidate->addItem(pFlightData);
						strFlight = FormatFlightIdNodeForCandidate(configname,pFlightData) ;
						insertFlightOnTree(hParent,pFltTypeCandidate,pFlightData,strFlight);
					}
				}
				else
				{
					COnboardFlight* pFlightData = new COnboardFlight();
					pFlightData->setFlightType(DepFlight);

					pFlightData->setACType(szACType);
					pFlightData->setFlightID(szID);
					pFlightData->setDay(nDay);
					pFltTypeCandidate->addItem(pFlightData);

					strFlight = FormatFlightIdNodeForCandidate(configname,pFlightData) ;
					insertFlightOnTree(hParent,pFltTypeCandidate,pFlightData,strFlight);
				}
			}	
		}
	}
	pFltTypeCandidate->SaveData(m_nProjID);


	GetTreeCtrl().Expand(hParent,TVE_EXPAND);
	GetTreeCtrl().Expand(m_hRightClkItem,TVE_EXPAND);
	GetTreeCtrl().EnsureVisible(hVisibleItem);
	//AddFlightTYNodeForlayout(pFltData,m_hItemLayout) ;
}

void MSV::COnBoardMSView::insertFlightOnTree(HTREEITEM hParentItem,CFlightTypeCandidate* pFlightType,COnboardFlight* pFlightAnalysis,CString strFlight)
{
	//pFlightType->addItem(pFlightAnalysis);
	//pFlightAnalysis->SaveData(pFlightType->getID());

	COnBoardNodeData* pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
	pNodeData->nSubType = Normal_FlightData;
	pNodeData->nIDResource = IDR_MENU_ONBOARD_NEWFLIGHTID;
	//pNodeData->nOtherData = pFlightAnalysis->getID();
	pNodeData->strDesc = pFlightAnalysis->getACType() ;
	pNodeData->dwData = (DWORD_PTR)pFlightAnalysis ;
	HTREEITEM hCabin = AddARCBaseTreeItem(strFlight,hParentItem);
	GetTreeCtrl().SetItemData(hCabin,(DWORD_PTR)pNodeData);
	HTREEITEM hAcCapacity = AddARCBaseTreeItem(_T("AC capacity"),hCabin);
	COnBoardNodeData* pNodeDataACCap = new COnBoardNodeData(OnBoardNodeType_Normal);
	GetTreeCtrl().SetItemData(hAcCapacity,(DWORD_PTR)pNodeDataACCap);
	{
		CString strFlightSeats = _T("Passengers seat: (N/A)");
		HTREEITEM hPassenger = AddARCBaseTreeItem(strFlightSeats,hAcCapacity);
		COnBoardNodeData* pNodeDataPassenger = new COnBoardNodeData(OnBoardNodeType_Normal);
		pNodeDataPassenger->nSubType = Normal_PassengerSeatNum;
		GetTreeCtrl().SetItemData(hPassenger,(DWORD_PTR)pNodeDataPassenger);

		AddARCBaseTreeItem(_T("Hold Cargo(TBD)"),hAcCapacity);
	}
	GetTreeCtrl().Expand(hAcCapacity,TVE_EXPAND);
	GetTreeCtrl().Expand(hCabin,TVE_EXPAND);

	AddSeatClassesLoadFactors(hCabin, pFlightAnalysis);
}


void MSV::COnBoardMSView::OnDefineFltRemove()
{
	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData/*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
	DeleteFlightTYNodeForlayoutMap(pFltData) ;
	pFltData->removeAllItem();
	m_pOnBoardList->removeItem(pFltData);
	m_pOnBoardList->SaveData(m_nProjID);
	GetTreeCtrl().DeleteItem(m_hRightClkItem);
}
void MSV::COnBoardMSView::DeleteFlightTYNodeForlayoutMap(CFlightTypeCandidate* pFltData)
{
   CAircarftLayoutManager* layoutManager =	CAircarftLayoutManager::GetInstance() ;
	for (int j = 0; j < pFltData->getCount(); j++)
	{
		CString strFlight = _T("");
		COnboardFlight* pFlightData = pFltData->getItem(j);
		layoutManager->RemoveMappingByFlightID(pFlightData->getID()) ;
	}
}
bool MSV::COnBoardMSView::SelectFlightType(FlightConstraint& fltcons, CWnd* pParent)
{
	CFlightDialog flightDlg(pParent);
	if(IDOK == flightDlg.DoModal())
	{
		fltcons = flightDlg.GetFlightSelection();
		return true;
	}

	return false;
}

HTREEITEM MSV::COnBoardMSView::AddARCBaseTreeItem(const CString& strNode,HTREEITEM hParent /* = TVI_ROOT */,NODE_EDIT_TYPE net,NODE_TYPE nt,HTREEITEM hInsertAfter /* = TVI_LAST */,int nImage /* = ID_NODEIMG_DEFAULT */,int nSelectImage /* = ID_NODEIMG_DEFAULT */)
{
	COOLTREE_NODE_INFO cni;
	ALTObjectList vObject;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt=nt;
	cni.net=net;
	cni.nImage = nImage;
	cni.nImageSeled = nSelectImage;
	return GetTreeCtrl().InsertItem(strNode,cni,FALSE,FALSE,hParent,TVI_LAST);
}
//////////////////////////////////////////////////////////////////////////
//
//void MSV::COnBoardMSView::OnInitOnBoardLayOut(HTREEITEM hItem)
//{
//	int nCount = m_pOnBoardList->getCount();
//	for (int i = 0; i < nCount; i++)
//	{
//		CFlightTypeCandidate* pFltData = m_pOnBoardList->getItem(i);
//        AddFlightTYNodeForlayout(pFltData,hItem);
//
//	}
//	GetTreeCtrl().Expand(hItem,TVE_EXPAND);
//}
//void MSV::COnBoardMSView::AddFlightTYNodeForlayout(CFlightTypeCandidate* _pFltData,HTREEITEM _item)
//{
//	if(_pFltData == NULL || _pFltData->getCount() == 0)
//		return ;
//	COnBoardNodeData* pNodeData = NULL;
//	pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
//	pNodeData->nOtherData = _pFltData->getID();
//	HTREEITEM hCurItem = AddARCBaseTreeItem(_pFltData->getFltName(),_item);
//	GetTreeCtrl().SetItemData(hCurItem,(DWORD_PTR)pNodeData);
//	for (int j = 0; j < _pFltData->getCount(); j++)
//	{
//		CString strFlight = _T("");
//		COnboardFlight* pFlightData = _pFltData->getItem(j);
//		AddFlightIDNodeForlayout(pFlightData,hCurItem) ;
//	}
//	GetTreeCtrl().Expand(hCurItem,TVE_EXPAND);
//}
//void MSV::COnBoardMSView::AddFlightIDNodeForlayout(COnboardFlight* _flightAnalysis ,HTREEITEM _item)
//{
//	if(_flightAnalysis == NULL)
//		return ;
//	CString strFlight = _T("");
//	COnboardFlight* pFlightData = _flightAnalysis;
//	COnBoardNodeData* pNodeData  = new COnBoardNodeData(OnBoardNodeType_Normal);
//	pNodeData->nIDResource = IDR_MENU_ONBOARD_LAYOUT_FlIGHT;
//	pNodeData->nOtherData = pFlightData->getID();
//	pNodeData->strDesc = pFlightData->getACType() ;
//	if (pFlightData->getFlightType() == ArrFlight)
//	{
//		strFlight.Format(_T("ARR ID: %s ACTYPE: %s"),pFlightData->getFlightID(),pFlightData->getACType());
//	}
//	else
//	{
//		strFlight.Format(_T("DEP ID: %s ACTYPE: %s"),pFlightData->getFlightID(),pFlightData->getACType());
//	}
//	HTREEITEM hChildItem = AddARCBaseTreeItem(strFlight,_item);
//	GetTreeCtrl().SetItemData(hChildItem,(DWORD_PTR)pNodeData);
//	OnInit3DModelNode(CString(pFlightData->getACType()),hChildItem);
//	OnInitOnBoardLayOutNode(hChildItem);
//	OnInitPlacementNode(hChildItem) ;
//}
//void MSV::COnBoardMSView::DeleteFlightTYNodeForlayout(CFlightTypeCandidate* _flightAnalysis )
//{
//   if(_flightAnalysis == NULL)
//	   return ;
//   HTREEITEM item = GetTreeCtrl().GetChildItem(m_hItemLayout) ;
//   COnBoardNodeData* pNodeData = NULL;
//   COnboardFlight* pFlightData = NULL ;
//   while(item)
//   {
//	   
//       pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(item) ;
//       if(_flightAnalysis->getID() == pNodeData->nOtherData)
//		   break ;
//	   item = GetTreeCtrl().GetNextSiblingItem(item) ;
//   }
//   if(item == NULL)
//	   return ;
//   HTREEITEM FlightTYchild = GetTreeCtrl().GetChildItem(item) ;
//   while(FlightTYchild != NULL)
//   {
//	   HTREEITEM pchild =  GetTreeCtrl().GetChildItem(FlightTYchild);
//	   while(pchild)
//	   {
//		   COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(pchild) ;
//		   if(data->nOtherData == TYPE_LAYOUT)
//		   {
//			   CAircarftLayoutManager::GetInstance()->DeleteLayout((CAircaftLayOut*)data->dwData) ;
//		   }
//		   pchild = GetTreeCtrl().GetNextSiblingItem(pchild) ;
//	   }
//       FlightTYchild = GetTreeCtrl().GetNextSiblingItem(FlightTYchild) ;
//   }
//   GetTreeCtrl().DeleteItem(item) ;
//}
//void MSV::COnBoardMSView::OnInitPlacementNode(HTREEITEM _item)
//{
//	COnBoardNodeData* playoutnode = new COnBoardNodeData(OnBoardNodeType_Normal);
//	playoutnode->nOtherData =TYPE_PLACEMENT ;
//	HTREEITEM hlayouDeckItem = AddARCBaseTreeItem("Placements",_item);
//	GetTreeCtrl().SetItemData(hlayouDeckItem,(DWORD_PTR)playoutnode) ;
//}
//CString MSV::COnBoardMSView::FormatAircraftModelNodeName(const CString& acty)
//{
//	CAirportDatabase* PDataBase = GetARCDocument()->GetTerminal().m_pAirportDB ;
//	CACType* PAcType = PDataBase->getAcMan()->getACTypeItem(acty) ;
//	CString filename ;
//	if(PAcType == NULL)
//		filename = "this AcType have not been defined";
//	else
//	{
//		if(PAcType->m_sFileName.IsEmpty())
//			filename = "No Aircraft Model file.DBClick To Assign..";
//		else
//			filename = PDataBase->getAcMan()->getDBPath() + "\\" + PAcType->m_sFileName ;
//	}
//	CString path ;
//	path.Format("Aircraft 3D Model(%s)",filename);
//	return path ;
//}
//void MSV::COnBoardMSView::OnInit3DModelNode(CString& acty,HTREEITEM _item)
//{
//
//    CString path  = FormatAircraftModelNodeName(acty);
//    HTREEITEM hChildItem = AddARCBaseTreeItem(path,_item);
//    COnBoardNodeData* playoutnode = new COnBoardNodeData(OnBoardNodeType_Normal);
//	playoutnode->nOtherData = TYPE_3DMODEL ;
//	playoutnode->strDesc = acty ;
//	GetTreeCtrl().SetItemData(hChildItem,(DWORD_PTR)playoutnode) ;
//
//}
//
//void MSV::COnBoardMSView::OnInitOnBoardLayOutNode(HTREEITEM hitem)
//{
//   COnBoardNodeData* pnodedata = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hitem);
//   CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pnodedata->nOtherData) ;
//   if(_layout == NULL)
//   {
//	   _layout = CAircarftLayoutManager::GetInstance()->AddNewLayoutWithFlightID(pnodedata->nOtherData,pnodedata->strDesc) ;
//       CAircarftLayoutManager::GetInstance()->WriteLayOutToDataSet(_layout) ;
//        CAircarftLayoutManager::GetInstance()->WriteMapping() ;
//   }
//  AddAircraftLayOutTreeNode(_layout,hitem) ;
//}
//
//void MSV::COnBoardMSView::AddAircraftLayOutTreeNode(CAircaftLayOut* _layout , HTREEITEM hitem)
//{
//	COnBoardNodeData* playoutnode = new COnBoardNodeData(OnBoardNodeType_Normal);
//	playoutnode->nIDResource = IDR_MENU_ONBOARD_LAYOUT;
//	playoutnode->dwData = (DWORD_PTR)_layout ;
//	playoutnode->nOtherData = TYPE_LAYOUT ;
//	HTREEITEM hlayouItem = AddARCBaseTreeItem(_layout->GetName(),hitem);
//	GetTreeCtrl().SetItemData(hlayouItem,(DWORD_PTR)playoutnode) ;
//	CDeckManager* deckmanager = _layout->GetDeckManager() ;
//	for (int i = 0 ; i < deckmanager->getCount() ; i++)
//	{
//		OnInitOnBoardLayoutDeckNode(deckmanager->getItem(i),hlayouItem) ;
//	}
//	GetTreeCtrl().Expand(hlayouItem,TVE_EXPAND) ;
//}
//void MSV::COnBoardMSView::OnInitOnBoardLayoutDeckNode(CDeck* _deck , HTREEITEM hitem)
//{
//	if(_deck == NULL)
//		return ;
//	COnBoardNodeData* playoutnode = new COnBoardNodeData(OnBoardNodeType_Normal);
//	playoutnode->nIDResource = IDR_MENU_ONBOARD_LAYOUT_DECK;
//	playoutnode->dwData = (DWORD_PTR)_deck ;
//	playoutnode->nOtherData =TYPE_DECK ;
//	HTREEITEM hlayouDeckItem = AddARCBaseTreeItem(_deck->GetName(),hitem);
//	GetTreeCtrl().SetItemData(hlayouDeckItem,(DWORD_PTR)playoutnode) ;
//	CSectionManager* sectionManager = _deck->GetSectionManager() ;
//	for (int i = 0 ; i < sectionManager->getCount() ; i++)
//	{
//      OnInitOnBoardLayOutSectionNode(sectionManager->getItem(i),hlayouDeckItem) ;
//	}
//	 GetTreeCtrl().Expand(hlayouDeckItem,TVE_EXPAND) ;
//}
//void MSV::COnBoardMSView::OnInitOnBoardLayOutSectionNode(CSection* _section , HTREEITEM hitem)
//{
//	if(_section == NULL)
//		return ;
//	COnBoardNodeData* playoutnode = new COnBoardNodeData(OnBoardNodeType_Normal);
//	playoutnode->nIDResource = IDR_MENU_ONBOARD_LAYOUT_SECTION;
//	playoutnode->dwData = (DWORD_PTR)_section ;
//	playoutnode->nOtherData = TYPE_SECTION ;
//	HTREEITEM hlayouSectionItem = AddARCBaseTreeItem(_section->GetName(),hitem);
//	GetTreeCtrl().SetItemData(hlayouSectionItem,(DWORD_PTR)playoutnode) ;
//     GetTreeCtrl().Expand(hlayouSectionItem,TVE_EXPAND) ;
//}
//////////////////////////////////////////////////////////////////////////
void MSV::COnBoardMSView::OnInitOnBoardAnalysis(HTREEITEM hItem)
{
	int nCount = m_pOnBoardList->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CFlightTypeCandidate* pFltData = m_pOnBoardList->getItem(i);
		COnBoardNodeData* pNodeData = NULL;
		pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
		pNodeData->nIDResource = IDR_MENU_ONBOARD_DELFLIGHTTYPE;
		//pNodeData->nOtherData = pFltData->getID();
		pNodeData->dwData = (DWORD_PTR)pFltData;
		char szBuff[1024] = {0};
		pFltData->GetFltType().screenPrint(szBuff);
		HTREEITEM hCurItem = AddARCBaseTreeItem(_T("Flight Type ")+CString(_T(szBuff)),hItem);
		GetTreeCtrl().SetItemData(hCurItem,(DWORD_PTR)pNodeData);
		for (int j = 0; j < pFltData->getCount(); j++)
		{
			CString strFlight = _T("");
			COnboardFlight* pFlightData = pFltData->getItem(j);
			pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
			pNodeData->nSubType = Normal_FlightData;
			pNodeData->nIDResource = IDR_MENU_ONBOARD_NEWFLIGHTID;
			pNodeData->nOtherData = pFlightData->getID();
			pNodeData->strDesc = pFlightData->getACType() ;
			pNodeData->dwData = (DWORD_PTR)pFlightData ;

			CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID());
			CString configname ;
			if(_layout != NULL)
				configname = _layout->GetName() ;
            strFlight = FormatFlightIdNodeForCandidate(configname,pFlightData) ;

			HTREEITEM hChildItem = AddARCBaseTreeItem(strFlight,hCurItem);
			GetTreeCtrl().SetItemData(hChildItem,(DWORD_PTR)pNodeData);

			HTREEITEM hAcCapacity = AddARCBaseTreeItem(_T("AC capacity"),hChildItem);
			COnBoardNodeData* pNodeDataACCap = new COnBoardNodeData(OnBoardNodeType_Normal);
			GetTreeCtrl().SetItemData(hAcCapacity,(DWORD_PTR)pNodeDataACCap);
			{
				CString strFlightSeats;
				if (_layout)
				{
					strFlightSeats.Format(_T("Passengers seat: %d"), _layout->GetPlacements()->GetSeatData()->GetItemCount());
				}
				else
				{
					strFlightSeats = _T("Passengers seat: (N/A)");
				}
				HTREEITEM hPassenger = AddARCBaseTreeItem(strFlightSeats,hAcCapacity);
				COnBoardNodeData* pNodeDataPassenger = new COnBoardNodeData(OnBoardNodeType_Normal);
				pNodeDataPassenger->nSubType = Normal_PassengerSeatNum;
				GetTreeCtrl().SetItemData(hPassenger,(DWORD_PTR)pNodeDataPassenger);

				AddARCBaseTreeItem(_T("Hold Cargo(TBD)"),hAcCapacity);

				GetTreeCtrl().Expand(hAcCapacity,TVE_EXPAND);
			}

			AddSeatClassesLoadFactors(hChildItem, pFlightData);


			for (int k = 0; k < pFlightData->getCount(); k++)
			{
				FlightCabinData* pCabinData = pFlightData->getItem(k);
				pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
				//pNodeData->nOtherData = pCabinData->getID();
				pNodeData->nIDResource = IDR_MENU_ONBOARD_CABIN;
				HTREEITEM hCabin = AddARCBaseTreeItem(pCabinData->m_strCabinName,hChildItem);
				GetTreeCtrl().SetItemData(hCabin,(DWORD_PTR)pNodeData);


				CString strCabinData = _T("");
				strCabinData.Format(_T("Number of seats: %d"),pCabinData->nSeats);
				HTREEITEM hNumItem = AddARCBaseTreeItem(strCabinData,hCabin,NET_EDIT_WITH_VALUE);
				GetTreeCtrl().SetItemData(hNumItem,(DWORD_PTR)(pCabinData->nSeats * 100));
				HTREEITEM hItem = AddARCBaseTreeItem(_T("Seat parameters"),hCabin);
				{
					strCabinData.Format(_T("Seat Width: %.2f"),pCabinData->fSeatWdth);
					HTREEITEM CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fSeatWdth * 100));

					strCabinData.Format(_T("Seat Depth: %.2f"),pCabinData->fSeatDepth);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fSeatDepth * 100));

					strCabinData.Format(_T("Seat Height: %.2f"),pCabinData->fSeatHeight);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fSeatHeight * 100));

					strCabinData.Format(_T("Armrest Height: %.2f"),pCabinData->fArmrestHeight);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fArmrestHeight * 100));

					strCabinData.Format(_T("Armrest Width Single: %.2f"),pCabinData->fArmrestWidthSingle);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->fArmrestWidthSingle * 100);

					strCabinData.Format(_T("Armrest Width Shared: %.2f"),pCabinData->fArmrestWidthShared);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fArmrestWidthShared * 100));

					strCabinData.Format(_T("Backrest Hight: %.2f"),pCabinData->fBackrestHight);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fBackrestHight * 100));

					strCabinData.Format(_T("Backrest thickness: %.2f"),pCabinData->fBackrestThickness);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fBackrestThickness * 100));

					strCabinData.Format(_T("Underseat space clear width: %.2f"),pCabinData->fUnderWidth);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fUnderWidth * 100));

					strCabinData.Format(_T("Underseat space clear depth: %.2f"),pCabinData->fUnderDepth);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fUnderDepth * 100));

					strCabinData.Format(_T("Underseat space clear height: %.2f"),pCabinData->fUnderHeight);
					CabinItem = AddARCBaseTreeItem(strCabinData,hItem,NET_EDIT_WITH_VALUE);
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)(pCabinData->fUnderHeight * 100));


					if (pCabinData->bCoatHook)
					{
						CabinItem = AddARCBaseTreeItem(_T("Coat hook on seat back: YES"),hItem,NET_COMBOBOX);
					}
					else
					{
						CabinItem = AddARCBaseTreeItem(_T("Coat hook on seat back: NO"),hItem,NET_COMBOBOX);
					}
					GetTreeCtrl().SetItemData(CabinItem,(DWORD_PTR)pCabinData->bCoatHook);
					GetTreeCtrl().Expand(hItem,TVE_EXPAND);
			   }
				GetTreeCtrl().Expand(hCabin,TVE_EXPAND);
		   }
		  GetTreeCtrl().Expand(hChildItem,TVE_EXPAND);
	   }
		GetTreeCtrl().Expand(hCurItem,TVE_EXPAND);
   }
   GetTreeCtrl().Expand(hItem,TVE_EXPAND);
}

HTREEITEM MSV::COnBoardMSView::getLevelItem(HTREEITEM hItem,int nLevel)
{
	while (hItem && nLevel)
	{
		hItem = GetTreeCtrl().GetParentItem(hItem);
		nLevel--;
	}
	return hItem;
}
LRESULT MSV::COnBoardMSView::OnEndLabelEdit(WPARAM wParam, LPARAM lParam)
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;

	if (pDispInfo->item.pszText == NULL)
	{
		return 0;
	}
	HTREEITEM hItem = (HTREEITEM)pDispInfo->item.hItem;

	CString str;
	GetTreeCtrl().GetEditControl()->GetWindowText(str);
	GetTreeCtrl().SetItemText(m_hRightClkItem,str) ;
	COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
	//if(data != NULL)
	//{
	//	/*switch(data->nOtherData)
	//	{
	//	case TYPE_LAYOUT:
	//		((CAircaftLayOut*)(data->dwData))->SetName(str) ;
	//		CAircarftLayoutManager::GetInstance()->WriteLayOutToDataSet(((CAircaftLayOut*)(data->dwData))) ;
	//		return 0;
	//	case TYPE_DECK:
	//		{
	//			CAircaftLayOut* _layout = GetLayOutFormTreeNode(GetTreeCtrl().GetParentItem(m_hRightClkItem)) ;
	//			if(_layout != NULL)
	//			{
	//				((CDeck*)(data->dwData))->SetName(str) ;
	//				_layout->GetDeckManager()->WriteDeckToDataSet(*((CDeck*)(data->dwData)),_layout->GetID()) ;
	//			}
	//		}
	//		return 0;
	//	case TYPE_SECTION:
	//		{
	//			CDeck* _deck = GetDeckFromTreeNode(GetTreeCtrl().GetParentItem(m_hRightClkItem)) ;
	//			if(_deck != NULL)
	//			{
	//				((CSection*)(data->dwData))->SetName(str) ;
	//				_deck->GetSectionManager()->WriteSectionToDataSet(*((CSection*)(data->dwData)),_deck->GetID()) ;
	//			}
	//		}
	//		return 0;*/
	//	default:
	//		break;
	//	}
	//}
	HTREEITEM hParentItem = GetTreeCtrl().GetParentItem(hItem);
	HTREEITEM hGrandItem = GetTreeCtrl().GetParentItem(hParentItem);
	COnBoardNodeData* pNodeData = NULL;
	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hGrandItem);
	if (pNodeData == NULL) return 0;
	CFlightTypeCandidate* pFltData =(CFlightTypeCandidate*)pNodeData->dwData /*m_pOnBoardList->findItem(pNodeData->nOtherData)*/;
	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParentItem);
	if (pNodeData == NULL) return 0;
	COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData/*pFltData->findItem(pNodeData->nOtherData)*/;
	pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hItem);
	if (pNodeData == NULL) return 0;
		
	FlightCabinData* pCabin = (FlightCabinData*)pNodeData->dwData/*pFlightData->findItem(pNodeData->nOtherData)*/;

	for (int i = 0; i < pFlightData->getCount(); i++)
	{
		FlightCabinData* pData = pFlightData->getItem(i);
		if (pData->m_strCabinName.CompareNoCase(pDispInfo->item.pszText) == 0 && pCabin->m_nID != pData->m_nID)
		{
			::AfxMessageBox(_T("please rename cabin class name"));
			GetTreeCtrl().SetItemText(hItem,pData->m_strCabinName);
			return 0;
		}
	}
	pCabin->m_strCabinName = pDispInfo->item.pszText;
	pCabin->UpdateData();
	return 0;


	// TODO: Add your control notification handler code here
	
}

LRESULT MSV::COnBoardMSView::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam)
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;
	COOLTREE_NODE_INFO* cni = GetTreeCtrl().GetItemNodeInfo(pDispInfo->item.hItem);
	if(cni->net != NET_NORMAL)
	{
		*pResult = TRUE;
		return FALSE;
	}
	COnBoardNodeData *pNodeData = reinterpret_cast<COnBoardNodeData *>(GetTreeCtrl().GetItemData(pDispInfo->item.hItem));
	if (pNodeData == NULL)
	{
		*pResult = TRUE;
		return FALSE;
	}
	if (pNodeData->nIDResource == IDR_MENU_ONBOARD_CABIN)
	{
		*pResult = FALSE;
		return FALSE;
	}
	if(pNodeData->nOtherData == TYPE_LAYOUT || pNodeData->nOtherData == TYPE_DECK || pNodeData->nOtherData == TYPE_SECTION)
	{
		*pResult = FALSE;
		return FALSE;
	}
	*pResult = TRUE;
	return FALSE;
}

void MSV::COnBoardMSView::OnUpdatePasteCabin(CCmdUI* pCmdUI)
{
	if (m_pOnBoardList->getCopyCabin())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
//the function fro rclick the layout->actype->flightId
//////////////////////////////////////////////////////////////////////////
//void MSV::COnBoardMSView::OnLoadAirCraftLayOut()
//{
//
//}
//void MSV::COnBoardMSView::OnImportAircraftLayOut()
//{
//	CFileDialog filedlg( TRUE,"zip", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
//		"ZIP File (*.zip)|*.zip;*.zip|All type (*.*)|*.*|", NULL );
//	if(filedlg.DoModal()!=IDOK)
//		return;
//	COnBoardNodeData* Rclickdata = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CString CAcType ;
//	if(Rclickdata != NULL)
//       CAcType = Rclickdata->strDesc ;
//	CString csFileName=filedlg.GetPathName();
//	CAircraftModelConfigLoadManager LoadManager(&GetARCDocument()->GetTerminal()) ;
//	CAircaftLayOut* p_layout = new CAircaftLayOut;
//	if(!LoadManager.ImportAircraftModelConfiguration(p_layout,csFileName,CAcType))
//	{
//		MessageBox(_T("Open the File Error !"),_T("Error"),MB_OK) ;
//		delete p_layout ;
//		return ;
//	}
//	if(m_hRightClkItem == NULL)
//	{
//		delete p_layout ;
//		return ;
//	}
//	HTREEITEM pchild = GetTreeCtrl().GetChildItem(m_hRightClkItem) ;
//	while(pchild != NULL)
//	{
//		COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(pchild) ;
//	    if(data->nOtherData == TYPE_LAYOUT)
//		{
//           CAircarftLayoutManager::GetInstance()->DeleteLayout((CAircaftLayOut*)data->dwData) ;
//		   GetTreeCtrl().DeleteItem(pchild) ;
//		}
//		if(data->nOtherData == TYPE_3DMODEL)
//		{
//		    CACType* PAcType =  GetARCDocument()->GetTerminal().m_pAirportDB->getAcMan()->getACTypeItem(CAcType) ;
//          if(PAcType != NULL)
//			  GetTreeCtrl().SetItemText(pchild,FormatAircraftModelNodeName(CAcType)) ;
//		}
//       pchild = GetTreeCtrl().GetNextSiblingItem(pchild) ;
//	}
//	CAircarftLayoutManager::GetInstance()->addItem(p_layout) ;
//	CAircarftLayoutManager::GetInstance()->LinkTheLayOutWithFlightID(p_layout,Rclickdata->nOtherData);
//    CAircarftLayoutManager::GetInstance()->WriteLayOutToDataSet(p_layout) ;
//	CAircarftLayoutManager::GetInstance()->WriteMapping() ;
//	AddAircraftLayOutTreeNode(p_layout,m_hRightClkItem) ;
//}
//void MSV::COnBoardMSView::OnExportAircraftLayOut()
//{
//	CFileDialog filedlg( FALSE,"zip", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
//		"ZIP File (*.zip)|*.zip;*.zip|All type (*.*)|*.*|", NULL );
//	filedlg.m_ofn.lpstrTitle = "Export AirCraft Configuration" ;
//	if(filedlg.DoModal()!=IDOK)
//		return;
//	CString csFileName=filedlg.GetPathName();
//	HTREEITEM pchild = GetTreeCtrl().GetChildItem(m_hRightClkItem) ;
//	 COnBoardNodeData* data = NULL ;
//	 while(pchild != NULL)
//	 {
//		 COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(pchild) ;
//		 if(data->nOtherData == TYPE_LAYOUT)
//			 break ;
//		 pchild = GetTreeCtrl().GetNextSiblingItem(pchild) ;
//	 }
//	if(pchild != NULL)
//	{
//        data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(pchild) ;
//	}else
//	{
//		MessageBox(_T("Have no Aircraft LayOut For This Filght!"),"Error",MB_OK) ;
//		return ;
//	}
//	CAircraftModelConfigLoadManager xmlGenerator(&GetARCDocument()->GetTerminal()) ;
//	if(!xmlGenerator.ExportAircraftModelConfiguration((CAircaftLayOut*)(data->dwData),csFileName))
//		MessageBox(_T("Export Configuration Error!"),"Error",MB_OK) ;
//	else
//		MessageBox(_T("Export Configuration Successfully !"),"Success",MB_OK) ;
//}
////////////////////////////////////////////////////////////////////////////
//
//void MSV::COnBoardMSView::OnLayOutReName()
//{
//   GetTreeCtrl().EditLabel(m_hRightClkItem) ;
//}
//void MSV::COnBoardMSView::OnLayoutAddDeck()
//{
//	COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CAircaftLayOut* layout = (CAircaftLayOut*)(data->dwData );
//	CString defineDeckName ;
//	defineDeckName.Format("Deck%d",layout->GetDeckManager()->getCount() + 1);
//	CDeck* p_deck= new CDeck ;
//	p_deck->SetName(defineDeckName) ;
//	layout->GetDeckManager()->addItem(p_deck) ;
//	layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,layout->GetID());
//	OnInitOnBoardLayoutDeckNode(p_deck,m_hRightClkItem) ;
//	GetTreeCtrl().Expand(m_hRightClkItem,TVE_EXPAND) ;
//}
//void MSV::COnBoardMSView::OnLayOutDelDeck()
//{
//	HTREEITEM selitem = GetTreeCtrl().GetSelectedItem() ;
//	if(selitem == NULL)
//		return ;
//    COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(selitem) ;
//	if(data->nOtherData != TYPE_DECK)
//		return ;
//    COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CAircaftLayOut* layout = (CAircaftLayOut*)(lay_data->dwData );
//	layout->GetDeckManager()->RemoveDeck((CDeck*)(data->dwData)) ;
//	layout->GetDeckManager()->DeleteDeck((CDeck*)(data->dwData)) ;
//	GetTreeCtrl().DeleteItem(selitem) ;
//	GetTreeCtrl().Expand(m_hRightClkItem,TVE_EXPAND) ;
//}
//void MSV::COnBoardMSView::OnDeckNewSection()
//{
//    COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* deck = (CDeck*)(data->dwData) ;
//	CString defaultName ;
//	defaultName.Format("X-Section%d",deck->GetSectionManager()->getCount() + 1) ;
//	CSection* p_section = new CSection ;
//	p_section->SetName(defaultName) ;
//	deck->GetSectionManager()->addItem(p_section) ;
//	deck->GetSectionManager()->WriteSectionToDataSet(*p_section,deck->GetID()) ;
//	OnInitOnBoardLayOutSectionNode(p_section,m_hRightClkItem) ;
//	GetTreeCtrl().Expand(m_hRightClkItem,TVE_EXPAND) ;
//}
//void MSV::COnBoardMSView::OnDeckDelSection()
//{
//	HTREEITEM selitem = GetTreeCtrl().GetSelectedItem() ;
//	if(selitem == NULL)
//		return ;
//	COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(selitem) ;
//	if(data->nOtherData != TYPE_SECTION)
//		return ;
//	COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* deck = (CDeck*)(lay_data->dwData );
//	deck->GetSectionManager()->RemoveSection((CSection*)(data->dwData)) ;
//	deck->GetSectionManager()->DelSectionFromDB((CSection*)(data->dwData));
//	GetTreeCtrl().DeleteItem(selitem) ;
//	GetTreeCtrl().Expand(m_hRightClkItem,TVE_EXPAND) ;
//}
//void MSV::COnBoardMSView::OnDeckReName()
//{
//      GetTreeCtrl().EditLabel(m_hRightClkItem) ;
//}
//void MSV::COnBoardMSView::OnDeckActive()
//{
//	 HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	 if(_parents == NULL )
//		 return ;
//      COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(_parents) ;
//	  CAircaftLayOut* p_layout= (CAircaftLayOut*)(lay_data->dwData) ;
//      lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	  CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	  p_layout->GetDeckManager()->ActiveDeck(p_deck) ;
//	  p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckProperty()
//{
//
//}
//CAircaftLayOut* MSV::COnBoardMSView::GetLayOutFormTreeNode(HTREEITEM _item)
//{
//	COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(_item) ;
//	CAircaftLayOut* p_layout = NULL ;
//	if(lay_data->nOtherData == TYPE_LAYOUT)
//	  p_layout = (CAircaftLayOut*)(lay_data->dwData) ;
//	return p_layout ;
//}
//void MSV::COnBoardMSView::OnDeckIsolater()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//	 COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	if(p_deck->IsIsolate())
//	    p_deck->IsIsolate(FALSE) ;
//	else
//		p_deck->IsIsolate(TRUE) ;
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckHide()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	if(p_deck->IsShow())
//		p_deck->IsShow(FALSE);
//	else
//		p_deck->IsShow(TRUE) ;
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckGirdOn()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	if(p_deck->IsGrid())
//		p_deck->IsGrid(FALSE) ;
//	else
//		p_deck->IsGrid(TRUE) ;
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//
//void MSV::COnBoardMSView::OnDeckThicknesson()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	if(p_deck->GetComposition()->IsStickness())
//		p_deck->GetComposition()->IsStickness(FALSE);
//	else
//		p_deck->GetComposition()->IsStickness(TRUE) ;
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckVisibleRegion()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//		p_deck->GetComposition()->IsVisableRegion(TRUE) ;
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckInVisibleRegion()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	p_deck->GetComposition()->IsVisableRegion(FALSE);
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckColorForMonochrome()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	p_deck->SetColorMode(MONOCHROME) ;
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckColor_Vivid()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	p_deck->SetColorMode(VIVID) ;
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckColor_Desaturated()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CAircaftLayOut* p_layout= GetLayOutFormTreeNode(_parents) ;
//
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	p_deck->SetColorMode(DESATURATED) ;
//
//	p_layout->GetDeckManager()->WriteDeckToDataSet(*p_deck,p_layout->GetID()) ;
//}
//void MSV::COnBoardMSView::OnDeckDelete()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(_parents) ;
//	CAircaftLayOut* p_layout= (CAircaftLayOut*)(lay_data->dwData) ;
//	lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//	p_layout->GetDeckManager()->RemoveDeck(p_deck) ;
//	p_layout->GetDeckManager()->DeleteDeck(p_deck) ;
//	GetTreeCtrl().DeleteItem(m_hRightClkItem) ;
//}
//void MSV::COnBoardMSView::OnDeckComment()
//{
//	COnBoardNodeData*  lay_data = (COnBoardNodeData*)(COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CDeck* p_deck =  (CDeck*)(lay_data->dwData) ;
//}
//void MSV::COnBoardMSView::OnDeckHelp()
//{
//
//}
//void MSV::COnBoardMSView::OnSectionRename()
//{ 
//	GetTreeCtrl().EditLabel(m_hRightClkItem) ;
//
//}
//void MSV::COnBoardMSView::OnSectionPosition()
//{
//   
//}
//void MSV::COnBoardMSView::OnSectionDefine()
//{
//
//}
//CDeck* MSV::COnBoardMSView::GetDeckFromTreeNode(HTREEITEM _item)
//{
//	COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(_item) ;
//	CDeck* p_deck = NULL ;
//	if(lay_data->nOtherData == TYPE_DECK)
//		p_deck = (CDeck*)(lay_data->dwData) ;
//	return p_deck ;
//}
//void MSV::COnBoardMSView::OnSectionDelete()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(_parents) ;
//	CDeck* p_deck= (CDeck*)(lay_data->dwData) ;
//	lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CSection* p_section =  (CSection*)(lay_data->dwData) ;
//	if(p_section == NULL)
//		return ;
//	p_deck->GetSectionManager()->RemoveSection(p_section) ;
//	p_deck->GetSectionManager()->DelSectionFromDB(p_section) ;
//	GetTreeCtrl().DeleteItem(m_hRightClkItem) ;
//}
//void MSV::COnBoardMSView::OnSectionGrid()
//{
//	HTREEITEM _parents = GetTreeCtrl().GetParentItem(m_hRightClkItem) ;
//	if(_parents == NULL )
//		return ;
//	CDeck* p_deck = GetDeckFromTreeNode(_parents) ;
//	if(p_deck == NULL)
//		return ;
//	COnBoardNodeData* lay_data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
//	CSection* p_section= (CSection*)(lay_data->dwData) ;
//     if(p_section->IsGrid())
//		 p_section->IsGrid(FALSE) ;
//	 else
//		 p_section->IsGrid(TRUE) ;
//	 p_deck->GetSectionManager()->WriteSectionToDataSet(*p_section,p_deck->GetID()) ;
//}
void MSV::COnBoardMSView::OnEndlabelTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString str;
	GetTreeCtrl().GetEditControl()->GetWindowText(str);
	GetTreeCtrl().SetItemText(m_hRightClkItem,str) ;
	COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
	/*switch(data->nOtherData)
	{
	case TYPE_LAYOUT:
			((CAircaftLayOut*)(data->dwData))->SetName(str) ;
			CAircarftLayoutManager::GetInstance()->WriteLayOutToDataSet(((CAircaftLayOut*)(data->dwData))) ;
		break;
	case TYPE_DECK:
		{
			CAircaftLayOut* _layout = GetLayOutFormTreeNode(GetTreeCtrl().GetParentItem(m_hRightClkItem)) ;
			if(_layout != NULL)
			{
            ((CDeck*)(data->dwData))->SetName(str) ;
			_layout->GetDeckManager()->WriteDeckToDataSet(*((CDeck*)(data->dwData)),_layout->GetID()) ;
			}
		}
		break;
	case TYPE_SECTION:
		{
			CDeck* _deck = GetDeckFromTreeNode(GetTreeCtrl().GetParentItem(m_hRightClkItem)) ;
			if(_deck != NULL)
			{
		    ((CSection*)(data->dwData))->SetName(str) ;
			_deck->GetSectionManager()->WriteSectionToDataSet(*((CSection*)(data->dwData)),_deck->GetID()) ;
			}
		}
	    break;
	default:
	    break;  
	}*/
}

CString MSV::COnBoardMSView::FormatFlightIdNodeForCandidate(const CString& _configName,COnboardFlight* pFlight)
{
	CString strFlight ;
	if (pFlight->getFlightType() == ArrFlight)
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Arr Day%d"),pFlight->getFlightID(),pFlight->getACType(), pFlight->getDay());
	}
	else
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Dep Day%d"),pFlight->getFlightID(),pFlight->getACType(), pFlight->getDay());
	}    
	if(!_configName.IsEmpty())
	{
		CString config;
		config.Format("(%s)",_configName) ;
        strFlight.Append(config) ;
	}
    return strFlight ;
}

void MSV::COnBoardMSView::OnSelectConfigureInteror()
{
	//////////////////////////////////////////////////////////////////////////
	COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
	if(data == NULL)
		return ;
	COnboardFlight* flight = (COnboardFlight*)data->dwData ;


	CDlgAircraftConfiguration dlgairconfig(data->strDesc,flight->getID(),&(GetARCDocument()->GetTerminal()),this) ;
	CAircaftLayOut* _layout = NULL ;
	if(dlgairconfig.DoModal() == IDOK)
	{
		_layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(flight->getID()) ;
		CString configname ;
		if(_layout != NULL)
		{
			// update seat number after selected interior
			HTREEITEM hSeatNum = FindTreeChildItem(m_hRightClkItem,
				COnBoardNodeData::TypeSubTypeMatcher(OnBoardNodeType_Normal, Normal_PassengerSeatNum), true);
			if (hSeatNum)
			{
				CString strSeatNum;
				strSeatNum.Format(_T("Passengers seat: %d"), _layout->GetPlacements()->GetSeatData()->GetItemCount());
				GetTreeCtrl().SetItemText(hSeatNum, strSeatNum);
			}
			else
			{
				ASSERT(FALSE);
			}

			configname = _layout->GetName() ;
			flight->RebuildLoadFactors(_layout->GetID());
			AddSeatClassesLoadFactors(m_hRightClkItem, flight);
		}

		CString nodename = FormatFlightIdNodeForCandidate(configname,flight) ;
		GetTreeCtrl().SetItemText(m_hRightClkItem,nodename) ;
	}
}
void MSV::COnBoardMSView::OnConfigureInteror()
{
	//////////////////////////////////////////////////////////////////////////
	COnBoardNodeData* data = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
	if(data == NULL)
		return ;
	COnboardFlight* flight = (COnboardFlight*)data->dwData ;

	CTermPlanDoc* pTermPlanDoc = GetARCDocument();
	CDlgAircraftConfiguration dlgairconfig(data->strDesc,flight->getID(),&pTermPlanDoc->GetTerminal(),this) ;
	CAircaftLayOut* _layout = NULL ;
	_layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(flight->getID()) ;
    if(_layout == NULL)
	{
		MessageBox(_T("You have't select a configuration for this flight. Please select one first!"),"Warning",MB_OK) ;
		return ;
	}
	if(pTermPlanDoc)
	{
		CWaitCursor wc;
		pTermPlanDoc->GetAircraftLayoutEditor()->SetEditLayOut(_layout) ;

		CMainFrame* pMainFrame = pTermPlanDoc->GetMainFrame();
		CAircraftLayOutFrame* layoutFrame = (CAircraftLayOutFrame*)pMainFrame->CreateOrActivateFrame(  theApp.m_pAircraftLayoutTermplate, RUNTIME_CLASS(CAircraftLayout3DView) );
		if (layoutFrame)
		{
			/*	CLayoutView* view = pActiveDoc->GetLayoutMSVView() ;;*/
			layoutFrame->OnUpdateFrameTitle(TRUE) ;
			layoutFrame->ShowWindow(SW_SHOWMAXIMIZED);
			pMainFrame->ShowControlBar(&pMainFrame->m_wndACFurnishingShapesBar, TRUE, FALSE);
	
			//layoutFrame->CreateLayoutTool() ;
			//view->Invalidate(FALSE);
// 			pTermPlanDoc->UpdateAllViews((CView*)this, VM_ONBOARD_LAYOUT_CHANGE, (CObject*)_layout);
		}
	}
}
void MSV::COnBoardMSView::OnRemoveFlight()
{
	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	if (pNodeData)
	{
		COnboardFlight* pFlight = (COnboardFlight*)pNodeData->dwData;
		HTREEITEM hParenItem = GetTreeCtrl().GetParentItem(m_hRightClkItem);
		pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hParenItem);
		
		if (pNodeData)
		{
			if(MessageBox(_T("It will delete all data relate with this flight, are you sure?"),NULL,MB_OKCANCEL) != IDOK )
			{
				return;
			}
			try
			{
				CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData;
				pFltData->deleteItem(pFlight);
				//pFltData->SaveData(m_nProjID);
				CAircarftLayoutManager::GetInstance()->RemoveMappingByFlightID(pFlight->getID());
				GetTreeCtrl().DeleteItem(m_hRightClkItem);
				if (pFltData->getCount() == 0)
				{
					m_pOnBoardList->removeItem(pFltData);
					m_pOnBoardList->SaveData(m_nProjID);
					GetTreeCtrl().DeleteItem(hParenItem);
				}
			}	
			catch (CADOException& e)
			{
				AfxMessageBox(e.ErrorMessage());
			}
			
			
		} 
	}
}

//void MSV::COnBoardMSView::OnPaint() 
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: Add your message handler code here
//	// Create a memory DC compatible with the paint DC
//	CDC memDC;
//	memDC.CreateCompatibleDC( &dc );
//
//	CRect rcClip, rcClient;
//	dc.GetClipBox( &rcClip );
//	GetClientRect(&rcClient);
//
//	// Select a compatible bitmap into the memory DC
//	CBitmap bitmap;
//	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
//	memDC.SelectObject( &bitmap );
//
//	// Set clip region to be same as that in paint DC
//	CRgn rgn;
//	rgn.CreateRectRgnIndirect( &rcClip );
//	memDC.SelectClipRgn(&rgn);
//	rgn.DeleteObject();
//
//	// First let the control do its default drawing.
//	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0 );
//
//	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
//		rcClip.left, rcClip.top, SRCCOPY );
//}

BOOL MSV::COnBoardMSView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void MSV::COnBoardMSView::AddSeatClassesLoadFactors( HTREEITEM hFlight, COnboardFlight* pFlightData )
{
	// 1. search for seat classes item
	HTREEITEM hSeatClasses = FindTreeChildItem(hFlight, COnBoardNodeData::TypeSubTypeMatcher(OnBoardNodeType_Normal, Normal_SeatClasses));
	// 2. if found, delete all child items, else, add the seat classes item
	if (hSeatClasses)
	{
		DeleteTreeAllChildItems(hSeatClasses);
	} 
	else
	{
		hSeatClasses = AddItem(_T("Seating Classes"),hFlight);
		COnBoardNodeData* pNodeData = new COnBoardNodeData(OnBoardNodeType_Normal);
		pNodeData->nSubType = Normal_SeatClasses;
		pNodeData->nIDResource = IDR_MENU_SEAT_CLASSES;
		pNodeData->bHasExpand = true;
		pNodeData->dwData = (DWORD)pFlightData;
		GetTreeCtrl().SetItemData(hSeatClasses,(DWORD_PTR)pNodeData);
	}
	// 3. add sub items
	for(int i=0;i<pFlightData->GetLoadFactorCount();i++)
	{
		FlightAnalysisLoadFactor* pLoadFactor = pFlightData->GetLoadFactor(i);
		CString strLoadFactor;
		strLoadFactor.Format(_T("%s class (%s)"), pLoadFactor->GetSeatClassName(),
			pLoadFactor->GetProbDist()->printDistribution());
		HTREEITEM hLoadFactor = AddItem(strLoadFactor,hSeatClasses);
		COnBoardNodeData* pNodeData = new COnBoardNodeData(OnBoardNodeType_Dlg);
		pNodeData->nSubType = Dlg_OnboardSeatClassLoadFactor;
		pNodeData->bHasExpand = false;
		pNodeData->dwData = (DWORD)pLoadFactor;
		GetTreeCtrl().SetItemData(hLoadFactor,(DWORD_PTR)pNodeData);
	}
	GetTreeCtrl().Expand(hSeatClasses, TVE_EXPAND);
}

void MSV::COnBoardMSView::OnUpdateSeatClasses()
{
	//////////////////////////////////////////////////////////////////////////
	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem) ;
	if (pNodeData && pNodeData->nodeType==OnBoardNodeType_Normal && pNodeData->nSubType==Normal_SeatClasses)
	{
		COnboardFlight* flight = (COnboardFlight*)pNodeData->dwData ;
		CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(flight->getID());
		if(_layout)
		{
			flight->RebuildLoadFactors(_layout->GetID());
			AddSeatClassesLoadFactors(GetTreeCtrl().GetParentItem(m_hRightClkItem), flight);
		}
		else
		{
			MessageBox(_T("You have't select a configuration for this flight. Please select one first!"),"Warning",MB_OK) ;
		}
	}
}

bool MSV::COnBoardMSView::RearrangeFlightToNewCandidate( COnboardFlight* pFlight, CFlightTypeCandidate* pNewCandidate )
{
	CFlightTypeCandidate* pFltTypeNow = m_pOnBoardList->GetCandidateFlightBelong(pFlight);
		
	int iNow = -1, iNew = -1;
	int nCount = m_pOnBoardList->getCount();
	for (int i =0; i< nCount; i++)
	{
		if (m_pOnBoardList->getItem(i) == pFltTypeNow)
			iNow = i;

		if (m_pOnBoardList->getItem(i) == pNewCandidate)
			iNew = i;

		if (iNow > -1 && iNew > -1)
			break;
	}

	if (iNow < iNew)		//the new flight type more detail than exist flight type
		return false;

	return true;
}

void MSV::COnBoardMSView::RemoveOnboardFlight( COnboardFlight* pFlight,HTREEITEM hCandidatesItem )
{
	if (hCandidatesItem == NULL)
		return;

	CFlightTypeCandidate* pFltTypeNow = m_pOnBoardList->GetCandidateFlightBelong(pFlight);

	HTREEITEM hNextItem;
	HTREEITEM hFltTypeCandidate = GetTreeCtrl().GetChildItem(hCandidatesItem);
	while (hFltTypeCandidate != NULL)
	{
		COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hFltTypeCandidate);
		if (pFltTypeNow == (CFlightTypeCandidate*)pNodeData->dwData)
			break;

		hNextItem = GetTreeCtrl().GetNextItem(hFltTypeCandidate, TVGN_NEXT);
		hFltTypeCandidate = hNextItem;
	}

	HTREEITEM hFlightItem = GetTreeCtrl().GetChildItem(hFltTypeCandidate);
	while(hFlightItem != NULL)
	{
		COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hFlightItem);
		if (pFlight == (COnboardFlight*)pNodeData->dwData)
		{
			pFltTypeNow->removeItem(pFlight);
			GetTreeCtrl().DeleteItem(hFlightItem);
			pFltTypeNow->SaveData(m_nProjID);
			return;
		}

		hNextItem = GetTreeCtrl().GetNextItem(hFlightItem, TVGN_NEXT);
		hFlightItem = hNextItem;	
	}
}

void MSV::COnBoardMSView::OnAddFlightToFltTypeCandidate()
{
	COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	CFlightTypeCandidate* pFltData = (CFlightTypeCandidate*)pNodeData->dwData;

	DlgOnboardFlightSelection dlg(&GetARCDocument()->GetTerminal(),pFltData);

	if (dlg.DoModal() != IDOK)
		return;

	Flight* pFlight = dlg.m_pSelFlight;
	CString strFlight = dlg.m_strFlight;

	char szID[1024] = {0};
	char szACType[1024] = {0};


	if (strFlight.Find("Arr") >=0)
	{
		pFlight->getFullID(szID,'A');
		pFlight->getACType(szACType);
		int nDay = pFlight->getArrTime().GetDay();

		COnboardFlight* pExistFlight = m_pOnBoardList->GetExistOnboardFlight(szID,szACType,ArrFlight,nDay);
		if (pExistFlight != NULL)
		{
			AfxMessageBox(_T("The onboard flight already exist!"));
			return;
		}

		COnboardFlight* pFlightData = new COnboardFlight();
		pFlightData->setFlightType(ArrFlight);
		pFlightData->setACType(szACType);
		pFlightData->setFlightID(szID);
		pFlightData->setDay(nDay);
		pFltData->addItem(pFlightData);

		insertFlightOnTree(m_hRightClkItem,pFltData,pFlightData,strFlight);
	}
	else
	{
		pFlight->getFullID(szID,'D');
		pFlight->getACType(szACType);
		int nDay = pFlight->getDepTime().GetDay();

		COnboardFlight* pExistFlight = m_pOnBoardList->GetExistOnboardFlight(szID,szACType,DepFlight,nDay);
		if (pExistFlight != NULL)
		{
			AfxMessageBox(_T("The onboard flight already exist!"));
			return;
		}

		COnboardFlight* pFlightData = new COnboardFlight();
		pFlightData->setFlightType(DepFlight);

		pFlightData->setACType(szACType);
		pFlightData->setFlightID(szID);
		pFlightData->setDay(nDay);
		pFltData->addItem(pFlightData);

		insertFlightOnTree(m_hRightClkItem,pFltData,pFlightData,strFlight);
	}
	pFltData->SaveData(m_nProjID);
}

void MSV::COnBoardMSView::UpdateFlightSeatNum(CAircaftLayOut* _layout)
{
	HTREEITEM_VECTOR flightDataItems;
	FindTreeChildItems(m_hRefTreeItems[Item_OnboardAnalysisCandidates],
		COnBoardNodeData::TypeSubTypeMatcher(OnBoardNodeType_Normal, Normal_FlightData), flightDataItems, true);
	for (HTREEITEM_VECTOR::const_iterator ite = flightDataItems.begin();ite!=flightDataItems.end();ite++)
	{
		HTREEITEM hFlightData = *ite;
		COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hFlightData);
		COnboardFlight* pFlightData = (COnboardFlight*)pNodeData->dwData ;

		CAircaftLayOut* _layout2 = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID());
		if (_layout2 == _layout && !_layout)
			continue;

		HTREEITEM hSeatNum = FindTreeChildItem(hFlightData,
			COnBoardNodeData::TypeSubTypeMatcher(OnBoardNodeType_Normal, Normal_PassengerSeatNum), true);
		if (hSeatNum)
		{
			CString strSeatNum;
			if (_layout2)
			{
				strSeatNum.Format(_T("Passengers seat: %d"), _layout2->GetPlacements()->GetSeatData()->GetItemCount());
			}
			else
			{
				strSeatNum = _T("Passengers seat: (N/A)");
			}

			GetTreeCtrl().SetItemText(hSeatNum, strSeatNum);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}
