#pragma once
#include "../EngineDll.h"
#include "../../InputAirside/OutboundRouteAssignment.h"
#include "TaxiwayResource.h"
#include <iostream>
#include <vector>

using namespace std;
class TaxiwayResourceManager;
class IntersectionNodeInSim;
class StandInSim;
class COutboundRoute;
class COutboundRouteItem;
class CInOutboundRouteBranchInSim;
class CAirportGroundNetwork;
class GroundRouteItem;

class ENGINE_TRANSFER COutboundRouteInSim
{
public:
	COutboundRouteInSim(TaxiwayResourceManager *pTaxiRes, COutboundRoute *pOutboundRoute);
	~COutboundRouteInSim(void);

	BOOL GetBranch(StandInSim* pStandInSim, TaxiwayDirectSegList& taxiwayDirectSegList);
	int GetBranchCount();
	COutboundRoute* GetOutboundRouteInput();

	int GetRunwayExitID();
	int GetStandGroupID();

private:
	void InitOutboundRouteBranchInSimList(CInOutboundRouteBranchInSim *pInOutboundRouteBranchInSim,COutboundRouteItem *pOutboundRouteItem);
	void AddTaxiwaySegmentToOutboundRouteBranchInSim(CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim, int nTaxiwayID, int nIntersectNodeIDFrom, int nIntersectNodeIDTo);
	void AddTaxiwayInfoToOutboundRouteBranchInSim(CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim, Taxiway *pTaxiway, int nTaxiwayIntersectNodeID, COutboundRouteItem *pOutboundRouteItem);
	void AddGroundRouteInfoToOutboundRouteBranchInSim(CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim,  COutboundRouteItem *pOutboundRouteItem);
	void AddGroundRouteItemInfoToOutboundRouteBranchInsim(GroundRouteItem *pGRItem, int nIntersectNodeID, CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim,  COutboundRouteItem *pOutboundRouteItem);
	void SetTaxiwayIntersectStandID(Taxiway *pTaxiway, int nTaxiwayIntersectNodeID, CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim);

	//True:remove successfully , False:remove error
	BOOL RemoveBranch(CInOutboundRouteBranchInSim * pBranch);

private:
	TaxiwayResourceManager               *m_pTaxiRes;
	COutboundRoute                        *m_pOutboundRoute;
	vector<CInOutboundRouteBranchInSim*> m_OutboundRouteBranchInSimList;   //inboundroute branch
	CAirportGroundNetwork                *m_pAirportGroundNetWork;
};

