#pragma once
#include "MobElementBehavior.h"
#include <common/ARCVector.h>
#include "LandsideTrafficSystem.h"
#include "core/SAgent.h"
class Person;
class PaxVisitor;
class LandsideVehicleInSim;
class LandsideResourceInSim;
class CLandsideWalkTrafficObjectInSim;
class LandsideCurbSideInSim;
class LandsideBusStationInSim;
class ArrivalPaxLandsideBehavior;
class DeparturePaxLandsideBehavior;
class LandsideResidentVehicleInSim;
class LandsideSimulation;
class LandsideLayoutObjectInSim;


class LandsideBaseBehavior : public MobElementBehavior
{
	friend class VisitorLandsideBehavior;
public:	
	LandsideBaseBehavior(Person* p);
	virtual BehaviorType getBehaviorType()const{ return LandsideBehavior; }

	virtual ArrivalPaxLandsideBehavior* toArrBehavior(){ return NULL; }
	virtual DeparturePaxLandsideBehavior* toDepBehavior(){ return NULL; }


	//It writes next track to ElementTrack log.
	// in:
	// _bBackup: if the log is backwards.
	virtual void writeLog (ElapsedTime time, bool _bBackup=false, bool bOffset = true ) ;
	virtual void flushLog (ElapsedTime p_time, bool bmissflight = false) = 0;
	virtual void FlushLogforFollower( ElapsedTime _time );



	//Sets Element's destination
	virtual void setDestination (const ARCVector3& p,MobDir emWalk = FORWARD) = 0;
	
	virtual void UpdateEntryTime( const ElapsedTime& eEntryTime ) = 0;


	virtual void setLocation( const ARCVector3& _ptLocation ){ location = _ptLocation;	}
	void setState(short nState);


	int getState( void ) const;
	void GenerateEvent( ElapsedTime eventTime );
	void GenerateEvent(Person *pPerson, ElapsedTime eventTime );

	void setVehicle(LandsideVehicleInSim* pVehicle){  m_pVehicle = pVehicle; }
	LandsideVehicleInSim* getVehicle()const {return m_pVehicle;}

	ElapsedTime moveTime(void)const;
	Person* getPerson()const{ return m_pPerson; }
	void terminate(const ElapsedTime&t);

	void setResource(LandsideResourceInSim* pres){ m_pResource = pres; }
	LandsideResourceInSim* getResource()const{ return m_pResource; }

	void setCurrentVertex(const LandsideTrafficGraphVertex& landsideGraphvertex){m_landsideVertex = landsideGraphvertex;}
	const LandsideTrafficGraphVertex& getCurrentVertex()const{return m_landsideVertex;}



	//Returns current coordinates
	const ARCVector3& getPoint (void) const { return location; }

	//Returns current destination.
	const ARCVector3& getDest (void) const { return m_ptDestination; }

	int GetPersonID()const;
	//write log

	//do the thing when vehicle parking at the place
	virtual bool OnVehicleParkAtPlace(LandsideVehicleInSim* pVehicle, LandsideResourceInSim* pRes,const ElapsedTime&t){ return false; }
	virtual bool CanBoardingOrBoarding(LandsideResourceInSim* pRes){ return false; }

	virtual bool CanPaxTakeThisBus(LandsideResidentVehicleInSim  *pResidentVehicle, LandsideSimulation *pSimulation);
	virtual bool CanPaxTkeOffThisBusAtStation(LandsideBusStationInSim* pBusStation, LandsideSimulation *pSimulation);

	//check if this resource linked to first terminal processor, pRource is bus station or curbside
	virtual bool CanPaxTakeOffThisResource(LandsideResourceInSim* pResource,LandsideSimulation* pSimulation)const;
	//decide takeoff at this resource  to entry terminal after 
	virtual void ChooseTakeOffResource(LandsideLayoutObjectInSim* pLayout,LandsideSimulation* pSimulation);


protected:
	enum theVisitorLogNum
	{
		firstLog = 0,
		secondLog,
	};
	enum UseVehicleType
	{
		Use_PrivateVehicle,
		Use_PublicVehicle
	};
protected:
	Person* m_pPerson;
	//Current coordinates
	ARCVector3 location;       
	//Current destination, used for
	ARCVector3 m_ptDestination;      

	//pure location( no offset )
	ARCVector3 pure_location;

	LandsideVehicleInSim* m_pVehicle;
	LandsideResourceInSim* m_pResource;
	//CVehicleItemDetail *m_pVehicleType;
	LandsideTrafficGraphVertex m_landsideVertex;

	//LandsideCurbSideInSim *m_pAssCurbside;
	//----------------------------------------------
	//for public vehicle(bus) pax.
	//for arrival pax:the bus station pax get on bus
	//for depature pax:the bus station pax get off bus
//	LandsideBusStationInSim *m_pUseBusStation;
	//----------------------------------------------

	//UseVehicleType m_eUseVehicleType;
	
	//bool WriteVisitorEntryLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time, int ntype,bool _bBackup = false, bool bOffset  = true );

	void SetFollowerEnterLandsideLocation(const ARCVector3& _ptCurrent);
	void SetFollowerDestination(const ARCVector3& _ptCurrent, const ARCVector3& _ptDestination, float* _pRotation );
	
	void SetRelativePosition(const Point& paxDirection,const Point& paxLocation,MobDir emWalk);

	void SetEnterLandsideLocation(const ARCVector3& _ptCurrent);

public:
	//set follower Behavior
	//if passenger, include visitor
	virtual void InitializeBehavior() = 0;
	virtual bool IsGetOff(LandsideResourceInSim* pres){  return false; }
};

