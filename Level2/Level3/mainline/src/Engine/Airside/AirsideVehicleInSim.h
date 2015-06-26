#pragma once
#include "../../Results/AirsideVehicleLogEntry.h"
#include "../../Common/AIRSIDE_BIN.h"
#include "../../Common/point2008.h"
#include "../../Common/elaptime.h"
#include "CommonInSim.h"
#include "AirsideResource.h"
#include "../../Common/ProbabilityDistribution.h"
#include "./VehicleRouteInSim.h"
#include "../../InputAirside/VehicleSpecifications.h"
#include "AirsideMobElementWriteLogItem.h"

#include "AirsideVehicleState.h"

class VehiclePoolInSim;
class OutputAirside;
class StandInSim;
//class AirsideFlightInSim;
//class ProbabilityDistribution;
class VehicleRouteResourceManager;
class Clearance;
class ClearanceItem;
//class CVehicleSpecifications;
class CVehicleSpecificationItem;
class AirsideEvent;
class AirsideVehicleLogItem;

//class ENGINE_TRANSFER VehicleWakeupCaller : public CAirsideObserver
//{
//public:
//	void SetVehicle(AirsideVehicleInSim* pVehicle)
//	{
//		m_pVehicle = pVehicle;
//	}
//	void Update(const ElapsedTime& tTime);
//protected:
//	AirsideVehicleInSim * m_pVehicle;
//};



class ENGINE_TRANSFER AirsideVehicleInSim: public CAirsideMobileElement
{

public:
	explicit AirsideVehicleInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem);
	virtual ~AirsideVehicleInSim(void);

	virtual void WakeUp(const ElapsedTime& tTime);
	virtual void ServiceFlight(const ElapsedTime& tTime){}
public:
	void SetVehicleTypeID(int nID) { m_nVehicleTypeID = nID;}
	int GetVehicleTypeID() { return m_nVehicleTypeID;}

	void SetVehiclePool(VehiclePoolInSim* pPool){ m_pVehiclePool = pPool;}
	VehiclePoolInSim* GetVehiclePool(){ return m_pVehiclePool; }

	ElapsedTime GetBirthTime()const{ return m_tBirthTime;}
	void SetBirthTime(ElapsedTime tTime){ m_tBirthTime = tTime;}
	
	ElapsedTime GetTime()const{ return m_tCurrentTime; }
	virtual void SetTime(const ElapsedTime& tTime){ m_tCurrentTime = tTime; }

	CPoint2008 GetPosition()const{ return m_pPosition; }
	virtual void SetPosition(const CPoint2008& pos){ m_pPosition = pos; }

	double GetSpeed()const{ return  m_vSpeed; }
	virtual void SetSpeed(double speed){ m_vSpeed = speed; }

	AirsideMobileElementMode GetMode(){ return m_vehicleMode; }
	virtual void SetMode(AirsideMobileElementMode mode){ m_vehicleMode = mode; }

	void SetDistInResource(DistanceUnit dist){ m_dist = dist; }
	DistanceUnit GetDistInResource()const{ return m_dist; }

	DistanceUnit getCurDistInDirPath()const{ return m_dist; }
	RouteDirPath* getCurDirPath()const;

	void SetResource(AirsideResource * pRes ){ m_pResource = pRes; }
	AirsideResource * GetResource()const { return m_pResource; }

	virtual void InitLogEntry(VehiclePoolInSim*pool, OutputAirside * pOut);
	virtual void WirteLog(const CPoint2008& p, const double speed, const ElapsedTime& t, bool bPushBack = false);
	virtual void FlushLog(const ElapsedTime& endTime);



	void SetLeavePoolTime(ElapsedTime tTime){ m_tLeavePoolTime = tTime;}
	ElapsedTime GetLeavePoolTime(){ return m_tLeavePoolTime;}

	AirsideFlightInSim* GetServiceFlight(){ return m_pServiceFlight;}
	double GetStuffCount(){ return m_vStuffPercent;}

	//service location count for a flight
	void SetServicePointCount(int nCount) { m_nServicePointCount = nCount;}
	int GetServicePointCount() { return m_nServicePointCount;}


	ProbabilityDistribution* GetServiceTimeDistribution()const{ return m_pServiceTimeDistribution;}
	void SetServiceTimeDistribution(ProbabilityDistribution* pDistribution){ m_pServiceTimeDistribution = pDistribution;}
	ProbabilityDistribution* GetSubServiceTimeDistribution()const{ return m_pSubServiceTimeDistribution;}
	void SetSubServiceTimeDistribution(ProbabilityDistribution* pDistribution){ m_pSubServiceTimeDistribution = pDistribution;}

	bool IsAvailable() { return m_bAvailable; }
	void SetAvailable(bool bAvailable);

	virtual	void GetNextCommand();

	virtual void SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent);

	virtual void ReturnVehiclePool(ProbabilityDistribution* pTurnAroundDistribute);
	
	virtual void ResetServiceCapacity();
		
	//calculate time between two clearance item
	ElapsedTime GetTimeBetween(const ClearanceItem& item1, const ClearanceItem& item2)const;
	//virtual void NotifyObservers();
	//
	double GetOnRouteSpeed()const;
	double GetVehicleLength() const;
	double GetVehicleHeight() const;
	double GetVehicleWidth() const;
	double GetVehicleDec()const;
	double GetVehicleAcc()const;

	virtual double GetVehicleActualLength()const;

	double GetLength()const{ return GetVehicleLength(); }

	DistanceUnit GetSeparationDist(AirsideVehicleInSim * pVehicleAhead)const;
	//Moving On The Route 
	void MoveOnRoute(const ElapsedTime& time);

	//
	virtual TYPE GetType() { return AIRSIDE_VEHICLE; }
	enumVehicleBaseType GetVehicleBaseType();

	//generate write log event
	void GenerateWriteLogEvent();
	void GenerateNextEvent(AirsideEvent *pNextEvent);
	//write log 
	void WritePureLog(AirsideMobElementWriteLogItem* pItem);

	void getDesc(ARCMobileDesc& desc)const;

protected:
	CString m_strVehicleType;
	int		m_nVehicleTypeID;		   
	VehiclePoolInSim* m_pVehiclePool;
	VehicleRouteInSim m_Route;
	ElapsedTime ignorLogTime;
	int ignorLogCount;
	//leave time ,at that time it will be out of server
	ElapsedTime m_LeaveTime ;
//status
public:
	ElapsedTime m_tCurrentTime;
	CPoint2008 m_pPosition;
	double m_vSpeed;    //cm/s
	CPoint2008 m_vDirection;  //
	AirsideMobileElementMode m_vehicleMode;
	AirsideResource* m_pResource;
	DistanceUnit m_dist;  
	ElapsedTime tMinTurnAroundTime;

	void PerformClearance( Clearance& clearance);
	void PerformClearanceItem( ClearanceItem& clearanceItem);

	VehicleRouteResourceManager * m_pRouteResMan;

	VehicleRouteResourceManager * GetRouteResourceManager()const{ return m_pRouteResMan; } 
	void SetRouteResourceManager(VehicleRouteResourceManager * pRouteResource){ m_pRouteResMan = pRouteResource; }
	
	ElapsedTime m_tBirthTime;


	virtual double getCurSpeed()const{ return GetSpeed(); }	
	virtual ElapsedTime getCurTime()const{ return GetTime(); } 
	virtual DistanceUnit getCurSeparation()const{ return 3.0*SCALE_FACTOR; }//the separation other mobile need to keep;{}


	virtual void OnNotify( SimAgent* pAgent, SimMessage& msg );	//notify by other agents
	virtual bool IsActive()const{ return true; }

//output
	//AirsideVehicleLogEntry m_LogEntry;
	OutputAirside * m_pOutput;
	AirsideVehicleLogItem* m_pReportlogItem; //log 
	void FlushLogToFile(); //save logs to report file

//service status
	double  m_vStuffPercent;
	AirsideFlightInSim* m_pServiceFlight;
	ElapsedTime m_tLeavePoolTime;
	bool m_bAvailable;
	int m_nServicePointCount;
	ProbabilityDistribution* m_pServiceTimeDistribution;
	ProbabilityDistribution* m_pSubServiceTimeDistribution;
	int m_pCurrentPointIndex;

	//debug
	//CVehicleSpecifications* m_pVehicleSpecifications;
	CVehicleSpecificationItem* m_pVehicleSpecificationItem;

	//VehicleWakeupCaller m_wakeupCaller;
	//VehicleWakeupCaller& GetWakeUpCaller(){ return m_wakeupCaller; }

	//static void DiagWarning(AirsideVehicleInSim* pVehicle,const CString& strWarn);
protected:
	void EchoLog(const Clearance& clearance) const;

	virtual void _ChangeToService();
public:
	int GetID() const {return m_id ;} 
	void SetID(int id ){ m_id = id; }


public:
	virtual int ArriveAtStand(ElapsedTime time) { return 1 ;};
	//virtual int ArriveAtParking(ElapsedTime time) {return 1 ;};
	ElapsedTime GetLeaveTime() { return m_LeaveTime ;} ;
	void SetLeaveTime(ElapsedTime time) { m_LeaveTime = time ;} ;
	BOOL IsArrivedAtStand() { return m_IsArriveStand ;} ;
	void IsArrivedAtStand(BOOL _res) { m_IsArriveStand = _res ;} ;
	CPoint2008 GetRanddomPoint();
	double GetVehicleRandomZ();
private:
	BOOL m_IsArriveStand ;

	//write log event list
	AirsideMobElementWriteLogItemList m_lstWirteLog;
	//AirsideVehicleState m_curState;

	AirsideEvent *m_pNextEvent;
	
	//indicate the next event executed is write log event
	bool m_bExecuteWriteLogEvent;
private:
	ElapsedTime m_PreEventTime ;
	CPoint2008 m_PreEventPoint ;
	int m_id;

};
