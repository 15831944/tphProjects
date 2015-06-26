#pragma once

#include "../../Common/ARCVector.h"
#include "ResourceManager.h"

#include "../../InputAirside/Runway.h"
#include "RunwayInSim.h"
#include "../../InputAirside/WaveCrossingSpecification.h"
#include "../../InputAirside/GoAroundCriteriaDataProcess.h"
#include "ApproachSeparationInSim.h"
#include "TakeoffSeparationInSim.h"

class TaxiwayResourceManager;
class TakeoffQueuesManager;
class FlightGroundRouteResourceManager;
class AircraftClassificationManager;
class RunwayExitDescription;

class ENGINE_TRANSFER RunwayResourceManager : public ResourceManager 
{

friend class AirportResourceManager;

public:
	~RunwayResourceManager();
	bool Init(int nPrjId, int nAirportId, AircraftClassificationManager* pAircraftClassification);

	//init relations with other resources;
	bool InitRelations(IntersectionNodeInSimList& NodeList, 
		FlightGroundRouteResourceManager * pGroundRouteResource,
		TakeoffQueuesManager * pTakeoffQueues,
		TaxiwayResourceManager& taxiwayResManager);
	
	LogicRunwayInSim * GetLogicRunway(int nrwId, RUNWAY_MARK rwMark);

	const RunwayInSim * GetRunwayByID(int rwid)const;
	RunwayInSim * GetRunwayByID(int rwid);
	RunwayExitInSim* GetExitByRunwayExitDescription(const RunwayExitDescription& runwayDes);
	RunwayExitInSim* GetRunwayExitByID(int nExitID);

	int GetRunwayCount() const{ return m_vRunways.size(); }
	RunwayInSim * GetRunwayByIndex(int idx){ return m_vRunways.at(idx).get(); }


	std::vector<RunwayInSim::RefPtr>& getRunwayResource();
	RunwayInSim * GetRunwayByIdx( int rwidx );

	bool IsNodeOnRunway(int nRunwayID, int nIntersectNodeID)const;
	void GetRunwaySegments(int nRunwayID, int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegLst);



	FlightGroundRouteDirectSegList GetAllLinkedDirectSegments( IntersectionNodeInSim * pNode );
	FlightGroundRouteDirectSegList GetLinkedDirectSegments( IntersectionNodeInSim * pNode );

	void GetRunwaySegmentss(int nRunwayID, int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegLst);

	CApproachSeparationInSim* GetApproachSeparation(){return &m_pApproachSeparationInSim;}
	CTakeoffSeparationInSim* GetTakeoffSeparation(){return &m_pTakeoffSeparationInSim; }

protected:
	//init wave crossing specifications
	void InitWaveCrossing(RunwayInSim * pRunwayInSim,TaxiwayResourceManager& taxiwayResManager);
	
protected:
	std::vector<RunwayInSim::RefPtr> m_vRunways;
	CWaveCrossingSpecification m_waveCrossingSpec;
	CGoAroundCriteriaDataProcess m_GoAroundSpecs;	

	CApproachSeparationInSim m_pApproachSeparationInSim;
	CTakeoffSeparationInSim m_pTakeoffSeparationInSim;


};