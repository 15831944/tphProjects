#include "StdAfx.h"
#include "resource.h"
#include "nodeviewdbclickhandler.h"
//#include "DlgAirsideSimSetting.h"
//#include "RunAssignPropSheet.h"
//#include "DlgAirRoutes.h"
//#include "DlgAircraftRelatedCategories.h"
#include "../MFCExControl/TreePropSheet/TreePropSheet.h"
//#include "SimSettingsGeneral.h"
#include "..\Database\ARCportDatabaseConnection.h"
#include "../AirsideGUI/DlgControllerIntervention.h"
#include <CommonData/BaseProcess.h>
//#include "DlgIntersectionBlocking.h"
//#include "DlgPushBackBlocking.h"
#include "../Engine/RunwayProc.h"
//#include "DlgExclusiveUseCriteria.h"
//#include "ATCPropSheet.h"
//#include "RunwayExitPerformanceSheet.h"
#include "DlgTaxiInbound.h"
#include "DlgTaxiOutbound.h"
#include "DlgPushBack.h"
#include "DlgEngineParameters.h"
#include "DlgPerformanceTakeoff.h"
#include "DlgDepClimbOut.h"
#include "DlgFlightPerformanceCruise.h"
#include "DlgFlightPerformanceTerminal.h"
#include "DlgPerformLanding.h"
#include "DlgApproachToLand.h"
//#include "DlgTowingOperationParam.h"
//#include "DlgPosAssignCriteria.h"
//#include "TaxiwayUsageCriteriaSheet.h"
//#include "DlgExitUsageCriteria.h"
//#include "DlgContingency.h"
//#include "DlgFlightPlan.h"
//#include "DlgDepInTrail.h"
#include "DlgArrivalInit.h"
#include "DlgAirRoute.h"
#include "DlgSideStepSpecification.h"

#include "DlgAircraftClassifications.h"
#include "DlgTakeoffRunwayAssign.h"
#include "DlgLandingRunwayAssign.h"
#include "DlgLandingRunwayExit.h"
#include "DlgRunwayTakeoffPosition.h"
#include "DlgAircraftClassifications.h"
#include "DlgStandBuffer.h"
#include "DlgOccupiedAssignedStand.h"
#include "DlgTakeoffQueues.h"
#include "DlgTakeoffSequence.h"
//#include "DlgInTrailSeparation.h"
#include "DlgInTrailSeperationEx.h"

//#include "DlgApprochSeparationCriteria.h"
#include "DlgItinerantTraffic.h"
#include "DlgTaxiwayConstraint.h"
#include "DlgTemporaryParkingCriteria.h"
#include "DlgDirectRoutingCriteria.h"
#include "DlgConflictResolution.h"

#include "WeatherImpactsDlg.h"
#include "StarAssignmentDlg.h"
#include "DlgSIDAssignment.h"

#include "DlgTBTSameRunway.h"
#include "DlgTBTIntersect.h"
#include "DlgTBLSameRunway.h"
#include "DlgTBLIntersect.h"
#include "../InputAirside/TakeoffClearances.h"
#include "../InputAirside/PadAssignment.h"
#include "../InputAirside/AircraftClassifications.h"
#include "CtrlInterventionSpecDlg.h"

#include "OutboundRouteAssignmentDlg.h"
#include "DlgBoundRouteAssignment2.h"

#include "DlgGroundRoutes.h"
#include "DlgVehicleSpecification.h"
#include "DlgGoAroundCriteriaDataProcess.h"
#include "DlgPushBackClearanceCriteria.h"
#include "ServiceRequirementdlg.h"
#include "DlgVehiclePoolsAndDeployment.h"
#include "DlgFlightTypeDimensions.h"
#include "DlgFlightRelativeServiceLocations.h"
#include "DlgVehicleRoute.h"
#include "DlgTaxiwayFlightTypeRestriction.h"
//#include "DlgDepartureSlotSpec.h"
#include "DlgDepartureSlotSpec.h"
#include "DialogWPImport.h"
#include "DlgDirectionalityConstraints.h"
#include "DlgTaxiSpeedConstraints.h"
#include "TowOffStandAssignmentSpecificationDlg.h"
#include "DlgTowingRoute.h"
#include "DlgStretchOption.h"
#include "ArrivalETAOffsetDlg.h"
#include "Approach.Takeoff.Criteria/DlgApproachSeparationCriteria.h"
#include "DlgTakeoffClearance.h"
#include "DlgPadAissgnment.h"
#include "DlgDeicePadRoute.h"
#include "DlgStandService.h"
#include "DlgDeiceQueue.h"
#include "DlgSectorFlightMixSpecification.h"
#include "DlgVehicularMovementAllowed.h"
#include "../Database/DBElementCollection_Impl.h"
#include "DlgStandCriteriaAssignment.h"
#include "DlgTaxiInterruptTimes.h"
#include "DlgDeicingAndAntiIcingFluidSpc.h"
#include "DlgDe_icingAndAnti_icingStrategies.h"
#include "DlgTowOperationPerformance.h"
#include "DlgPushBackAndTowOperationSpec.h"
#include "DlgWeatherScript.h"
#include "DlgAircraftSurfaceInitialCondition.h"
#include "./DeiceRouteAssigmentDlg.h"
#include "DlgIgnitionSpecification.h"
#include "DlgTowOperationPerformance.h"
#include "PostDeicingRouteAssignmentDlg.h"
#include "DlgIntersectionNodeBlock.h"
#include "DlgTakeOffPositionSelection.h"
#include "DlgTrainFlightSpec.h"
#include "DlgTrainFlightsManagement.h"

using namespace AirsideGUI;
using namespace TreePropSheet;

NodeViewDbClickHandler::NodeViewDbClickHandler(BaseProcessorList* _pProcessorList, InputAirside* _pInputAirside)
 :m_pProcessorList(_pProcessorList)
 ,m_pSelectFlightType(NULL)
 ,m_pGroundRoutesDlg(NULL)
 ,m_pTakeoffQueuesDlg(NULL)
 ,m_pTakeoffSequenceDlg(0)
 ,m_pInputAirside(_pInputAirside)
 ,m_pInboundRouteAssignmentDlg(NULL)
 ,m_pOutboundRouteAssignmentDlg(NULL)
 ,m_pDeiceRouteAssignmentDlg(NULL)
 ,m_pPostDeiceRouteDlg(NULL)
 ,m_pTowingRouteDlg(0)
 ,m_pDirectionalityConstraintDlg(0)
 ,m_pTaxiwayFlightTypeRestrictionDlg(0)
 ,m_pTaxiwayConstraintDlg(0)
 ,m_pTemporaryParkingCriteriaDlg(0)
 ,m_pFuncSelectFlightType(NULL)
 ,m_pTaxiSpeedConstraintsDlg(NULL)
{
//	m_pAirsideInput->ReadAllData();
	m_vArrivalInitPara.clear();
}
NodeViewDbClickHandler::~NodeViewDbClickHandler()
{
	if (NULL != m_pGroundRoutesDlg)
	{
		delete m_pGroundRoutesDlg;
	}

	if (NULL != m_pInboundRouteAssignmentDlg)
	{
		delete m_pInboundRouteAssignmentDlg;
	}

	if (NULL != m_pOutboundRouteAssignmentDlg)
	{
		delete m_pOutboundRouteAssignmentDlg;
	}
	if (NULL != m_pTakeoffQueuesDlg)
	{
		delete m_pTakeoffQueuesDlg;
	}
	if (m_pTakeoffSequenceDlg) {
		delete m_pTakeoffSequenceDlg;
		m_pTakeoffSequenceDlg = 0;
	}
	if (m_pTowingRouteDlg) {
		delete m_pTowingRouteDlg;
		m_pTowingRouteDlg = 0;
	}
	if (m_pDirectionalityConstraintDlg) {
		delete m_pDirectionalityConstraintDlg;
		m_pDirectionalityConstraintDlg = 0;
	}
	if (m_pTaxiwayFlightTypeRestrictionDlg) {
		delete m_pTaxiwayFlightTypeRestrictionDlg;
		m_pTaxiwayFlightTypeRestrictionDlg = 0;
	}
	if (m_pTaxiwayConstraintDlg) {
		delete m_pTaxiwayConstraintDlg;
		m_pTaxiwayConstraintDlg = 0;
	}
	if (m_pTemporaryParkingCriteriaDlg) {
		delete m_pTemporaryParkingCriteriaDlg;
		m_pTemporaryParkingCriteriaDlg = 0;
	}

	if (m_pTaxiSpeedConstraintsDlg) {
		delete m_pTaxiSpeedConstraintsDlg;
		m_pTaxiSpeedConstraintsDlg = NULL;
	}
	delete m_pDeiceRouteAssignmentDlg;
	m_pDeiceRouteAssignmentDlg = NULL;

	if (m_pPostDeiceRouteDlg)
	{
		delete m_pPostDeiceRouteDlg;
		m_pPostDeiceRouteDlg = NULL;
	}
}

void NodeViewDbClickHandler::SetACCategoryList(CACCATEGORYLIST* pvACCategoryList)
{
	m_pvACCategoryList = pvACCategoryList;
}

void NodeViewDbClickHandler::SetSubAirlineList(CSUBAIRLINELIST* pvSubAirlineList)
{
	m_pvSubAirlineList = pvSubAirlineList;
}
void NodeViewDbClickHandler::SetSegmentName(std::vector<CString>& vSegName)
{
	m_vSegmentName = vSegName;
}
void NodeViewDbClickHandler::SetIntersectionsName(std::vector<CString> vIntersectionsNameList)
{
	m_vIntersectionsNameList.clear();
	m_vIntersectionsNameList = vIntersectionsNameList;
}

/************************************************************************
FUNCTION: pop up dialog according to lpszTreeItemString
IN		:lpszTreeItemString,name of node view tree item
		:std::vector<CString>& vProcNameList,when lpszTreeItemString == settings,it is a vector of runway mark
                                                                      
/************************************************************************/

void NodeViewDbClickHandler::Handle(LPCTSTR lpszTreeItemString, std::vector<CString>& vProcNameList)
{
	//m_pAirsideInput->ReadAllData();

	//CString strItem(lpszTreeItemString);
	//if(strItem.CompareNoCase(_T("STAR-Runway assignment and criteria")) == 0)
	//{
	//	//CDlgRunwayAssignment dlg(m_pAirsideInput->GetRunwayAssignmentByStar(),
	//	//	m_pAirsideInput->GetAACatManager(),
	//	//	&m_pAirsideInput->m_AirRoutes,
	//	//	vProcNameList, RAT_STAR_RUNWAY, NULL);
	//	//dlg.DoModal();
	//	CRunAssignPropSheet dlg("STAR-Runway assignment criteria", 
	//							m_pAirsideInput->GetRunwayAssignmentByStar(),
	//							m_pAirsideInput->GetAACatManager(),
	//							m_pAirsideInput->GetAirRoutes(),
	//							vProcNameList);
	//	dlg.DoModal();
	//}
	//else if(strItem.CompareNoCase(_T("Runway-SID assignment and criteria")) == 0)
	//{
	//	//CDlgRunwayAssignment dlg(m_pAirsideInput->GetRunwayAssignmentBySid(),
	//	//	m_pAirsideInput->GetAACatManager(),
	//	//	&m_pAirsideInput->m_AirRoutes,
	//	//	vProcNameList, RAT_RUNWAY_SID, NULL);
	//	//dlg.DoModal();
	//}
}

//void NodeViewDbClickHandler::ATCSeparation( )
//{
//	std::vector<BaseProcessor*> vProcList;
//	m_pProcessorList->GetProcessorsByType(vProcList, RunwayProcessor);
//	std::map<CString,std::vector<CString> > runwaymap;
//	for(int nSize = 0; nSize < (int)vProcList.size(); nSize++)
//	{
//		CString strName = vProcList[nSize]->getIDName();
//		std::vector<CString > vecMark;
//
//		RunwayProc* pRunway = (RunwayProc*)vProcList[nSize];
//		BaseRunwayProcessor* pBaseRunway = (BaseRunwayProcessor*)pRunway;
//
//		CString strMark1(pBaseRunway->GetMarking1().c_str());
//		vecMark.push_back(strMark1);
//		CString strMark2(pBaseRunway->GetMarking2().c_str());
//		vecMark.push_back(strMark2 );
//
//		runwaymap.insert(std::map<CString,std::vector<CString> >::value_type(strName,vecMark));
//	}
//
//	CATCPropSheet sh(m_pAirsideInput->GetRunways(),
//					m_pAirsideInput->GetAACatManager()->GetCompCatManager(),
//					runwaymap, _T("ATC Separation"));
//
//	DATABASECONNECTION.GetConnection()->BeginTrans();
//	if (sh.DoModal() == IDOK)
//	{
//		m_pAirsideInput->GetRunways()->SaveData();
//		DATABASECONNECTION.GetConnection()->CommitTrans();			
//	}
//	else
//	{
//		DATABASECONNECTION.GetConnection()->RollbackTrans();
//		m_pAirsideInput->GetRunways()->ReadData();		
//	}
//}

//void NodeViewDbClickHandler::SimSetting()
//{
//	std::vector<BaseProcessor*> vProcList;
//	m_pProcessorList->GetProcessorsByType(vProcList, RunwayProcessor);
//	std::vector<CString > vecMark;
//	for(int nSize = 0; nSize < (int)vProcList.size(); nSize++)
//	{
//		CString strName = vProcList[nSize]->getIDName();
//
//		RunwayProc* pRunway = (RunwayProc*)vProcList[nSize];
//		BaseRunwayProcessor* pBaseRunway = (BaseRunwayProcessor*)pRunway;
//		
//
//		CString strMark1;
//		strMark1.Format(_T("%s:%s"),strName,pBaseRunway->GetMarking1().c_str());
//		vecMark.push_back(strMark1);
//		CString strMark2;//(pBaseRunway->GetMarking2().c_str());
//		strMark2.Format(_T("%s:%s"),strName,pBaseRunway->GetMarking2().c_str());
//		vecMark.push_back(strMark2 );
//	}
//
//	CSimSettingsGeneral SimSettingsGeneral(m_pAirsideInput->GetSettings());
//	CDlgSimDynRunwayAssign simDynrwassign(m_pAirsideInput->GetSettings());
//	CDlgSimWeatherSettings simWeathersetting(m_pAirsideInput->GetSettings());
//	CDlgRwAvailableAssign  simRwavialAssign(m_pAirsideInput->GetSettings(), vecMark);
//
//	//disable help button
//	SimSettingsGeneral.m_psp.dwFlags&=~PSP_HASHELP;
//	simDynrwassign.m_psp.dwFlags&=~PSP_HASHELP;
//	simWeathersetting.m_psp.dwFlags&=~PSP_HASHELP;
//	simRwavialAssign.m_psp.dwFlags&=~PSP_HASHELP;
//
//	CTreePropSheet sht(_T("Simulation Engine Setting"));
//	sht.m_psh.dwFlags&= ~PSH_HASHELP;
//	sht.AddPage(&SimSettingsGeneral);
//	sht.AddPage(&simDynrwassign);
//	sht.AddPage(&simWeathersetting);
//	sht.AddPage(&simRwavialAssign);
//	sht.SetEmptyPageText(_T("Please select a child item of '%s'."));
//
//	sht.SetTreeViewMode(TRUE, TRUE, FALSE);
//	//	sht.SetTreeDefaultImages(&DefaultImages);
//	sht.SetActivePage(&SimSettingsGeneral);
//
//	//	CWnd * m_pMainWnd  = &sht;
//	DATABASECONNECTION.GetConnection()->BeginTrans();
//	if (sht.DoModal() == IDOK)
//	{
//		m_pAirsideInput->GetSettings()->SaveData(*(m_pAirsideInput->GetSettings()));
//		DATABASECONNECTION.GetConnection()->CommitTrans();
//	}
//	else
//	{
//		DATABASECONNECTION.GetConnection()->RollbackTrans();
//		m_pAirsideInput->GetSettings()->ReadData();
//
//	}
//	sht.DestroyPageIcon(&SimSettingsGeneral);
//	sht.DestroyPageIcon(&simDynrwassign);	
//	sht.DestroyPageIcon(&simWeathersetting);
//	sht.DestroyPageIcon(&simRwavialAssign);
//}

//void NodeViewDbClickHandler::AirRoute()
//{
//	CDlgAirRoutes dlg(m_pAirsideInput->GetAirRoutes(), m_pProcessorList);
//	dlg.DoModal();
//}

void NodeViewDbClickHandler::WingspanCategories(int nProjID)
{
	CDlgAircraftClassifications dlg(nProjID, WingspanBased);
	dlg.DoModal();
}

void NodeViewDbClickHandler::VortexWeightCategories(int nProjID)
{
	CDlgAircraftClassifications dlg(nProjID, WakeVortexWightBased);
	dlg.DoModal();
}

void NodeViewDbClickHandler::SpeedCategories(int nProjID)
{
	CDlgAircraftClassifications dlg(nProjID, ApproachSpeedBased);
	dlg.DoModal();
}

void NodeViewDbClickHandler::SurfaceBearingWeightCategories(int nProjID)
{
	CDlgAircraftClassifications dlg(nProjID, SurfaceBearingWeightBased);
	dlg.DoModal();
}

//void NodeViewDbClickHandler::AirlineGroup()
//{
//	CDlgAirlineGroup dlg(m_pvSubAirlineList, m_pAirsideInput->GetAirlineGroup());
//	dlg.DoModal();
//}

//void NodeViewDbClickHandler::TakeoffPosAssignCriteria()
//{
//	CDlgPosAssignCriteria dlg(m_pAirsideInput->GetPosAssignCriteria());
//	dlg.DoModal();
//}

//void NodeViewDbClickHandler::ExitUsageCriteria()
//{
//	CDlgExitUsageCriteria dlg(m_pAirsideInput->GetExitUsageCriteria());
//	dlg.DoModal();
//
//	//CDlgContingency dlg1(m_pAirsideInput->GetContingency());
//	//dlg1.DoModal();
//}

//void NodeViewDbClickHandler::RunwayExitCategories()
//{
//	CDlgRunwayExitDef dlg(m_pAirsideInput->GetRunwayExit());
//	dlg.DoModal();
//}

//void NodeViewDbClickHandler::IntersectionBlocking()
//{
//	CDlgIntersectionBlocking dlg(m_pAirsideInput->GetIntersecDelays(), m_vIntersectionsNameList);
//	dlg.DoModal();
//}

//void NodeViewDbClickHandler::PushBackBlocking()
//{
//	std::vector<BaseProcessor*> vProcList;
//	m_pProcessorList->GetProcessorsByType(vProcList, RunwayProcessor);
//	std::vector<CString> vRunwayName;
//	for(int nSize = 0; nSize < (int)vProcList.size(); nSize++)
//	{
//		CString strName = vProcList[nSize]->getIDName();
//		vRunwayName.push_back(strName);
//	}
//
//	CDlgPushBackBlocking dlg(vRunwayName, m_pAirsideInput->GetPushBackBlocking());
//	dlg.DoModal();
//}

//void NodeViewDbClickHandler::ExclusiveRunwayUse()
//{
//	CDlgExclusiveUseCriteria dlg(m_pAirsideInput->GetRunwayEUCriteria());
//	dlg.DoModal();
//}

void NodeViewDbClickHandler::FlightPlan()
{
	//CDlgFlightPlan dlg(/*m_pAirsideInput->GetFlightPlans(), */m_pSelectFlightType); 
	//dlg.DoModal();
}

void NodeViewDbClickHandler::ItinerantTraffic(int nProjID)
{
	CDlgItinerantTraffic dlg(nProjID, m_pSelectFlightType, m_pInputAirside, m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::PerformanceTakeoff(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgPerformanceTakeoff dlg(nProjID, m_pFuncSelectFlightType, pAirportDB, _T("Takeoff Performance Specification")); 
	dlg.DoModal();
}

void NodeViewDbClickHandler::DepClimbOut(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgDepClimbOut dlg(nProjID, m_pFuncSelectFlightType, pAirportDB, _T("Departure Climb Out Performance Specification"));
	dlg.DoModal();
}

void NodeViewDbClickHandler::TaxiInbound(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgTaxiInbound dlg(nProjID, m_pFuncSelectFlightType, pAirportDB, _T("Taxi Inbound Performance Specification"));
	dlg.DoModal();
}

//void NodeViewDbClickHandler::RunwayExitPerformance()
//{
//
//	std::vector<BaseProcessor*> vProcList;
//	m_pProcessorList->GetProcessorsByType(vProcList, RunwayProcessor);
//	std::vector<CString > vecMark;
//	for(int nSize = 0; nSize < (int)vProcList.size(); nSize++)
//	{
//		CString strName = vProcList[nSize]->getIDName();
//
//		RunwayProc* pRunway = (RunwayProc*)vProcList[nSize];
//		BaseRunwayProcessor* pBaseRunway = (BaseRunwayProcessor*)pRunway;
//
//
//		CString strMark1 = pBaseRunway->GetMarking1().c_str();
////		strMark1.Format(_T("%s:%s"),strName,pBaseRunway->GetMarking1().c_str());
//		vecMark.push_back(strMark1);
//		CString strMark2 = (pBaseRunway->GetMarking2().c_str());
////		strMark2.Format(_T("%s:%s"),strName,pBaseRunway->GetMarking2().c_str());
//		vecMark.push_back(strMark2 );
//	}
//
//	CRunwayExitPerformanceSheet dlg(_T("Runway Exit Performance"), m_pSelectFlightType,
//		m_pAirsideInput->GetRunwayExitPerformance(),
//		m_pAirsideInput->GetAACatManager(),vecMark,m_mapRunwayExit);
//
//	DATABASECONNECTION.GetConnection()->BeginTrans();
//	if (dlg.DoModal() == IDOK)
//	{
//		m_pAirsideInput->GetRunwayExitPerformance()->SaveData();
//		DATABASECONNECTION.GetConnection()->CommitTrans();			
//	}
//	else
//	{
//		DATABASECONNECTION.GetConnection()->RollbackTrans();
//		m_pAirsideInput->GetRunwayExitPerformance()->ReadData();		
//	}
//}

void NodeViewDbClickHandler::TaxiOutbound(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgTaxiOutbound dlg(nProjID, m_pFuncSelectFlightType, pAirportDB);
	dlg.DoModal();
}

void NodeViewDbClickHandler::PushBack(int nProjID, CAirportDatabase* pAirportDB)
{
	DlgTowOperationPerformance dlg(m_pFuncSelectFlightType,pAirportDB, m_pInputAirside,m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::EngineStartAndParameters(int nProjID,CAirportDatabase* pAirportDB)
{
	CDlgEngineParameters dlg(m_pInputAirside,pAirportDB,nProjID,m_pFuncSelectFlightType,m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::StandService(int nPrjID,CAirportDatabase* pAirportDB)
{
	CDlgStandService dlg(nPrjID, m_pFuncSelectFlightType, pAirportDB,m_pInputAirside,m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::FlightPerformanceTerminal(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgFlightPerformanceTerminal dlg(nProjID, m_pFuncSelectFlightType, pAirportDB);
	dlg.DoModal();
}

void NodeViewDbClickHandler::FlightPerformanceCruise(int nProjID,CAirportDatabase* pAirportDB)
{
	CDlgFlightPerformanceCruise dlg(nProjID, m_pFuncSelectFlightType, pAirportDB);
	dlg.DoModal();
}

void NodeViewDbClickHandler::ApproachToLand(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgApproachToLand dlg(nProjID, m_pFuncSelectFlightType, pAirportDB,
						   _T("Approach to Land Performance Specification"));
	dlg.DoModal();
}

void NodeViewDbClickHandler::PerformLanding(int nProjID, CAirportDatabase* pAirportDB)
{
	std::vector<BaseProcessor*> vProcList;
	std::vector<CString> m_vRunwayNames;
	m_pProcessorList->GetProcessorsByType(vProcList, RunwayProcessor);
	
	for(int nSize = 0; nSize < (int)vProcList.size(); nSize++)
		m_vRunwayNames.push_back(vProcList[nSize]->getIDName());

	CDlgPerformLanding dlg(nProjID, m_vRunwayNames, m_pFuncSelectFlightType, pAirportDB,
					     _T("Landing Performance Specification"));
	dlg.DoModal();
}

//void NodeViewDbClickHandler::GatesTowingOperationParam()
//{
//	CDlgTowingOperationParam dlg(m_pAirsideInput->GetGatesTowingOperationParam());
//	dlg.DoModal();
//}

//void NodeViewDbClickHandler::TaxiwayUsageCriteria()
//{
//	CTaxiwayUsageCriteriaSheet dlg(_T("Taxiway Usage Criteria"),
//		m_pAirsideInput->GeTaxiwayUsageCriteria(),
//		m_pAirsideInput->GetAirlineGroup(),
//		m_vSegmentName);
//		DATABASECONNECTION.GetConnection()->BeginTrans();
//	if (dlg.DoModal() == IDOK)
//	{
//		m_pAirsideInput->GeTaxiwayUsageCriteria()->SaveData();
//		DATABASECONNECTION.GetConnection()->CommitTrans();			
//	}
//	else
//	{
//		DATABASECONNECTION.GetConnection()->RollbackTrans();
//		m_pAirsideInput->GeTaxiwayUsageCriteria()->ReadData();		
//	}
//}
//TEMP
//#include "DlgGateAssignBuffers.h"
//TEMP END
//void NodeViewDbClickHandler::DepInTrail()
//{
//	CDlgDepInTrail dlg(m_pAirsideInput->GetDepInTrail(), m_pAirsideInput->GetAirRoutes());
//	dlg.DoModal();
////TEMP
//	//CDlgGateAssignBuffers dlgEx(m_pAirsideInput->GetGateAssignBuffers(),
//	//							m_pAirsideInput->GetAirlineGroup(),_T("Gate Assignment Buffers"), TRUE);
//
//	//dlgEx.DoModal();
////TEMP END
//}
void NodeViewDbClickHandler::SetmapRunwayExit(const std::map<CString,int>& mapRunwayExit)
{
	m_mapRunwayExit = m_mapRunwayExit;
}

void NodeViewDbClickHandler::ArrivalInit()
{
	CDlgArrivalInit dlg(m_vArrivalInitPara);
	dlg.DoModal();
}

void NodeViewDbClickHandler::SetFlightArrivalInitList(std::vector<FlightArrivalInit>& vArrivalInitPara)
{
	m_vArrivalInitPara = vArrivalInitPara;
}

int NodeViewDbClickHandler::DefineAirRoute(BOOL bEditAirRoute,int nAirspaceID,int nAirRouteID)
{
	AirsideGUI::CDlgAirRoute dlg(bEditAirRoute,nAirspaceID,nAirRouteID,NULL);

	if (dlg.DoModal() == IDOK)
	{	
		return dlg.getProjID();
	}
	return NULL;
}

//Runway

void NodeViewDbClickHandler::LandingRunwayAssignment(int nProjID, CAirportDatabase *pAirPortdb)
{
	CDlgLandingRunwayAssign dlg(nProjID, m_pSelectFlightType, pAirPortdb);
	dlg.DoModal();
}

//tow off stand assignment
void NodeViewDbClickHandler::TowOffStandAssignment(int nProjID, CAirportDatabase *pAirPortdb)
{
	CTowOffStandAssignmentSpecificationDlg dlg(nProjID, m_pSelectFlightType,m_pSelectNewFlightType, pAirPortdb);
	dlg.DoModal();
	
}

void NodeViewDbClickHandler::SidAssignment(int nProjID, CAirportDatabase *pAirPortdb)
{
	CDlgSIDAssignment dlg(nProjID, m_pSelectFlightType, pAirPortdb);
	dlg.DoModal();
}

void NodeViewDbClickHandler::ArrivalETAOffset(int nProjID, CAirportDatabase *pAirPortdb)
{
	CArrivalETAOffsetDlg dlg(nProjID, m_pSelectFlightType, m_pInputAirside, pAirPortdb, m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::TakeoffRunwayAssignment(int nProjID, CAirportDatabase *pAirPortdb,int nAirPortID)
{
	CDlgTakeoffRunwayAssign dlg(nProjID, m_pSelectFlightType, pAirPortdb,nAirPortID);
	dlg.DoModal();
}

void NodeViewDbClickHandler::TakeoffSequence(int nProjID, CAirportDatabase *pAirPortdb)
{
	if (m_pTakeoffSequenceDlg) {
		m_pTakeoffSequenceDlg->DestroyWindow();
		delete m_pTakeoffSequenceDlg;
		m_pTakeoffSequenceDlg = 0;
	}
	m_pTakeoffSequenceDlg = new CDlgTakeoffSequence(nProjID, m_pSelectFlightType,pAirPortdb);
	if (m_pTakeoffSequenceDlg && m_pTakeoffSequenceDlg->Create(CDlgTakeoffSequence::IDD)) 	
	{
		m_pTakeoffSequenceDlg->SetAirportDB(pAirPortdb);
		m_pTakeoffSequenceDlg->CenterWindow();
		m_pTakeoffSequenceDlg->ShowWindow(SW_SHOW);
		m_pTakeoffSequenceDlg->SetForegroundWindow();
	}
}

void NodeViewDbClickHandler::LandingRunwayExit(int nProjID)
{
	CDlgLandingRunwayExit dlg(nProjID, m_pFuncSelectFlightType, m_pInputAirside);
	dlg.DoModal();

}
 
void NodeViewDbClickHandler::RunwayTakeoffPosition(int nProjID)
{
	CDlgRunwayTakeoffPosition dlg(nProjID, m_pSelectFlightType, m_pInputAirside);
	dlg.DoModal();
}

void NodeViewDbClickHandler::StandBuffers(int nProjID,CAirportDatabase* pAirportdb)
{
	CDlgStandBuffer dlg(nProjID, m_pSelectFlightType,pAirportdb);
	dlg.DoModal();
}
void NodeViewDbClickHandler::OccupiedAssignedStandAction(int nProjID)
{
 	CDlgOccupiedAssignedStand dlg(nProjID, m_pFuncSelectFlightType, m_pInputAirside);
 	dlg.DoModal();
}

void NodeViewDbClickHandler::IgnitionSpecs(int nPrjID,CAirportDatabase* pAirportDB)
{
	DlgIgnitionSpecification dlg(nPrjID,m_pSelectFlightType,pAirportDB);
	dlg.DoModal();
}
void NodeViewDbClickHandler::GoAroundCriteriaCustomize(int nProjID)
{
	CDlgGoAroundCriteriaDataProcess dlg(nProjID,m_pGlobalUnits);
	dlg.DoModal();
}

void  NodeViewDbClickHandler::SetStretchOption(int nProjID)
{
	CDlgStretchOption  dlg(nProjID);
	dlg.DoModal();
}

void NodeViewDbClickHandler::PushBackClearanceCriteria(int nProjID,CAirportDatabase* pAirportdb)
{
	CDlgPushBackClearanceCriteria dlg(nProjID,m_pGlobalUnits,m_pSelectFlightType,pAirportdb);
	dlg.DoModal();
}

void NodeViewDbClickHandler::VehiclePoolsAndDeployment(int nProjID)
{
	CDlgVehiclePoolsAndDeployment dlg(nProjID, m_pSelectFlightType, m_pInputAirside, m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::VehicleRoute(int nProjID)
{
	CDlgVehicleRoute dlg(nProjID);
	dlg.DoModal();
}

void NodeViewDbClickHandler::SectorFlightMixSpec(int nPrjID)
{
	CDlgSectorFlightMixSpecification dlg(nPrjID);
	dlg.DoModal();
}
void NodeViewDbClickHandler::TakeoffQueues(int nProjID,CAirportDatabase *pAirportDB)
{
	//CDlgTakeoffQueues dlg(nProjID);
	//dlg.DoModal();
	if (m_pTakeoffQueuesDlg != NULL)
	{
		delete m_pTakeoffQueuesDlg;
	}
	m_pTakeoffQueuesDlg = new CDlgTakeoffQueues(nProjID,pAirportDB);
	m_pTakeoffQueuesDlg->Create(CDlgTakeoffQueues::IDD);
	m_pTakeoffQueuesDlg->CenterWindow();
	m_pTakeoffQueuesDlg->ShowWindow(SW_SHOWNORMAL);
	m_pTakeoffQueuesDlg->SetForegroundWindow();
}


void NodeViewDbClickHandler::ApprochSeparationCriteria(int nProjID)
{/*
	CDlgApprochSeparationCriteria dlg(nProjID,m_pGlobalUnits);
	dlg.DoModal();
*/
	//New
	CDlgApproachSeparationCriteria dlg(nProjID, m_pInputAirside, m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::InTrailSeparation(int nProjID, Terminal* pTerminal,CAirportDatabase *pAirportDB, CWnd *pWnd)
{ 
//	CDlgInTrailSeparation dlg(nProjID, pTerminal, pWnd);
	CDlgInTrailSeperationEx dlg(nProjID, pTerminal,pAirportDB, m_pInputAirside, m_pSelectProbDistEntry, pWnd);
	dlg.DoModal();
}
void NodeViewDbClickHandler::DirectRoutingCriteria(int nProjID)
{
	CDlgDirectRoutingCriteria dlg(nProjID);
	dlg.DoModal();
}
void NodeViewDbClickHandler::TakeoffClearance(int nProjID)
{
	//new 
	CDlgTakeoffClearance dlg(nProjID, m_pInputAirside, m_pSelectProbDistEntry);
	dlg.DoModal();
	return;

	//Check if Aircraft classification defined;

	//{
	//	BOOL bACDefined = FALSE;

	//	do {
	//		AircraftClassifications *pACInfo =
	//			new AircraftClassifications(nProjID, WingspanBased);
	//		pACInfo->ReadData();
	//		bACDefined = pACInfo->GetCount() > 0 ? TRUE : FALSE;
	//		delete pACInfo;
	//		if(bACDefined)
	//			break;

	//		pACInfo = new AircraftClassifications(nProjID, SurfaceBearingWeightBased);
	//		pACInfo->ReadData();
	//		bACDefined = pACInfo->GetCount() > 0 ? TRUE : FALSE;
	//		delete pACInfo;
	//		if(bACDefined)
	//			break;

	//		pACInfo = new AircraftClassifications(nProjID, WakeVortexWightBased);
	//		pACInfo->ReadData();
	//		bACDefined = pACInfo->GetCount() > 0 ? TRUE : FALSE;
	//		delete pACInfo;
	//		if(bACDefined)
	//			break;

	//		pACInfo = new AircraftClassifications(nProjID, ApproachSpeedBased);
	//		pACInfo->ReadData();
	//		bACDefined = pACInfo->GetCount() > 0 ? TRUE : FALSE;
	//		delete pACInfo;
	//		if(bACDefined)
	//			break;

	//		pACInfo = new AircraftClassifications(nProjID, NoneBased);
	//		pACInfo->ReadData();
	//		bACDefined = pACInfo->GetCount() > 0 ? TRUE : FALSE;
	//		delete pACInfo;
	//		if(bACDefined)
	//			break;

	//		AfxMessageBox(_T("Please define Aircraft classification first"));
	//		return;
	//		
	//	} while(TRUE);
	//}

	//TakeoffClearances* m_pTakeoffClearances
	//	= new TakeoffClearances(nProjID);
	//m_pTakeoffClearances->ReadData();

	//CDlgTBTSameRunway dlgTBTSameRunway(nProjID, 
	//	m_pTakeoffClearances->GetTakeoffClearanceItem(TakeoffBehindTakeoffSameRW));
	//CDlgTBTIntersect dlgTBTIntersect(nProjID, m_pTakeoffClearances->GetTakeoffClearanceItem(TakeoffBehindTakeoffIntersectRW));
	//CDlgTBLSameRunway dlgTBLSameRunway(nProjID, m_pTakeoffClearances->GetTakeoffClearanceItem(TakeoffBehindLandingSameRW));
	//CDlgTBLIntersect dlgTBLIntersect(nProjID, m_pTakeoffClearances->GetTakeoffClearanceItem(TakeoffBehindLandingIntersectRW));

	//dlgTBTSameRunway.m_psp.dwFlags&=~PSP_HASHELP;
	//dlgTBTIntersect.m_psp.dwFlags&=~PSP_HASHELP;
	//dlgTBLSameRunway.m_psp.dwFlags&=~PSP_HASHELP;
	//dlgTBLIntersect.m_psp.dwFlags&=~PSP_HASHELP;



	//CTreePropSheet sht(_T("Takeoff Clearance Criteria"));
	//sht.m_psh.dwFlags&= ~PSH_HASHELP;
	//sht.SetTreeWidth(220);

	//sht.AddPage(&dlgTBTSameRunway); 
	//sht.AddPage(&dlgTBTIntersect);
	//sht.AddPage(&dlgTBLSameRunway);
	//sht.AddPage(&dlgTBLIntersect);

	//sht.SetEmptyPageText(_T("Please select a child item of '%s'."));
	//sht.SetTreeViewMode(TRUE, FALSE, FALSE);

	//sht.SetActivePage(&dlgTBTSameRunway);
	//
	//if(sht.DoModal() == IDOK)
	//	m_pTakeoffClearances->SaveData();
	//
	//sht.DestroyPageIcon(&dlgTBTSameRunway);
	//sht.DestroyPageIcon(&dlgTBTIntersect);	
	//sht.DestroyPageIcon(&dlgTBLSameRunway);
	//sht.DestroyPageIcon(&dlgTBLIntersect);


	//delete m_pTakeoffClearances;
	//m_pTakeoffClearances = NULL;

}

void NodeViewDbClickHandler::TaxiwayConstraint(int nProjID, TaxiwayConstraintType emType,CAirportDatabase* pAirportDB, CWnd *pWnd)
{
	if (m_pTaxiwayConstraintDlg) {
		delete m_pTaxiwayConstraintDlg;
		m_pTaxiwayConstraintDlg = 0;
	}
	m_pTaxiwayConstraintDlg = new CDlgTaxiwayConstraint(nProjID, emType, pAirportDB,pWnd);
	if (m_pTaxiwayConstraintDlg) {
		m_pTaxiwayConstraintDlg->Create(CDlgTaxiwayConstraint::IDD);
		m_pTaxiwayConstraintDlg->CenterWindow();
		m_pTaxiwayConstraintDlg->ShowWindow(SW_SHOWNORMAL);
		m_pTaxiwayConstraintDlg->SetForegroundWindow();
	}
}

void NodeViewDbClickHandler::TaxiwayFlightTypeRestriction(int nProjID, Terminal* pTerminal, CWnd *pWnd)
{
	//CDlgTaxiwayFlightTypeRestriction dlg(nProjID,pTerminal,pWnd);
	//dlg.DoModal();
}

void NodeViewDbClickHandler::TemporaryParkingCriteria(int nProjID, CWnd *pWnd)
{
	if (m_pTemporaryParkingCriteriaDlg) {
		delete m_pTemporaryParkingCriteriaDlg;
		m_pTemporaryParkingCriteriaDlg = 0;
	}
	m_pTemporaryParkingCriteriaDlg = new CDlgTemporaryParkingCriteria(nProjID, pWnd);
	if (m_pTemporaryParkingCriteriaDlg) {
		m_pTemporaryParkingCriteriaDlg->Create(CDlgTemporaryParkingCriteria::IDD);
		m_pTemporaryParkingCriteriaDlg->CenterWindow();
		m_pTemporaryParkingCriteriaDlg->ShowWindow(SW_SHOWNORMAL);
		m_pTemporaryParkingCriteriaDlg->SetForegroundWindow();
	}
}
void NodeViewDbClickHandler::ConflictResolutionCriteria(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgConflictResolution dlg(nProjID, m_pSelectFlightType, pAirportDB);
	dlg.DoModal();
}
void NodeViewDbClickHandler::WeatherImpacts(int nProjID)
{
	CWeatherImpactsDlg dlg(nProjID, m_pSelectFlightType);
	dlg.DoModal();
}

void NodeViewDbClickHandler::PadAssignment(int nProjID)
{
	CDlgPadAissgnment dlg(nProjID,m_pSelectFlightType);
	dlg.DoModal();
}

void NodeViewDbClickHandler::DeicePadRoute(int nProjID,int nAirportID, CAirportDatabase* pAirportDB)
{
	if (NULL != m_pDeiceRouteAssignmentDlg)
	{
		delete m_pDeiceRouteAssignmentDlg;
		m_pDeiceRouteAssignmentDlg = NULL;
	}

	CString strTitle = "Deice Operation Route Assignment";

	m_pDeiceRouteAssignmentDlg = new CDeiceRouteAssigmentDlg(nProjID,m_pFuncSelectFlightType,pAirportDB,strTitle);
	m_pDeiceRouteAssignmentDlg->Create(CBoundRouteAssignmentDlg::IDD);
	m_pDeiceRouteAssignmentDlg->CenterWindow();
	m_pDeiceRouteAssignmentDlg->ShowWindow(SW_SHOWNORMAL);
	m_pDeiceRouteAssignmentDlg->SetForegroundWindow();
}

void AirsideGUI::NodeViewDbClickHandler::PostDeiceRoute( int nProjID,int nAirportID,CAirportDatabase *pAirportDB )
{
	if (NULL != m_pPostDeiceRouteDlg)
	{
		delete m_pPostDeiceRouteDlg;
		m_pPostDeiceRouteDlg = NULL;
	}

	CString strTitle = "Post Deicing Route Assignment";

	m_pPostDeiceRouteDlg = new CPostDeicingRouteAssignmentDlg(nProjID,m_pFuncSelectFlightType,pAirportDB,strTitle);
	m_pPostDeiceRouteDlg->Create(CBoundRouteAssignmentDlg::IDD);
	m_pPostDeiceRouteDlg->CenterWindow();
	m_pPostDeiceRouteDlg->ShowWindow(SW_SHOWNORMAL);
	m_pPostDeiceRouteDlg->SetForegroundWindow();
}

void NodeViewDbClickHandler::ServicingRequirement(int nProj, CAirportDatabase* pAirportDB)
{
	CServiceRequirementDlg dlg(nProj, m_pSelectFlightType, m_pInputAirside, m_pSelectProbDistEntry, pAirportDB);
	dlg.DoModal();
}

void NodeViewDbClickHandler::DeiceQueue(int nProjID,int nAirportID)
{
	CDlgDeiceQueue dlg(nProjID,nAirportID,NULL);
	dlg.DoModal();
}
void NodeViewDbClickHandler::StarAssignment(int nProjID, CAirportDatabase* pAirportdb)
{
	CStarAssignmentDlg dlg(nProjID, m_pSelectFlightType, pAirportdb);
	dlg.DoModal();
}
void NodeViewDbClickHandler::DeiceAntiStrategies(int nProjID,CAirportDatabase* pAirportdb)
{
	CDlgDe_icingAndAnti_icingStrategies dlg(nProjID,pAirportdb, m_pSelectFlightType);
	dlg.DoModal();
}

void NodeViewDbClickHandler::CtrlInterventionSpec(int nProjID)
{
	CCtrlInterventionSpecDlg dlg(nProjID);
	dlg.DoModal();
}
void NodeViewDbClickHandler::SideSepSpecification(int nProjID, CAirportDatabase* pAirportDB)
{
	CDlgSideStepSpecification dlg(nProjID, m_pSelectFlightType, pAirportDB);
	dlg.DoModal();
}
void NodeViewDbClickHandler::InboundRouteAssignment(int nProjID, CAirportDatabase* pAirportDB)
{
	if (NULL != m_pInboundRouteAssignmentDlg)
	{
		delete m_pInboundRouteAssignmentDlg;
	}

	CString strTitle = "Inbound Route Assignment";

	m_pInboundRouteAssignmentDlg = new InboundRouteAssignmentDlg(nProjID,m_pFuncSelectFlightType,pAirportDB,strTitle);
	m_pInboundRouteAssignmentDlg->Create(CBoundRouteAssignmentDlg::IDD);
	m_pInboundRouteAssignmentDlg->CenterWindow();
	m_pInboundRouteAssignmentDlg->ShowWindow(SW_SHOWNORMAL);
//	m_pInboundRouteAssignmentDlg->SetForegroundWindow();
}

void NodeViewDbClickHandler::OutboundRouteAssignment(int nProjID, CAirportDatabase* pAirportDB)
{
	if (m_pOutboundRouteAssignmentDlg)
	{
		delete m_pOutboundRouteAssignmentDlg;
	}
	
	CString strTitle = "Outbound Route Assignment";

	m_pOutboundRouteAssignmentDlg = new CDlgOutboundRouteAssignment2(nProjID,m_pFuncSelectFlightType, pAirportDB,strTitle);
	m_pOutboundRouteAssignmentDlg->Create(CDlgOutboundRouteAssignment2::IDD);
	m_pOutboundRouteAssignmentDlg->CenterWindow();
	m_pOutboundRouteAssignmentDlg->ShowWindow(SW_SHOWNORMAL);
//	m_pOutboundRouteAssignmentDlg->SetForegroundWindow();
}

void NodeViewDbClickHandler::GroundRoutesDlg(int nProjID, int nAirportID, CWnd *pWnd)
{
	if (NULL != m_pGroundRoutesDlg)
	{
		delete m_pGroundRoutesDlg;
	}

	m_pGroundRoutesDlg = new CDlgGroundRoutes(nProjID, nAirportID, pWnd);
	m_pGroundRoutesDlg->Create(CDlgGroundRoutes::IDD, pWnd);
	m_pGroundRoutesDlg->CenterWindow();	
	m_pGroundRoutesDlg->ShowWindow(SW_SHOWNORMAL);
}
void NodeViewDbClickHandler::DoModalVehicleSpecification(int nProjID)
{
	CDlgVehicleSpecification dlg(nProjID);
	dlg.DoModal();
}

void NodeViewDbClickHandler::FlightTypeDimensions(int nProjID,CAirportDatabase* pAirportDB)
{
	CDlgFlightTypeDimensions dlg(nProjID, m_pSelectFlightType, pAirportDB);
	dlg.DoModal();

}

void NodeViewDbClickHandler::FlightTypeRelativeServiceLocations(int nProjID,CAirportDatabase* pAirportDB)
{
	CDlgFlightRelativeServiceLocations dlg(nProjID, m_pSelectFlightType, pAirportDB);
	dlg.DoModal();
}

void NodeViewDbClickHandler::StandCriteriaAssign(int nProjID, CAirportDatabase* pAirportDB)
{
	DlgStandCriteriaAssignment dlg(nProjID,pAirportDB,m_pSelectFlightType);
	dlg.DoModal();
}
void NodeViewDbClickHandler::FlightTypeRestrictions(int nProjID, CAirportDatabase *pAirPortdb, CWnd *pWnd)
{
	if (m_pTaxiwayFlightTypeRestrictionDlg) {
		delete m_pTaxiwayFlightTypeRestrictionDlg;
		m_pTaxiwayFlightTypeRestrictionDlg = 0;
	}
	m_pTaxiwayFlightTypeRestrictionDlg = new CDlgTaxiwayFlightTypeRestriction(nProjID,m_pSelectFlightType, pAirPortdb, pWnd);
	if (m_pTaxiwayFlightTypeRestrictionDlg) {
		m_pTaxiwayFlightTypeRestrictionDlg->Create(CDlgTaxiwayFlightTypeRestriction::IDD);
		m_pTaxiwayFlightTypeRestrictionDlg->CenterWindow();
		m_pTaxiwayFlightTypeRestrictionDlg->ShowWindow(SW_SHOWNORMAL);
		m_pTaxiwayFlightTypeRestrictionDlg->SetForegroundWindow();
	}
}

void NodeViewDbClickHandler::DepartureSlotSpecification(int nProjID,CAirportDatabase* pAirportDB)
{
	CDlgDepartureSlotSpec dlg(nProjID, m_pSelectFlightType, pAirportDB,m_pInputAirside,m_pSelectProbDistEntry);
	dlg.DoModal();
}

void NodeViewDbClickHandler::WaypointImport(int nProjID)
{
	CDialogWPImport import(nProjID);
	import.DoModal();
}

void NodeViewDbClickHandler::DirectionalityConstraints(int nProjID, int nAirportID)
{
	if (m_pDirectionalityConstraintDlg) {
		delete m_pDirectionalityConstraintDlg;
		m_pDirectionalityConstraintDlg = 0;
	}
	m_pDirectionalityConstraintDlg = new CDlgDirectionalityConstraints(nProjID, nAirportID);
	if (m_pDirectionalityConstraintDlg) {
		m_pDirectionalityConstraintDlg->Create(CDlgDirectionalityConstraints::IDD);
		m_pDirectionalityConstraintDlg->CenterWindow();
		m_pDirectionalityConstraintDlg->ShowWindow(SW_SHOWNORMAL);
		m_pDirectionalityConstraintDlg->SetForegroundWindow();
	}
}
void NodeViewDbClickHandler::TowingRoute(int nProjID)
{
	if(m_pTowingRouteDlg){
		delete m_pTowingRouteDlg;
		m_pTowingRouteDlg = 0;
	}
	m_pTowingRouteDlg = new CDlgTowingRoute(nProjID);
	if (m_pTowingRouteDlg) {
		m_pTowingRouteDlg->Create(CDlgTowingRoute::IDD);
		m_pTowingRouteDlg->CenterWindow();
		m_pTowingRouteDlg->ShowWindow(SW_SHOWNORMAL);
		m_pTowingRouteDlg->SetForegroundWindow();
	}
}
void NodeViewDbClickHandler::setControllerIntervention(int nProjID,CWnd *pWnd) 
{
	CControllerIntervention dlgsector(nProjID,pWnd) ;
	dlgsector.DoModal() ;
}

void AirsideGUI::NodeViewDbClickHandler::DoModalDlgVehicularMovementAllowed( int nProjID, CWnd *pWnd )
{
	CDlgVehicularMovementAllowed dlg(nProjID,pWnd);
	dlg.DoModal();
}
void AirsideGUI::NodeViewDbClickHandler::DoTaxiInterruptTime(int nProjID,CAirportDatabase* pAirportDB,CWnd *pWnd ) 
{
	CDlgTaxiInterruptTimes dlg(nProjID,m_pSelectFlightType,pAirportDB,m_pSelectProbDistEntry,m_pInputAirside,pWnd) ;
	dlg.DoModal() ;
}
void AirsideGUI::NodeViewDbClickHandler::DoDeicingAndAnticings_demand(InputTerminal* inputterminal , CAirportDatabase *pAirPortdb, CWnd *pWnd)
{
        CDlgDeicingAndAntiIcingFluidSpc dlg(inputterminal,m_pInputAirside,m_pSelectProbDistEntry,m_pSelectFlightType,pWnd) ;
		dlg.DoModal() ;
}
void AirsideGUI::NodeViewDbClickHandler::DoAircraftSurfaceCondition(CAirportDatabase *pAirPortdb, CWnd *pWnd)
{
	CDlgAircraftSurfaceInitialCondition dlg(pAirPortdb,m_pSelectFlightType,pWnd) ;
	dlg.DoModal() ;
}
void AirsideGUI::NodeViewDbClickHandler::DoWeatherScript( CWnd *pWnd)
{
	CDlgWeatherScript dlg(pWnd) ;
	dlg.DoModal() ;
}

void AirsideGUI::NodeViewDbClickHandler::DoModelPushBackAndTowOperationSpecification( int nProjID,CAirportDatabase* pAirportDB,CWnd *pWnd )
{
	CDlgPushBackAndTowOperationSpec dlg(nProjID,m_pFuncSelectFlightType,pAirportDB,pWnd);
	dlg.DoModal();
}

void AirsideGUI::NodeViewDbClickHandler::TaxiSpeedConstraints( int nProjID,CAirportDatabase *pAirPortDB )
{
	if (m_pTaxiSpeedConstraintsDlg) {
		delete m_pTaxiSpeedConstraintsDlg;
		m_pTaxiSpeedConstraintsDlg = NULL;
	}
	m_pTaxiSpeedConstraintsDlg = new CDlgTaxiSpeedConstraints(nProjID, pAirPortDB, m_pFuncSelectFlightType);
	if (m_pTaxiSpeedConstraintsDlg) {
		m_pTaxiSpeedConstraintsDlg->Create(CDlgTaxiSpeedConstraints::IDD);
		m_pTaxiSpeedConstraintsDlg->CenterWindow();
		m_pTaxiSpeedConstraintsDlg->ShowWindow(SW_SHOWNORMAL);
		m_pTaxiSpeedConstraintsDlg->SetForegroundWindow();
	}
}

void AirsideGUI::NodeViewDbClickHandler::IntersectionNodeOption( int nAirportID )
{
	CDlgIntersectionNodeBlock dlg(nAirportID);
	dlg.DoModal();
}

bool AirsideGUI::NodeViewDbClickHandler::TakeoffPositionSelect( int nProjID, CString& sTakeoffPosition,CAirportDatabase* pAirportDB )
{
	CDlgSelectTakeoffPosition dlg(nProjID,-1,-1);
	ASSERT(pAirportDB);
	if (pAirportDB == NULL)
		return false;
	
	dlg.SetAirportDB(pAirportDB);
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_arSelName.GetSize() == 0)
		{
			return false;
		}

		sTakeoffPosition = dlg.m_arSelName[0];
		return true;
	}
	return false;
}

void AirsideGUI::NodeViewDbClickHandler::TrainingFlgihtSpecification( int nProjID,InputTerminal* pInTerm,InputAirside* pInputAirside )
{
	CDlgTrainFlightSpec dlg(nProjID,pInTerm,pInputAirside,m_pSelectProbDistEntry);
	dlg.DoModal();
}

void AirsideGUI::NodeViewDbClickHandler::TrainingFlgihtSManagement(CAirportDatabase *pAirPortdb,int nProjID,CWnd *pWnd)
{
	CTrainingFlightContainer m_trainFlightContainer;
	m_trainFlightContainer.ReadData();
	if (m_trainFlightContainer.GetElementCount() == 0)
	{
		MessageBoxA(NULL,_T("No 'Training Flight Specification' Defined.\nPlease Go To The Dialog 'Training Flight Specification' And define the data first!"),NULL,NULL);
		return;
	}
	CDlgTrainFlightsManagement dlg(pAirPortdb,nProjID,pWnd);
	dlg.DoModal();
}






