#pragma once
#include "AirspaceResource.h"
#include "..\..\InputAirside\FlightPlan.h"

class FlightPlanGenericPhaseInSim
{
public:
	FlightPlanGenericPhaseInSim(ns_FlightPlan::FlightPlanGenericPhase phase);
	FlightPlanGenericPhaseInSim();
	~FlightPlanGenericPhaseInSim();

	double GetDistanceToRunway()const;
	void  SetDistanceToRunway(double dDist);

	double GetUpAltitude()const;
	void  SetUpAltitude(double dDist);

	double GetDownAltitude()const;
	void SetDownAltitude(double dDist);

	double GetAltitudeSeparate()const;
	void SetAltitudeSeparate(double dDist);

	double GetUpSpeed()const;
	void SetUpSpeed(double dDist);

	double GetDownSpeed()const;
	void SetDownSpeed(double dDist);

	ns_FlightPlan::FlightPlanGenericPhase::Phase GetPhase()const;

protected:
	ns_FlightPlan::FlightPlanGenericPhase m_phase;


protected:
	void Initialize();

private:
	double		m_dDistToRunway;
	double		m_dUpAltitude;
	double		m_dDownAltitude;
	double		m_dAltitudeSep;
	double		m_dUpSpeed;
	double		m_dDownSpeed;

};

class FlightPlanGenericInSim
{
	friend class FlightPlanGenericRouteInSim;
public:
	FlightPlanGenericInSim(ns_FlightPlan::FlightPlanGeneric *pGenericPlan);
	~FlightPlanGenericInSim();

protected:
	void Initialize();

	static bool CompareItem(const FlightPlanGenericPhaseInSim& phase1, const FlightPlanGenericPhaseInSim& phase2);

public:

	ElapsedTime GetFlightTimeInSeg(AirsideFlightInSim* pFlight, double dDistFrom, double dDistTo);
	ElapsedTime GetFlightMaxTimeInSeg(AirsideFlightInSim* pFlight, double dDistFrom, double dDistTo);
	ElapsedTime GetFlightMinTimeInSeg(AirsideFlightInSim* pFlight, double dDistFrom, double dDistTo);

	//STAR
	ElapsedTime CalculateLastSegToRunway(AirsideFlightInSim *pFlight,double dLandingSpeed ,double dDistanceToRunway);
	ElapsedTime CalculateMinTimeLastSegToRunway(AirsideFlightInSim *pFlight,double dLandingSpeed ,double dDistanceToRunway);

	//SID
	ElapsedTime CalculateTakeoffTimeFromRwyToFirstSeg(AirsideFlightInSim *pFlight, double dClimboutSpeed, double dDistanceTravel);



	bool GetSTARPhase( double dDistanceToRunway, FlightPlanGenericPhaseInSim& phase) const;
	bool GetSIDPhase( double dDistanceToRunway, FlightPlanGenericPhaseInSim& phase) const;

	std::vector<FlightPlanGenericPhaseInSim> getConcernPhase(double dDistFrom, double dDistanceTo) const;
protected:
	double getSpeed(FlightPlanGenericPhaseInSim& phase) const;//average speed
	double getMinSpeed(FlightPlanGenericPhaseInSim& phase) const;
	double getMaxSpeed(FlightPlanGenericPhaseInSim& phase) const;


	
private:
	ns_FlightPlan::FlightPlanGeneric *m_pGenericPlan;

	std::vector<FlightPlanGenericPhaseInSim> m_vPhases;


};

class FlightPlanGenericRouteInSim :
	public FlightRouteInSim
{
public:
	FlightPlanGenericRouteInSim(ns_FlightPlan::FlightPlanGeneric *pGenericPlan);
	~FlightPlanGenericRouteInSim(void);



public:
	virtual ns_FlightPlan::FlightPlanBase::ProfileType getType();

public:
	virtual ElapsedTime GetFlightTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);
	virtual ElapsedTime GetFlightMaxTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);
	virtual ElapsedTime GetFlightMinTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);


	//STAR
	virtual ElapsedTime CalculateTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight);
	virtual ElapsedTime CalculateMinTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight);

	//SID
	virtual ElapsedTime CalculateTakeoffTimeFromRwyToFirstSeg(AirsideFlightInSim *pFlight);

protected:

	//calculate the distance from runway
	void InitializeSegments(AirsideFlightInSim *pFlight);

private:
	
	//check the conflicts between flight performance and flight plan
	void CheckFlightPlan(AirsideFlightInSim *pFlight, bool bArrival);

	void InitializeSTAR(AirsideFlightInSim *pFlight);
	void InitializeSID(AirsideFlightInSim *pFlight);

	double GetSpeed(AirsideFlightInSim *pFlight, const FlightPlanGenericPhaseInSim& phase) const;
	
	//Calculate  the distance
	FlightPlanPropertyInSim& getPropInSim(AirRoutePointInSim *pPoint);
protected:
	FlightPlanGenericInSim *m_pGenericPlan;

	AirsideMobileElementMode getPhaseMode(const FlightPlanGenericPhaseInSim& phase, bool bArrival) const;
	void UpdatePointProperties( AirRoutePointInSim *pPoint,FlightPlanGenericPhaseInSim& phase ,bool bArrival);

	bool m_bInitialzied;

};
