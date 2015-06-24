#include "StdAfx.h"
#include ".\landsideparkinglotgraph.h"
#include "BoostPathFinder.h"
#include "LandsideParkingLotInSim.h"
#include "LandsideVehicleInSim.h"

LandsideParkingLotGraph::LandsideParkingLotGraph(void)
{
	mpPathFinder = new CBoostDijPathFinder();
}

LandsideParkingLotGraph::~LandsideParkingLotGraph(void)
{
	delete mpPathFinder;
	mpPathFinder =NULL;
}

typedef std::vector<int> IndexList;
void LandsideParkingLotGraph::BuildGraph()
{
	typedef cpputil::nosort_map< LandsideResourceInSim*,IndexList > ResourceNodeMap;
	ResourceNodeMap EntryNodesMap;
	ResourceNodeMap ExitNodesMap;

	for(int i=0;i<(int)m_vNodeList.size();i++){
		LandsideResourceLinkNode& node = m_vNodeList[i];
		IndexList& fromNodes = EntryNodesMap[node.m_pTo];
		fromNodes.push_back(i);

		IndexList& toNodes = ExitNodesMap[node.m_pFrom];
		toNodes.push_back(i);
	}

	mpPathFinder->mGraph.Init(m_vNodeList.size());
	for(ResourceNodeMap::const_iterator itr=EntryNodesMap.begin();itr!=EntryNodesMap.end(); itr++)
	{
		LandsideResourceInSim* pRes = itr->first;
		const IndexList& idxEntryNodes = itr->second;
		if(LandsideParkingLotDrivePipeInSim* pDrivePipe = pRes->toParkDrivePipe())
		{
			IndexList& idxExitNodes = ExitNodesMap[pDrivePipe];
			for(int i=0;i<(int)idxEntryNodes.size();++i)
			{
				LandsideResourceLinkNode& nodeEntry = m_vNodeList[idxEntryNodes[i]];
				for(int j=0;j<(int)idxExitNodes.size();++j)
				{
					LandsideResourceLinkNode& nodeExit =m_vNodeList[idxExitNodes[j]];
					if(nodeEntry.m_distT < nodeExit.m_distF) //add edge
					{
						mpPathFinder->mGraph.AddEdge(idxEntryNodes[i],idxExitNodes[j],nodeExit.m_distF-nodeEntry.m_distT);						
					}
					else if(pDrivePipe->IsBiDirection()){
						mpPathFinder->mGraph.AddEdge(idxEntryNodes[i],idxExitNodes[j], nodeEntry.m_distT - nodeExit.m_distF);
					}
				}
			}			
		
		}		
	}
}

bool LandsideParkingLotGraph::FindPath( LandsideResourceInSim* pFrom, LandsideResourceInSim* pTo,DrivePathInParkingLot& path )
{
	IndexList fromNodes;
	IndexList toNodes;
	for(int i=0;i<(int)m_vNodeList.size();i++)
	{
		LandsideResourceLinkNode& node = m_vNodeList[i];		
		if(node.m_pFrom==pFrom)
			fromNodes.push_back(i);		
		if(node.m_pTo == pTo)
			toNodes.push_back(i);
	}
	for(size_t i=0;i<fromNodes.size();++i)
		for(size_t j=0;j<toNodes.size();++j)
		{
			int idxF = fromNodes[i];
			int idxT = toNodes[j];
			std::vector<myvertex_descriptor> retpath;double pathLen;
			if( mpPathFinder->FindPath(idxF,idxT,retpath,pathLen) )
			{
				//build path
				for(size_t k = 0;k<retpath.size()-1;++k)
				{
					int idx1 = retpath[k];
					int idx2 = retpath[k+1];
					LandsideResourceLinkNode& node1 = m_vNodeList[idx1];
					LandsideResourceLinkNode& node2 = m_vNodeList[idx2];
					ASSERT(node1.m_pTo == node2.m_pFrom);
					if(LandsideParkingLotDrivePipeInSim* pPipe = node1.m_pTo->toParkDrivePipe() )
					{
						path.AddSeg(pPipe,node1.m_distT, node2.m_distF);	
					}					
				}				
				//path.Update();
				return true;
			}
		}

	return false;

}


void DrivePathInParkingLot::AddSeg( LandsideParkingLotDrivePipeInSim* pPipe , DistanceUnit distF, DistanceUnit distT )
{
	LandsideParkinglotDriveLaneInSim* plane = pPipe->getLane(distF>distT);
	if(!plane)
		return;
	DistanceUnit lanedistF = plane->getPath().GetPointDist(pPipe->getPath().GetDistPoint(distF));
	DistanceUnit lanedistT = plane->getPath().GetPointDist(pPipe->getPath().GetDistPoint(distT));
 	CPath2008 path = plane->getPath().GetSubPath(lanedistF,lanedistT); 
	addPath(plane,&path,lanedistF);
}


