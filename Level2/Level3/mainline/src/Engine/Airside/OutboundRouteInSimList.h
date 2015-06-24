#pragma once
#include "../../InputAirside/OutboundRouteAssignment.h"
#include "TaxiwayResource.h"
#include <iostream>
#include <vector>
using namespace std;

class TaxiwayResourceManager;
class IntersectionNodeInSim;
class StandInSim;
enum OUTBOUNDROUTEASSIGNMENTSTATUS;
class COutboundRouteInSim;
class StandInSim;
class RunwayExitInSim;

class ENGINE_TRANSFER COutboundRouteInSimList
{
public:
	COutboundRouteInSimList(void);
	~COutboundRouteInSimList(void);

	BOOL Init(int nPorjID, int nAirPortID, TaxiwayResourceManager * pTaxiwayRes);
	OUTBOUNDROUTEASSIGNMENTSTATUS GetOutboundRouteType();
	BOOL GetOutboundRoute(RunwayExitInSim *pRWExitInSim, StandInSim* pStand, TaxiwayDirectSegList& taxiwayDirectSegList);

private:
	BOOL FindBestFitOutBoundRoute(COutboundRouteInSim **ppBestFitOutBoundRouteInSim, StandInSim *pStandInSim, RunwayExitInSim *pRwExitInSim);

private:
	TaxiwayResourceManager         * m_pTaxiRes;
	OUTBOUNDROUTEASSIGNMENTSTATUS  m_enumOutboundRouteType;
	vector<COutboundRouteInSim*>    m_OutboundRouteInSimList;
};