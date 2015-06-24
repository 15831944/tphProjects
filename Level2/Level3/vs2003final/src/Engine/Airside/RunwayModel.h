#ifndef __AIRSIDE_RUNWAYMODEL_DEF
#define  __AIRSIDE_RUNWAYMODEL_DEF

#include "../../Common/ARCVector.h"

#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "Entity.h"
#include "../SimClock.h"
#include "../../Common/point.h"


class AirsideInput;
class CAirsideInput;


NAMESPACE_AIRSIDEENGINE_BEGINE


class AirportModel;
class SimFlight;

class RunwayExit : public Entity{
public:
	double getAngle(){ return m_dAngle; }
	void setAngle(double angleIndegree){ m_dAngle  = angleIndegree; }
	
public:
	//properties
	double m_dAngle;       // exit angle in degree;
	int m_NodeId;          // id of the taxiway node;
	int m_NodeIdNext;      //  id of the next node id;
};
typedef EntContainer<RunwayExit> RwExitList;
typedef RwExitList::iterator RwExitIter;
typedef RwExitList::const_iterator RwExitIter_const;

class Runway : public Entity{
friend class RunwayModel;
public:
	typedef ref_ptr<Runway> RefPtr;	
public:
	
	Point getThresholdPoint();
	
	void setThresholdoffset(double dthreshold){ m_dThresholdoffset = dthreshold; }
	double getThresholdoffset()const { return m_dThresholdoffset; }
	Point getPhysicalEnd1(){ return m_phyEnd1;} 
	Point getPhysicalEnd2(){ return m_phyEnd2; }
	void setPhysicalEnd1(const Point& pos ){ m_phyEnd1 = pos; }
	void setPhysicalEnd2(const Point& pos ){ m_phyEnd2 = pos; }
	void setDependRunway(Runway *m_pDependrunway){ m_pdependRw = m_pDependrunway; }
	Runway* getDependedRunway(){ return m_pdependRw; }
	RwExitList& getExitList() { return m_vExits; }
protected:
	//properties
	Runway * m_pdependRw;         //the physical same runway
	Point m_phyEnd1;
	Point m_phyEnd2;
	double m_dThresholdoffset;
	RwExitList m_vExits;
	//stat
};
typedef EntContainer<Runway> runwayList;
typedef runwayList::iterator runwayIter;
typedef runwayList::const_iterator runwaIter_const;

//Contains the runways
class ENGINE_TRANSFER RunwayModel{
public:	
	RunwayModel(AirportModel * _airport){ m_pAirport = _airport; m_pAirsideInput = NULL; }
	~RunwayModel();
	runwayList& getRunwayList(){ return m_runways; }
	//build
	void Build(CAirsideInput* _inputC, AirsideInput* _inputD);
	
	//get Runway by name
	Runway * getRunway(const CString& rwname);	
	// get The touch down point by  the flight and runway
	Point getTouchDownPoint(Runway * pRunway , SimFlight * _flight);

	Point getTakeoffPoint(Runway * pRunway , SimFlight * _flight);
	// get an exit of the runway 
	RunwayExit  * getOutExit(Runway * pRunway, SimFlight * _flight);

	// get distance from threshold to touchdown
	double getDistThresToTouch(SimFlight * _flight);

	double getDistEndToTakeoff( SimFlight * _flight);

	SimClock::TimeSpanType getReadyTime(SimFlight * _flight);
	// get out exit by the Runway and the flight
	RunwayExit * getInExit(Runway * pRunway ,SimFlight * _flight);

	//get Exit speed of a specified flight
	double getExitSpeed (RunwayExit * pExit, SimFlight * _flight);

	//get landing speed
	double getLandingSpeed(SimFlight * _flight);

	//get Deceleration speed
	double getDecelSpeed(SimFlight * _flight);   // knot/s

	//
	double getTakeoffAcceleration(SimFlight *_flight);
	//
	void  DistributeLandingProcess(SimFlight* _flight);

	void  DistributeTakeoffProcess(SimFlight* _flight);

protected:
	runwayList m_runways;
	AirportModel * m_pAirport;
	AirsideInput * m_pAirsideInput;

};

NAMESPACE_AIRSIDEENGINE_END

#endif