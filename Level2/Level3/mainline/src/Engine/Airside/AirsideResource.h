#pragma once
#include "../EngineDll.h"

#include "../../Common/elaptime.h"
#include "../../InputAirside/ALTObject.h"
#include "../../Common/point2008.h"
#include "../../Common/Referenced.h"
#include "./CommonInSim.h"
#include "AirsideMobileElement.h"

//class CAirsideMobileElement;
class AirsideFlightInSim ;
class AirsideVehicleInSim;

class ENGINE_TRANSFER OccupancyInstance
{
public:

	friend class AirsideResource;

public:
    OccupancyInstance(){
		m_pMobileElement = NULL;
		m_tEntry = ElapsedTime(-1L);
		m_tExit = ElapsedTime(-1L);
		m_dSpd = 0;
		m_ocyType = OnBirth;
	}
	explicit OccupancyInstance(CAirsideMobileElement * pFlight){
		m_pMobileElement = pFlight;
		m_tEntry = ElapsedTime(-1L);
		m_tExit = ElapsedTime(-1L);	
		m_dSpd = 0;
		m_ocyType = OnBirth;
	}
	

	bool operator < (const OccupancyInstance& _occupancy)const
	{
		return m_tEntry < _occupancy.m_tEntry;
	}
	bool operator == (const OccupancyInstance& _occupancy)const
	{
		return m_pMobileElement == _occupancy.m_pMobileElement;
	}

	ElapsedTime GetEnterTime()const{ return m_tEntry; }
	ElapsedTime GetExitTime()const{ return m_tExit; }
	AirsideFlightInSim * GetFlight()const{ return reinterpret_cast<AirsideFlightInSim *>(m_pMobileElement); }
	AirsideVehicleInSim *GetVehicle() const{ return reinterpret_cast<AirsideVehicleInSim *>(m_pMobileElement); }
	CAirsideMobileElement *GetElement()const{ return m_pMobileElement;}
	int GetOccupyType()const{ return m_ocyType; }
	double GetSpeed()const{ return m_dSpd; }

	bool IsEnterTimeValid()const{   return !(m_tEntry < ElapsedTime(0L)); }
	bool IsExitTimeValid()const{ return !(m_tExit< ElapsedTime(0L) ); }
	bool IsValid()const{ return m_pMobileElement!=NULL; }

	


protected:
	ElapsedTime m_tEntry;
	ElapsedTime m_tExit;
	CAirsideMobileElement* m_pMobileElement;
	AirsideMobileElementMode m_ocyType ;
	double m_dSpd;
};
typedef std::vector<OccupancyInstance> OccupancyTable;

class ResourceDesc;

class ENGINE_TRANSFER AirsideResource : public Referenced
{
	DynamicClassStatic(AirsideResource)
public:
	enum ResourceType{
		ResType_WayPoint = 0,
		ResType_AirRouteIntersection,
		ResType_AirRouteSegment,
		ResType_HeadingAirRouteSegment,
		ResType_FinalApproachSegment,
		ResType_LogicRunway,		
		ResType_IntersectionNode,
		ResType_TaxiwayDirSeg,		
		ResType_Stand,
		ResType_StandLeadInLine,
		ResType_StandLeadOutLine,
		ResType_DeiceStation,
		ResType_VehiclePool,
		ResType_InterruptLine,

		ResType_VehicleStretchLane,
		ResType_VehicleRoadIntersection,
		ResType_FlightServiceRingRoute,

		ResType_PaxBusParking,
		ResType_AirEntrySysHold,
		ResType_AirspaceHold,

		ResType_RunwayDirSeg,																																																																																					
		ResType_VehicleTaxiLane,
		ResType_LaneTaxiwayIntersection,
		ResType_MeetingPoint,
		ResType_StartPosition,

		ResType_FlightPlanPhase,
		ResType_BagCartsParkingPosition,
		//
		nResourceCount,
	};

	typedef ref_ptr<AirsideResource> RefPtr;

	bool IsStandResource()const;
public:	
	
	virtual ResourceType GetType()const = 0;
	virtual CString GetTypeName()const = 0;
	virtual CString PrintResource()const = 0;

	virtual CPoint2008 GetDistancePoint(double dist)const = 0; 

public:
	virtual void getDesc(ResourceDesc& resDesc)=0;
	
	//get the last enter resource flight and still in resource0
	virtual	AirsideFlightInSim * GetLastInResourceFlight();
	virtual	AirsideVehicleInSim * GetLastInResourceVehicle();

	//get the last Occupy Instance
	OccupancyInstance GetLastOccupyInstance()const;
	OccupancyInstance GetFirstOccupyInstance() const;

	//get the previous flight occupy instance of the flight
	OccupancyInstance GetPreviousMobileElmentOccupancy(CAirsideMobileElement* pFlight)const;
//	OccupancyInstance GetPreviousFlightOccupancy(AirsideVehicleInSim* pFlight)const;

	
	// set enter time of flight
	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode);
	// set exit time of flight
	virtual void SetExitTime(CAirsideMobileElement * pFlight, const ElapsedTime& exitT);

	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode,double dSpd);

	//lock resources functions
	virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime){}
	virtual const ElapsedTime GetLastReleaseTime()const{ return m_releaseTime; }
	virtual bool GetLock(AirsideFlightInSim * pFlight){ return true; }
	virtual AirsideFlightInSim * GetLockedFlight(){ return NULL; }
	virtual bool TryLock(AirsideFlightInSim * pFlight){ return true; }

	OccupancyTable& GetOccupancyTable(){ return m_vOccupancyTable; }
	const OccupancyTable& GetOccupancyTable()const{ return m_vOccupancyTable; }

	std::vector<CAirsideMobileElement *> GetInResouceMobileElement();
	
	OccupancyInstance GetOccupyInstance(CAirsideMobileElement * pMobile)const;

	AirsideFlightInSim * GetFlightAhead(AirsideFlightInSim *pFlight);

	//clear pMobile's occupancy information at the resource
	void RemoveElementOccupancyInfo(CAirsideMobileElement * pMobile);
	//get next flight after the given flight
	std::vector<AirsideFlightInSim *> GetFlightsAfterFlight(AirsideFlightInSim *pFlight);
protected:
	//get a flight occupy instance , if not exist add one
	OccupancyInstance& GetAddOccupyInstance(CAirsideMobileElement* pFlight);

	OccupancyTable m_vOccupancyTable;

	ElapsedTime m_releaseTime;

};


