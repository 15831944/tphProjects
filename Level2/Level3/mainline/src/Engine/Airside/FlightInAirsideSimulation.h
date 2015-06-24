#pragma once
#include "common\elaptime.h"
#include "../EngineDll.h"

#include <vector>
#include "../../Common/point.h"
#include "../../Common/ref_ptr.h"
#include "../../Common/ARCVector.h"
#include "../../Inputs/flight.h"
#include "../../Common/Referenced.h"

#include "TrafficController.h"
#include "FlightPerformanceManager.h"
#include "AirsideResource.h"
#include "./CommonInSim.h"
#include "PlanGroundRoute.h"

class AircraftSeparationManager;

class RunwayPortInSim;



//flight state
class FlightState
{	
public:

	Point m_pPosition;
	ElapsedTime m_tTime;
	double m_vSpeed;
	ARCVector3 m_vDirection;
	AirsideResource::RefPtr m_pResource;
	DistanceUnit m_dist;                     // already walked dist in the resource
	AirsideMobileElementMode m_fltMode;

	FlightState();
	FlightState(const Clearance * pClearance);
	Point GetPosition()const;
};

//class Clearance : public Referenced
//{
//public:
//	typedef ref_ptr<Clearance> RefPtr;
//	
//	ElapsedTime m_tTime;            // expected start time;	
//	DistanceUnit m_dDist;           // the dist in resource
//	AirsideResource * m_pResource;  // Resource
//	double m_vSpeed;                // expected speed
//	double m_dAlt;                  //expected altitude;
//	//ARCVector3 m_vDir;            // expected direction
//	FlightMode m_nMode;
//	Clearance();
//	Clearance(const FlightState& fltstate);
//    
//};




class OutputAirside;
class AirsidePlaner;
class AirsideTrafficController;
class InputTerminal;
class StandInSim;
class FlightPlanningFlowInSim;
class PlanGroundRoute;

typedef std::vector<FlightState> FlightStateList;


class ENGINE_TRANSFER FlightInAirsideSimulation 
{

public:
	explicit FlightInAirsideSimulation(Flight* fltschedule);
	~FlightInAirsideSimulation(void);


	Flight *GetCFlight()const{ return m_pflight;} 
	
	ElapsedTime getFlightBirthTime();


	void SetOutput(OutputAirside* pOutput);
	OutputAirside * GetOutput()const;
	void WriteLog(const FlightState& fltstat)const;
	OutputAirside* m_pOutput;

	int IsTransfer()const{ return GetCFlight()->isFlightMode('T'); }
	int IsArrival()const{ return GetCFlight()->isFlightMode('A'); }
	int IsDeparture()const{ return GetCFlight()->isFlightMode('D');  }
	int IsThroughOut()const{ return false; }


	void Move(const FlightState& fltState);
	void RequestNextClearance();

	//perform movements from current state to the next clearance ,add movement events to event list
	ElapsedTime PerformMovementsToClearance(Clearance * pClearance);
		
	

	bool IsOnTaxiing()const;

	FlightPerformanceManager * getFlightPerformanceManager(){return m_pFlightPerformanceManager;}
	void SetFlightPerformanceManager(FlightPerformanceManager *pMan){ m_pFlightPerformanceManager = pMan;	}



	FlightState& GetFlightState(){ return m_CurrentState; }
	AirsideMobileElementMode GetFlightMode()const{ return m_CurrentState.m_fltMode; }
	AirsideResource * GetCurrentResource(){ return m_CurrentState.m_pResource.get(); } 
	Point GetCurrentPosition()const{ return m_CurrentState.m_pPosition; }
	ElapsedTime GetCurrentTime()const{ return m_CurrentState.m_tTime;	}
	DistanceUnit GetDistInResource()const{ return m_CurrentState.m_dist; }


	FlightPlanningFlowInSim& GetPlanningFlow(){ return m_planflow ;}
	PlanGroundRoute& GetCurrentRoute();
	RunwayExitInSim * GetRunwayExit(){ return m_pRunwayExit; }
	void SetRunwayExit(RunwayExitInSim * pRunwayExit){ m_pRunwayExit = pRunwayExit; }
 
protected:          //state machine of this flight
	FlightState m_CurrentState;	
	FlightPlanningFlowInSim m_planflow;
	RunwayExitInSim * m_pRunwayExit;

private:
	
	Flight* m_pflight;	

	FlightPerformanceManager* m_pFlightPerformanceManager;
public:	

};
