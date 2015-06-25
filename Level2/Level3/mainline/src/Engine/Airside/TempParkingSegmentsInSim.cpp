#include "StdAfx.h"
#include ".\tempparkingsegmentsinsim.h"
#include "TaxiwayResource.h"
#include "AirsideFlightInSim.h"
#include "IntersectionNodeInSim.h"
#include "TaxiRouteInSim.h"
#include "StandInSim.h"
#include "AirportResourceManager.h"
#include "AirTrafficController.h"
#include "standInSim.h"
#include "StandLeadInLineInSim.h"
#include "StandLeadOutLineInSim.h"
#include "StandBufferInSim.h"
#include "InputAirside\OccupiedAssignedStandAction.h"
#include "DynamicConflictGraph.h"
#include "In_OutBoundRouteAssignmentInSim.h"

/************************************************************************/
/* temp parking segments                                                                     */
/************************************************************************/

const DistanceUnit TempParkingSegmentsInSim::defaultAngle = 45;
const DistanceUnit TempParkingSegmentsInSim::defaultParkingSep = 3000;

TempParkingSegmentsInSim::TempParkingSegmentsInSim(void)
{
	m_nTempParkingNum = 0;
	m_pEnterNode = NULL;
	m_pExitNodeFrom = NULL;
	m_pExitNodeTo = NULL;
	m_pTaxiway = NULL;
	m_pParkingRouteTo = NULL;
	m_pParkingRouteFrom =NULL;
	m_bIsAngled = false;
	m_dRouteFromLeft = 0;
	m_dRouteToLeft = 0;
}

TempParkingSegmentsInSim::~TempParkingSegmentsInSim(void)
{
	delete m_pParkingRouteFrom;
	m_pParkingRouteFrom = NULL;

	delete m_pParkingRouteTo;
	m_pParkingRouteTo = NULL;
}

bool TempParkingSegmentsInSim::TryLock(AirsideFlightInSim* pFlight)
{
	typedef std::map<AirsideFlightInSim*,IntersectionNodeInSim::RefPtr>::iterator FlightNodeMapIter;
	const DistanceUnit cosParkingAngle = (IsAngleParking()?cos(TempParkingSegmentsInSim::defaultAngle):1.0);

	//calculate the left len of the parking segments
	if( m_vParkingFlights[pFlight].get() ){ 
		return true;	
	}
	//check efficient length route from 
	if( m_pParkingRouteFrom && m_pParkingRouteFrom->GetItemCount() )
	{		
		if( pFlight->GetLength() * cosParkingAngle < m_dRouteFromLeft )
		{
			return true;
		}
	}
	//route efficient length route to  
	if( m_pParkingRouteTo && m_pParkingRouteTo->GetItemCount()  )
	{
		if( pFlight->GetLength() * cosParkingAngle < m_dRouteToLeft )
		{
			return true;
		}
	}	
	return false;
	
}

void TempParkingSegmentsInSim::ReleaseLock(AirsideFlightInSim* pFlight)
{	
	const DistanceUnit cosParkingAngle = (IsAngleParking()?cos(TempParkingSegmentsInSim::defaultAngle):1.0);
	if( m_pExitNodeFrom == m_vParkingFlights[pFlight].get() )
	{
		m_dRouteFromLeft += pFlight->GetLength()*cosParkingAngle + defaultParkingSep;
	}
	if( m_pExitNodeTo == m_vParkingFlights[pFlight].get() )
	{
		m_dRouteToLeft += pFlight->GetLength() * cosParkingAngle + defaultParkingSep;
	}
	m_vParkingFlights.erase(pFlight);

}

bool TempParkingSegmentsInSim::GetLock( AirsideFlightInSim* pFlight )
{
	const DistanceUnit cosParkingAngle = (IsAngleParking()?cos(TempParkingSegmentsInSim::defaultAngle):1.0);
	if( m_pParkingRouteFrom && m_pParkingRouteFrom->GetItemCount() && m_dRouteFromLeft > m_dRouteToLeft )
	{
		if( pFlight->GetLength() * cosParkingAngle < m_dRouteFromLeft )
		{
			m_dRouteFromLeft -= (pFlight->GetLength()*cosParkingAngle + defaultParkingSep);
			m_vParkingFlights[pFlight] = m_pExitNodeFrom;
			return true;
		}
	}
	
	if( m_pParkingRouteTo && m_pParkingRouteTo->GetItemCount() && m_dRouteToLeft > m_dRouteFromLeft )
	{
		if( pFlight->GetLength() * cosParkingAngle < m_dRouteToLeft )
		{
			m_dRouteToLeft -= ( pFlight->GetLength()*cosParkingAngle + defaultParkingSep );
			m_vParkingFlights[pFlight] = m_pExitNodeTo;
		}
	}
	return true;
}


void TempParkingSegmentsInSim::InitTempParking(TaxiwayInSim* pTaxiway, int nStartNode, int nEndNode,int nEntryNode,AirportResourceManager* pResManager,bool bAngle)
{
	
	FlightGroundRouteDirectSegList  vTempParkingSegFrom;
	FlightGroundRouteDirectSegList  vTempParkingSegTo;
	m_pTaxiway = pTaxiway;
	if (nEntryNode != nEndNode)
	{
		pTaxiway->GetTaxiwayDirectSegmentList(nEntryNode,nEndNode,vTempParkingSegTo);
		m_pExitNodeTo = pResManager->GetIntersectionNodeList().GetNodeByID(nEndNode);
		m_pParkingRouteTo = new TaxiRouteInSim(OnHeldAtTempParking,m_pEnterNode,m_pExitNodeTo);
		m_pParkingRouteTo->AddTaxiwaySegList(vTempParkingSegTo);
		
		if( m_pParkingRouteTo->GetItemCount() )
		{
			m_dRouteToLeft = m_pParkingRouteTo->GetEndDist();
			HoldInTaxiRoute* firstHold  = NULL;
			HoldInTaxiRoute* lastHold = NULL;
			if( firstHold&& lastHold )
			{
				m_dRouteToLeft = lastHold->m_dDistInRoute - firstHold->m_dDistInRoute;
			}
		}
	}
	if (nEntryNode != nStartNode)
	{
		pTaxiway->GetTaxiwayDirectSegmentList(nEntryNode,nStartNode,vTempParkingSegFrom);
		m_pExitNodeFrom = pResManager->GetIntersectionNodeList().GetNodeByID(nStartNode);
		m_pParkingRouteFrom = new TaxiRouteInSim(OnHeldAtTempParking,m_pEnterNode,m_pExitNodeTo);
		m_pParkingRouteFrom->AddTaxiwaySegList(vTempParkingSegFrom);

		if( m_pParkingRouteFrom->GetItemCount() )
		{
			m_dRouteFromLeft = m_pParkingRouteFrom->GetEndDist();
			HoldInTaxiRoute* firstHold = NULL; 
			HoldInTaxiRoute* lastHold = NULL;
			if( firstHold&& lastHold )
			{
				m_dRouteFromLeft = lastHold->m_dDistInRoute - firstHold->m_dDistInRoute;
			}
		}
	}
	m_pEnterNode = pResManager->GetIntersectionNodeList().GetNodeByID(nEntryNode);
	m_bIsAngled = bAngle;
	for(int i=0;i<(int)vTempParkingSegFrom.size();i++)
	{
		FlightGroundRouteDirectSegInSim* pDirSeg = vTempParkingSegFrom.at(i);
		m_vUseSegs.push_back( pDirSeg->GetRouteSegInSim() );
	}
	for(int i=0;i<(int)vTempParkingSegTo.size();i++)
	{
		FlightGroundRouteDirectSegInSim* pDirSeg = vTempParkingSegTo.at(i);
		m_vUseSegs.push_back( pDirSeg->GetRouteSegInSim() );
	}
}

IntersectionNodeInSim* TempParkingSegmentsInSim::GetParkingTaxiwayNode(AirsideFlightInSim* pFlight)
{
	return m_vParkingFlights[pFlight].get();
}

bool TempParkingSegmentsInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance )
{
	bool bNotAtEnd = false;

	TaxiRouteInSim* pParkingRoute = NULL;

	if (m_vParkingFlights[pFlight].get() == m_pExitNodeFrom)
		pParkingRoute = m_pParkingRouteFrom;
	if(m_vParkingFlights[pFlight].get() == m_pExitNodeTo)
		pParkingRoute = m_pParkingRouteTo;

	if( pParkingRoute )
		bNotAtEnd = pParkingRoute->FindClearanceAsTempParking(pFlight, lastItem, newClearance, IsAngleParking()?defaultAngle:0);
	
	if(!bNotAtEnd) // at the end of the route ,try to lock the stand
	{
		StandInSim* pParkingStand = pFlight->GetOperationParkingStand();
		if( pParkingStand && pParkingStand->TryLock(pFlight) )
		{
			pParkingStand->GetLock(pFlight);
			return false;
		}
		else
		{
			pFlight->GetWakeUpCaller().OberverSubject(pParkingStand);
			return true;
		}
	}
	else {
		return true;
	}
	return false;
}

CString TempParkingSegmentsInSim::PrintTempParking() const
{
	CString str;
	str.Format("TempTaxiway Parking (%s)", m_pTaxiway->GetTaxiwayInput()->GetMarking().c_str() );
	return str;
}
/************************************************************************/
/* TempParkingStandInSim                                                 */
/************************************************************************/
IntersectionNodeInSim* TempParkingStandInSim::GetInNode() const
{
	return m_pStand->GetMinDistInNode();
}

void TempParkingStandInSim::ReleaseLock( AirsideFlightInSim* pFlight )
{
	return m_pStand->ReleaseLock(pFlight,pFlight->GetTime());
}

bool TempParkingStandInSim::TryLock( AirsideFlightInSim* pFlight )
{
	return m_pStand->TryLock(pFlight);
}

bool TempParkingStandInSim::GetLock( AirsideFlightInSim* pFlight )
{
	return m_pStand->GetLock(pFlight);
}

IntersectionNodeInSim* TempParkingStandInSim::GetOutNode( AirsideFlightInSim*pFlight )
{
	return m_pStand->GetMaxDistOutNode();
}

bool TempParkingStandInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance )
{	
	if( lastItem.GetMode() == OnTaxiToTempParking ) // get enter temp stand clearance
	{	
		StandLeadInLineInSim* pLeadInLine = m_pStand->AssignLeadInLine(pFlight);
		if( pLeadInLine ) 
		{			
			ClearanceItem enterStandItem(pLeadInLine, OnEnterTempStand, 0 ); // enter temp stand
			enterStandItem.SetSpeed(lastItem.GetSpeed());
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem, enterStandItem);
			ElapsedTime nextTime = lastItem.GetTime() + dT;		
			enterStandItem.SetTime(nextTime);
			lastItem = enterStandItem;
			newClearance.AddItem(lastItem);

			ClearanceItem heldAtStandItem(pLeadInLine, OnHeldAtTempParking, pLeadInLine->GetEndDist() );
			heldAtStandItem.SetSpeed(0);
			dT = pFlight->GetTimeBetween(lastItem, heldAtStandItem);
			nextTime = lastItem.GetTime() + dT;
			heldAtStandItem.SetTime(nextTime);
			lastItem = heldAtStandItem;
			newClearance.AddItem(lastItem);
			return true;
		}
	}
	if (lastItem.GetMode() == OnHeldAtTempParking ) // try lock the dest parking
	{
		StandInSim * pParkingStand = pFlight->GetOperationParkingStand();
		if(!pParkingStand) //still no parking stand ,what should we do?
		{
			return true;
		}
		if (pFlight->GetAirTrafficController()->IsGateAssignConstraintsConflict(pFlight))
		{
			return true;
		}

		StandInSim* pConflictStandInSim = pFlight->GetAirTrafficController()->IsCurrentOccupiedFlightAdjacencyConflict(pFlight);
		if(!pConflictStandInSim && pParkingStand->TryLock(pFlight))
		{
			pParkingStand->GetLock(pFlight);
			ElapsedTime tBuffer = pFlight->GetAirTrafficController()->GetStandBuffer()->GetStandBufferTime(pFlight,pParkingStand);
			ElapsedTime exitTime = pParkingStand->GetLastReleaseTime() + tBuffer;
			
			AirsideResource* atResource = m_pStand->AssignLeadOutLine(pFlight);
			if(!atResource)
				atResource = m_pStand;

			ASSERT(atResource);
			{
				if(lastItem.GetTime() < exitTime )
				{
					lastItem.SetDelayTime(exitTime - lastItem.GetTime());
					lastItem.SetTime(exitTime);
					newClearance.AddItem(lastItem);
				}
				ClearanceItem exitStandItem(atResource, OnExitTempStand,0);
				exitStandItem.SetTime(lastItem.GetTime());
				exitStandItem.SetSpeed(0);
				newClearance.AddItem(exitStandItem); // for direction 
				exitStandItem.SetDistInResource(1);
				lastItem = exitStandItem;
				newClearance.AddItem(lastItem);				
			}
			
			return true;
		}
		else
		{
			StandInSim* pTestStand = pConflictStandInSim?pConflictStandInSim:pParkingStand;
			pFlight->GetWakeUpCaller().OberverSubject(pTestStand);
			
			return true;
		}		
		
	}

	return false;	
	//return m_pStand->FindClearanceInConcern(pFlight,lastItem,radius,newClearance);
}

CString TempParkingStandInSim::PrintTempParking() const
{
	CString str;
	str.Format("Temp Parking Stand (%s)", m_pStand->PrintResource() );
	return str;
}
/************************************************************************/
/* temp parking node                                                     */
/************************************************************************/
bool TempParkingNodeInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance )
{		

	if(lastItem.GetMode()==OnTaxiToTempParking)
	{
		ClearanceItem heldItem = lastItem; // enter temp stand
		heldItem.SetMode(OnHeldAtTempParking);
		heldItem.SetSpeed(0);		 
		heldItem.SetTime(lastItem.GetTime());
		lastItem = heldItem;
		newClearance.AddItem(lastItem);
		return true;
	}
	if(lastItem.GetMode() == OnHeldAtTempParking)
	{		
		if( m_bCirculate )  //in circulating
		{		
			ASSERT(m_pCirculateRoute);
			if(m_pCirculateRoute)
			{
				bool bEnd = m_pCirculateRoute->FindClearanceInConcern(pFlight,lastItem,radius, newClearance);
				if(bEnd)
					return bEnd;	
				else
				{
					m_bCirculate  = false;
					m_nCirculatedNum++;
					m_pCirculateRoute->reset();
					ClearanceItem heldItem = lastItem; // enter temp stand
					heldItem.SetMode(OnHeldAtTempParking);
					heldItem.SetSpeed(0);		 
					heldItem.SetTime(lastItem.GetTime());
					lastItem = heldItem;
					newClearance.AddItem(lastItem);
					return true;
				}
			}

		}
		//waiting in the node
		if(!m_bCirculate)
		{
			StandInSim* pParkingStand = pFlight->GetOperationParkingStand();
			if( pParkingStand && pParkingStand->TryLock(pFlight) )
			{			
				pParkingStand->GetLock(pFlight);			
				return false;
			}
			else
			{			
				//wait
				pFlight->GetWakeUpCaller().OberverSubject(pParkingStand);
				return true;		
			}
		}
	}
	
	ASSERT(false);
	return m_pNode->FindClearanceInConcern(pFlight,lastItem,radius,newClearance);
}

void TempParkingNodeInSim::ReleaseLock( AirsideFlightInSim* pFlight )
{
	return m_pNode->ReleaseLock(pFlight,pFlight->GetTime());
}

bool TempParkingNodeInSim::TryLock( AirsideFlightInSim* pFlight )
{
	return m_pNode->TryLock(pFlight);
}

bool TempParkingNodeInSim::GetLock( AirsideFlightInSim* pFlight )
{
	return m_pNode->GetLock(pFlight);
}

IntersectionNodeInSim* TempParkingNodeInSim::GetOutNode( AirsideFlightInSim*pFlight )
{
	return m_pNode;
}

CString TempParkingNodeInSim::PrintTempParking() const
{
	CString str;
	str.Format("Temp Parking Node (%s)", GetInNode()->PrintResource() );
	return str;
}

TempParkingNodeInSim::TempParkingNodeInSim(IntersectionNodeInSim* pNode, COccupiedAssignedStandStrategy& strategy )
{
	m_pNode = pNode; 
	m_pCirculateRoute = NULL;
	m_nMaxCirculateNum =  strategy.GetTimes();
	m_nCirculatedNum =  0;	
	m_pRouteInput = strategy.GetCirculateRoute();
	m_bCirculate = false;
}

void TempParkingNodeInSim::initRoute( AirsideFlightInSim *pFlight )
{
	if(!m_pRouteInput)
		return;

	if(m_pCirculateRoute)
		return;
		
	FlightGroundRouteDirectSegList vseglist;
	In_OutBoundRouteAssignmentInSim* boundRoutAssign = pFlight->GetAirTrafficController()->GetBoundRouteAssignment();
	if( boundRoutAssign->getCirculateRoute(pFlight, m_pNode,m_pNode, m_pRouteInput, vseglist) )
	{
		CirculateRouteInSim* pCRoute = new CirculateRouteInSim(OnHeldAtTempParking,m_pNode,m_pNode);
		if(TaxiwayDirectSegInSim* pseg = pFlight->GetCurTaxiwayDirectSeg())
			vseglist.insert(vseglist.begin(), pseg);

		pCRoute->AddTaxiwaySegList(vseglist,true);
		setCirculateRoute(pCRoute);
	}
}

void TempParkingNodeInSim::setCirculateRoute( CirculateRouteInSim* pCRoute )
{
	m_pCirculateRoute = pCRoute;
}

void TempParkingNodeInSim::notifyCirculate(AirsideFlightInSim* pFlight,const ElapsedTime& t )
{
	if(m_bCirculate)
		return;

	if( m_nCirculatedNum < m_nMaxCirculateNum)
	{
		initRoute(pFlight);
		if(m_pCirculateRoute)
		{
			m_bCirculate  =true;
			pFlight->OnNotify(NULL,SimMessage().setTime(t) );
		}
	}			
}


DistanceUnit CirculateRouteInSim::GetExitRouteDist( AirsideFlightInSim* pFlight )
{
	DistanceUnit dExitRouteDist = GetEndDist() - pFlight->GetLength()*0.5;//default exit route dist

	/*HoldInTaxiRoute* lastEntryHold =  GetlastEntryHold();
	if(lastEntryHold)
	{ 
	dExitRouteDist = lastEntryHold->m_dDistInRoute;
	}*/
	return dExitRouteDist;
}
