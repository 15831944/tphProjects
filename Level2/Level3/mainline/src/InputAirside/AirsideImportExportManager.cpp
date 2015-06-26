#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\airsideimportexportmanager.h"
#include "../common/exeption.h"
#include "InputAirside.h"
#include "ALTAirport.h"
#include "ALTObject.h"
#include "ALTAirspace.h"
#include "Topography.h"
#include "FlightPlan.h"
#include "RunwayAssignmentStrategies.h"
#include "LandingRunwaySIDAssignmentList.h"
#include "LandingRunwayStarAssignmentList.h"
#include "LandingRunwaySIDAssignment.h"
#include "LandingRunwayStarAssignment.h"
#include "FlightUseStarList.h"
#include "GoAroundCriteriaDataProcess.h"
#include "FlightUseSIDList.h"
#include "InboundRouteAssignment.h"
#include "BoundRouteAssignment.h"
#include "RunwayExitStrategies.h"
#include "RunwayTakeOffPosition.h"
#include "TakeoffQueues.h"
#include "ConflictResolution.h"
#include "ItinerantFlight.h"
#include "SideStepSpecification.h"
#include "TakeoffSequencing.h"
#include "TemporaryParkingCriteria.h"
#include "OccupiedAssignedStandAction.h"
#include "..\Common\AirportDatabase.h"
#include "CPushBackClearanceProcessor.h"
#include "DirectRouting.h"
#include "CParkingStandBuffer.h"
#include "FlightPerformanceCruise.h"
#include "FlightPerformanceTerminal.h"
#include "ApproachLand.h"
#include "PerformLandings.h"
#include "TaxiInbound.h"
#include "TaxiOutbound.h"
#include "ApproachSeparation.h"
#include "AircraftClassifications.h"
#include "TakeoffClearances.h"
#include "PushBack.h"
#include "PerformanceTakeoffs.h"
#include "DepClimbOut.h"
#include "VehicleSpecifications.h"
#include "VehiclePoolsAndDeployment.h"
#include "VehicleRoute.h"
#include "FlightTypeDetailsManager.h"
#include "FlightServicingRequirementList.h"
#include "VehicleDispProps.h"
#include "DirectionalityConstraints.h"
#include "FlightTypeRestrictionList.h"
#include "VehicleTag.h"
#include "PaxBusParking.h"
#include "TowOffStandAssignmentDataList.h"
#include "BaggageParkingSpecData.h"
#include "DepartureSlotSepcification.h"
#include "TowingRoute.h"
#include "FiletTaxiway.h"
#include "StretchSetOption.h"
#include "../InputAirside/ARCUnitManager.h"
#include "ItinerantFlightSchedule.h"
#include "ArrivalETAOffsetList.h"
#include "SimSettingClosure.h"
#include "InTrailSeperationEx.h"
#include "HoldShortLines.h"
#include "ApproachSeparationCirteria.h"
#include "IntersectedStretch.h"
#include "AirRouteIntersection.h"
#include "GlobalDatabase.h"
#include "ProjectDatabase.h"

const static CString AirsideFileName[] =
{
	_T("import.log"),//0
	_T("export.log"),//1
	_T("project.prj"),//2
	_T("airport.apt"),//3
	_T("ground.asf"),//4
	_T("path.asf"),//5
	_T("object.asf"),//6
	_T("runwayIntersection.asf"),//7
	_T("HoldPosition.asf"),//8
	_T("RunwayExit.asf"),//9

	_T("flightplan.asf"),//10
	_T("RunwayAssignment.asf"),//11

	_T("lrwstarassign.asf"),//12
	_T("lrwsidsssign.asf"),//13
	_T("InboundRouteAssignment.asf"),//14
	_T("OutboundRouteAssignment.asf"),//15
	_T("lrwexitstrategies.asf"),//16
	_T("rwtakeoffposition.asf"),//17
	_T("aircraftclassifications.asf"),//18
	_T("goaround.asf"),//19
	_T("pushbackclearance.asf"),//20
	_T("takeoffqueues.asf"),//21
	_T("conflictresolution.asf"),//22
	_T("itinerantflights.asf"),//23
	_T("sidestepspecification.asf"),//24
	_T("parkingstandbuffer.asf"),//36
	_T("takeoffsequencing.asf"),//37
	_T("temporaryparking.asf"),//38
	_T("occupiedassignedstand.asf"),//39
	_T("directrouting.asf"),//40
	_T("flightperformancecruise.asf"),//41
	_T("flightperformanceterminal.asf"),//42
	_T("approachland.asf"),//43
	_T("performlanding.asf"),//44
	_T("taxiinbound.asf"),//45
	_T("taxioutbound.asf"),//46
	_T("approachseparation.asf"),//47
	_T("takeoffclearances.asf"),//48
	_T("pushback.asf"),//49
	_T("performancetakeoff.asf"),//50
	_T("depclimbout.asf"),//51
	_T("vehiclespecification.asf"),//52
	_T("vehiclepoolparking.asf"),//53
	_T("trafficlight.asf"),//54
	_T("tollgate.asf"),//55
	_T("stopsign.asf"),//56
	_T("yieldsign.asf"),//57
	_T("vehiclepoolsanddeployment.asf"),//58
	_T("vehicleroute.asf"),//59
	_T("FlightTypeDetailsManager.asf"),//60
	_T("servicingrequirement.asf"),//61
	_T("vechiledispprop.asf"), //62 
	_T("DirectionalityConstraints.asf"),//63
	_T("TaxiwayFliTypeRestriction.asf"),//64
	_T("slotspecification.asf"),//65
	_T("vehicletags.asf"), //66
	_T("paxbusparking.asf"), //67
	_T("towoffstandassignment.asf"), //68
	_T("bagcartparking.asf"), //69
	_T("towingroute.asf"),//70
	_T("fillettaxiway.asf"),//71
	_T("stretchoption.asf"),//72
	_T("unitmanagersetting.asf"),//73
	_T("ItinerantFlightSchedule.asf"),//74
	_T("ArrivalETAOffset.asf"),//75
	_T("AirsideSimulationClosure.asf"),//76
	_T("HoldShortLines.asf"),//77
	_T("NewApproachSeparationCriteria.asf"),//78
	_T("InTrailSeparation.asf"),//79
	_T("intersectedstretch.asf"),//80
	_T("AirrouteIntersection.asf"),//80
};

//////////////////////////////////////////////////////////////////////////
AirsideImportDBIndexMap::AirsideImportDBIndexMap()
{

}
AirsideImportDBIndexMap::~AirsideImportDBIndexMap()
{

}

void AirsideImportDBIndexMap::AddIndexMap(int nOldIndex,int nNewIndex)
{
	m_indexMap[nOldIndex] = nNewIndex;
}

int AirsideImportDBIndexMap::GetNewIndex(int nOldIndex)
{
	DBINDEXMAP::iterator iter =  m_indexMap.find(nOldIndex);
	
	if (iter != m_indexMap.end())
	{
		return iter->second;
	}

	return ERRORINDEX;
}
//////////////////////////////////////////////////////////////////////////
//class CAirsideFile

CAirsideFile::CAirsideFile(CAirsideImportExportManager* pManager, const CString& strDataFile,const CString& strLogFile,int p_op,int nVersion )
:m_strLogFilePath(strLogFile)
,m_nVersion(nVersion)
,m_fileType(p_op)
,m_pManager(pManager)
{
	m_fileData.openAirsideFile(strDataFile,p_op,nVersion);
}
CAirsideFile::~CAirsideFile()
{

}
void CAirsideFile::LogInfo(const CString& strInfo)
{
	try
	{
		m_fileLog.openAirsideFile(m_strLogFilePath,APPEND,m_nVersion);
		m_fileLog.writeField(_T("\n"));
		m_fileLog.writeField(strInfo);
		m_fileLog.closeIn();
	}
	catch (...) {
	}
}
//////////////////////////////////////////////////////////////////////////
//class CAirsideImportFile
CAirsideImportFile::CAirsideImportFile(CAirsideImportExportManager* pManager, const CString& strDataFile, const CString& strLogFile, int nVersion )
:CAirsideFile(pManager,strDataFile,strLogFile,READ,nVersion)
{
	
}

CAirsideImportFile::~CAirsideImportFile()
{

}
int CAirsideImportFile::getVersion()
{ 
	if(m_pManager )
		return m_pManager->getVersion();

	return -1;
};

void CAirsideImportFile::AddAircraftClassificationsIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddAircraftClassificationsIndexMap(nOldIndex,nNewIndex);
	}
}

int CAirsideImportFile::GetAircraftClassificationsNewID(int nOldObjectIndex)
{
	if (m_pManager != NULL)
	{
		return m_pManager->GetAircraftClassificationsNewID(nOldObjectIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::AddObjectIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddObjectIndexMap(nOldIndex,nNewIndex);
	}
}
void CAirsideImportFile::AddPathIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddPathIndexMap(nOldIndex,nNewIndex);
	}

}
int CAirsideImportFile::GetObjectNewID(int nOldObjectIndex)
{
	if (m_pManager != NULL)
	{
		return m_pManager->GetObjectNewID(nOldObjectIndex);
	}

	return ERRORINDEX;
}


int CAirsideImportFile::GetPathNewID(int nOldPathIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetPathNewID(nOldPathIndex);
	}

	return ERRORINDEX;
}
void CAirsideImportFile::AddAirportIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddAirportIndexMap(nOldIndex,nNewIndex);
	}
}
int CAirsideImportFile::GetAirportNewIndex(int nOldIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetAirportNewIndex(nOldIndex);
	}

	return ERRORINDEX;
}
void CAirsideImportFile::AddIntersecctionIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddIntersecctionIndexMap(nOldIndex,nNewIndex);
	}
}
int CAirsideImportFile::GetIntersectionIndexMap(int nOldIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetIntersectionIndexMap(nOldIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::AddRunwayExitIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddRunwayExitIndexMap(nOldIndex,nNewIndex);
	}
}

int CAirsideImportFile::GetRunwayExitIndexMap(int nOldIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetRunwayExitIndexMap(nOldIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::AddTemporaryParkingIndexMap(int nOldIndex, int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddTemporaryParkingIndexMap(nOldIndex,nNewIndex);
	}
}

int CAirsideImportFile::GetTemporaryParkingIndexMap(int nOldIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetTemporaryParkingIndexMap(nOldIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::SetNewProjectID(int nProjID)
{
	if (m_pManager != NULL)
	{
		m_pManager->SetNewProjectID(nProjID);
	}
}
int CAirsideImportFile::getNewProjectID()
{
	if (m_pManager)
	{
		return m_pManager->getNewProjectID();
	}

	return ERRORINDEX;

}
void CAirsideImportFile::AddAirRouteIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddAirRouteIndexMap(nOldIndex,nNewIndex);
	}
}
int CAirsideImportFile::GetAirRouteNewIndex(int nOldIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetAirRouteNewIndex(nOldIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::AddAirRouteARWaypointIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddAirRouteARWaypointIndexMap(nOldIndex,nNewIndex);
	}
}
int CAirsideImportFile::GetAirRouteARWaypointNewIndex(int nOldIndex)
{
	if (m_pManager)
	{
		return m_pManager->GetAirRouteARWaypointNewIndex(nOldIndex);
	}

	return ERRORINDEX;
}

int CAirsideImportFile::getNewAirspaceID()
{
	return getNewProjectID();
}
int CAirsideImportFile::getNewTopographyID()
{
	return getNewProjectID();
}

void CAirsideImportFile::AddTakeoffpositionRwmkdataIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddTakeoffpositionRwmkdataIndexMap(nOldIndex,nNewIndex);
	}
}
int CAirsideImportFile::GetTakeoffpositionRwmkdataNewID(int nOldIndex)
{
	if (m_pManager != NULL)
	{
		return m_pManager->GetTakeoffpositionRwmkdataNewID(nOldIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::AddVehicleDataSpecificationIndexMap(int nOldIndex,int nNewIndex)
{
	if (m_pManager != NULL)
	{
		m_pManager->AddVehicleDataSpecificationIndexMap(nOldIndex,nNewIndex);
	}
}
int CAirsideImportFile::GetVehicleDataSpecificationNewID(int nOldIndex)
{
	if (m_pManager != NULL)
	{
		return m_pManager->GetVehicleDataSpecificationNewID(nOldIndex);
	}

	return ERRORINDEX;
}

void CAirsideImportFile::AddIntersectItemIndexMap( int nOldIndex, int nNewIndex )
{
	if( m_pManager!= NULL)
	{
		m_pManager->AddIntersectItemIndexMap(nOldIndex,nNewIndex);
	}

}

int CAirsideImportFile::GetIntersectItemNewID( int nOldIndex )
{
	if( m_pManager!= NULL)
	{
		return m_pManager->GetIntersectItemNewID(nOldIndex);
	}
	return ERRORINDEX;
}

void CAirsideImportFile::AddStandLeadInLineItemIndexMap( int nOldIndex, int nNewIndex )
{
	if( m_pManager ){
		m_pManager->AddLeadInLineIndexMap(nOldIndex, nNewIndex);
	}	
}

int CAirsideImportFile::GetStandLeadInLineNewID( int nOldIndex )
{
	if( m_pManager )
	{
		return m_pManager->GetLeadInLineNewID(nOldIndex);
	}
	return ERRORINDEX;
}

void CAirsideImportFile::AddStandLeadOutLineItemIndexMap( int nOldIndex, int nNewIndex )
{
	if( m_pManager )
	{
		m_pManager->AddLeadOutLineIndexMap(nOldIndex, nNewIndex);
	}
}

int CAirsideImportFile::GetStandLeadOutLineNewID( int nOldIndex )
{
	if(m_pManager)
	{
		return m_pManager->GetLeadOutLineNewID(nOldIndex);
	}
	return ERRORINDEX;
}
//////////////////////////////////////////////////////////////////////////
//CAirsideExportFile
CAirsideExportFile::CAirsideExportFile(	CAirsideImportExportManager* pManager,const CString& strDataFile,const CString& strLogFile,int nVersion)
:CAirsideFile(pManager,strDataFile,strLogFile,WRITE,nVersion)
{

}

CAirsideExportFile::~CAirsideExportFile()
{

}
void CAirsideExportFile::AddAirportID(int nAirportID)
{ 
	if (m_pManager)
	{
		m_pManager->AddAirportID(nAirportID);
	}
}
std::vector<int>& CAirsideExportFile::GetAirportIDList()
{
	ASSERT(m_pManager != NULL);
	return m_pManager->GetAirportIDList();
}
int CAirsideExportFile::GetAirspaceID()
{
	ASSERT(m_pManager != NULL);
	return m_pManager->getNewProjectID();
}
int CAirsideExportFile::GetTopographyID()
{
	ASSERT(m_pManager != NULL);
	return m_pManager->getNewProjectID();
}
int CAirsideExportFile::GetProjectID()
{
	ASSERT(m_pManager != NULL);
	return m_pManager->getNewProjectID();
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CAirsideImportExportManager
CAirsideImportExportManager::CAirsideImportExportManager(const CString& strAirsideDir,const CString& strProjName)
:m_nVersion(1001)
,m_strAirsideDir(strAirsideDir)
,m_strProjName(strProjName)
,m_nProjectID(-1)
{
}

CAirsideImportExportManager::~CAirsideImportExportManager(void)
{
}
void CAirsideImportExportManager::AddObjectIndexMap(int nOldIndex,int nNewIndex)
{
	m_objectIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetObjectNewID(int nOldObjectIndex)
{
	return m_objectIndexMap.GetNewIndex(nOldObjectIndex);
}

void CAirsideImportExportManager::AddAircraftClassificationsIndexMap(int nOldIndex,int nNewIndex)
{
	m_aircraftclassificationsIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}

int CAirsideImportExportManager::GetAircraftClassificationsNewID(int nOldObjectIndex)
{
	return m_aircraftclassificationsIndexMap.GetNewIndex(nOldObjectIndex);
}

void CAirsideImportExportManager::AddPathIndexMap(int nOldIndex,int nNewIndex)
{
	m_pathIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetPathNewID(int nOldPathIndex)
{
	return m_pathIndexMap.GetNewIndex(nOldPathIndex);
}
void CAirsideImportExportManager::AddAirportIndexMap(int nOldIndex,int nNewIndex)
{
	m_airportIndexmap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetAirportNewIndex(int nOldIndex)
{
	return m_airportIndexmap.GetNewIndex(nOldIndex);
}
void CAirsideImportExportManager::SetNewProjectID(int nProjID)
{
	m_nProjectID = nProjID;
}
void CAirsideImportExportManager::AddAirRouteIndexMap(int nOldIndex,int nNewIndex)
{
	m_airRouteIndexmap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetAirRouteNewIndex(int nOldIndex)
{
	return m_airRouteIndexmap.GetNewIndex(nOldIndex);
}

void CAirsideImportExportManager::AddAirRouteARWaypointIndexMap(int nOldIndex,int nNewIndex)
{
	m_airRouteARWaypointIndexmap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetAirRouteARWaypointNewIndex(int nOldIndex)
{
	return m_airRouteARWaypointIndexmap.GetNewIndex(nOldIndex);
}
void CAirsideImportExportManager::AddIntersecctionIndexMap(int nOldIndex,int nNewIndex)
{
	m_intersectionNodeIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetIntersectionIndexMap(int nOldIndex)
{
	return m_intersectionNodeIndexMap.GetNewIndex(nOldIndex);
}

void CAirsideImportExportManager::AddRunwayExitIndexMap(int nOldIndex,int nNewIndex)
{
	m_runwayExitIndexMap.AddIndexMap(nOldIndex, nNewIndex);
}

int CAirsideImportExportManager::GetRunwayExitIndexMap(int nOldIndex)
{
	return m_runwayExitIndexMap.GetNewIndex(nOldIndex);
}

void CAirsideImportExportManager::AddTemporaryParkingIndexMap(int nOldIndex, int nNewIndex)
{
	m_temporaryParkingIndexMap.AddIndexMap(nOldIndex, nNewIndex);
}

int CAirsideImportExportManager::GetTemporaryParkingIndexMap(int nOldIndex)
{
	return m_temporaryParkingIndexMap.GetNewIndex(nOldIndex);
}

int CAirsideImportExportManager::getNewProjectID()
{
	return m_nProjectID;
}
void CAirsideImportExportManager::AddAirportID(int nAirportID)
{ 
	m_vAirportID.push_back(nAirportID);
}
std::vector<int>& CAirsideImportExportManager::GetAirportIDList()
{
	return m_vAirportID;
}

int CAirsideImportExportManager::getVersion()
{
	return m_nVersion;
}
CString CAirsideImportExportManager::GetFilePath(enumAirsideFile enumFile)
{
	ASSERT(m_strAirsideDir.GetLength() >0);
	ASSERT(enumFile >= IM_LOGFILE && enumFile < AS_FILECOUNT);

	return m_strAirsideDir+AirsideFileName[enumFile];
}
void CAirsideImportExportManager::AddTakeoffpositionRwmkdataIndexMap(int nOldIndex,int nNewIndex)
{
	m_takeoffpositionRwmkdataIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetTakeoffpositionRwmkdataNewID(int nOldIndex)
{
	return m_takeoffpositionRwmkdataIndexMap.GetNewIndex(nOldIndex);
}
void CAirsideImportExportManager::AddVehicleDataSpecificationIndexMap(int nOldIndex,int nNewIndex)
{
	m_vehicleDataSpecificationIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}
int CAirsideImportExportManager::GetVehicleDataSpecificationNewID(int nOldIndex)
{
	return m_vehicleDataSpecificationIndexMap.GetNewIndex(nOldIndex);
}

static void UpdateVersion1006(CAirsideImportExportManager* pMan , int m_nVersion ,CAirportDatabase* m_pAirportDB)
{
	//star assignment
	CAirsideImportFile importFileStarAssign(pMan,pMan->GetFilePath(AS_LRWSTARASSIGNMENT),"",m_nVersion);
	CLandingRunwayStarAssignmentList::ImportLandingRunwayStarAssignment(importFileStarAssign);

	CAirsideImportFile importFileSIDAssign(pMan,pMan->GetFilePath(AS_LRWSIDASSIGNMEN),"",m_nVersion);
	CLandingRunwaySIDAssignmentList::ImportLandingRunwaySIDAssignment(importFileSIDAssign);

	//import inboundrouteassignment
	CAirsideImportFile importFileInboundrouteAssign(pMan,pMan->GetFilePath(AS_LRWINBOUNDROUTEASSIGNMEN),"",m_nVersion);
	CInboundRouteAssignment::ImportInboundRouteAssignment(importFileInboundrouteAssign);

	//import outboundrouteassignment
	//CAirsideImportFile importFileOutboundrouteAssign(this,GetFilePath(AS_LRWOUTBOUNDROUTEASSIGNMEN),"",m_nVersion);
	//COutboundRouteAssignment::ImportOutboundRouteAssignment(importFileOutboundrouteAssign);

	//import RunwayExitStrategies
	CAirsideImportFile importFileRunwayExitStrategies(pMan,pMan->GetFilePath(AS_LRWEXITSTRATEGIES),"",m_nVersion);
	RunwayExitStrategies::ImportRunwayExitStrategies(importFileRunwayExitStrategies);

	//import RunwayTakeOffPosition
	CAirsideImportFile importFileRunwayTakeOffPosition(pMan,pMan->GetFilePath(AS_RWTAKEOFFPOSITION),"",m_nVersion);
	CRunwayTakeOffPosition::ImportRunwayTakeOffPosition(importFileRunwayTakeOffPosition);

	//import AircraftClassifications
	CAirsideImportFile importAircraftClassifications(pMan,pMan->GetFilePath(AS_AIRCRAFTCLASSIFICATIONS),"",m_nVersion);
	AircraftClassifications::ImportData(importAircraftClassifications); 

	//import GoAroundCriteriaData
	CAirsideImportFile importGoAroundCriteriaData(pMan,pMan->GetFilePath(AS_GOAROUND),"",m_nVersion);
	CGoAroundCriteriaDataProcess::ImportGoAroundCriteriaData(importGoAroundCriteriaData);

	//Export PushBackClearance
	CAirsideImportFile importPushBackClearance(pMan,pMan->GetFilePath(AS_PUSHBACKCLEARANCE),"",m_nVersion);
	CPushBackClearanceProcessor::ImportPushBackClearance(importPushBackClearance);	

	//import TakeoffQueues
	CAirsideImportFile importFileTakeoffQueues(pMan,pMan->GetFilePath(AS_TAKEOFFQUEUES),"",m_nVersion);
	TakeoffQueues::ImportTakeoffQueues(importFileTakeoffQueues);

	//import ConflictResolution
	CAirsideImportFile importFileConflictResolution(pMan,pMan->GetFilePath(AS_CONFLICTRESOLUTION),"",m_nVersion);
	CConflictResolution::ImportConflictResolution(importFileConflictResolution);

	//import ItinerantFlights
	CAirsideImportFile importFileItinerantFlights(pMan,pMan->GetFilePath(AS_ITINERANTFLIGHTS),"",m_nVersion);
	ItinerantFlightList::ImportItinerantFlights(importFileItinerantFlights);

	//import SideStepSpecification
	CAirsideImportFile importFileSideStepSpecification(pMan,pMan->GetFilePath(AS_SIDESTEPSPECIFICATION),"",m_nVersion);
	CSideStepSpecification::ImportSideStepSpecification(importFileSideStepSpecification);

	//import TakeoffSequencing
	CAirsideImportFile importFileTakeoffSequencing(pMan,pMan->GetFilePath(AS_TAKEOFFSEQUENCING),"",m_nVersion);
	TakeoffSequencingList::ImportTakeoffSequencing(importFileTakeoffSequencing,m_pAirportDB);

	//import TemporaryParking
	CAirsideImportFile importFileTemporaryParking(pMan,pMan->GetFilePath(AS_TEMPORARYPARKING),"",m_nVersion);
	CTemporaryParkingCriteriaList::ImportTemporaryParkingCriterias(importFileTemporaryParking);

	//import OccupiedAssignedStandAction//--c
	// 		CAirsideImportFile importFileOccupiedAssignedStandAction(pMan,pMan->GetFilePath(AS_OCCUPIEDASSIGNEDSTAND),"",m_nVersion);
	// 		COccupiedAssignedStandAction::ImportOccupiedAssignedStandAction(importFileOccupiedAssignedStandAction);

	//import ParkingStandBuffer
	CAirsideImportFile importParkingStandBuffer(pMan,pMan->GetFilePath(AS_PARKINGSTANDBUFFER),"",m_nVersion);
	CParkingStandBuffer::ImportParkingStandBuffer(importParkingStandBuffer);

	//import DirectRouting
	CAirsideImportFile importFileDirectRouting(pMan,pMan->GetFilePath(AS_DIRECTROUTING),"",m_nVersion);
	CDirectRoutingList::ImportDirectRoutings(importFileDirectRouting);

	//import FlightPerformanceCruise
	CAirsideImportFile importFileFlightPerformanceCruise(pMan,pMan->GetFilePath(AS_FLIGHTPERFORMANCECRUISE),"",m_nVersion);
	ns_AirsideInput::vFlightPerformanceCruise::ImportFlightPerformanceCruises(importFileFlightPerformanceCruise);

	//import FlightPerformanceTerminal
	CAirsideImportFile importFileFlightPerformanceTerminal(pMan,pMan->GetFilePath(AS_FLIGHTPERFORMANCETERMINAL),"",m_nVersion);
	ns_AirsideInput::vFlightPerformanceTerminal::ImportFlightPerformanceTerminals(importFileFlightPerformanceTerminal);

	//import ApproachLand
	CAirsideImportFile importFileApproachLand(pMan,pMan->GetFilePath(AS_APPROACHLAND),"",m_nVersion);
	ApproachLands::ImportApproachLands(importFileApproachLand);

	//import PerformLanding
	CAirsideImportFile importFilePerformLanding(pMan,pMan->GetFilePath(AS_PERFORMLANDING),"",m_nVersion);
	PerformLandings::ImportPerformLandings(importFilePerformLanding);

	//import TaxiInbound
	CAirsideImportFile importFileTaxiInbound(pMan,pMan->GetFilePath(AS_TAXIINBOUND),"",m_nVersion);
	CTaxiInbound::ImportTaxiInbounds(importFileTaxiInbound);

	//import TaxiOutbound
	CAirsideImportFile importFileTaxiOutbound(pMan,pMan->GetFilePath(AS_TAXIOUTBOUND),"",m_nVersion);
	CTaxiOutbound::ImportTaxiOutbounds(importFileTaxiOutbound);

	//import ApproachSeparation
	CAirsideImportFile importApproachSeparation(pMan,pMan->GetFilePath(AS_APPROACHSEPARATION),"",m_nVersion);
	ApproachSeparation::ImportApproachSeparation(importApproachSeparation); 		

	//import TakeoffClearances
	CAirsideImportFile importTakeoffClearances(pMan,pMan->GetFilePath(AS_TAKEOFFCLEARANCES),"",m_nVersion);
	TakeoffClearances::ImportTakeoffClearances(importTakeoffClearances); 	

	//import Pushback
	CAirsideImportFile importFilePushback(pMan,pMan->GetFilePath(AS_PUSHBACK),"",m_nVersion);
	CPushBack::ImportPushBacks(importFilePushback);

	//import PerformanceTakeoff
	CAirsideImportFile importFilePerformanceTakeoff(pMan,pMan->GetFilePath(AS_PERFORMANCETAKEOFF),"",m_nVersion);
	PerformTakeOffs::ImportPerformTakeOffs(importFilePerformanceTakeoff);

	//import DepClimbOut
	CAirsideImportFile importFileDepClimbOut(pMan,pMan->GetFilePath(AS_DEPCLIMBOUT),"",m_nVersion);
	DepClimbOuts::ImportDepClimbOuts(importFileDepClimbOut);

	//import VehicleSpecification
	CAirsideImportFile importFileVehicleSpecification(pMan,pMan->GetFilePath(AS_VEHICLESPECIFICATION),"",m_nVersion);
	CVehicleSpecifications::ImportVehicleSpec(importFileVehicleSpecification);

	////import VehiclePoolsAndDeployment
	//CAirsideImportFile importFileVehiclePoolsAndDeployment(pMan,pMan->GetFilePath(AS_VEHICLEPOOLSANDDEPLOYMENT),"",m_nVersion);
	//CVehiclePoolsAndDeployment::ImportVehiclePoolsAndDeployment(importFileVehiclePoolsAndDeployment);

	//import VehicleRoute
	CAirsideImportFile importFileVehicleRoute(pMan,pMan->GetFilePath(AS_VEHICLEROUTE),"",m_nVersion);
	CVehicleRouteList::ImportVehicleRoutes(importFileVehicleRoute);

	//Import FlightTypeDetailsManager
	CAirsideImportFile importFileFlightTypeDetailsManager(pMan,pMan-> GetFilePath(AS_FLIGHTTYPEDETAILSMANAGER), "", m_nVersion);
	FlightTypeDetailsManager::ImportFlightTypeDetailsManager(importFileFlightTypeDetailsManager);

	//import ServicingRequirement
	CAirsideImportFile importFileServicingRequirement(pMan,pMan->GetFilePath(AS_SERVICINGREQUIREMENT),"",m_nVersion);
	CFlightServicingRequirementList::ImportFlightServiceRequirement(importFileServicingRequirement);

	//Import VehicleDispProp
	CAirsideImportFile importVehicleDispProp(pMan,pMan->GetFilePath(AS_VEHICLEDISPPROP),"",m_nVersion);
	CVehicleDispProps::ImportVehicleDispProps(importVehicleDispProp);

	//Import departureslot
	CAirsideImportFile importDeparture(pMan,pMan->GetFilePath(AS_SLOTSPECIFICATION),"",m_nVersion);
	DepartureSlotSepcifications::ImportDepartureSlotSepc(importDeparture);

	// import DirectionalityConstraintItem
	CAirsideImportFile importDirectionalityCons(pMan,pMan-> GetFilePath(AS_DIRECTIONALITYCONS), "", m_nVersion);
	DirectionalityConstraints::ImportDirectionalityConstraints(importDirectionalityCons);


	//Import TaxiwayFlightTypeRestriction
	CAirsideImportFile importTaxiwayFlightTypeRestriction(pMan,pMan->GetFilePath(AS_TAXIWAYFLITYPERESTIRCTION),"",m_nVersion);
	CFlightTypeRestrictionList::ImportTaxiwayFlightTypeRestriction(importTaxiwayFlightTypeRestriction);

	//Import VehicleTags
	CAirsideImportFile importVehicleTags(pMan,pMan->GetFilePath(AS_VEHICLETAGS),"",m_nVersion);
	CVehicleTag::ImportVehicleTag(importVehicleTags);

	//import PaxBusParking
	CAirsideImportFile importFilePaxBusParking(pMan,pMan->GetFilePath(AS_PAXBUSPARKING),"",m_nVersion);
	CPaxBusParkingList::ImportPaxBusParkings(importFilePaxBusParking);

	//import tow off stand assignment
	CAirsideImportFile importFileTowOffStandAssignment(pMan,pMan->GetFilePath(AS_TOWOFFSTANDASSIGNMENT),"",m_nVersion);
	CTowOffStandAssignmentDataList::ImportTowOffStandAssignment(importFileTowOffStandAssignment);

	//import BagCartParking
	CAirsideImportFile importFileBagCartParking(pMan,pMan->GetFilePath(AS_BAGCARTPARKING),"",m_nVersion);
	BaggageCartParkingSpecData::ImportBagCartParking(importFileBagCartParking);

	//import StretchOption
	CAirsideImportFile importStretchSetOption(pMan,pMan->GetFilePath(AS_STRETCHOPTION),"",m_nVersion);
	CStretchSetOption::ImportStretchSetOption(importStretchSetOption);

	//import TowingRoute
	CAirsideImportFile importFileTowingRoute(pMan,pMan->GetFilePath(AS_TOWINGROUTE),"",m_nVersion);
	CTowingRouteList::ImportTowingRoutes(importFileTowingRoute);

	//import UnitManager Setting
	CAirsideImportFile importUnitManagerSetting(pMan,pMan->GetFilePath(AS_UNITMANAGERSETTING),"",m_nVersion);
	CARCUnitManager::ImportARCUnit(importUnitManagerSetting);

	//import FilletTaxiway
	CAirsideImportFile importFileFillet(pMan,pMan->GetFilePath(AS_FILLETTAXIWAY),"",m_nVersion);
	FilletTaxiwaysInAirport::ImportData(importFileFillet);

	//import ItinerantFlightSchedule
	CAirsideImportFile importFileItinerantFlightSchedule(pMan,pMan->GetFilePath(AS_ITINERANTFLIGHTSCHEDULE), "", m_nVersion);
	ItinerantFlightSchedule::ImportItinerantFlightSchedule(importFileItinerantFlightSchedule);

	//import ArrivalETAOffset
	CAirsideImportFile importFileArrivalETAOffset(pMan,pMan->GetFilePath(AS_ARRIVALSETAOFFSET), "", m_nVersion);
	CArrivalETAOffsetList::ImportArrivalETAOffset(importFileArrivalETAOffset);

	//import AirsideSimSettingClosure
	CAirsideImportFile importAirsideSimSettingClosure(pMan,pMan->GetFilePath(AS_AIRSIDESIMSETTINGCLOSURE), "", m_nVersion);
	CAirsideSimSettingClosure::ImportSimSettingClosure(importAirsideSimSettingClosure);

	//import HoldShortLines
	//CAirsideImportFile importHoldShortLines(pMan,pMan->GetFilePath(AS_HOLDSHORTLINES), "", m_nVersion);
	//CHoldShortLines::ImportHoldShortLines(importHoldShortLines);

	//import New ApproachSeparationCriteria
	CAirsideImportFile importNewApproachSeparationCriteria(pMan,pMan->GetFilePath(AS_APPROACHSEPARATIONCRITERIA_NEW), "", m_nVersion);
	CApproachSeparationCriteria::ImportApproachSeparationCriteria(importNewApproachSeparationCriteria);

	//import InTrailSeparation
	CAirsideImportFile importInTrailSeparation(pMan,pMan->GetFilePath(AS_INTRAILSEPATATION),"",m_nVersion);
	CInTrailSeparationListEx::ImportInTrailSeparation(importInTrailSeparation);

	//import IntersectedStretch
	CAirsideImportFile importFileIntersectedStretch(pMan,pMan->GetFilePath(AS_FILEINTERSECTEDSTRETCH),"",m_nVersion);
	IntersectedStretchInAirport::ImportData(importFileIntersectedStretch);

	//import AirrouteIntersection
	CAirsideImportFile importAirrouteIntersection(pMan,pMan->GetFilePath(AS_AIRROUTEINTERSECTION),"",m_nVersion);
	AirRouteIntersectionList::ImportAirRouteIntersections(importAirrouteIntersection);//star assignment
	
}

void CAirsideImportExportManager::ImportAirside(CMasterDatabase *pMasterDatabase,const CString& strAppPath,CString& strOldProjName, CString& strDatabaseName,bool bExistProject)
{
	//read the version from export.log file
	
	ImportProjectInfo(strDatabaseName,strOldProjName);
	pMasterDatabase->SetImportedProjectName(strOldProjName,strDatabaseName);
	

	if (getVersion() > 1041)//new database structure
	{
		//if (bExistProject)
		//{
		//	//detach database
		//	pMasterDatabase->DeleteOldProject(m_strProjName,strAppPath);
		//	//pMasterDatabase->InsertProjectDatabaseToDB(m_strProjName,strDatabaseName);
		//}
		//else
		//{
		//	pMasterDatabase->InsertProjectDatabaseToDB(m_strProjName,strDatabaseName);
		//}
		return;
	}

	//add new database 
	CString strProjPath = strAppPath + _T("\\Project\\") + m_strProjName;
	CProjectDatabase projDatabase(strAppPath);
	projDatabase.OpenProject(pMasterDatabase,m_strProjName,strProjPath);
	
	m_nProjectID = InputAirside::GetProjectID(m_strProjName);
	ImportAirportInfo();
	ImportObjects();

	try
	{
		if (getVersion() >= 1005)//have no those files till version 1005,
	{
	/*	CAirsideImportFile importFileFlightPlan(this,GetFilePath(AS_FLIGHTPLAN),"",m_nVersion);
		ns_FlightPlan::FlightPlans::ImportFlightPlans(importFileFlightPlan);*/

		CAirsideImportFile importFileRunwayAssignment(this,GetFilePath(AS_RUNWAYASSIGNMENT),"",m_nVersion);
		RunwayAssignmentStrategies::ImportRunwayAssignment(importFileRunwayAssignment);
	}

		if (getVersion() >1006)
	{
			UpdateVersion1006(this,m_nVersion,m_pAirportDB);
	}
	}
	catch (FileOpenError *exception)
	{	
	//	projDatabase.CloseProject(pMasterDatabase,m_strProjName);
		throw exception;
		//return;
	}
	//projDatabase.CloseProject(pMasterDatabase,m_strProjName);
}

void CAirsideImportExportManager::ExportAirside(const CString& strDatabaseName,int nVersion)
{
	//get db version from the database, and write the info into export.log
	m_nVersion = nVersion;//CAirsideDatabase::GetDatabaseVersion();
	if (m_nVersion == ERRORDBVERSION)
	{
		return;
	}

	try
	{
		//export project 
		ExportProjectInfo(strDatabaseName);

		return;
		//the following operation are misc

//
//		//airport info
//		ExportAirportInfo();
//		//export air side object
//		ExportObjects();
//
//		//export flight plan
//		//CAirsideExportFile exportFileFlightPlan(this,GetFilePath(AS_FLIGHTPLAN),"",m_nVersion);
//		//ns_FlightPlan::FlightPlans::ExportFlightPlans(exportFileFlightPlan);
//
//		CAirsideExportFile exportFileRunwayAssignment(this,GetFilePath(AS_RUNWAYASSIGNMENT),"",m_nVersion);
//		RunwayAssignmentStrategies::ExportRunwayAssignment(exportFileRunwayAssignment);
//
//		CAirsideExportFile exportFilelrwStarAssign(this,GetFilePath(AS_LRWSTARASSIGNMENT),"",m_nVersion);
//		CLandingRunwayStarAssignmentList::ExportLandingRunwayStarAssignment(exportFilelrwStarAssign);
//
//
//		CAirsideExportFile exportFilelrwSidAssign(this,GetFilePath(AS_LRWSIDASSIGNMEN),"",m_nVersion);
//		CLandingRunwaySIDAssignmentList::ExportLandingRunwaySIDAssignment(exportFilelrwSidAssign);
//
//		//Export InboundRoute
//		CAirsideExportFile exportFilelrwInboundRouteAssign(this,GetFilePath(AS_LRWINBOUNDROUTEASSIGNMEN),"",m_nVersion);
//		CInboundRouteAssignment::ExportInboundRouteAssignment(exportFilelrwInboundRouteAssign);
//
//		//Export OutboundRoute
//		//CAirsideExportFile exportFilelrwOutboundRouteAssign(this,GetFilePath(AS_LRWOUTBOUNDROUTEASSIGNMEN),"",m_nVersion);
//		//CBoundRouteAssignment::ExportOutboundRouteAssignment(exportFilelrwOutboundRouteAssign);
//
//		//Export RunwayExitStrategies
//		CAirsideExportFile exportFileRunwayExitStrategies(this,GetFilePath(AS_LRWEXITSTRATEGIES),"",m_nVersion);
//		RunwayExitStrategies::ExportRunwayExitStrategies(exportFileRunwayExitStrategies);
//
//		//Export RunwayTakeOffPosition
//		CAirsideExportFile exportFileRunwayTakeOffPosition(this,GetFilePath(AS_RWTAKEOFFPOSITION),"",m_nVersion);
//		CRunwayTakeOffPosition::ExportRunwayTakeOffPosition(exportFileRunwayTakeOffPosition,m_pAirportDB);
//
//		//Export AircraftClassifications
//		CAirsideExportFile exportAircraftClassifications(this,GetFilePath(AS_AIRCRAFTCLASSIFICATIONS),"",m_nVersion);
//		AircraftClassifications::ExportData(exportAircraftClassifications);
//
//		//Export GoAroundCriteriaData
//		CAirsideExportFile exportGoAroundCriteriaData(this,GetFilePath(AS_GOAROUND),"",m_nVersion);
//		CGoAroundCriteriaDataProcess::ExportGoAroundCriteriaData(exportGoAroundCriteriaData);
//		
//		//Export PushBackClearance
//		CAirsideExportFile exportPushBackClearance(this,GetFilePath(AS_PUSHBACKCLEARANCE),"",m_nVersion);
//		CPushBackClearanceProcessor::ExportPushBackClearance(exportPushBackClearance,m_pAirportDB);	
//
//		//Export TakeoffQueues
//		CAirsideExportFile exportFileTakeoffQueues(this,GetFilePath(AS_TAKEOFFQUEUES),"",m_nVersion);
//		TakeoffQueues::ExportTakeoffQueues(exportFileTakeoffQueues);
//
//		//Export ConflictResolution
//		CAirsideExportFile exportFileConflictResolution(this,GetFilePath(AS_CONFLICTRESOLUTION),"",m_nVersion);
//		CConflictResolution::ExportConflictResolution(exportFileConflictResolution,m_pAirportDB);
//
//		//Export ItinerantFlights
//		CAirsideExportFile exportFileItinerantFlights(this,GetFilePath(AS_ITINERANTFLIGHTS),"",m_nVersion);
//		ItinerantFlightList::ExportItinerantFlights(exportFileItinerantFlights,m_pAirportDB);
//
//		//Export SideStepSpecification
//		CAirsideExportFile exportFileSideStepSpecification(this,GetFilePath(AS_SIDESTEPSPECIFICATION),"",m_nVersion);
//		CSideStepSpecification::ExportSideStepSpecification(exportFileSideStepSpecification, m_pAirportDB);
//
//		//Export TakeoffSequencing
//		CAirsideExportFile exportFileTakeoffSequencing(this,GetFilePath(AS_TAKEOFFSEQUENCING),"",m_nVersion);
//		TakeoffSequencingList::ExportTakeoffSequencing(exportFileTakeoffSequencing,m_pAirportDB);
//
//		//Export TemporaryParking
//		CAirsideExportFile exportFileTemporaryParking(this,GetFilePath(AS_TEMPORARYPARKING),"",m_nVersion);
//		CTemporaryParkingCriteriaList::ExportTemporaryParkingCriterias(exportFileTemporaryParking);
//
//		//Export OccupiedAssignedStandAction//--c
//// 		CAirsideExportFile exportFileOccupiedAssignedStandAction(this,GetFilePath(AS_OCCUPIEDASSIGNEDSTAND),"",m_nVersion);
//// 		COccupiedAssignedStandAction::ExportOccupiedAssignedStandAction(exportFileOccupiedAssignedStandAction,m_pAirportDB);
//
//		//Export ParkingStandBuffer
//		CAirsideExportFile exportParkingStandBuffer(this,GetFilePath(AS_PARKINGSTANDBUFFER),"",m_nVersion);
//		CParkingStandBuffer::ExportParkingStandBuffer(exportParkingStandBuffer,m_pAirportDB);
//
//		//Export DirectRouting
//		CAirsideExportFile exportFileDirectRouting(this,GetFilePath(AS_DIRECTROUTING),"",m_nVersion);
//		CDirectRoutingList::ExportDirectRoutings(exportFileDirectRouting);
//
//		//Export FlightPerformanceCruise
//		CAirsideExportFile exportFileFlightPerformanceCruise(this,GetFilePath(AS_FLIGHTPERFORMANCECRUISE),"",m_nVersion);
//		ns_AirsideInput::vFlightPerformanceCruise::ExportFlightPerformanceCruises(exportFileFlightPerformanceCruise,m_pAirportDB);
//
//		//Export FlightPerformanceTerminal
//		CAirsideExportFile exportFileFlightPerformanceTerminal(this,GetFilePath(AS_FLIGHTPERFORMANCETERMINAL),"",m_nVersion);
//		ns_AirsideInput::vFlightPerformanceTerminal::ExportFlightPerformanceTerminals(exportFileFlightPerformanceTerminal,m_pAirportDB);
//
//		//Export ApproachLand
//		CAirsideExportFile exportFileApproachLand(this,GetFilePath(AS_APPROACHLAND),"",m_nVersion);
//		ApproachLands::ExportApproachLands(exportFileApproachLand,m_pAirportDB);
//
//		//Export PerformLanding
//		CAirsideExportFile exportFilePerformLanding(this,GetFilePath(AS_PERFORMLANDING),"",m_nVersion);
//		PerformLandings::ExportPerformLandings(exportFilePerformLanding,m_pAirportDB);
//
//		//Export TaxiInbound
//		CAirsideExportFile exportFileTaxiInbound(this,GetFilePath(AS_TAXIINBOUND),"",m_nVersion);
//		CTaxiInbound::ExportTaxiInbounds(exportFileTaxiInbound,m_pAirportDB);
//
//		//Export TaxiOutbound
//		CAirsideExportFile exportFileTaxiOutbound(this,GetFilePath(AS_TAXIOUTBOUND),"",m_nVersion);
//		CTaxiOutbound::ExportTaxiOutbounds(exportFileTaxiOutbound,m_pAirportDB);
//
//		//Export ApproachSeparation
//		CAirsideExportFile exportApproachSeparation(this,GetFilePath(AS_APPROACHSEPARATION),"",m_nVersion);
//		ApproachSeparation::ExportApproachSeparation(exportApproachSeparation);		
//
//		//Export TakeoffClearances
//		CAirsideExportFile exportTakeoffClearances(this,GetFilePath(AS_TAKEOFFCLEARANCES),"",m_nVersion);
//		TakeoffClearances::ExportTakeoffClearances(exportTakeoffClearances);		
//
//		//Export Pushback
//		CAirsideExportFile exportFilePushback(this,GetFilePath(AS_PUSHBACK),"",m_nVersion);
//		CPushBack::ExportPushBacks(exportFilePushback,m_pAirportDB);
//
//		//Export PerformanceTakeoff
//		CAirsideExportFile exportFilePerformanceTakeoff(this,GetFilePath(AS_PERFORMANCETAKEOFF),"",m_nVersion);
//		PerformTakeOffs::ExportPerformTakeOffs(exportFilePerformanceTakeoff,m_pAirportDB);
//
//		//Export DepClimbOut
//		CAirsideExportFile exportFileDepClimbOut(this,GetFilePath(AS_DEPCLIMBOUT),"",m_nVersion);
//		DepClimbOuts::ExportDepClimbOuts(exportFileDepClimbOut,m_pAirportDB);
//
//		//Export VehicleSpecification
//		CAirsideExportFile exportFileVehicleSpecification(this,GetFilePath(AS_VEHICLESPECIFICATION),"",m_nVersion);
//		CVehicleSpecifications::ExportVehicleSpec(exportFileVehicleSpecification);
//
//		//Export VehiclePoolsAndDeployment
//		CAirsideExportFile exportFileVehiclePoolsAndDeployment(this,GetFilePath(AS_VEHICLEPOOLSANDDEPLOYMENT),"",m_nVersion);
//		CVehiclePoolsAndDeployment::ExportVehiclePoolsAndDeployment(exportFileVehiclePoolsAndDeployment,m_pAirportDB);
//
//		//Export VehicleRoute
//		CAirsideExportFile exportFileVehicleRoute(this,GetFilePath(AS_VEHICLEROUTE),"",m_nVersion);
//		CVehicleRouteList::ExportVehicleRoutes(exportFileVehicleRoute);
//
//		//Export FlightTypeDetailsManager
//		CAirsideExportFile exportFileFlightTypeDetailsManager(this, GetFilePath(AS_FLIGHTTYPEDETAILSMANAGER), "", m_nVersion);
//		FlightTypeDetailsManager::ExportFlightTypeDetailsManager(exportFileFlightTypeDetailsManager, m_pAirportDB);
//
//		//Export ServicingRequirement
//		CAirsideExportFile exportFileServicingRequirement(this,GetFilePath(AS_SERVICINGREQUIREMENT),"",m_nVersion);
//		CFlightServicingRequirementList::ExportFlightServiceRequirement(exportFileServicingRequirement, m_pAirportDB);
//
//		//Export VehicleDispProp
//		CAirsideExportFile exportVehicleDispProp(this, GetFilePath(AS_VEHICLEDISPPROP), "", m_nVersion);
//		CVehicleDispProps::ExportDataVehicleDispProps(exportVehicleDispProp);
//		
//		CAirsideExportFile exportDeparture(this, GetFilePath(AS_SLOTSPECIFICATION), "", m_nVersion);
//		DepartureSlotSepcifications::ExportDepartureSlotSepc(exportDeparture,m_pAirportDB);
//		// Export DirectionalityConstraintItem
//		CAirsideExportFile exportDirectionalityCons(this, GetFilePath(AS_DIRECTIONALITYCONS), "", m_nVersion);
//		DirectionalityConstraints::ExportDirectionalityConstraints(exportDirectionalityCons);
//
//		//Export TaxiwayFlightTypeRestriction
//		CAirsideExportFile exportTaxiwayFlightTypeRestriction(this, GetFilePath(AS_TAXIWAYFLITYPERESTIRCTION), "", m_nVersion);
//		CFlightTypeRestrictionList::ExportTaxiwayFlightTypeRestriction(exportTaxiwayFlightTypeRestriction);
//
//
//		//Export VehicleTags
//		CAirsideExportFile exportVehicleTags(this, GetFilePath(AS_VEHICLETAGS), "", m_nVersion);
//		CVehicleTag::ExportVehicleTag(exportVehicleTags);
//
//		//Export PaxBusParking
//		CAirsideExportFile exportFilePaxBusParking(this,GetFilePath(AS_PAXBUSPARKING),"",m_nVersion);
//		CPaxBusParkingList::ExportPaxBusParkings(exportFilePaxBusParking);
//
//		//export tow off stand assignment
//		CAirsideExportFile exportFileTowOffStandAssignment(this,GetFilePath(AS_TOWOFFSTANDASSIGNMENT),"",m_nVersion);
//		CTowOffStandAssignmentDataList::ExportTowOffStandAssignment(exportFileTowOffStandAssignment, m_pAirportDB);
//
//		//Export BugCartParking
//		CAirsideExportFile exportFileBugCartParking(this, GetFilePath(AS_BAGCARTPARKING),"",m_nVersion);
//		BaggageCartParkingSpecData::ExportBagCartParking(exportFileBugCartParking);
//
//		//Export TowingRoute
//		CAirsideExportFile exportFileTowingRoute(this,GetFilePath(AS_TOWINGROUTE),"",m_nVersion);
//		CTowingRouteList::ExportTowingRoutes(exportFileTowingRoute);
//
//		//Export FilletTaxiway
//		CAirsideExportFile exportFileFillets(this, GetFilePath(AS_FILLETTAXIWAY),"",m_nVersion);
//		FilletTaxiwaysInAirport::ExportData(exportFileFillets);
//
//
//		//Export StretchOption
//		CAirsideExportFile exportStretchSetOption(this,GetFilePath(AS_STRETCHOPTION),"",m_nVersion);
//		CStretchSetOption::ExportStretchSetOption(exportStretchSetOption);
//
//
//		//Export UnitManager Setting
//		CAirsideExportFile exportUnitManagerSetting(this,GetFilePath(AS_UNITMANAGERSETTING),"",m_nVersion);
//		CARCUnitManager::ExportARCUnit(exportUnitManagerSetting);
//
//		//Export ItinerantFlightSchedule
//		CAirsideExportFile exportItinerantFlightSchedule(this, GetFilePath(AS_ITINERANTFLIGHTSCHEDULE), "", m_nVersion);
//		ItinerantFlightSchedule::ExportItinerantFlightSchedule(exportItinerantFlightSchedule, m_pAirportDB);
//
//		//Export ArrivalETAOffset
//		CAirsideExportFile exportFileArrivalETAOffset(this,GetFilePath(AS_ARRIVALSETAOFFSET), "", m_nVersion);
//		CArrivalETAOffsetList::ExportArrivalETAOffset(exportFileArrivalETAOffset, m_pAirportDB);
//
//		//Export AirsideSimSettingClosure
//		CAirsideExportFile exportAirsideSimSettingClosure(this,GetFilePath(AS_AIRSIDESIMSETTINGCLOSURE), "", m_nVersion);
//		CAirsideSimSettingClosure::ExportSimSettingClosure(exportAirsideSimSettingClosure);
//
//		//Export HoldShortLines
//		//CAirsideExportFile exportHoldShortLines(this,GetFilePath(AS_HOLDSHORTLINES), "", m_nVersion);
//		//CHoldShortLines::ExportHoldShortLines(exportHoldShortLines);
//
//		//Export New ApproachSeparationCriteria
//		CAirsideExportFile exportNewApproachSeparationCriteria(this,GetFilePath(AS_APPROACHSEPARATIONCRITERIA_NEW), "", m_nVersion);
//		CApproachSeparationCriteria::ExportApproachSeparationCriteria(exportNewApproachSeparationCriteria);
//
//		//Export InTrailSeparation
//		CAirsideExportFile exportInTrailSeparation(this,GetFilePath(AS_INTRAILSEPATATION),"",m_nVersion);
//		CInTrailSeparationListEx::ExportInTrailSeparation(exportInTrailSeparation);
//
//		//Export IntersectedStretch
//		CAirsideExportFile exportFileIntersectedStretch(this,GetFilePath(AS_FILEINTERSECTEDSTRETCH),"",m_nVersion);
//		IntersectedStretchInAirport::ExportData(exportFileIntersectedStretch);
//
//		//Export AirrouteIntersection
//		CAirsideExportFile exportAirrouteIntersection(this,GetFilePath(AS_AIRROUTEINTERSECTION),"",m_nVersion);
//		AirRouteIntersectionList::ExportAirRouteIntersections(exportAirrouteIntersection);

	}
	catch (FileOpenError *exception)
	{
		delete exception;
		return;
	}

}
void CAirsideImportExportManager::ExportProjectInfo(const CString& strDatabaseName)
{
	//m_nProjectID = InputAirside::GetProjectID(m_strProjName);
	//int nDBVersion = CAirsideDatabase::GetDatabaseVersion();
	
	CAirsideExportFile exportFile(this,GetFilePath(AS_PROJECT),"",m_nVersion);
	exportFile.getFile().writeField(_T("Project ID, Project Name, Database Version"));
	exportFile.getFile().writeLine();
	exportFile.getFile().writeInt(m_nProjectID);
	exportFile.getFile().writeField(m_strProjName);
	exportFile.getFile().writeInt(m_nVersion);

	exportFile.getFile().writeField(strDatabaseName);

	exportFile.getFile().writeLine();
	exportFile.getFile().endFile();

}
void CAirsideImportExportManager::ImportProjectInfo(CString& strDatabaseName,CString& strOldProjName)
{
	CAirsideImportFile importFile(this,GetFilePath(AS_PROJECT),"",m_nVersion);
	char szChar[1024];
	//importFile.getFile().skipLine();
	importFile.getFile().getInteger(m_nOldProjectID);
	importFile.getFile().getField(szChar,1024);
	strOldProjName = CString(szChar);
	importFile.getFile().getInteger(m_nVersion);
	
	if (importFile.getVersion() > 1041)
	{
		importFile.getFile().getField(strDatabaseName.GetBuffer(1024),1024);
		return;
	}


	////check import into new project or into old project
	//if(InputAirside::IsProjectNameExist(m_strProjName))
	//{
	//	//delete project
	//	InputAirside::DeleteProject(m_strProjName);
	//}




}
void CAirsideImportExportManager::ExportAirportInfo()
{
	CAirsideExportFile exportFile(this,GetFilePath(AS_AIRPORT),"",m_nVersion);
	ALTAirport::ExportAirports(exportFile,m_nProjectID);
	exportFile.getFile().endFile();

	////airport ground data
	//CAirsideImportFile exportGroundFile(this,GetFilePath(AS_GROUND),"",m_nVersion);	
	//std::vector<int>::size_type i ;
	//for (i = 0; i<m_vAirportID.size(); ++i)
	//{
	//	CAirsideGround::ExportAirsideGround(m_vAirportID[i],exportGroundFile);
	//}
	//exportGroundFile.getFile().endFile();
}
void CAirsideImportExportManager::ImportAirportInfo()
{
	CAirsideImportFile importFile(this,GetFilePath(AS_AIRPORT),"",m_nVersion);
	ALTAirport::ImportAirports(importFile);

	////airport ground data
	//CAirsideImportFile importGroundFile(this,GetFilePath(AS_GROUND),"",m_nVersion);	
	//CAirsideGround::ImportAirsideGrounds(importGroundFile);

}
void CAirsideImportExportManager::ExportObjects()
{
	CAirsideExportFile exportFile(this,GetFilePath(AS_OBJECT),"",m_nVersion);
	
	exportFile.getFile().writeField(_T("Airside Object Type, object id, airport/airspace/topography id,..."));
	exportFile.getFile().writeLine();


	////airport data
	//std::vector<int>::size_type i ;
	//for (i = 0; i<m_vAirportID.size(); ++i)
	//{
	//	ALTAirport::ExportObjects(m_vAirportID[i],exportFile);
	//}

	//export airport misc data
	//ALTAirport::ExportMiscObjects(this);

	
	//airspace data
	//ALTAirspace::ExportObjects(exportFile);

	//topography data

	//Topography::ExportObjects(exportFile);

	exportFile.getFile().endFile();
}
void CAirsideImportExportManager::ImportObjects()
{
	//import the object data
	CAirsideImportFile importFile(this,GetFilePath(AS_OBJECT),"",m_nVersion);
	//ALTAirport::ImportObjects(importData);
	ALTObject::ImportObjects(importFile);

	ALTAirport::ImportMiscObjects(this);

}

void CAirsideImportExportManager::AddIntersectItemIndexMap( int nOldIndex, int nNewIndex )
{
	m_intersectItemIndexMap.AddIndexMap(nOldIndex,nNewIndex);
}

int CAirsideImportExportManager::GetIntersectItemNewID( int nOldIndex )
{
	return m_intersectItemIndexMap.GetNewIndex(nOldIndex);
}

void CAirsideImportExportManager::AddLeadInLineIndexMap( int nOldIndex, int nNewIndex )
{
	m_standLeadInLineIndexMap.AddIndexMap( nOldIndex, nNewIndex);
}

int CAirsideImportExportManager::GetLeadInLineNewID( int nOldIndex )
{
	return m_standLeadInLineIndexMap.GetNewIndex(nOldIndex);
}

void CAirsideImportExportManager::AddLeadOutLineIndexMap( int nOldIndex, int nNewIndex )
{
	return m_standLeadOutLineIndexMap.AddIndexMap(nOldIndex, nNewIndex);
}

int CAirsideImportExportManager::GetLeadOutLineNewID( int nOldIndex )
{
	return m_standLeadOutLineIndexMap.GetNewIndex(nOldIndex);
}
















