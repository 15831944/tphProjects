#pragma once

#include "StandInSim.h"
#include "InputAirside/FlightTypeServiceLoactions.h"
#include "InputAirside/RingRoad.h"
#include "../VehicleParkingPlaceMgrInSim.h"
class AirsideFlightInSim;


class FlightServiceLocationInSim
{
public:
	FlightServiceLocationInSim(const FlightTypeRelativeElement& element);
	~FlightServiceLocationInSim();

protected:

public:
	
	FlightTypeRelativeElement m_serviceLocation;
	DistanceUnit m_dDistLoctToPath;
	bool m_bHasServiced;


	std::vector<AirsideVehicleInSim *> m_vServiceVehicle;
	AirsideVehicleInSim *m_pOccupyVehicle;

private:
};

class CFlightServiceRoute :public AirsideResource
{
public:
	typedef ref_ptr<CFlightServiceRoute> RefPtr;
	CFlightServiceRoute(AirsideFlightInSim* pFlightInSim);
	~CFlightServiceRoute(void);

public:
	//const CPath2008& getRoutePath(){ return m_pathRoute;}
	void SetRingRoute(const RingRoad& ringRoad){ m_OriginalRingRoad = ringRoad;}
	void SetFltServiceLocations(const FlightTypeServiceLoactions& fltServiceLocations);
	void SetFlightServiceStand(StandInSim* pStand);

public:	

	virtual ResourceType GetType()const { return ResType_FlightServiceRingRoute;}
	virtual CString GetTypeName()const { return _T("FlightServiceRingRoute");}
	virtual CString PrintResource()const {return _T("Flight service Ring route");}
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const;
	bool FindClearanceInConcern(  AirsideVehicleInSim  * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance );

	void WakeupTheWaitingVehicleInRingRoad(const ElapsedTime &eTime);
	bool SetVehicleServicePointServiced(int nVehicleTypeID);
	void SetUnreachableVehicleType(int vehicleType);
	int GetFltServicePointCount(int nVehicleTypeID);
	void GenerateNextEvent(AirsideVehicleInSim *pVehicle,const ElapsedTime& eTime);
	StandInSim *m_pStand;
protected:
	//originals data
	FlightTypeServiceLoactions m_originalFltServiceLocations;
	RingRoad m_OriginalRingRoad;	
	
	//StandInSim *m_standInSim;
	AirsideFlightInSim *m_pFlightInSim;

	//converted data
	RingRoad m_ringRoad;
	//std::vector<CPoint2008> vServiceEntryPoint;
	FlightTypeServiceLoactions m_fltServiceLocations;
	//this vector is sorted by the distance in the path
	//std::vector<FlightServiceLocationInSim *> m_vFltServiceLoctInSim;
	std::vector<AirsideVehicleInSim *> m_vWaitingFlightVehicle;

	bool m_bInitialized;
	bool m_bArrivalAtStand;//flight arrival at stand
	ElapsedTime m_tArrivalAtStand;//flight arrival at stand time

	CPoint2008 m_ptHead;

	class VehicleTempPark
	{
	public:
		VehicleTempPark()
		{
			pVehicle =NULL;
			nRoutepointIndex = -1;
		}
		~VehicleTempPark(){}

	public:
		AirsideVehicleInSim *pVehicle;
		//CPoint2008 pt;
		int nRoutepointIndex;
	private:
	};

    CPoint2008 m_CurServerPoint ;
	std::vector<VehicleTempPark> m_vVehicleTempPark;

	std::vector<int > m_vUnreachableVehicleType;

	class VehicleRoutePoint
	{
	public:
		VehicleRoutePoint(const CPoint2008& pt)
		{
			point = pt;
			pOccupyVehicle = NULL;
			pTempParkingVehicle = NULL;
			//pWaitingVehicle = NULL;
		}
		~VehicleRoutePoint()
		{

		}
	public:
		CPoint2008 point;// the vehicle point
		AirsideVehicleInSim *pOccupyVehicle;
		AirsideVehicleInSim *pTempParkingVehicle;
		std::vector<AirsideVehicleInSim *> vWaitingVehicle;
		
		std::vector<FlightServiceLocationInSim> vServicePoint;

	public:
		bool HasServicePoint(AirsideVehicleInSim* pVehicle);
	
		//return 0, cannot serve
		//return 1, can serve
		//return 2,can serve & has a flight is serving
		int TheVehicleCanService(AirsideVehicleInSim* pVehicle,CFlightServiceRoute *pServiceRoute);

		void WakeupWaitingVehicle(CFlightServiceRoute *pServiceRoute);

		void VehicleServicePoint(AirsideVehicleInSim* pVehicle);

		void AddWaitingVehicle(AirsideVehicleInSim *pWaitingVehicle);
	};

	std::vector<VehicleRoutePoint > m_vVehicleRoutePoint;

	VehicleParkingPlaceMgrInSim m_VehicleParkingPlaceMgr;
	//vehicle type, occupy the count of vehicle route point
	std::map<int,int> m_mapVehicleLength;	

protected:
	void InitResource(AirsideVehicleInSim  * pVehicle);
	void InitVehicleRoute(CPath2008& routePath);
	void ConvertPathLocationsIntoLocalCoordinate(const CPoint2008& pt, const double& dRotatedegree);
	void AddFltServiceLoctInSim(const FlightServiceLocationInSim& );
	CPoint2008 GetRingRouteEntryPoint();
	CPoint2008 GetRingRouteExitPoint();
	void ClearVehicleOccupyEntryPoint(AirsideVehicleInSim *pVehicle);

	int GetHasNotServicedPointCount(int nVehicleType);
	
	CPath2008 GetRingRoutePath();

	void WriteVehicleLog(AirsideVehicleInSim *pVehicle,ElapsedTime ePreTime,DistanceUnit dPreDist, DistanceUnit dCurDist,bool bPushBack = false);
	void WriteVehicleLog(AirsideVehicleInSim *pVehicle,ElapsedTime ePreTime,CPoint2008& dPrePoint, CPoint2008& dCurPoint,bool bPushBack = false);
	//CPoint2008 GetNextVehiclePosition();
	bool CheckTheServiceFlightAllServiceCompleted(AirsideFlightInSim *pFlight);

	//get the index of the header vehicle
	int GetVehiclePostionInRingRoute(CPoint2008 pt);
	int TheVehicleCanMoveForward(AirsideVehicleInSim *pVehicle,AirsideVehicleInSim *& pPreVehicle,int& nextPointIndex);
	AirsideVehicleInSim * IsThePointOccupanyByVehicle(AirsideVehicleInSim*pVehicle,CPoint2008 pt);

	//if the flight is not arrive, get the temp parking place
	int GetTempParkingPoint();
	bool IsTheParkingPointOccupany(CPoint2008 pt);
	bool CheckServicePoint(AirsideVehicleInSim *pVehicle, CPoint2008 ptNext);
	void RemoveVehicleTempParkList(AirsideVehicleInSim *pVehicle);
	bool IsTheVehicleInTempParkList(AirsideVehicleInSim *pVehicle);
	bool IsExistServicePointVehicleCanServe(AirsideVehicleInSim *pVehicle);

	int GetVehiclePostionInRoute(AirsideVehicleInSim *pVehicle);

	int GetVehicleLength(AirsideVehicleInSim *pVehicle);

	//vehicle movements
	protected:
		void VehicleOnMoveToRingRoute(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnServiceMovement(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnVehicleMoveToTempParking(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnVehicleMoveOutTempParking(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnService(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnLeaveServicePoint(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnLeaveServiceMovement(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
		void VehicleOnLeaveRingRoute(AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);
};
