#pragma once


#include "AirsideResourceManager.h"
#include "FlightPerformanceManager.h"
#include "AirTrafficController.h"
#include "AirsideSimConfig.h"
#include "../../InputAirside/ArrivalETAOffsetList.h"


class EventList;

class OutputAirside;
class InputTerminal;
class AirsideSimConfig;
class AirsideVehicleInSim;
class VehicleRequestDispatcher;
class CFlightServiceManagerInSim;
class LaneFltGroundRouteIntersectionInSim;
class GateAssignmentConstraintList;
class CVehicleSpecifications;
class CVehicleSpecificationItem;
class CStand2GateConstraint;
class FltOperatingDoorSpecInSim;
class AircraftClassificationManager;
class CARCportEngine;
class CTrainingFlightsManage;

class ENGINE_TRANSFER AirsideSimulation
{
public:
	AirsideSimulation(int nPrjID, CARCportEngine *_pEngine,const CString& strProjPath,FltOperatingDoorSpecInSim* pFltOperatingDoorspec);
	AirsideSimulation() {};
	~AirsideSimulation(void);
	
	bool Initialize(int nPrjID,const AirsideSimConfig& simconf);

	int AirsideEventListInitialize(EventList* _eventlist);

	static void WriteStandPlanLog(AirsideFlightInSim* pFlight, StandInSim* pStand, FLIGHT_PARKINGOP_TYPE eParkingOpType, ElapsedTime tStart, ElapsedTime tEnd);
	
	void FlushLog();
	
	void SetOutputAirside(OutputAirside* pOutput);
	
	int m_nPrjID;  // input air side
	OutputAirside * m_pOuput;
	//Terminal * m_pInterm;
	CARCportEngine *m_pEngine;
    
	void SetProjectID(int nProID) { m_nPrjID = nProID ;} ;
	//void SetInputTerminal(Terminal* pInterm) { m_pInterm = pInterm ;} ;
	void SetProPath(const CString& str) { m_strProjPath = str ;} ;

public:
	// engine requisite data which saved in file
	void SetGateAssignmentConstraints(GateAssignmentConstraintList* pConstraints);

public:
	std::vector<AirsideFlightInSim* >& GetAirsideFlightList() { return m_vFlights;}
     AirsideFlightInSim* GetAirsideFlight(int FlightIndex) ;
	VehicleRequestDispatcher* GetVehicleRequestDispatcher(){ return m_pVehicleRequestDispatcher;}
	CFlightServiceManagerInSim *GetFlightServiceManager(){ return m_pFlightServiceManager;}
	AirTrafficController* GetAirTrafficController(){ return &m_AirTrafficController;}
	AirsideResourceManager* GetAirsideResourceManager(){ return &m_Resource;}
	FlightPerformanceManager* GetFlightPerformanceManager(){ return &m_FlightPerformanceManager;}

	void SetSimStartTime(ElapsedTime estSimStartTime)                  {      m_estSimStartTime = estSimStartTime;        }
	void SetSimEndTime(ElapsedTime estSimEndTime)                      {      m_estSimEndTime   = estSimEndTime;          }
	ElapsedTime GetSimStartTime(void)                  { return(m_estSimStartTime);        }
	ElapsedTime GetSimEndTime(void)                      { return(m_estSimEndTime);          }

	bool AllowCyclicGroundRoute(){return m_bAllowCyclicGroundRoute;}

private:
	void VehicleGeneration(int& nNextVehicleUnqiueID, int nCount,CVehicleSpecificationItem* pVehicleSpecItem, int nPoolId, ElapsedTime tBirthTime, CVehicleSpecifications *pVehicleSpecifications);
	AirsideVehicleInSim* GenerateBaseTypeVehicle(int& nNextVehicleUnqiueID,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem, CVehicleSpecifications *pVehicleSpecifications);
	void ItinerantFlightGeneration();
	void InitAirsideFlightExtraInfomation();
	void OnboardFlightGeneration(AirsideFlightInSim * pFlightInSim);

private:
	AirsideResourceManager m_Resource;
	FlightPerformanceManager m_FlightPerformanceManager;
	AirTrafficController m_AirTrafficController;
//AirsidePlaner m_Planer; 

	//
	CTrainingFlightsManage* m_pTrainFlightsManger;
	std::vector<AirsideFlightInSim* > m_vFlights;
	std::vector<AirsideVehicleInSim*> m_vVehicles;
	bool m_bInitialized;
	AirsideSimConfig m_simConfig;
	VehicleRequestDispatcher* m_pVehicleRequestDispatcher;
	VehiclePoolsManagerInSim* m_pVehiclePoolsDeployment;

	CFlightServiceManagerInSim *m_pFlightServiceManager;
	
	// engine requisite data which saved in file
	GateAssignmentConstraintList* m_pGateAssignConstraints;
	
	//vehicle specification
	CVehicleSpecifications *m_pVehicleSpecifications;

	ElapsedTime    m_estSimStartTime;
	ElapsedTime    m_estSimEndTime;

	CString m_strProjPath;

	CStand2GateConstraint* m_pStand2gateconstraint;
	AircraftClassificationManager* m_pAircraftClassification;

	bool m_bAllowCyclicGroundRoute;//add by colin,2011/4/23
};
