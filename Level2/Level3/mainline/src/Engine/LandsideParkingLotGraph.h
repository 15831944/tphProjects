#pragma once
#include "Common/ARCVector.h"
#include "Common/ARCPath.h"
#include "LandsideResourceLinkNode.h"
#include "LandsideParkingLotInSim.h"
#include "Landside/LandsidePath.h"


class CBoostPathFinder;

class LandsideParkinglotDriveLaneInSim;
class LandsideParkingLotDrivePipeInSim;
class CWalkwayInSim;


class DrivePathInParkingLot : public LandsidePath
{
public:
	void AddSeg(LandsideParkingLotDrivePipeInSim* pPipe , DistanceUnit distF, DistanceUnit distT);
};

class LandsideParkingLotGraph
{
public:
	LandsideParkingLotGraph(void);
	~LandsideParkingLotGraph(void);


	bool FindPath(LandsideResourceInSim* pFrom, LandsideResourceInSim* pTo,DrivePathInParkingLot& path);

	void BuildGraph();
	//
	void AddNode(const LandsideResourceLinkNode& node){ m_vNodeList.push_back(node); }

	//init graph
protected:
	
	CBoostPathFinder * mpPathFinder;
	typedef std::vector<LandsideResourceLinkNode> DriveNodeList;
	DriveNodeList m_vNodeList;
};
