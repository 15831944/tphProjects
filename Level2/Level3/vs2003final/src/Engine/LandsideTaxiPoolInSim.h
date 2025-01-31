#pragma once
#include "landsidestretchinsim.h"
#include "LaneParkingSpot.h"
#include "LandsideTaxiQueueInSim.h"

/*class LandsideLaneNodeList;*/
class LaneSegInSim;
class LandsideResourceManager;
class LandsideTaxiPool;
class PaxLandsideBehavior;

//class LandsideTaxiPoolLaneInSim;
class LandsideTaxiPoolInSim :
	public LandsideResourceInSim, public LandsideLayoutObjectInSim, public IUpdateOrderingVehicle
{
public:
	//resource functions
	LandsideTaxiPoolInSim(LandsideTaxiPool *pStation);
	~LandsideTaxiPoolInSim(void);	
	virtual LandsideTaxiPoolInSim* toTaxiPool(){ return this; }
	LandsideResourceInSim* IsResource(){ return this; }
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual CString print()const;

	//relate with other object
	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const;
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const;
	//properties
	bool GetPoolLanePath( int i,CPath2008* lanePath );
	const CPath2008& GetPath()const;
	DistanceUnit GetTotalLen()const{ return m_dTotalLength; }
	DistanceUnit GetVehicleSep()const{ return m_dsepInPool; }

	//vehicle functions
	void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);
	void OnVehicleEnter(LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t);

	void NotifyVehicleArrival(LandsideVehicleInSim* pV);
	//send the nCount Vehicle to service q, return sended number
	int SentVehicleToQueue(const CString& vehicleType, int nCount, LandsideTaxiQueueInSim* pQ,CARCportEngine* pEngine);
	
	LandsideVehicleInSim* GetHeadVehicle()const;
	LandsideVehicleInSim* GetVehicleBefore(LandsideVehicleInSim* pVeh)const;
	LandsideVehicleInSim* GetVehicleAfter(LandsideVehicleInSim* pVeh)const;
	virtual bool IsHaveCapacityAndFull(LandsideVehicleInSim*pVeh)const;	

	virtual void _UpdateOrderingVehicle(CARCportEngine* pEngine);
	int GetNonServiceTaxiCount()const;
	std::vector<CPath2008*> m_pLanes;
private:
	LandsideResourceManager* m_pAllRes;
	
	std::vector<LandsideLaneEntry*> m_vLaneEntries;	
	std::vector<LandsideLaneExit*> m_vLaneExits;
	DistanceUnit m_dTotalLength;
	DistanceUnit m_dsepInPool;

	LandsideTaxiPool* getTaxiPoolInput()const{ return (LandsideTaxiPool*)getInput(); }

	VehicleList m_vOrderingVehicles;

};