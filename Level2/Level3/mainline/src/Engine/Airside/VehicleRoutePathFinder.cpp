#include "stdafx.h"
#include "VehicleRoutePathFinder.h"
#include "VehicleStretchInSim.h"
#include "VehicleRoadIntersectionInSim.h"
#include "VehicleRouteInSim.h"
#include "VehicleRouteUserDefineInSim.h"
#include <algorithm>
#include "../BoostPathFinder.h"
CVehicleRoutePathFinder::CVehicleRoutePathFinder()
{
	mpPathFinder = NULL;
}

int CVehicleRoutePathFinder::GetNodeIndex( VehicleRouteNode* pNode ) const
{
	std::vector<VehicleRouteNode*>::const_iterator itr = std::find(mNodes.begin(), mNodes.end(), pNode);
	if(itr!=mNodes.end())
		return itr - mNodes.begin();
	return -1;
	
}

VehicleRouteNode* CVehicleRoutePathFinder::GetNode( int idx ) const
{
	return mNodes[idx];
}

void CVehicleRoutePathFinder::Init( const std::vector<VehicleStretchInSim*>& vStretches,const std::vector<VehicleRoadIntersectionInSim*>& vIntersections )
{
	
	m_vStretches = vStretches;
	m_vIntersections = vIntersections;
	//////////////////////////////////////////////////////////////////////////	
	for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
	{
		VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
		for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
		{
			VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
			pSeg->InitGraph();
			for( int i=0;i< pSeg->GetEdgeCount(); i++)
			{
				VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);				
				AddNode(entryExit.GetNodeFrom());
				AddNode(entryExit.GetNodeTo());
			}
		}
	}

	for( int nodeidx = 0; nodeidx <(int)m_vIntersections.size();nodeidx++)
	{
		VehicleRoadIntersectionInSim* pRoadIntersection = m_vIntersections.at(nodeidx);
		pRoadIntersection->InitGraph();
		/*for( int i=0;i< pRoadIntersection->GetEdgeCount();i++)
		{
			VehicleRouteNodePairDist exitEntry = pRoadIntersection->GetEdge(i);			
			AddNode(exitEntry.GetNodeFrom());
			AddNode(exitEntry.GetNodeTo());
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	//
	if(mpPathFinder)
	{
		delete mpPathFinder;
		mpPathFinder = NULL;
	}
	//

	if(false)
	{
		CBoostAStarPathFinder* pAstarFinder = new CBoostAStarPathFinder();
		CBoostAStarPathFinder::VertexLocationMap& vertexLocMap = pAstarFinder->GetVertexLocation();
		vertexLocMap.clear();
		vertexLocMap.reserve(mNodes.size());
		for(int i=0;i<(int)mNodes.size();i++)
		{
			VehicleRouteNode* pNode = mNodes[i];
			vertexLocMap.push_back( pNode->GetPosition() );
		}		
		mpPathFinder = pAstarFinder;
	}
	else
	{
		CBoostDijPathFinder* pDijFinder = new CBoostDijPathFinder();
		mpPathFinder = pDijFinder;
	}

	//////////////////////////////////////////////////////////////////////////
	//init boost
	CBoostDirectedGraph& bg = mpPathFinder->mGraph;
	bg.Init(mNodes.size());
	for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
	{
		VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
		for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
		{
			VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);			
			for( int i=0;i< pSeg->GetEdgeCount(); i++)
			{
				VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);	ASSERT(entryExit.GetNodeFrom()->GetType()!=entryExit.GetNodeTo()->GetType());			
				int nodeidx1 = GetNodeIndex(entryExit.GetNodeFrom());
				int nodeidx2 = GetNodeIndex(entryExit.GetNodeTo());
				bg.AddEdge(nodeidx1, nodeidx2, entryExit.GetDistance() );
			}
		}
	}

	for( int nodeidx = 0; nodeidx <(int)m_vIntersections.size();nodeidx++)
	{
		VehicleRoadIntersectionInSim* pRoadIntersection = m_vIntersections.at(nodeidx);		
		for( int i=0;i< pRoadIntersection->GetEdgeCount();i++)
		{
			VehicleRouteNodePairDist exitEntry = pRoadIntersection->GetEdge(i);		ASSERT(exitEntry.GetNodeFrom()->GetType()!=exitEntry.GetNodeTo()->GetType());	
			int nodeidx1 = GetNodeIndex(exitEntry.GetNodeFrom());
			int nodeidx2 = GetNodeIndex(exitEntry.GetNodeTo());
			bg.AddEdge(nodeidx1, nodeidx2, exitEntry.GetDistance() );
		}
	}
}

void CVehicleRoutePathFinder::Init( const std::set<VehicleStretchSegmentInSim*>& vStretches,const std::vector<VehicleRoadIntersectionInSim*>& vIntersections )
{

	m_vIntersections = vIntersections;
	//////////////////////////////////////////////////////////////////////////	
	//for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
	//{
	//	VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
	//	for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
	//	{
	//		VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
	//		pSeg->InitGraph();
	//		for( int i=0;i< pSeg->GetEdgeCount(); i++)
	//		{
	//			VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);				
	//			AddNode(entryExit.GetNodeFrom());
	//			AddNode(entryExit.GetNodeTo());
	//		}
	//	}
	//}
	{
		std::set<VehicleStretchSegmentInSim*>::const_iterator iter = vStretches.begin();
		for (;iter != vStretches.end(); ++iter)
		{
			VehicleStretchSegmentInSim* pSeg = *iter;
			for( int i=0;i< pSeg->GetEdgeCount(); i++)
			{
				VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);				
				AddNode(entryExit.GetNodeFrom());
				AddNode(entryExit.GetNodeTo());
			}
		}

	}

	for( int nodeidx = 0; nodeidx <(int)m_vIntersections.size();nodeidx++)
	{
		VehicleRoadIntersectionInSim* pRoadIntersection = m_vIntersections.at(nodeidx);
		//pRoadIntersection->InitGraph();
		for( int i=0;i< pRoadIntersection->GetEdgeCount();i++)
		{
			VehicleRouteNodePairDist exitEntry = pRoadIntersection->GetEdge(i);			
			AddNode(exitEntry.GetNodeFrom());
			AddNode(exitEntry.GetNodeTo());
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//
	if(mpPathFinder)
	{
		delete mpPathFinder;
		mpPathFinder = NULL;
	}
	//
	if(false){
		CBoostAStarPathFinder* pAstarFinder = new CBoostAStarPathFinder();
		CBoostAStarPathFinder::VertexLocationMap& vertexLocMap = pAstarFinder->GetVertexLocation();
		vertexLocMap.clear();
		vertexLocMap.reserve(mNodes.size());
		for(int i=0;i<(int)mNodes.size();i++)
		{
			VehicleRouteNode* pNode = mNodes[i];
			vertexLocMap.push_back( pNode->GetPosition() );
		}		
		mpPathFinder = pAstarFinder;
	}
	else{
		CBoostDijPathFinder* pDijFinder = new CBoostDijPathFinder;
		mpPathFinder = pDijFinder;
	}

	//////////////////////////////////////////////////////////////////////////
	//init boost
	CBoostDirectedGraph& bg = mpPathFinder->mGraph;
	bg.Init(mNodes.size());
	//for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
	{
		//VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
		//for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
		//{
		std::set<VehicleStretchSegmentInSim*>::const_iterator iter = vStretches.begin();
		for (;iter != vStretches.end(); ++iter)
		{
			VehicleStretchSegmentInSim* pSeg = *iter;			
			for( int i=0;i< pSeg->GetEdgeCount(); i++)
			{
				VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);	
				
				int nodeidx1 = GetNodeIndex(entryExit.GetNodeFrom());
				int nodeidx2 = GetNodeIndex(entryExit.GetNodeTo());
				bg.AddEdge(nodeidx1, nodeidx2, entryExit.GetDistance() );
			}
		}
		//}
	}

	for( int nodeidx = 0; nodeidx <(int)m_vIntersections.size();nodeidx++)
	{
		VehicleRoadIntersectionInSim* pRoadIntersection = m_vIntersections.at(nodeidx);		
		for( int i=0;i< pRoadIntersection->GetEdgeCount();i++)
		{
			VehicleRouteNodePairDist exitEntry = pRoadIntersection->GetEdge(i);		

			
			int nodeidx1 = GetNodeIndex(exitEntry.GetNodeFrom());
			int nodeidx2 = GetNodeIndex(exitEntry.GetNodeTo());
			bg.AddEdge(nodeidx1, nodeidx2, exitEntry.GetDistance() );
		}
	}
}
int CVehicleRoutePathFinder::AddNode( VehicleRouteNode* pNode )
{
	int nIdx = GetNodeIndex(pNode);
	if(nIdx<0)
	{
		mNodes.push_back(pNode);
		return mNodes.size()-1;
	}
	
	return nIdx;
}


DistanceUnit GetRouteLength(const std::vector<VehicleRouteNodePairDist>& routeRet)
{
	DistanceUnit distLen = 0;
	for(int i=0;i<(int)routeRet.size();i++)
	{
		distLen+= routeRet.at(i).GetDistance();
	}
	return distLen;
}
bool CVehicleRoutePathFinder::GetRoute(  AirsideResource * pResFrom, AirsideResource* pResTo, std::vector<VehicleRouteNodePairDist>& routeRet )
{
	typedef std::vector<VehicleRouteNode*> VehicleRouteNodeVector;
	
	VehicleRouteNodeVector vFromNodes;
	VehicleRouteNodeVector vToNodes;

	for(VehicleRouteNodeVector::iterator nodeItr = mNodes.begin(); nodeItr!= mNodes.end();nodeItr++)
	{	
		VehicleRouteNode* pNode = *nodeItr;

		if(pNode == NULL || pNode->GetOrignResource() == NULL || pNode->GetDestResource() == NULL)
			continue;

		if( pNode->GetOrignResource() == pResFrom )
		{
			if(pResFrom->GetType() == AirsideResource::ResType_VehicleStretchLane )
			{
				if( pNode->GetDestResource() == pResTo 
					|| pNode->GetDestResource()->GetType() == AirsideResource::ResType_VehicleRoadIntersection )
				{
					if(vFromNodes.size()==0)
						vFromNodes.push_back(pNode);
				}
			}
			else
				vFromNodes.push_back(pNode);
		}
		if( pNode->GetDestResource() == pResTo )
		{
			vToNodes.push_back(pNode);
		}
	}


		
	bool bFindRoute = false;
	DistanceUnit distMin;
	for(VehicleRouteNodeVector::iterator itr = vFromNodes.begin();itr!=vFromNodes.end();itr++)
	{
		for(VehicleRouteNodeVector::iterator itrTo = vToNodes.begin();itrTo!=vToNodes.end();itrTo++)
		{
			std::vector<VehicleRouteNodePairDist> result;
			if( GetRoute(*itr, *itrTo, result) )
			{
				DistanceUnit rDist= GetRouteLength(result);
				if(!bFindRoute)
				{
					bFindRoute = true;					
					distMin = rDist;
					routeRet = result;
				}
				else
				{
					if(rDist<distMin)
					{
						distMin = rDist;
						routeRet = result;
					}
				}
			}
		}
	}
	
	
	return bFindRoute;
}

bool CVehicleRoutePathFinder::GetRoute( VehicleRouteNode* pNodeF,VehicleRouteNode* pNodeT, std::vector<VehicleRouteNodePairDist>& routeRet )
{
	int srcIndex = GetNodeIndex(pNodeF);
	int destIndex = GetNodeIndex(pNodeT);

	ASSERT(srcIndex>=0 && destIndex>=0);
	ASSERT(mpPathFinder);

	if(mpPathFinder && srcIndex>=0 && destIndex>=0 )
	{
		CBoostDirectedGraph& bg = mpPathFinder->mGraph;
		std::vector<myvertex_descriptor> vRetPath;
		double dPathWeight = (std::numeric_limits<double>::max)();
		if( mpPathFinder->FindPath(srcIndex, destIndex, vRetPath, dPathWeight) )
		{
			routeRet.clear();
			for(int i=0;i<(int)vRetPath.size()-1;i++)
			{
				int nNodeidx1 = vRetPath[i];
				int nNodeidx2 = vRetPath[i+1];
				VehicleRouteNode* pNode1 = GetNode(nNodeidx1);
				VehicleRouteNode* pNode2 = GetNode(nNodeidx2);							

				double dEdgeWeight = bg.GetEdgeWeight(nNodeidx1, nNodeidx2);	
				ASSERT(pNode1->GetType()!=pNode2->GetType());
				routeRet.push_back( VehicleRouteNodePairDist(pNode1,pNode2, dEdgeWeight) );
			}
			return true;
		}
	}
	return false;
}
bool CVehicleRoutePathFinder::GetEntryExitNode( AirsideResource * pResFrom, AirsideResource* pResTo,std::vector<VehicleRouteNode* >& vFromNodes,std::vector<VehicleRouteNode*>& vToNodes )
{
	for(std::vector<VehicleRouteNode*>::iterator nodeItr = mNodes.begin();  nodeItr!= mNodes.end();nodeItr++)
	{
		VehicleRouteNode* pNode = *nodeItr;
		if( pNode->GetOrignResource() == pResFrom )
		{
			vFromNodes.push_back(pNode);
		}
		if( pNode->GetDestResource() == pResTo )
		{
			vToNodes.push_back(pNode);
		}
	}
	return true;
}

CVehicleRoutePathFinder::~CVehicleRoutePathFinder()
{
	delete mpPathFinder;
	mpPathFinder = NULL;
}