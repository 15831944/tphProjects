#pragma once
#include "TaxiwayResourceManager.h"
#include "AirspaceResourceManager.h"
#include "CommonInSim.h"


#include <vector>

class FlightPlanInSim;
class AirspaceTrafficController;
class RunwayPortInSim;
class AirsideFlightInSim;



class ENGINE_TRANSFER PlanGroundRouteItem
{

public:
	PlanGroundRouteItem(){ pTaxiSeg = NULL; bFinished = false; }
	PlanGroundRouteItem(TaxiwayDirectSegInSim * pSeg){ pTaxiSeg = pSeg; bFinished = false; }

	TaxiwayDirectSegInSim * GetTaxiwaySeg(){ return pTaxiSeg; }
	bool IsFinished()const { return bFinished; }
	void SetFinished(bool b){ bFinished =b;}

protected:
	TaxiwayDirectSegInSim * pTaxiSeg;

	bool bFinished;

};

class ENGINE_TRANSFER PlanGroundRoute
{
public:	
	PlanGroundRoute(){ nVersion = 0 ; }
	PlanGroundRouteItem * GetNextItem();

	void SetRoute(const TaxiwayDirectSegList& vSegments);

	int GetItemCount()const{ return (int)m_vRoute.size(); }
	PlanGroundRouteItem * GetItem(int idx)	;
	const PlanGroundRouteItem * GetItem(int idx)const;
	int GetFirstUnDoneItemId()const;

	void Activate();

	std::vector<PlanGroundRouteItem>& GetItemList(){ return m_vRoute; }		

	int GetVersion()const{ return nVersion; }
	void Upgrade(){ nVersion ++; }

	//find Clearance return true , if till the end
	bool FindClearanceInRadiusofConcern(AirsideFlightInSim * pFlight, Clearance& clearace);

protected:
	std::vector<PlanGroundRouteItem> m_vRoute;
	int nVersion;
};


//Flight 
class ENGINE_TRANSFER FlightFlowItemInSim
{
public:
	FlightFlowItemInSim();
	FlightFlowItemInSim(AirsideResource *pRes);

	void SetDone(){ m_bFinished = true; }
	bool IsDone()const{ return m_bFinished; }
	
protected:
	AirsideResource * pResource;
	AirsideMobileElementMode OnMode;
	bool m_bFinished;
};

class CAirRoute;

class ENGINE_TRANSFER FlightPlanningFlowInSim
{
public:	
	FlightPlanningFlowInSim();

	PlanGroundRoute& GetInBoundRoute(){ return m_InBoundRoute; }
	PlanGroundRoute& GetOutBoundRoute(){ return m_OutBoundRoute; }



	FlightPlanInSim* GetDepartureAirRoute(){ return m_pDepartureAirRoute; }
	FlightPlanInSim* GetArrivalAirRoute(){ return m_pArrivalAirRoute; }
	void SetDepartureAirRoute(FlightPlanInSim& flightPlan){ m_pDepartureAirRoute = &flightPlan; }
	void SetArrivalAirRoute(FlightPlanInSim& flightPlan){ m_pArrivalAirRoute = &flightPlan; }

	RunwayPortInSim* GetTakeoffRunwayPort(){return m_pTakeoffRunwayPort;}
	void SetTakeoffRunwayPort(RunwayPortInSim* pRunwayPort){ m_pTakeoffRunwayPort = pRunwayPort; }

	RunwayPortInSim* GetLandingRunwayPort(){return m_pLandingRunwayPort;}
	void SetLandingRunwayPort(RunwayPortInSim* pRunwayPort){ m_pLandingRunwayPort = pRunwayPort; }

	CAirRoute* GetSTAR(){ return m_pSTAR; }
	CAirRoute* GetSID(){ return m_pSID; }
	void SetSTAR(CAirRoute* pSTAR){ m_pSTAR = pSTAR; }
	void SetSID(CAirRoute* pSID){ m_pSID = pSID; }

	
	void AddNewItem(AirsideResource *pRes, AirsideMobileElementMode onMode);
	void AddNewItems(std::vector<AirsideResource* > vRes, AirsideMobileElementMode onMode);	

	int GetItemCount();

	int GetFirstUnDoneItemId()const;
	
	FlightFlowItemInSim * GetItem(int idx);

	StandInSim * GetParkingStand(){ return m_pStand; }
	void SetParkingStand(StandInSim * pStand){ m_pStand = pStand; }

protected:

	
	PlanGroundRoute m_InBoundRoute;
	PlanGroundRoute m_OutBoundRoute;
	StandInSim * m_pStand;

	FlightPlanInSim* m_pDepartureAirRoute;
	FlightPlanInSim* m_pArrivalAirRoute;

	RunwayPortInSim* m_pTakeoffRunwayPort;
	RunwayPortInSim* m_pLandingRunwayPort;

	CAirRoute* m_pSTAR;
	CAirRoute* m_pSID;
	
};
