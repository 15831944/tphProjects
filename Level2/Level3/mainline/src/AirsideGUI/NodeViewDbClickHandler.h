#ifndef __NODEVIEWDBLCLICK_H
#define __NODEVIEWDBLCLICK_H

#include "../InputAirside/AirRoute.h"
//#include "../AirsideInput/ASATCSepData.h"
#include "../common/aircraft.h"
#include "../common/Subairline.h"
#include "ArrivalInitPara.h"
#include "../InputAirside/TaxiwayConstraint.h"
#include "../InputAirside/InputAirside.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/UnitsManager.h"
#include "../Common/AirsideFlightType.h"
#include "AirsideGUIAPI.h"

class BaseProcessorList;
class CDlgGroundRoutes;
class CDlgTakeoffQueues;
class CDlgTakeoffSequence;
class CDlgGoAroundCriteriaDataProcess;
class CDlgTowingRoute;
class CDlgDirectionalityConstraints;
class CDlgTaxiwayFlightTypeRestriction;
class CDlgTaxiwayConstraint;
class CDlgTemporaryParkingCriteria;
class CDlgSectorFlightMixSpecification;
class DlgStandCriteriaAssignment;
class InboundRouteAssignmentDlg ;
class CDlgOutboundRouteAssignment2 ;
class CDeiceRouteAssigmentDlg;
class CDlgTaxiSpeedConstraints;
class CPostDeicingRouteAssignmentDlg;
//class CDlgStretchOption;

typedef FlightConstraint (*PSelectFlightType)(CWnd* pParent);
typedef BOOL (*PFuncSelectFlightType)(CWnd* pParent,FlightConstraint&);
typedef CProbDistEntry* (*PSelectProbDistEntry)(CWnd* pParent, InputAirside* pInputAirside);
typedef BOOL (*PSelectNewFlightType)(CWnd* pParent, AirsideFlightType&);

namespace AirsideGUI
{

class AIRSIDEGUI_API NodeViewDbClickHandler
{
public:
	NodeViewDbClickHandler(BaseProcessorList* _pProcessorList, InputAirside* _pInputAirside);
	~NodeViewDbClickHandler();

	void Handle(LPCTSTR lpszTreeItemString, std::vector<CString>& vProcNameList);
	
	int DefineAirRoute(BOOL bEditAirRoute,int nAirspaceID,int nAirRouteID);
	
//	void SimSetting();
//	void AirRoute();
	
	void WingspanCategories(int nProjID);
	void VortexWeightCategories(int nProjID);
	void SurfaceBearingWeightCategories(int nProjID);
	void SpeedCategories(int nProjID);

	void GroundRoutesDlg(int nProjID, int nAirportID, CWnd *pWnd);

	void AirlineGroup();
	//void ExitUsageCriteria();
	//void TakeoffPosAssignCriteria();
//	void IntersectionBlocking();
//	void PushBackBlocking();
	//void ATCSeparation();
	//void ExclusiveRunwayUse();
	void FlightPlan();
	void ItinerantTraffic(int nProjID);
	void PerformanceTakeoff(int nProjID, CAirportDatabase* pAirportDB);
	void DepClimbOut(int nProjID, CAirportDatabase* pAirportDB);
	void TaxiInbound(int nProjID, CAirportDatabase* pAirportDB);
	void TaxiOutbound(int nProjID, CAirportDatabase* pAirportDB);
	void PushBack(int nProjID, CAirportDatabase* pAirportDB);
	void EngineStartAndParameters(int nProjID,CAirportDatabase* pAirportDB);
	void StandService(int nPrjID,CAirportDatabase* pAirportDB);
	void ApproachToLand(int nProjID, CAirportDatabase* pAirportDB);
	void PerformLanding(int nProjID, CAirportDatabase* pAirportDB);
//	void RunwayExitPerformance();
	void FlightPerformanceTerminal(int nProjID, CAirportDatabase* pAirportDB);
	void FlightPerformanceCruise(int nProjID, CAirportDatabase* pAirportDB);
//	void GatesTowingOperationParam();
//	void TaxiwayUsageCriteria();	
//	void DepInTrail();
	void ArrivalInit();
	void InTrailSeparation(int nProjID, Terminal* pTerminal,CAirportDatabase *pAirportDB, CWnd *pWnd);
	void WeatherImpacts(int nProjID);
	void PadAssignment(int nProjID);
	void DeicePadRoute(int nProjID,int nAirportID,CAirportDatabase *pAirportDB);
	void PostDeiceRoute(int nProjID,int nAirportID,CAirportDatabase *pAirportDB);
	void DeiceQueue(int nProjID,int nAirportID);
	void DeiceAntiStrategies(int nProjID,CAirportDatabase* pAirportdb);
	void StarAssignment(int nProjID, CAirportDatabase* pAirportdb);
	void CtrlInterventionSpec(int nProjID);
	void SideSepSpecification(int nProjID, CAirportDatabase* pAirportDB);
	void StandCriteriaAssign(int nProjID, CAirportDatabase* pAirportDB);
	//Vehicles(GSE)
	void ServicingRequirement(int nProj, CAirportDatabase* pAirportDB);


	//Runway
	void LandingRunwayAssignment(int nProjID, CAirportDatabase *pAirPortdb);
	void SidAssignment(int nProjID, CAirportDatabase *pAirPortdb);
	void TakeoffRunwayAssignment(int nProjID, CAirportDatabase *pAirPortdb,int nAirPortID);
	void LandingRunwayExit(int nProjID);
	void RunwayTakeoffPosition(int nProjID);
	void TakeoffQueues(int nProjID,CAirportDatabase *pAirportDB);
	void TakeoffSequence(int nProjID, CAirportDatabase *pAirPortdb);

	void ArrivalETAOffset(int nProjID, CAirportDatabase *pAirPortdb);

	//tow off stand assignment
	void TowOffStandAssignment(int nProjID, CAirportDatabase *pAirPortdb);

	//GoAroundCriteria
	void GoAroundCriteriaCustomize(int nProjID);

	//StretchOption
	void SetStretchOption(int nProjID);

	//#include "TakeoffClearance.h"
	void TakeoffClearance(int nProjID);

	//Gate
	void StandBuffers(int nProjID,CAirportDatabase* pAirportdb);
	void OccupiedAssignedStandAction(int nProjID);
	void IgnitionSpecs(int nPrjID,CAirportDatabase* pAirportDB);

	//ApprochSeparationCriteria
	void ApprochSeparationCriteria(int nProjID);

	//PushBackClearanceCriteria
	void PushBackClearanceCriteria(int nProjID,CAirportDatabase* pAirportdb);

	// Taxiways
	void TaxiwayConstraint(int nProjID, TaxiwayConstraintType emType,CAirportDatabase* pAirportDB, CWnd *pWnd);
	void TaxiwayFlightTypeRestriction(int nProjID, Terminal* pTerminal, CWnd *pWnd);
	void TemporaryParkingCriteria(int nProjID, CWnd *pWnd);
	void InboundRouteAssignment(int nProjID, CAirportDatabase* pAirportDB);
	void OutboundRouteAssignment(int nProjID, CAirportDatabase* pAirportDB);
	void DirectRoutingCriteria(int nProjID);
	void ConflictResolutionCriteria(int nProjID, CAirportDatabase* pAirportDB);
	void DoModalVehicleSpecification(int nProjID);
	void FlightTypeRestrictions(int nProjID, CAirportDatabase* pAirportDB, CWnd *pWnd);
	void TowingRoute(int nProjID);
	//vehicular movement allowed
	void DoModalDlgVehicularMovementAllowed(int nProjID, CWnd *pWnd );
    void DoTaxiInterruptTime(int nProjID,CAirportDatabase* pAirportDB,CWnd *pWnd  ) ;
	
	void SetFlightTypeSelectionCallBack(PSelectFlightType pSelectFlightType) { m_pSelectFlightType = pSelectFlightType;	}
	void SetFlightTypeFuncSelectionCallBack(PFuncSelectFlightType pSelectFlightType) { m_pFuncSelectFlightType = pSelectFlightType;	}
	void SetProbDistEntrySelectionCallBack(PSelectProbDistEntry pSelectProbDistEntry) { m_pSelectProbDistEntry = pSelectProbDistEntry;	}
	void SetNewFlightTypeSelectionCallBack(PSelectNewFlightType pSelectNewFllightType){ m_pSelectNewFlightType = pSelectNewFllightType; }
	void SetACCategoryList(CACCATEGORYLIST* pvACCategoryList);
	void SetSubAirlineList(CSUBAIRLINELIST* pvSubAirlineList);
	void SetIntersectionsName(std::vector<CString> vIntersectionsNameList);
	void SetSegmentName(std::vector<CString>& vSegName);
	void SetmapRunwayExit(const std::map<CString,int>& mapRunwayExit);
	void SetFlightArrivalInitList(std::vector<FlightArrivalInit>& vArrivalInitPara);

	//Set Units Pointer
	void SetUnitsManager(CUnitsManager * pGlobalUnits){m_pGlobalUnits = pGlobalUnits;}
	void VehiclePoolsAndDeployment(int nProjID);
	void VehicleRoute(int nProjID);
	void SectorFlightMixSpec(int nPrjID);

	void FlightTypeDimensions(int nProjID,CAirportDatabase* pAirportDB);
	void FlightTypeRelativeServiceLocations(int nProjID,CAirportDatabase* pAirportDB);
	void DepartureSlotSpecification(int nProjID,CAirportDatabase* pAirportDB);

	static bool TakeoffPositionSelect(int nProjID, CString& sTakeoffPosition,CAirportDatabase* pAirportDB);

	void WaypointImport(int nProjID);

	void DirectionalityConstraints(int nProjID, int nAirportID);

	void TaxiSpeedConstraints(int nProjID,CAirportDatabase *pAirPortDB);

	void IntersectionNodeOption(int nAirportID);

    //set controller intervention
	void setControllerIntervention(int nProjID,CWnd *pWnd) ;

	//training flight specification
	void TrainingFlgihtSpecification(int nProjID,InputTerminal* pInTerm,InputAirside* pInputAirside);

	void TrainingFlgihtSManagement(CAirportDatabase *pAirPortdb,int nProjID,CWnd *pWnd);


	 void DoDeicingAndAnticings_demand(InputTerminal* inputterminal , CAirportDatabase *pAirPortdb, CWnd *pWnd) ;

	void WaveCrossRunwaySpecification(int nProjID, int nAirportID);
	  void DoAircraftSurfaceCondition(CAirportDatabase *pAirPortdb, CWnd *pWnd) ;
	   void DoWeatherScript( CWnd *pWnd) ;
    void DoModelPushBackAndTowOperationSpecification(int nProjID,CAirportDatabase* pAirportDB,CWnd *pWnd  ) ;
private:
//	AirsideInput*		m_pAirsideInput;
	CACCATEGORYLIST*	m_pvACCategoryList;
	CSUBAIRLINELIST*    m_pvSubAirlineList;
	BaseProcessorList*	m_pProcessorList;
	PSelectFlightType	m_pSelectFlightType;   // from now, mostly use m_pFuncSelectFlightType
	PFuncSelectFlightType m_pFuncSelectFlightType;
	PSelectProbDistEntry    m_pSelectProbDistEntry;
	InboundRouteAssignmentDlg *m_pInboundRouteAssignmentDlg;
	CDlgOutboundRouteAssignment2 *m_pOutboundRouteAssignmentDlg;
	CDeiceRouteAssigmentDlg* m_pDeiceRouteAssignmentDlg;
	CPostDeicingRouteAssignmentDlg* m_pPostDeiceRouteDlg;
	CDlgTowingRoute *m_pTowingRouteDlg;
	CDlgDirectionalityConstraints *m_pDirectionalityConstraintDlg;
	CDlgTaxiwayFlightTypeRestriction *m_pTaxiwayFlightTypeRestrictionDlg;
	CDlgTaxiwayConstraint *m_pTaxiwayConstraintDlg;
	CDlgTemporaryParkingCriteria *m_pTemporaryParkingCriteriaDlg;
	CDlgGroundRoutes*    m_pGroundRoutesDlg;
	CDlgTakeoffQueues* m_pTakeoffQueuesDlg;
	CDlgTakeoffSequence * m_pTakeoffSequenceDlg ;
	CDlgTaxiSpeedConstraints* m_pTaxiSpeedConstraintsDlg;
	std::vector<CString> m_vIntersectionsNameList;
	std::vector<CString> m_vSegmentName;
	std::map<CString,int> m_mapRunwayExit;
	std::vector<FlightArrivalInit> m_vArrivalInitPara;
	InputAirside*		m_pInputAirside;

	CUnitsManager * m_pGlobalUnits;
	PSelectNewFlightType m_pSelectNewFlightType;
};

} // namespace AirsideGUI

#endif
