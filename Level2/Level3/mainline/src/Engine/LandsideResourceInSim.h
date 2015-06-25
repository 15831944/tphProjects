#pragma once
#include "Common\CPPUtil.h"
#include "Common\POINT.H"
#include "core\State.h"
#include "core\Observer.h"


class LandsideResourceManager;
class LandsideVehicleInSim;
class CLandsideWalkTrafficObjectInSim;
class LandsideIntersectionInSim;
class LandsideCurbSideInSim;
class LandsideLaneInSim;
class LandsideExternalNodeInSim;
class LandsideParkingSpotInSim;
class LandsideParkingLotInSim;
class LandsideParkingLotDoorInSim;
class LandsideParkinglotDriveLaneInSim;
class LandsideIntersectLaneLinkInSim;
class LandsideBusStationInSim;
class CPath2008;
class CPollygon2008;
class CPoint2008;
class LandsideRoundaboutInSim;
class LandsideLayoutObjectInSim;
class ARCVector3;
class LaneParkingSpot;
class LandsideParkingLotDrivePipeInSim;
class StretchSpeedControlData;
class PaxLandsideBehavior;
class LandsideTaxiQueueInSim;
class LandsideTaxiPoolInSim;
class LandsideLaneNodeInSim;
class LeadToInfo;
class ARCPolygon;
class IParkingSpotInSim;

class LandsideResourceInSim :  public IObserveSubject
{
public:
	virtual ~LandsideResourceInSim(){}
	virtual LandsideLayoutObjectInSim* getLayoutObject()const=0;
	virtual CString print()const;


	//child class 
	virtual LandsideIntersectionInSim* toIntersection(){ return NULL; }
	virtual LandsideCurbSideInSim* toCurbSide(){ return NULL; };
	virtual LandsideLaneInSim* toLane(){ return NULL; } 
	virtual LandsideExternalNodeInSim* toExtNode(){ return NULL; }
	virtual LandsideParkingLotInSim* toParkLot(){ return NULL; }
	virtual LandsideParkingLotDoorInSim *toParkLotDoor(){ return NULL; }
	virtual LandsideParkingSpotInSim* toParkLotSpot(){ return NULL; }
	virtual LandsideParkinglotDriveLaneInSim* toParkDriveLane(){ return NULL; }
	virtual LandsideParkingLotDrivePipeInSim* toParkDrivePipe(){ return NULL; }
	virtual LandsideIntersectLaneLinkInSim* toIntersectionLink(){ return NULL; }
	virtual LandsideRoundaboutInSim* toAbout(){ return NULL; }
	virtual LandsideBusStationInSim* toBusSation(){return NULL;}
	virtual IParkingSpotInSim* toLaneSpot(){ return NULL; }
	virtual LandsideTaxiQueueInSim* toTaxiQueue(){ return NULL; }
	virtual LandsideTaxiPoolInSim* toTaxiPool(){ return NULL; }


	virtual void OnVehicleEnter(LandsideVehicleInSim* pVehicle, DistanceUnit dist, const ElapsedTime& t);
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);

	int GetInResVehicleCount()const{ return (int)m_vInResVehicle.size(); }
	LandsideVehicleInSim* GetInResVehicle(int idx)const;
	bool bHasVehicle(LandsideVehicleInSim* pVechile)const;
	int GetVehicleIndex(LandsideVehicleInSim* pVehicle)const;

	virtual StretchSpeedControlData* GetSpeedControl(double dist)const; 	

	virtual CPoint2008 GetPaxWaitPos()const;
 
	//------------------------------------------------------------------------------
	//Summary:
	//		intersect with area and select the shortest walkway
	//Parameters:
	//		pt[in]: current point to calculate the distance to select best walkway
	//		LandsideResourceManager[in]: get walkway list to select best walkway
	//Return:
	//		retrieve the best walkway
	//------------------------------------------------------------------------------
	virtual CLandsideWalkTrafficObjectInSim* getBestTrafficObjectInSim(const CPoint2008& pt,LandsideResourceManager* allRes);

	void GetInsidePathOfPolygon(const CPath2008& path, const CPollygon2008& polygon,std::vector<CPath2008>& resultPathList)const;

	virtual void PassengerMoveInto(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);
	virtual void PassengerMoveOut(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);

	typedef cpputil::TPtrVector<LandsideVehicleInSim> VehicleList;
	VehicleList& InResVehicle(){ return m_vInResVehicle; }

	//all resource can check ahead vehicles
	virtual LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist){ return NULL; }
	virtual LandsideVehicleInSim* CheckSpaceConflict(LandsideVehicleInSim* pVeh, const ARCPolygon& poly){ return NULL; }
	virtual LandsideVehicleInSim* CheckPathConflict(LandsideVehicleInSim* pVeh, const CPath2008& path,  DistanceUnit& Aheaddist){ return NULL; }
protected:
	VehicleList m_vInResVehicle;

	void AddInResVehicle(LandsideVehicleInSim* pVehicle);
	void RemoveInResVehicle(LandsideVehicleInSim* pVehicle);
	void RemoveInResVehicles(std::vector<LandsideVehicleInSim*>& vVehicles);

	virtual LandsideVehicleInSim* GetAheadVehicleByDistInRes(LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist);
	
};

