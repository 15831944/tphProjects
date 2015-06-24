#pragma once
#include "AirsideCircuitFlightInSim.h"
#include "AirspaceResource.h"

#include "IFlightProcess.h"

//class AirsideCircuitFlightInSim;
class LogicRunwayInSim;
class FlightRouteInSim;
class ElapsedTime;

class ICircuteFlightProcess : public IFlightProcess
{
public:
	ICircuteFlightProcess(AirsideCircuitFlightInSim* pFlight):m_pFlight(pFlight){}

	bool GenerateClimoutItem( ClearanceItem& lastItem,ClearanceItem& climoutItem);
	bool CheckIsTakeoffDelay(ClearanceItem& lastItem, Clearance& newClearance,FlightRouteInSim *pAirRouteInSim,AirsideCircuitFlightInSim::LandingOperation emType,bool bCanWait);

	void GenerateFullStopClearance(FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance);

	bool GetLandingDelayTime(ElapsedTime& eLandingDelayTime,ClearanceItem& lastItem);
	bool GetMisApproach(LogicRunwayInSim* pLandRunway,ClearanceItem& lastItem, Clearance& newClearance);

	virtual bool Process(ClearanceItem& lastItem, Clearance& newClearance);
protected:
	AirsideCircuitFlightInSim* m_pFlight;
	virtual bool DoActivate(ClearanceItem& lastItem, Clearance& newClearance){ return true; }
	virtual bool DoProcess(ClearanceItem& lastItem, Clearance& newClearance){ return true; }
};

// intension CircuteRoute
class ICirciteIntensionProcess : public ICircuteFlightProcess
{
public:
	ICirciteIntensionProcess(AirsideCircuitFlightInSim* pFlight):ICircuteFlightProcess(pFlight){ 
		m_op = AirsideCircuitFlightInSim::Normal_Operation;
		m_pOutBoundProcess= NULL;
		m_pLandingRoute = NULL;
		m_pTakeoffRoute = NULL;
	}
	void SetLandingTakeoffRoute(FlightRouteInSim* plandingRoute,FlightRouteInSim* pTakeoffRoute){  m_pLandingRoute = plandingRoute; m_pTakeoffRoute = pTakeoffRoute; }
protected:
	void GenerateClearance(const AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& clearance );

	bool SchemeOnAirRouteOnce(const ClearanceItem& _lastItem, const AirsideResourceList& ResourceList,ElapsedTime& tDelay);

	bool SchemeOnAirRoute(ClearanceItem& inOutItem, const AirsideResourceList& ResourceList);

	bool _ProcessFullStopAndGo(ClearanceItem& lastItem, Clearance& newClearance);
	bool _ProcessLanding(ClearanceItem& lastItem, Clearance& newClearance);
	bool _ProcessPostLanding(ClearanceItem& lastItem, Clearance& newClearance);


	virtual bool DoProcess(ClearanceItem& lastItem, Clearance& newClearance);
	virtual bool DoActivate(ClearanceItem& lastItem, Clearance& newClearance);


	virtual FlightRouteInSim*GetResourceRoute(AirsideResource* pRes)const;

	AirsideCircuitFlightInSim::LandingOperation m_op;	
	ICircuteFlightProcess* m_pOutBoundProcess;

	FlightRouteInSim* m_pLandingRoute;
	FlightRouteInSim* m_pTakeoffRoute;
};

//////////////////////////////////////////////////////////////////////////
class ENGINE_TRANSFER AirsideCircuitFlightProcess2 : public ICircuteFlightProcess
{
public:
	AirsideCircuitFlightProcess2(AirsideCircuitFlightInSim* pFlight);
	~AirsideCircuitFlightProcess2(void);

	//-------------------------------------------------------------
	//Summary:
	//			Process order: start-->intension-->end
	//-------------------------------------------------------------
	bool Process(ClearanceItem& lastItem, Clearance& newClearance);

private:
	bool StartBegine()const;
	bool SIDEnd()const;
	bool _ProcessIntenion(ClearanceItem& lastItem, Clearance& newClearance);
	bool _ProcessEnd(ClearanceItem& lastItem, Clearance& newClearance);

private:
	ICircuteFlightProcess* m_pIntensionProcess;
	IFlightProcess* m_pBeginProcess;
	IFlightProcess* m_pEndProcess;
	
};

class CircuteFlightBeginStandProcess: public ICircuteFlightProcess
{
public:
	CircuteFlightBeginStandProcess(AirsideCircuitFlightInSim* pFlight)
		:ICircuteFlightProcess(pFlight){
		m_pBeginOutBoundProcess = NULL;
		m_pCircuteRouteProcess = NULL;
	}
	bool Process(ClearanceItem& lastItem, Clearance& newClearance);
	~CircuteFlightBeginStandProcess(){  delete m_pBeginOutBoundProcess;delete m_pCircuteRouteProcess;}

protected:
	IFlightProcess* m_pBeginOutBoundProcess;
	IFlightProcess* m_pCircuteRouteProcess;
};

//end SID Process from heading to SID
class CircuteFligthEndSIDProcess : public ICirciteIntensionProcess
{
public:
	CircuteFligthEndSIDProcess(AirsideCircuitFlightInSim* pFlight);
	bool DoActivate(ClearanceItem& lastItem, Clearance& newClearance);
	~CircuteFligthEndSIDProcess(){ cpputil::autoPtrReset(m_pOutBound); }
protected:
	int m_firstMode;
	IFlightProcess* m_pOutBound;
};

//from last process to stand
class CircuteFlightEndStandProcess : public ICirciteIntensionProcess
{
public:
	CircuteFlightEndStandProcess(AirsideCircuitFlightInSim* pFlight):ICirciteIntensionProcess(pFlight){}
	bool DoActivate(ClearanceItem& lastItem, Clearance& newClearance);
	bool DoProcess( ClearanceItem& lastItem, Clearance& newClearance );
};

//out bound for full stop and go
class TaxiRouteInSim;
class OutBoundRouteProcess : public ICircuteFlightProcess
{
public:
	OutBoundRouteProcess(AirsideCircuitFlightInSim* pFlight,TaxiRouteInSim * pOutBound ):ICircuteFlightProcess(pFlight){ m_pOutBound = pOutBound;  }
	bool DoProcess( ClearanceItem& lastItem, Clearance& newClearance );
	bool DoActivate(ClearanceItem& lastItem, Clearance& newClearance);
protected:
	TaxiRouteInSim * m_pOutBound;
};
