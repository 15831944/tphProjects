#pragma once
#include "./CommonInSim.h"
#include "../EngineDll.h"
#include "../../InputAirside/FlightPlan.h"

//namespace ns_FlightPlan{
//	class FlightPlanSpecific;
//	class FlightPlans;
//	class FlightPlanBase;
//};

class AirsideResourceManager;
class AirsideResource;
class AirRoutePointInSim;
class AirsideFlightInSim;
class CAirportDatabase;
class FlightRouteInSim;
class LogicRunwayInSim;


//FlghtPlan In Simulation

class ENGINE_TRANSFER FlightPlanPropertyInSim
{
public:
	FlightPlanPropertyInSim( AirRoutePointInSim* pRoutePoint )
	{
		m_pRoutePoint = pRoutePoint;			
		m_bArried = false;
	}	

	void SetSpeed(double dSpd ){ m_dSpeed = dSpd; }
	void SetAlt(double dAlt){ m_dAlt = dAlt; }
	void SetMode(AirsideMobileElementMode mode){ m_fltMode = mode;}
	void SetArrival(){ m_bArried = true;}

	double GetSpeed(){ return m_dSpeed; }
	double GetAlt(){ return m_dAlt; }
	AirsideMobileElementMode GetMode(){ return m_fltMode; }
	bool IsArrival(){ return m_bArried; }
	AirRoutePointInSim* GetRoutePoint(){ return m_pRoutePoint ;}

protected:
	AirRoutePointInSim* m_pRoutePoint;
	double m_dAlt;
	double m_dSpeed;
	AirsideMobileElementMode m_fltMode;
	bool m_bArried;

};

typedef std::vector<FlightPlanPropertyInSim> FlightPlanPropertiesInSim;

class ENGINE_TRANSFER FlightPlanInSim
{
public:

	FlightPlanInSim(){ m_pFlightPlansInput = NULL;m_pResManager = NULL;}
	~FlightPlanInSim();

	void Init( int nPrjID, AirsideResourceManager* pResManger,CAirportDatabase* pAirportDB);

	bool GetLandingEnrouteFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute);
	bool GetTakeoffEnrouteFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute);
	bool GetSTARFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute,bool bRunwayFlightPlan);
	bool GetSIDFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute,bool bRunwayFlightPlan);
	//----------------------------------------------------------------------------------------------------------------------
	//Summary:
	//			Retrieve circuit flight plan
	//----------------------------------------------------------------------------------------------------------------------
	bool GetLandingCircuitFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute);
	bool GetTakeoffCircuitFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute);
	ns_FlightPlan::FlightPlans* m_pFlightPlansInput;
	AirsideResourceManager* m_pResManager;
	CAirportDatabase* m_pAirportDB;

protected:
	//return the best fit flight plan
	ns_FlightPlan::FlightPlanBase *GetBestFitFlightPlan(AirsideFlightInSim *pFlight, ns_FlightPlan::FlightPlanBase::OperationType opType) const;

	CAirRoute *GetBestAirRoute(AirsideFlightInSim* pFlight, CAirRoute::RouteType enumType, LogicRunwayInSim *pRunway) const;

};

