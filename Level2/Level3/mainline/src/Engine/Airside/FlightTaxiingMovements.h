#pragma once



#include "FlightMovementsOnResource.h"



//only handle taxing movements
class TaxiwayDirectSegInSim;
class AirWayPointInSim;


class ENGINE_TRANSFER FlightTaxiing : public FlightMovements // movements on a res
{
public:
	explicit FlightTaxiing(FlightInAirsideSimulation * pFlight):FlightMovements(pFlight){}

	void EndInSeg(TaxiwayDirectSegInSim * pSeg, double dist, double vSpeed);

	virtual void End(Clearance * pClearance);	

	void GetTaxiPath(Clearance * pClearance , Path& path);
protected:

};

class ENGINE_TRANSFER FlightPushOutStand : public FlightMovements
{
public:
	explicit FlightPushOutStand(FlightInAirsideSimulation * pFlight): FlightMovements(pFlight){}
    
	virtual void End(Clearance * pClearance);

	void GetPushOutPath(Clearance *pClearance, Path& path);
};
class ENGINE_TRANSFER FlightBirthing : public FlightMovements
{
public:
	explicit FlightBirthing(FlightInAirsideSimulation * pFlight) : FlightMovements(pFlight){}
	virtual void End(Clearance * pClearace);

};
class ENGINE_TRANSFER FlightHeldAtStand : public FlightMovements
{
public:
	explicit FlightHeldAtStand(FlightInAirsideSimulation * pFlight) : FlightMovements(pFlight){}
	virtual void End(Clearance *pClearance);
};