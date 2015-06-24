#include "StdAfx.h"
#include "TaxiwayResource.h"
#include "../../Common/ARCVector.h"
#include "./AirsideFlightInSim.h"
#include "TaxiwayConstraintsManager.h"
#include "TaxiwaySegConstraintsProperties.h"
#include "AirsideEvent.h"
#include "HoldShortLineInSim.h"
#include "AirsideMeetingPointInSim.h"
#include "../../InputAirside/HoldShortLines.h"
#include "../../InputAirside/MeetingPoint.h"
#include "StartPositionInSim.h"
#include "../../InputAirside/StartPosition.h"
#include "AirsideResourceManager.h"
#include "AirTrafficController.h"
#include "DynamicConflictGraph.h"
#include "..\..\Results\AirsideFlightEventLog.h"
//class ENGINE_TRANSFER TaxiSegReleaseDirLockEvent : public AirsideEvent
//{
//public:
//	TaxiSegReleaseDirLockEvent(TaxiwaySegInSim* pSeg,AirsideFlightInSim* pFlight){
//		m_pSeg = pSeg; m_pFlight = pFlight;
//	}
//
//	virtual const char *getTypeName (void) const { return "ReleaseDirectionLock";}
//
//	virtual int getEventType (void) const {return FlightGetClearnce;}
//
//	int Process()
//	{
//		m_pSeg->GetDirectionLock()->ReleaseLock(m_pFlight, getTime() );
//		return TRUE;
//	}
//protected:
//	TaxiwaySegInSim * m_pSeg;
//	AirsideFlightInSim* m_pFlight;
//};




CPoint2008 TaxiwaySegInSim::GetDistancePoint( double dist ) const
{	
	return m_pTaxiway->GetTaxiwayInput()->GetPath().GetDistPoint(dist );
}




TaxiwaySegInSim::TaxiwaySegInSim( IntersectionNodeInSim * pNodeFrom , IntersectionNodeInSim * pNodeTo, TaxiwayInSim * pTaxiway )
:FlightGroundRouteSegInSim(pNodeFrom,pNodeTo,pTaxiway)
{
	//m_pNode1 = pNodeFrom;
	//m_pNode2 = pNodeTo;
	m_pTaxiway = pTaxiway;	
	m_positiveDirSeg = new TaxiwayDirectSegInSim(this, PositiveDirection);
	m_negativeDirSeg = new TaxiwayDirectSegInSim(this, NegativeDirection);

	m_pConstraintsProperties = new TaxiwaySegConstraintsProperties();
	//m_pDirLock = new TaxiwaySegDirectionLock(this);
	m_direction = BothDirection;
	m_vHoldShortLines.clear();
	
}

int TaxiwaySegInSim::GetTaxiwayID()
{ 
	return m_pTaxiway->GetTaxiwayID();
}

TaxiwaySegInSim::~TaxiwaySegInSim()
{
	//delete m_pDirLock;
	delete m_pConstraintsProperties;
}

//constraints interface
void TaxiwaySegInSim::AddWeightWingspanConstraint(TaixwayConstraintType type, int nValue)
{
	m_pConstraintsProperties->AddWeightWingspanConstraint(type, nValue);
}

void TaxiwaySegInSim::AddFlightTypeRestrictions(FlightConstraint& fltCons)
{
	m_pConstraintsProperties->AddFlightTypeRestrictions(fltCons);
}

void TaxiwaySegInSim::SetDirectionConstraintType(DirectionalityConstraintType dir)
{
	m_pConstraintsProperties->SetDirectionConstraintType(dir);
}
void TaxiwaySegInSim::AddClosureTimeConstraint(ElapsedTime epStartTime,ElapsedTime epEndTime)
{
	m_pConstraintsProperties->AddClosureTimeConstraint(epStartTime,epEndTime);
}
void TaxiwaySegInSim::AddTaxiSpeedConstraint( const FlightConstraint& fltCons, double dSpeed )
{
	m_pConstraintsProperties->AddTaxiSpeedConstraint(fltCons, dSpeed);
}
void TaxiwaySegInSim::SetBearingDegree(double degree)
{
	m_pConstraintsProperties->SetBearingDegree(degree);
}

double TaxiwaySegInSim::GetNode1Pos() const
{
	return m_pNode1->GetNodeInput().GetDistance(m_pTaxiway->GetTaxiwayInput()->getID());
}

double TaxiwaySegInSim::GetNode2Pos() const
{
	return m_pNode2->GetNodeInput().GetDistance(m_pTaxiway->GetTaxiwayInput()->getID());
}

void TaxiwaySegInSim::SetHoldShortLine(TaxiInterruptLineInSim* pHoldShortLine)
{
	if (std::find(m_vHoldShortLines.begin(),m_vHoldShortLines.end(),pHoldShortLine) != m_vHoldShortLines.end())
		return;

	m_vHoldShortLines.push_back(pHoldShortLine) ;
}

const std::vector<TaxiInterruptLineInSim*>& TaxiwaySegInSim::GetHoldShortLine()
{
	return m_vHoldShortLines;
}

////////////////////////////////////////////////////////////
TaxiwayInSim::TaxiwayInSim( Taxiway* pTaxiwayInput )
:FlightGroundRouteInSim(pTaxiwayInput->GetWidth(), pTaxiwayInput->getID(),pTaxiwayInput->GetPath())
{
	m_pTaxiwayInput = pTaxiwayInput;	
}
int TaxiwayInSim::GetTaxiwayID()
{ 
	return m_pTaxiwayInput->getID();
}
TaxiwayDirectSegInSim * TaxiwayInSim::GetTaxiwayDirectSegment( IntersectionNodeInSim* pNodeFrom, IntersectionNodeInSim * pNodeTo )
{
	for(int i =0 ; i < GetSegmentCount(); ++i)
	{
		TaxiwaySegInSim * pSegment = GetSegment(i);
		TaxiwayDirectSegInSim *pDirSeg = ( TaxiwayDirectSegInSim *)pSegment->GetPositiveDirSeg();
		if( pDirSeg )
		{
			if(pDirSeg->GetEntryNode() == pNodeFrom && pDirSeg->GetExitNode()==pNodeTo ) return pDirSeg;
		}
		pDirSeg = ( TaxiwayDirectSegInSim *)pSegment->GetNegativeDirSeg();
		if( pDirSeg )
		{
			if( pDirSeg->GetEntryNode() == pNodeFrom && pDirSeg->GetExitNode()== pNodeTo ) return pDirSeg;
		}	
		
	}
	return NULL;
}

void TaxiwayInSim::GetTaxiwayDirectSegmentList(int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegList)
{	
	{//find positive dir segments
		FlightGroundRouteDirectSegList vFindreslt;
		bool bStartFlag = false;
		bool bEndFlag = false;
		for(int i=0;i<(int) m_vSegments.size();++i)
		{
			FlightGroundRouteDirectSegInSim* pPosSeg = m_vSegments[i]->GetPositiveDirSeg();

			if(pPosSeg->GetEntryNode()->GetID() == nIntersectNodeIDFrom)
			{
				bStartFlag = true;
			}		

			if(bStartFlag && !bEndFlag )
				vFindreslt.push_back(pPosSeg);

			if(pPosSeg->GetExitNode()->GetID() == nIntersectNodeIDTo && bStartFlag)
			{
				bEndFlag  = true;
			}

			if(bStartFlag  && bEndFlag && !vFindreslt.empty())
			{
				taxiwayDirectSegList.insert(taxiwayDirectSegList.end(), vFindreslt.begin(),vFindreslt.end());
				return ;
			}
		}
	}
	
	//	
	{//find negative dir segments
		FlightGroundRouteDirectSegList vFindreslt;
		bool bStartFlag = false;
		bool bEndFlag = false;
		std::vector<TaxiwaySegInSim*>::reverse_iterator revIter;
		for(revIter = m_vSegments.rbegin(); revIter!=m_vSegments.rend(); ++revIter)
		{
			FlightGroundRouteDirectSegInSim* pNegSeg = (*revIter)->GetNegativeDirSeg();
			if(pNegSeg->GetEntryNode()->GetID() == nIntersectNodeIDFrom)
			{
				bStartFlag = true;
			}		

			if(bStartFlag && !bEndFlag )
				vFindreslt.push_back(pNegSeg);

			if(pNegSeg->GetExitNode()->GetID() == nIntersectNodeIDTo && bStartFlag)
			{
				bEndFlag  = true;
			}

			if(bStartFlag  && bEndFlag && !vFindreslt.empty())
			{
				taxiwayDirectSegList.insert(taxiwayDirectSegList.end(), vFindreslt.begin(),vFindreslt.end());
				return ;
			}
		}
	}
	
}

bool TaxiwayInSim::includeSeg(const FlightGroundRouteDirectSegList& segList,TaxiwaySegInSim* pSegment,FlightGroundRouteSegDirection& emType)
{
	for (size_t i = 0; i < segList.size(); i++)
	{
		TaxiwayDirectSegInSim *pTwDirectSegInSim = (TaxiwayDirectSegInSim *)segList.at(i);
		if (pTwDirectSegInSim->GetTaxiwaySeg() == pSegment)
		{
			if (pSegment->GetPositiveDirSeg() == pTwDirectSegInSim)
			{
				emType = PositiveDirection;
			}
			else
			{
				emType = NegativeDirection;
			}
			return true;
		}
	}
	return false;
}

//In_parameter: user input data that init every taxiway direct segment
//taxiway constraint include two case: parallel and negative
void TaxiwayInSim::InitTaxiwayAdjacencyConstraints(const CWingspanAdjacencyConstraintsList* pAdjacencyConstraint)
{
	for (int i = 0; i < pAdjacencyConstraint->GetCount(); i++)
	{
		CWingspanAdjacencyConstraints* pConstraint = pAdjacencyConstraint->GetConstraint(i);
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstrain = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstrain = pConstraint->m_secondTaxiway;

		//find fit first record
		if (firstConstrain.m_pTaxiway.getID() == GetTaxiwayInput()->getID())
		{
			FlightGroundRouteDirectSegList segList;
			GetTaxiwayDirectSegmentList(firstConstrain.m_startNode.GetID(),firstConstrain.m_endNode.GetID(),segList);
			AdjacencyConstraint(pConstraint,segList,true);
		}
		//find fit second record
		if (secondConstrain.m_pTaxiway.getID() == GetTaxiwayInput()->getID())
		{
			FlightGroundRouteDirectSegList segList;
			GetTaxiwayDirectSegmentList(secondConstrain.m_startNode.GetID(),secondConstrain.m_endNode.GetID(),segList);
			AdjacencyConstraint(pConstraint,segList,false);
		}
	}
}

void TaxiwayInSim::AdjacencyConstraint(CWingspanAdjacencyConstraints* constraint,const FlightGroundRouteDirectSegList& segList,bool firstConstraint)
{
	for(int i =0 ; i < GetSegmentCount(); ++i)
	{
		TaxiwaySegInSim * pSegment = GetSegment(i);
		FlightGroundRouteSegDirection emType;
		if (includeSeg(segList,pSegment,emType))
		{
			if (emType == PositiveDirection)
			{
				TaxiwayDirectSegInSim *pDirSeg = ( TaxiwayDirectSegInSim *)pSegment->GetPositiveDirSeg();
				if( pDirSeg )
				{
					if (firstConstraint)
					{
						pDirSeg->AddFirstAdjacencyParallelConstraint(constraint);
					}
					else
					{
						pDirSeg->AddSecondAdjacencyParallelConstraint(constraint);
					}
					
				}
				pDirSeg = ( TaxiwayDirectSegInSim *)pSegment->GetNegativeDirSeg();
				if( pDirSeg )
				{
					if (firstConstraint)
					{
						pDirSeg->AddFirstAdjacencyNegativeConstraint(constraint);
					}
					else
					{
						pDirSeg->AddSecondAdjacencyNegativeConstraint(constraint);
					}
					
				}	
			}
			else
			{
				TaxiwayDirectSegInSim *pDirSeg = ( TaxiwayDirectSegInSim *)pSegment->GetPositiveDirSeg();
				if( pDirSeg )
				{
					if (firstConstraint)
					{
						pDirSeg->AddFirstAdjacencyNegativeConstraint(constraint);
					}
					else
					{
						pDirSeg->AddSecondAdjacencyNegativeConstraint(constraint);
					}
					
				}
				pDirSeg = ( TaxiwayDirectSegInSim *)pSegment->GetNegativeDirSeg();
				if( pDirSeg )
				{
					if (firstConstraint)
					{
						pDirSeg->AddFirstAdjacencyParallelConstraint(constraint);
					}
					else
					{
						pDirSeg->AddSecondAdjacencyParallelConstraint(constraint);
					}
				}	
			}
		}
	}
}

TaxiwaySegInSim * TaxiwayInSim::GetSegment( int idx )
{
	ASSERT(idx<(int)m_vSegments.size()); 
	return m_vSegments.at(idx);
}

bool TaxiwaySegInSim::CanServeFlight(AirsideFlightInSim *pFlight)
{
	if (m_pConstraintsProperties != NULL)
		return m_pConstraintsProperties->canServe(pFlight);
	return true;
}

DistanceUnit TaxiwaySegInSim::GetSegmentLength() const
{
	return abs(GetNode2Pos() - GetNode1Pos());
}

bool TaxiwaySegInSim::bMobileInResource( ARCMobileElement* pmob ) const
{
	if( GetPositiveDirSeg()->bMobileInPath(pmob))return true;
	if( GetNegativeDirSeg()->bMobileInPath(pmob))return true;
	//if( m_pNode1->GetLockedFlight() == pmob)return true;
	//if( m_pNode2->GetLockedFlight() == pmob) return true;
	return false;
}

void TaxiwaySegInSim::AddMeetingPoints( AirsideMeetingPointInSim* pMeetingPoint )
{
	if (std::find(m_vMeetingPoints.begin(),m_vMeetingPoints.end(), pMeetingPoint) != m_vMeetingPoints.end())
		return;

	m_vMeetingPoints.push_back(pMeetingPoint);
}

const std::vector<AirsideMeetingPointInSim*>& TaxiwaySegInSim::GetMeetingPoints()
{
	return m_vMeetingPoints;
}
bool TaxiwaySegInSim::IsHoldShortLineOnSegment( int nInterruptlineID )
{
	int nCount = m_vHoldShortLines.size();
	for (int i =0; i <nCount; i++)
	{
		TaxiInterruptLineInSim* pLineInSim = m_vHoldShortLines.at(i);
		if (pLineInSim->GetInterruptLineInput()->GetID() == nInterruptlineID)
			return true;
	}

	return false;
}

AirsideMeetingPointInSim* TaxiwaySegInSim::GetMeetingPointInSeg( const std::vector<ALTObjectID>& vObjNames )
{
	int nCount = (int)m_vMeetingPoints.size();
	int nNameCount = (int)vObjNames.size();
	for (int i =0; i < nCount; i++)
	{
		AirsideMeetingPointInSim* pMeetingPoint = m_vMeetingPoints.at(i);
		for (int j =0; j < nNameCount; j++)
		{
			ALTObjectID objName = vObjNames.at(j);
			if (pMeetingPoint->GetMeetingPointInput()->GetObjectName().idFits(objName))
				return pMeetingPoint;
		}
	}

	return NULL;
}

void TaxiwaySegInSim::AddStartPosition( StartPositionInSim* pStartPosition )
{
	if (pStartPosition == NULL)
		return;

	if (std::find(m_vStartPositions.begin(),m_vStartPositions.end(), pStartPosition) != m_vStartPositions.end())
		return;

	m_vStartPositions.push_back(pStartPosition);

}

const std::vector<StartPositionInSim*>& TaxiwaySegInSim::GetStartPostions()
{
	return m_vStartPositions;
}

StartPositionInSim* TaxiwaySegInSim::GetStartPostionInSeg( const std::vector<ALTObjectID>& vObjNames )
{
	size_t nCount = m_vStartPositions.size();
	size_t nNameCount = vObjNames.size();
	for (size_t i =0; i < nCount; i++)
	{
		StartPositionInSim* pStartPos = m_vStartPositions.at(i);
		for (size_t j =0; j < nNameCount; j++)
		{
			ALTObjectID objName = vObjNames.at(j);
			if (pStartPos->GetStartPosition()->GetObjectName().idFits(objName))
				return pStartPos;
		}
	}

	return NULL;
}

bool TaxiwaySegInSim::GetTaxiwayConstrainedSpeed( const AirsideFlightInSim* pFlight, double& dSpeed ) const
{
	return m_pConstraintsProperties->GetTaxiwayConstrainedSpeed(pFlight, dSpeed);
}


class IntersectionNodeInSimCompareDistInTaxiway
{
public:
	IntersectionNodeInSimCompareDistInTaxiway(int nTaxiID){ m_nTaxiId = nTaxiID; }
	bool operator()(IntersectionNodeInSim* pNode1, IntersectionNodeInSim* pNode2){
		return pNode1->GetNodeInput().GetDistance(m_nTaxiId) < pNode2->GetNodeInput().GetDistance(m_nTaxiId);
	}
protected:
	int m_nTaxiId;
};


//void GetMinMaxFilletDistToNode(IntersectionNodeInSim* pNodeInSim,const int& nObjID, DistanceUnit& dMin, DistanceUnit& dMax )
//{
//	std::vector<FilletTaxiway>& vFillets = pNodeInSim->GetFilletList();
//	
//	for(int i=0;i< (int)vFillets.size();i++)
//	{
//		FilletTaxiway& theFillet = vFillets[i];
//		{
//			FilletPoint filetPt;
//			if( theFillet.GetObject1ID() == nObjID )
//			{
//				filetPt = theFillet.GetFilletPoint1();
//			}
//			if( theFillet.GetObject2ID() == nObjID )
//			{
//				filetPt = theFillet.GetFilletPoint2();
//			}
//
//			DistanceUnit distToNode = filetPt.GetDistToIntersect();
//			if( distToNode < 0 && distToNode < dMin )
//			{
//				dMin = distToNode;
//			}
//			if( distToNode > 0 && distToNode > dMax )
//			{
//				dMax = distToNode;
//			}
//		}
//	}
//}


bool TaxiwayInSim::InitSegments( IntersectionNodeInSimList& NodeList,const HoldShortLinePropList& vHoldList )
{
	
	std::vector<IntersectionNodeInSim*> vNodesInSim;

	for(int i=0;i< (int)NodeList.GetNodeCount();i++)
	{
		IntersectionNodeInSim* pNodeInSim = NodeList.GetNodeByIndex(i);
		if( pNodeInSim->GetNodeInput().HasObject( GetTaxiwayID() ) )
		{
			vNodesInSim.push_back( pNodeInSim );
		}
	}

	std::sort(vNodesInSim.begin(),vNodesInSim.end(), IntersectionNodeInSimCompareDistInTaxiway(GetTaxiwayID()) );
	
	
	for(int i=0;i<(int)vNodesInSim.size() -1; i++)
	{
		IntersectionNodeInSim * pNode1 = vNodesInSim.at(i);
		IntersectionNodeInSim * pNode2 = vNodesInSim.at(i+1);
		
		DistanceUnit dOffset1 = 0;
		DistanceUnit doffset2 = 0;
		{			
			DistanceUnit dMin =0;DistanceUnit dMax = 0;
			GetMinMaxFilletDistToNode(pNode1, m_pTaxiwayInput->getID() , dMin, dMax);
			dOffset1 = max(dMax, DefaultHoldOffset);
		}	
		
		
		{
			DistanceUnit dMin =0;DistanceUnit dMax = 0;
			GetMinMaxFilletDistToNode( pNode2 ,m_pTaxiwayInput->getID(), dMin, dMax);
			doffset2 = min(dMin, -DefaultHoldOffset);
			
		}		
	
		TaxiwaySegInSim * newSeg = new TaxiwaySegInSim(pNode1,pNode2,this);
		newSeg->SetHold1Offset(dOffset1);
		newSeg->SetHold2Offset(doffset2);
		//newSeg->InitLanes();

		//set user define hold
		for(int j=0;j<(int)vHoldList.size();j++)
		{
			const HoldPositionInSim& hold = vHoldList[j];
			if (GetTaxiwayID() == hold.GetTaxiwayID())
			{
				if(hold.m_nNodeID == pNode1->GetID() && hold.m_dDistOffNode > 0 )
				{				
					newSeg->SetUserHold1Offset(hold.m_dDistOffNode,hold.GetID());
				}

				if(hold.m_nNodeID == pNode2->GetID() && hold.m_dDistOffNode < 0 )
				{
					newSeg->SetUserHold2Offset(hold.m_dDistOffNode,hold.GetID());
				}
			}
			
		}
		m_vSegments.push_back(newSeg);
	}

	return true;
}


void TaxiwayInSim::InitConstraints(TaxiwayConstraintsManager& consManager)
{
	// Weight Wingspan
	for(int i=0; i<consManager.m_weightWinspanCons.GetItemCount(); i++)
	{
		CTaxiwayConstraint* pItem = consManager.m_weightWinspanCons.GetItem(i);
		if(pItem->GetTaxiwayID() == this->GetTaxiwayInput()->getID())
		{
			for (int j=0; j<pItem->GetItemCount(); j++)
			{
				CTaxiwayConstraintData* pData = pItem->GetItem(j);
				int nFromID = pData->GetFromID();
				int nToID = pData->GetToID();

				//TRACE("\nFromID:%d, ToID:%d", nFromID, nToID);
				FlightGroundRouteDirectSegList segList;
				GetTaxiwayDirectSegmentList(nFromID, nToID, segList);

				for (int k=0; k<(int)segList.size(); k++)
				{
					TaxiwayDirectSegInSim *pTwDirectSegInSim = (TaxiwayDirectSegInSim *)segList.at(k);
					TaxiwaySegInSim* pSegItem = pTwDirectSegInSim->GetTaxiwaySeg();
					pSegItem->AddWeightWingspanConstraint(pData->GetConstraintType(), pData->GetMaxNumber());
				}
			}
			//break;
		}
	}

	// FlightType Restriction
	size_t nFltResCount = consManager.m_flightTypeRestrictions.GetElementCount();
	for (size_t i = 0; i < nFltResCount ; i++)
	{
		//CFlightTypeRestrictionList *flightTypeRestrictions = &consManager.m_flightTypeRestrictions;
		//flightTypeRestrictions->GetItem(i);
		CFlightTypeRestriction* pItem = consManager.m_flightTypeRestrictions.GetItem(i);
		if(pItem->GetTaxiwayID() == this->GetTaxiwayInput()->getID())
		{
			int nFromID = pItem->GetFromPos();
			int nToID = pItem->GetToPos();

			FlightGroundRouteDirectSegList segList;
			GetTaxiwayDirectSegmentList(nFromID, nToID, segList);

			for (int k=0; k<(int)segList.size(); k++)
			{
				TaxiwayDirectSegInSim *pTwDirectSegInSim = (TaxiwayDirectSegInSim *)segList.at(k);
				TaxiwaySegInSim* pSegItem = pTwDirectSegInSim->GetTaxiwaySeg();
				pSegItem->AddFlightTypeRestrictions(pItem->GetFltType());
			}

			//break;
		}
	}

	// Directionality constraints
	size_t nDirCons = consManager.m_directionalityCons.GetElementCount();
	for(size_t i = 0; i < nDirCons; i++)
	{
		DirectionalityConstraintItem* pItem = consManager.m_directionalityCons.GetItem(i);
		if(pItem->GetTaxiwayID() == this->GetTaxiwayInput()->getID())
		{
			int nFromID = pItem->GetSegFromID();
			int nToID = pItem->GetSegToID();

			FlightGroundRouteDirectSegList segList;
			GetTaxiwayDirectSegmentList(nFromID, nToID, segList);

			for (int k=0; k<(int)segList.size(); k++)
			{
				TaxiwayDirectSegInSim *pTwDirectSegInSim = (TaxiwayDirectSegInSim *)segList.at(k);
				TaxiwaySegInSim* pSegItem = pTwDirectSegInSim->GetTaxiwaySeg();
				DirectionalityConstraintType emType = pItem->GetDirConsType(); 
				pSegItem->SetDirectionConstraintType(emType);
				if(emType == Direction_Bearing)
				{
					pSegItem->SetBearingDegree(pItem->GetBearingDegree());
					pSegItem->SetSegmentDirection(pTwDirectSegInSim->IsPositiveDir()?PositiveDirection:NegativeDirection);
				}
				if (emType == Direction_Reciprocal)
				{
					pSegItem->SetSegmentDirection(pTwDirectSegInSim->IsPositiveDir()?NegativeDirection:PositiveDirection);
				}
			}

			//break;
		}
	}
	//closure constraint
	{
		CAirsideSimSettingClosure *m_simSettingClosure = consManager.m_simSettingClosure;
		if (m_simSettingClosure)
		{
			CAirsideTaxiwayClosure& m_taxiwayClosure = m_simSettingClosure->getTaxiwayClosure();
			
			int nCount = m_taxiwayClosure.GetAirportCount();
			for (int nAirport =0; nAirport < nCount;++ nAirport)
			{

				CTaxiwayClosureAtAirport *pTaxiwayCons = m_taxiwayClosure.GetTaxiwayClosureAtAirport(nAirport);

				int nTxiwayConsCount = pTaxiwayCons->GetTaxiwayClosureNodeCount();
				for (int nTaxiwayCons = 0; nTaxiwayCons< nTxiwayConsCount; ++ nTaxiwayCons)
				{
					CTaxiwayClosureNode *pCons = pTaxiwayCons->GetTaxiwayClosureNode(nTaxiwayCons);

					if (pCons->m_nTaxiwayID == GetTaxiwayInput()->getID())
					{

						int nFromID = pCons->m_nIntersectNodeIDFrom;
						int nToID = pCons->m_nIntersectNodeIDTo;

						FlightGroundRouteDirectSegList segList;
						GetTaxiwayDirectSegmentList(nFromID, nToID, segList);

						for (int k=0; k<(int)segList.size(); k++)
						{
							TaxiwayDirectSegInSim *pTwDirectSegInSim = (TaxiwayDirectSegInSim *)segList.at(k);
							TaxiwaySegInSim* pSegItem = pTwDirectSegInSim->GetTaxiwaySeg();
							pSegItem->AddClosureTimeConstraint(pCons->m_startTime,pCons->m_endTime);

						}

						break;
					}

				}
			}
		}
	}

	// Speed Constraint
	{
		const TaxiSpeedConstraints& taxiSpeedConstraints = consManager.m_taxiSpeedConstraints;
		size_t nTaxiwayCount = taxiSpeedConstraints.GetElementCount();
		for (size_t i = 0; i < nTaxiwayCount ; i++)
		{
			const TaxiwaySpeedConstraintTaxiwayItem* pTaxiwayItem = taxiSpeedConstraints.GetItem(i);
			if(pTaxiwayItem->GetTaxiwayID() == GetTaxiwayInput()->getID())
			{
				const TaxiwaySpeedConstraintFlttypeList& flttypeList = pTaxiwayItem->GetConstraintFltType();
				size_t nFlttypeCount = flttypeList.GetElementCount();
				for(size_t j=0;j<nFlttypeCount;j++)
				{
					const TaxiwaySpeedConstraintFlttypeItem* pFlttypeItem = flttypeList.GetItem(j);
					const TaxiwaySpeedConstraintDataList& constraintsList = pFlttypeItem->GetSpeedConstraints();
					size_t nConstraintsCount = constraintsList.GetElementCount();
					for(size_t k=0;k<nConstraintsCount;k++)
					{
						const TaxiwaySpeedConstraintDataItem* pConstraintItem = constraintsList.GetItem(k);
						int nFromID = pConstraintItem->GetFromNodeID();
						int nToID = pConstraintItem->GetToNodeID();

						FlightGroundRouteDirectSegList segList;
						GetTaxiwayDirectSegmentList(nFromID, nToID, segList);

						for (size_t n=0; n<segList.size(); n++)
						{
							TaxiwayDirectSegInSim* pTwDirectSegInSim = (TaxiwayDirectSegInSim*)segList.at(n);
							pTwDirectSegInSim->GetTaxiwaySeg()->AddTaxiSpeedConstraint(
								pFlttypeItem->GetFlightType(), pConstraintItem->GetMaxSpeedAsCMpS());
						}
					}
				}
			}
		}
	}

	//

}

TaxiwayInSim::~TaxiwayInSim()
{
	for(int i=0;i<(int)m_vSegments.size();++i)
	{
		delete m_vSegments[i];
	}
	m_vSegments.clear();
}

TaxiwaySegInSim* TaxiwayInSim::GetPointInSegment( const CPoint2008& point )	//get the segment which the point in
{
	double dDistInTaxiway = m_pTaxiwayInput->getPath().GetPointDist(point);

	int nCount = m_vSegments.size();
	for (int i =0; i < nCount; i++)
	{
		TaxiwaySegInSim* pSeg = m_vSegments.at(i);

		double dDistStart = m_pTaxiwayInput->getPath().GetPointDist(pSeg->GetNode1()->GetNodeInput().GetPosition());
		double dDistEnd = m_pTaxiwayInput->getPath().GetPointDist(pSeg->GetNode2()->GetNodeInput().GetPosition());

		if ((dDistInTaxiway >= dDistStart && dDistInTaxiway <= dDistEnd) || (dDistInTaxiway >= dDistEnd && dDistInTaxiway <= dDistStart))
			return pSeg;
	}

	return NULL;
}

IntersectionNodeInSimList TaxiwayInSim::GetIntersectionNodeList()
{
	IntersectionNodeInSimList vNodeList;
	int nCount = GetSegmentCount();
	for(int i=0;i<nCount;i++)
	{
		if (i == 0)
		{
			vNodeList.Add(GetSegment(i)->GetNode1());
		}
		vNodeList.Add(GetSegment(i)->GetNode2());
	}
	return vNodeList;
}

TaxiwayDirectSegInSim::TaxiwayDirectSegInSim( TaxiwaySegInSim *pSeg, FlightGroundRouteSegDirection dir )
:FlightGroundRouteDirectSegInSim(pSeg,dir)
{
	//m_pSeg = pSeg; m_dir = dir;
	m_dirPath = GetSubPath(GetTaxiwaySeg()->m_pTaxiway->GetTaxiwayInput()->GetPath(),GetEnterPosInTaxiway(),GetExitPosInTaxiway());
	m_distance = m_dirPath.GetTotalLength();
}






TaxiwayDirectSegInSim  * TaxiwayDirectSegInSim::GetOppositeSegment()
{
	if(IsPositiveDir()) 
		return (TaxiwayDirectSegInSim *)m_pSeg->GetNegativeDirSeg();
	else 
		return (TaxiwayDirectSegInSim *)m_pSeg->GetPositiveDirSeg();
}

const TaxiwayDirectSegInSim * TaxiwayDirectSegInSim::GetOppositeSegment() const
{
	if(IsPositiveDir())
		return (TaxiwayDirectSegInSim *) m_pSeg->GetNegativeDirSeg();
	else
		return (TaxiwayDirectSegInSim *) m_pSeg->GetPositiveDirSeg();
}

bool TaxiwayDirectSegInSim::operator<( const TaxiwayDirectSegInSim& otherSeg ) const
{
	return m_pSeg->GetNodesDistance()  < otherSeg.m_pSeg->GetNodesDistance();
}

void TaxiwayDirectSegInSim::AddFirstAdjacencyNegativeConstraint(CWingspanAdjacencyConstraints* constraint)
{
	m_vFirstNegativeConflictConstraint.push_back(constraint);
}

void TaxiwayDirectSegInSim::AddSecondAdjacencyNegativeConstraint(CWingspanAdjacencyConstraints* constraint)
{
	m_vSecondNegativeConflictConstraint.push_back(constraint);
}

void TaxiwayDirectSegInSim::AddFirstAdjacencyParallelConstraint(CWingspanAdjacencyConstraints* constraint)
{
	m_vFirstParallelConflictConstraint.push_back(constraint);
}

void TaxiwayDirectSegInSim::AddSecondAdjacencyParallelConstraint(CWingspanAdjacencyConstraints* constraint)
{
	ASSERT(constraint);
	m_vSecondParallelConflictConstraint.push_back(constraint);
}

int TaxiwayDirectSegInSim::GetTaxiwayID()const
{
	return ((TaxiwaySegInSim *)m_pSeg)->GetTaxiwayID();
}
	
CString TaxiwayDirectSegInSim::PrintResource() const
{
	CString strName;
	strName.Format("%s(%s - %s)",GetTaxiwaySeg()->m_pTaxiway->GetTaxiwayInput()->GetMarking().c_str(), GetEntryNode()->PrintResource(),GetExitNode()->PrintResource());
	return strName;
}

double project(const CPoint2008& p1, const CPoint2008& p2, const CPoint2008& p)
{
	ARCVector3 v12 = p2 - p1;
	double sqrlen = v12.dot(v12);
	if(sqrlen<=0)
		return 0;

	return v12.dot(p-p1)/sqrlen;
}

bool SnapPointInPath(const CPath2008& path,const CPoint2008& pt,double& dDist)
{
	if (path.getCount() == 0)
		return false;
	
	CPoint2008 point = path.getPoint(0);
	for (int i = 1; i < path.getCount(); i++)
	{
		CPoint2008 ptNext = path.getPoint(i);
		double dIndex = project(point,ptNext,pt);
		if (dIndex >=0 && dIndex <=1)
		{
			dDist = path.GetIndexDist((float)dIndex+i-1);
			return true;
		}
		point = ptNext;
	}
	return false;
}

ARCMobileElement* TaxiwayDirectSegInSim::getAdjacencyMappingHead(const FlightGroundRouteDirectSegList& segList,
																 ARCMobileElement* pmob,
																 const CWingspanAdjacencyConstraints* pConstraint,
																 bool bCheckFirstConstraint,
																 FlightGroundRouteSegDirection emType,
																 double& dDist)
{
	std::map<double,ARCMobileElement*> mapElement;
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pmob;

	double dCurDist = pmob->getCurDistInDirPath();
	if (pmob->getCurDirPath() != this)
		dCurDist = 0.0;
	
	const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
	const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;

	for (size_t i = 0; i < segList.size(); i++)
	{
		TaxiwayDirectSegInSim* pSegInSim = (TaxiwayDirectSegInSim*)segList[i];
		
		std::vector<ARCMobileElement*> vMobile = pSegInSim->getInPathMobile();
		std::vector<ARCMobileElement*>::iterator itrFind = vMobile.begin();
		for (; itrFind != vMobile.end(); ++itrFind)
		{
			ARCMobileElement* ptheMob = (*itrFind);
			if (!(ptheMob && ptheMob->IsKindof(typeof(AirsideFlightInSim))))
				continue;
			AirsideFlightInSim* pConFlight = (AirsideFlightInSim*)ptheMob;

			if (bCheckFirstConstraint)
			{
				if (firstConstraint.FlightFit(pFlight->GetFlightInput()))
				{
					if (!secondConstraint.FlightFit(pConFlight->GetFlightInput()))
					{
						//conflict
					}
					else
					{
						continue;
					}
				}
			}
			else
			{
				if (firstConstraint.FlightFit(pConFlight->GetFlightInput()))
				{
					if (!secondConstraint.FlightFit(pFlight->GetFlightInput()))
					{
						//conflict
					}
					else
					{
						continue;
					}
				}
			}
	
			CPoint2008 pt = pSegInSim->GetDistancePoint((*itrFind)->getCurDistInDirPath());
			double snapDist = 0.0;
			if(!SnapPointInPath(m_dirPath,pt,snapDist))
				continue;
			if (snapDist < dCurDist)
				continue;

			if (pFlight->GetAirTrafficController()->getConflictGraph()->IsAFollowB(pConFlight,pFlight))
				continue;
			
			mapElement.insert(std::make_pair(snapDist,ptheMob));
		}
	}
	if (mapElement.empty())
		return NULL;

	dDist = (*(mapElement.begin())).first;
	return (*(mapElement.begin())).second;
}

ARCMobileElement* TaxiwayDirectSegInSim::getAdjacencyLeadMobile(ARCMobileElement* pmob,double& dDist)
{
	if (!(pmob && pmob->IsKindof(typeof(AirsideFlightInSim))))
		return NULL;

	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pmob;
	AirTrafficController* pATC = pFlight->GetAirTrafficController();
	if (pATC == NULL)
		return NULL;
	
	AirsideResourceManager* pResManager = pATC->GetAirsideResourceManager(); 
	if (pResManager == NULL)
		return NULL;
	
	AirportResourceManager* pAirportResManager = pResManager->GetAirportResource();
	if (pAirportResManager == NULL)
		return NULL;
	
	TaxiwayResourceManager* pTaxiwayRes = pAirportResManager->getTaxiwayResource();
	if (pTaxiwayRes == NULL)
		return NULL;
	
	//check first constraint
	for (size_t i = 0; i < m_vFirstParallelConflictConstraint.size(); i++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vFirstParallelConflictConstraint[i];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;
	
		int nStartID = secondConstraint.m_startNode.GetID();
		int nEndID = secondConstraint.m_endNode.GetID();
	
		FlightGroundRouteDirectSegList segList;
		TaxiwayInSim* pTaxiwayInSim = pTaxiwayRes->GetTaxiwayByID(secondConstraint.m_pTaxiway.getID());
		if (pTaxiwayInSim == NULL)
			continue;
		
		pTaxiwayInSim->GetTaxiwayDirectSegmentList(nStartID,nEndID,segList);
		return getAdjacencyMappingHead(segList,pmob,pConstraint,true,PositiveDirection,dDist);
	}
	//check second constraint
	for (size_t ii = 0; ii < m_vSecondParallelConflictConstraint.size(); ii++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vSecondParallelConflictConstraint[ii];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;

		int nStartID = firstConstraint.m_startNode.GetID();
		int nEndID = firstConstraint.m_endNode.GetID();

		FlightGroundRouteDirectSegList segList;
		TaxiwayInSim* pTaxiwayInSim = pTaxiwayRes->GetTaxiwayByID(firstConstraint.m_pTaxiway.getID());
		if (pTaxiwayInSim == NULL)
			continue;
		pTaxiwayInSim->GetTaxiwayDirectSegmentList(nStartID,nEndID,segList);
		return getAdjacencyMappingHead(segList,pmob,pConstraint,false,PositiveDirection,dDist);
	}

	//check first negative constraint
	for (size_t i3 = 0; i3 < m_vFirstNegativeConflictConstraint.size(); i3++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vFirstNegativeConflictConstraint[i3];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;

		int nStartID = secondConstraint.m_startNode.GetID();
		int nEndID = secondConstraint.m_endNode.GetID();

		FlightGroundRouteDirectSegList segList;
		TaxiwayInSim* pTaxiwayInSim = pTaxiwayRes->GetTaxiwayByID(secondConstraint.m_pTaxiway.getID());
		if (pTaxiwayInSim == NULL)
			continue;

		pTaxiwayInSim->GetTaxiwayDirectSegmentList(nEndID,nStartID,segList);
		return getAdjacencyMappingHead(segList,pmob,pConstraint,true,NegativeDirection,dDist);
	}

	//check second negative constraint
	for (size_t i4 = 0; i4 < m_vSecondParallelConflictConstraint.size(); i4++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vSecondParallelConflictConstraint[i4];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;

		int nStartID = firstConstraint.m_startNode.GetID();
		int nEndID = firstConstraint.m_endNode.GetID();

		FlightGroundRouteDirectSegList segList;
		TaxiwayInSim* pTaxiwayInSim = pTaxiwayRes->GetTaxiwayByID(firstConstraint.m_pTaxiway.getID());
		if (pTaxiwayInSim == NULL)
			continue;
		pTaxiwayInSim->GetTaxiwayDirectSegmentList(nEndID,nStartID,segList);
		return getAdjacencyMappingHead(segList,pmob,pConstraint,false,NegativeDirection,dDist);
	}
	return NULL;
}

//IN_parameter: pPath is conflict taxiway segment
//IN_parameter: pMob plan enter to taxiway segment
//IN_parameter:ptheMob is conflict with pMob
//return value is mark pmob and pthemob whether conflict
bool TaxiwayDirectSegInSim::adjacencyNegativeConflict(RouteDirPath* pPath,ARCMobileElement* pmob,ARCMobileElement* ptheMob)
{
	if(!(pPath && pPath->IsKindof(typeof(TaxiwayDirectSegInSim))))
		return false;
	
	if (!(pmob && pmob->IsKindof(typeof(AirsideFlightInSim))))
		return false;
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pmob;

	if (!(ptheMob && ptheMob->IsKindof(typeof(AirsideFlightInSim))))
		return false;
	AirsideFlightInSim* pConflight = (AirsideFlightInSim*)ptheMob;

	TaxiwayDirectSegInSim* pSeg = (TaxiwayDirectSegInSim*)pPath;


	//check first negative constraint
	for (size_t i = 0; i < m_vFirstNegativeConflictConstraint.size(); i++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vFirstNegativeConflictConstraint[i];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;
		if (firstConstraint.FlightFit(pFlight->GetFlightInput()))
		{
			int nStartID = secondConstraint.m_startNode.GetID();
			int nEndID = secondConstraint.m_endNode.GetID();
	
			FlightGroundRouteDirectSegList segList;
			TaxiwaySegInSim* pSegInSim = pSeg->GetTaxiwaySeg();
			if (pSegInSim == NULL)
				continue;
			TaxiwayInSim* pTaxiwayInSim = pSegInSim->GetTaxiway();
			if (pTaxiwayInSim == NULL)
				continue;
			
			pTaxiwayInSim->GetTaxiwayDirectSegmentList(nStartID,nEndID,segList);

			FlightGroundRouteSegDirection emType;
			if(!TaxiwayInSim::includeSeg(segList,pSeg->GetTaxiwaySeg(),emType))
				continue;

			if (pSeg->GetDir() != emType)
				continue;
			
			if (!secondConstraint.FlightFit(pConflight->GetFlightInput()))
				return true;
		}
	}

	//check second negative constraint;
	for (size_t ii = 0; ii < m_vSecondNegativeConflictConstraint.size(); ii++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vSecondNegativeConflictConstraint[ii];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;

		if (firstConstraint.FlightFit(pConflight->GetFlightInput()))
		{
			int nStartID = firstConstraint.m_startNode.GetID();
			int nEndID = firstConstraint.m_endNode.GetID();
		
			FlightGroundRouteDirectSegList segList;
			TaxiwaySegInSim* pSegInSim = pSeg->GetTaxiwaySeg();
			if (pSegInSim == NULL)
				continue;
			TaxiwayInSim* pTaxiwayInSim = pSegInSim->GetTaxiway();
			if (pTaxiwayInSim == NULL)
				continue;
			pTaxiwayInSim->GetTaxiwayDirectSegmentList(nStartID,nEndID,segList);

			FlightGroundRouteSegDirection emType;
			if(!TaxiwayInSim::includeSeg(segList,pSeg->GetTaxiwaySeg(),emType))
				continue;

			if (pSeg->GetDir() != emType)
				continue;

			if (!secondConstraint.FlightFit(pFlight->GetFlightInput()))
				return true;
		}
	}

	//check parallel constraint
	for (size_t i3 = 0; i3 < m_vFirstParallelConflictConstraint.size(); i3++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vFirstParallelConflictConstraint[i3];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;
		if (firstConstraint.FlightFit(pFlight->GetFlightInput()))
		{
			int nStartID = secondConstraint.m_startNode.GetID();
			int nEndID = secondConstraint.m_endNode.GetID();

			FlightGroundRouteDirectSegList segList;
			TaxiwaySegInSim* pSegInSim = pSeg->GetTaxiwaySeg();
			if (pSegInSim == NULL)
				continue;
			TaxiwayInSim* pTaxiwayInSim = pSegInSim->GetTaxiway();
			if (pTaxiwayInSim == NULL)
				continue;

			pTaxiwayInSim->GetTaxiwayDirectSegmentList(nEndID,nStartID,segList);

			FlightGroundRouteSegDirection emType;
			if(!TaxiwayInSim::includeSeg(segList,pSeg->GetTaxiwaySeg(),emType))
				continue;

			if (pSeg->GetDir() != emType)
				continue;

			if (!secondConstraint.FlightFit(pConflight->GetFlightInput()))
				return true;
		}
	}

	for (size_t i4 = 0; i4 < m_vSecondParallelConflictConstraint.size(); i4++)
	{
		CWingspanAdjacencyConstraints* pConstraint = m_vSecondParallelConflictConstraint[i4];
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& firstConstraint = pConstraint->m_firstaxiway;
		const CWingspanAdjacencyConstraints::TaxiwayNodeItem& secondConstraint = pConstraint->m_secondTaxiway;

		if (firstConstraint.FlightFit(pConflight->GetFlightInput()))
		{
			int nStartID = firstConstraint.m_startNode.GetID();
			int nEndID = firstConstraint.m_endNode.GetID();

			FlightGroundRouteDirectSegList segList;
			TaxiwaySegInSim* pSegInSim = pSeg->GetTaxiwaySeg();
			if (pSegInSim == NULL)
				continue;
			TaxiwayInSim* pTaxiwayInSim = pSegInSim->GetTaxiway();
			if (pTaxiwayInSim == NULL)
				continue;
			pTaxiwayInSim->GetTaxiwayDirectSegmentList(nEndID,nStartID,segList);

			FlightGroundRouteSegDirection emType;
			if(!TaxiwayInSim::includeSeg(segList,pSeg->GetTaxiwaySeg(),emType))
				continue;

			if (pSeg->GetDir() != emType)
				continue;

			if (!secondConstraint.FlightFit(pFlight->GetFlightInput()))
				return true;
		}
	}
	return false;
}

bool TaxiwayDirectSegInSim::IsForbidDirection()
{
	if (((TaxiwaySegInSim *)m_pSeg)->GetSegmentDirection() == BothDirection )
		return false;

	if (((TaxiwaySegInSim *)m_pSeg)->GetSegmentDirection() == m_dir )
		return false;

	return true;
}

//
void TaxiwayDirectSegInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetTaxiwaySeg()->GetTaxiwayID();
	resDesc.resdir = (m_dir == PositiveDirection)?ResourceDesc::POSITIVE:ResourceDesc::NEGATIVE;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
	resDesc.fromnode= GetEntryNode()->GetID();
	resDesc.tonode = GetExitNode()->GetID();
}