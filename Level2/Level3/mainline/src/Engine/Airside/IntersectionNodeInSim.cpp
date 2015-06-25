#include "StdAfx.h"


#include "IntersectionNodeInSim.h"
#include "StandInSim.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
#include "StandBufferInSim.h"
#include "RunwayInSim.h"
#include "RunwaySegInSim.h"
#include "../../Database/ADODatabase.h"

#define POSITIVE_NUM 1.0
#define NEGTIVE_NUM -1.0

IntersectionNodeInSim * IntersectionNodeInSimList::GetNodeByID( int nId )
{
	for(int i=0;i < GetNodeCount() ;i++)
	{
		IntersectionNodeInSim * pNode = GetNodeByIndex(i);
		if(pNode->GetNodeInput().GetID() == nId )
		{
			return pNode;
		}
	}
	return NULL;
}

IntersectionNodeInSim* IntersectionNodeInSimList::GetNodeByName(const CString& sName)
{
	for(int i=0;i < GetNodeCount() ;i++)
	{
		IntersectionNodeInSim * pNode = GetNodeByIndex(i);
		if(pNode->GetNodeInput().GetName() == sName )
		{
			return pNode;
		}
	}
	return NULL;
}

bool IntersectionNodeInSimList::Init( int nPrjId, int nAirportId )
{
	m_vNodeInputs.ReadData(nAirportId);
	//read intersection nodes
	m_vNodes.clear();
	for (int i =0 ;i<m_vNodeInputs.GetCount();i++)
	{
		m_vNodes.push_back(new IntersectionNodeInSim(m_vNodeInputs.NodeAt(i)));
	}

	//set block attribute
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TB_INTERSECTIONNODEBLOCK"));

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nNodeID = -1;
		adoRecordset.GetFieldValue("NODEID",nNodeID);
		int nBlock = 0;
		adoRecordset.GetFieldValue("UNBLOCK",nBlock);
		IntersectionNodeInSim* pNode = GetNodeByID(nNodeID);
		if (pNode)
			pNode->SetNoParking(nBlock > 0?false:true);

		adoRecordset.MoveNextData();
	}
	return true;
}

IntersectionNodeInSim* IntersectionNodeInSimList::GetIntersectionNode( int obj1Id, int obj2Id , int idx )
{
	if (obj1Id <0 || obj2Id <0)
		return NULL;

	for(int i=0 ; i< GetNodeCount();i++)
	{
		IntersectionNodeInSim * pNode  = GetNodeByIndex(i);
		IntersectionNode nodeInput = pNode->GetNodeInput();
		if( nodeInput.HasObject(obj1Id) && nodeInput.HasObject(obj2Id) && idx == nodeInput.GetIndex() )
			return pNode;
	}
	return NULL;
}
void IntersectionNodeInSimList::Add(IntersectionNodeInSim* pIntersectionNodeInSim)
{
	m_vNodes.push_back(pIntersectionNodeInSim);    
}
void IntersectionNodeInSimList::Add(size_t nPos,IntersectionNodeInSim* pIntersectionNodeInSim)
{
	m_vNodes.insert(m_vNodes.begin() + nPos,pIntersectionNodeInSim);
}
//IntersectionNodeInSim* IntersectionNodeInSimList::GetIntersectionNode( int obj1Id, int obj2Id , int idx )
//{
//	/*for(int i=0 ; i< GetNodeCount();i++)
//	{
//		IntersectionNodeInSim * pNode  = GetNodeByIndex(i);
//		IntersectionNode nodeInput = pNode->GetNodeInput();
//		if( ((nodeInput.GetObject1ID() == obj1Id && nodeInput.GetObject2ID() == obj2Id) || (nodeInput.GetObject1ID() == obj2Id && nodeInput.GetObject2ID() == obj1Id) )&& idx == nodeInput.GetIndex() )
//			return pNode;
//	}*/
//	return NULL;
//}

CPoint2008 IntersectionNodeInSim::GetDistancePoint( double dist ) const
{
	return m_nodeinput.GetPosition();
}

CString IntersectionNodeInSim::PrintResource() const
{
	CString strName;
	strName.Format("%d", m_nodeinput.GetID() );
	return strName;
}

CPath2008 IntersectionNodeInSim::GetFiletRoutePath(const FilletTaxiway& fillet, int nLeadObjID)
{
	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(fillet) )
		{
			bool needReverse = thefilet.GetObject1ID()!= nLeadObjID;
			return thefilet.GetRoutePath(needReverse);
		}
	}

	return CPath2008();
}

IntersectionNodeInSim::IntersectionNodeInSim( const IntersectionNode& node )
{
	m_nodeinput = node;
	m_pLockFlight = NULL;
	m_bNoParking = true;
	FilletTaxiway::ReadIntersectionFillets(node,m_vFilletTaxiways);
}

bool IntersectionNodeInSim::FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem ,DistanceUnit radius, Clearance& newClearance)
{
	//if (lastItem.GetMode() < OnHeldAtTempParking)
	//{

	//	ClearanceItem atParkingItem = lastItem;
	//	atParkingItem.m_nMode = OnHeldAtTempParking;
	//	ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atParkingItem);
	//	atParkingItem.SetTime(lastItem.GetTime()  +dT);
	//	lastItem = atParkingItem;
	//	newClearance.AddItem(lastItem);		
	//	return true;
	//}

	if (lastItem.GetMode() < OnTaxiToStand)
	{
		ElapsedTime exitTime = 0L;
		StandInSim* pStand = pFlight->GetOperationParkingStand();
		ElapsedTime tBuffer = pFlight->GetAirTrafficController()->GetStandBuffer()->GetStandBufferTime(pFlight,pStand);
		if (pFlight->GetOperationParkingStand()->GetLockedFlight())
		{
			exitTime = pFlight->GetOperationParkingStand()->GetLockedFlight()->GetTime()+ tBuffer;
		}
		else
		{
			exitTime = pStand->GetLastOccupyInstance().GetExitTime() + tBuffer;
		}

		if(exitTime > lastItem.GetTime() )
		{
			lastItem.SetTime(exitTime);
			lastItem.SetMode(OnHeldAtTempParking);
			newClearance.AddItem(lastItem);	
			return true;
		}
		else
		{
			ClearanceItem taxiItem = lastItem;
			taxiItem.SetMode(OnTaxiToStand);
			lastItem = taxiItem;
			newClearance.AddItem(lastItem);
			m_pLockFlight = NULL;
			return false;
		}			

		return true;
	}
	return true;
}

//void IntersectionNodeInSim::ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime)
//{
//	if(GetLockedFlight() == pFlight){
//		m_pLockFlight = NULL;
//		m_releaseTime = tTime;
//	}
//}
//
//bool IntersectionNodeInSim::GetLock( AirsideFlightInSim * pFlight )
//{
//	if(TryLock(pFlight)){
//		m_pLockFlight = pFlight;
//		return true;	
//	}
//	return false;
//}
//
//AirsideFlightInSim * IntersectionNodeInSim::GetLockedFlight()
//{
//	return m_pLockFlight;
//}
//
//bool IntersectionNodeInSim::TryLock( AirsideFlightInSim * pFlight )
//{
//	if(m_pLockFlight && m_pLockFlight!= pFlight)
//		return false;
//	return true;
//}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( TaxiwayDirectSegInSim * pSeg1, TaxiwayDirectSegInSim* pSeg2,DistanceUnit distInSeg1, DistanceUnit dsitInSeg2 )
{
	
	FilletTaxiway matchfilet(pSeg1->GetExitNode()->GetNodeInput());
	int taxiid1 = pSeg1->GetTaxiwaySeg()->GetTaxiwayID();
	int taxiid2 = pSeg2->GetTaxiwaySeg()->GetTaxiwayID();

	int nTaxiItemID1 = -1;
	TaxiwayIntersectItem* pTaxiItem1 = m_nodeinput.GetTaxiwayItem(taxiid1);
	if(pTaxiItem1)
		nTaxiItemID1 = pTaxiItem1->GetUID();
	int nTaxiItemID2 = -1;
	TaxiwayIntersectItem* pTaxiItem2 = m_nodeinput.GetTaxiwayItem(taxiid2);
	if(pTaxiItem2)
		nTaxiItemID2 = pTaxiItem2->GetUID();


	matchfilet.SetFilletPoint1( FilletPoint(nTaxiItemID1,pSeg1->IsPositiveDir()?-1.0 : 1.0 ) );
	matchfilet.SetFilletPoint2( FilletPoint(nTaxiItemID2,pSeg2->IsPositiveDir()?1.0:-1.0 ) );
	DistanceUnit segDist = pSeg1->GetLength();
	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(matchfilet) )
		{
			DistanceUnit distOffset1=0;
			DistanceUnit distOffset2=0;
			if( thefilet.GetFilletPoint1().IsAtSameSide(matchfilet.GetFilletPoint1()) )
			{
				distOffset1 = thefilet.GetFilletPoint1().GetRealDist();
				distOffset2 = thefilet.GetFilletPoint2().GetRealDist();
			}else { 
				distOffset1 = thefilet.GetFilletPoint2().GetRealDist();
				distOffset2 = thefilet.GetFilletPoint1().GetRealDist();
			}
			if( abs(distOffset2)<=dsitInSeg2 && distInSeg1 <= segDist-abs(distOffset1) ){
				return &thefilet;
			}
			
		}
	}
	return NULL;
}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( LogicRunwayInSim* pRunway, TaxiwayDirectSegInSim* pSeg )
{
	FilletTaxiway matchfilet(pSeg->GetEntryNode()->GetNodeInput());
	int runwayid = pRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	int taxiid = pSeg->GetTaxiwaySeg()->GetTaxiwayID();

	int nRwItemID = -1;
	RunwayIntersectItem* pItem = m_nodeinput.GetRunwayItem(runwayid);
	if( pItem )
		nRwItemID = pItem->GetUID();
	int nTaxiItemID = -1;
	TaxiwayIntersectItem* pTaxiItem = m_nodeinput.GetTaxiwayItem(taxiid);
	if(pTaxiItem)
		nTaxiItemID = pTaxiItem->GetUID();


	DistanceUnit distoff = ( (pRunway->getLogicRunwayType() == RUNWAYMARK_FIRST) ? (-1):(1) );
	matchfilet.SetFilletPoint1( FilletPoint(nRwItemID, distoff) );
	matchfilet.SetFilletPoint2( FilletPoint(nTaxiItemID,pSeg->IsPositiveDir()?1.0:-1.0) );
	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(matchfilet) )
		{
			return &thefilet;
		}
	}
	return NULL;
}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( TaxiwayDirectSegInSim * pSeg, LogicRunwayInSim *pRunway )
{
	FilletTaxiway matchfilet(pSeg->GetExitNode()->GetNodeInput());
	int runwayid = pRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	int taxiid = pSeg->GetTaxiwaySeg()->GetTaxiwayID();

	DistanceUnit rwOFFSET = ( (pRunway->getLogicRunwayType() == RUNWAYMARK_FIRST) ?  POSITIVE_NUM : NEGTIVE_NUM );
	
	int nRwItemID = -1;
	RunwayIntersectItem* pItem = m_nodeinput.GetRunwayItem(runwayid);
	if( pItem )
		nRwItemID = pItem->GetUID();
	int nTaxiItemID = -1;
	TaxiwayIntersectItem* pTaxiItem = m_nodeinput.GetTaxiwayItem(taxiid);
	if(pTaxiItem)
		nTaxiItemID = pTaxiItem->GetUID();

	DistanceUnit taxiOFFSET = 0; 
	if(pSeg->GetTaxiwaySeg()->GetNode1()->GetID()== this->GetID())
	{
		taxiOFFSET = POSITIVE_NUM;
	}
	else
		taxiOFFSET = NEGTIVE_NUM;

	matchfilet.SetFilletPoint1( FilletPoint(nRwItemID, rwOFFSET) );
	matchfilet.SetFilletPoint2( FilletPoint(nTaxiItemID,taxiOFFSET) );
	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(matchfilet) )
		{
			return &thefilet;
		}
	}
	return NULL;
}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( LogicRunwayInSim* pRunway, FlightGroundRouteDirectSegInSim* pSeg )
{
	if (pSeg == NULL)
		return NULL;

	if(pSeg->GetType() == AirsideResource::ResType_RunwayDirSeg)
	{
		//pRunwa
		FilletTaxiway matchfilet(pSeg->GetEntryNode()->GetNodeInput());
		int runwayid = pRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		int runwaysegid = ((RunwayDirectSegInSim *)pSeg)->GetRunwaySeg()->GetRunwayID();

		int nRwItemID = -1;
		RunwayIntersectItem* pItem = m_nodeinput.GetRunwayItem(runwayid);
		if( pItem )
			nRwItemID = pItem->GetUID();
		int nRunwayiItemID = -1;
		RunwayIntersectItem* pRunwayItem = m_nodeinput.GetRunwayItem(runwaysegid);
		if(pRunwayItem)
			nRunwayiItemID = pRunwayItem->GetUID();


		DistanceUnit distoff = ( (pRunway->getLogicRunwayType() == RUNWAYMARK_FIRST) ? (-1):(1) );
		matchfilet.SetFilletPoint1( FilletPoint(nRwItemID, distoff) );
		matchfilet.SetFilletPoint2( FilletPoint(nRunwayiItemID,pSeg->IsPositiveDir()?1.0:-1.0) );
		for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
		{
			FilletTaxiway& thefilet = m_vFilletTaxiways[i];
			if(thefilet.IsOverLapWith(matchfilet) )
			{
				return &thefilet;
			}
		}
		return NULL;



	}

	if(pSeg->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		return GetFilletFromTo(pRunway,(TaxiwayDirectSegInSim *)pSeg);
	}	

	return NULL;

}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( FlightGroundRouteDirectSegInSim * pSeg, LogicRunwayInSim *pRunway )
{
	if (pSeg == NULL)
		return NULL;

	if(pSeg->GetType() == AirsideResource::ResType_RunwayDirSeg)
	{
		

		FilletTaxiway matchfilet(pSeg->GetExitNode()->GetNodeInput());
		int runwayid = pRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		int runwaysegid = ((RunwayDirectSegInSim *)pSeg)->GetRunwaySeg()->GetRunwayID();

		DistanceUnit distoff = ( (pRunway->getLogicRunwayType() == RUNWAYMARK_FIRST) ? (1):(-1) );

		int nRwItemID = -1;
		RunwayIntersectItem* pItem = m_nodeinput.GetRunwayItem(runwayid);
		if( pItem )
			nRwItemID = pItem->GetUID();
		int nTaxiItemID = -1;
		RunwayIntersectItem* pTaxiItem = m_nodeinput.GetRunwayItem(runwaysegid);
		if(pTaxiItem)
			nTaxiItemID = pTaxiItem->GetUID();

		matchfilet.SetFilletPoint1( FilletPoint(nRwItemID, distoff) );
		matchfilet.SetFilletPoint2( FilletPoint(nTaxiItemID, pSeg->IsPositiveDir()?-1.0:1.0 ) );
		for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
		{
			FilletTaxiway& thefilet = m_vFilletTaxiways[i];
			if(thefilet.IsOverLapWith(matchfilet) )
			{
				return &thefilet;
			}
		}


	}

	if(pSeg->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		return GetFilletFromTo((TaxiwayDirectSegInSim *)pSeg,pRunway);
	}	

	return NULL;
}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( TaxiwayDirectSegInSim * pSeg1, RunwayDirectSegInSim* pSeg2 )
{
	FilletTaxiway matchfilet(pSeg1->GetExitNode()->GetNodeInput());
	int taxiid1 = pSeg1->GetTaxiwaySeg()->GetTaxiwayID();
	int runwayid2 = pSeg2->GetRunwaySeg()->GetRunwayID();

	int nTaxiItemID1 = -1;
	TaxiwayIntersectItem* pTaxiItem1 = m_nodeinput.GetTaxiwayItem(taxiid1);
	if(pTaxiItem1)
		nTaxiItemID1 = pTaxiItem1->GetUID();
	int nTaxiItemID2 = -1;
	RunwayIntersectItem* pRunwayItem2 = m_nodeinput.GetRunwayItem(runwayid2);
	if(pRunwayItem2)
		nTaxiItemID2 = pRunwayItem2->GetUID();


	matchfilet.SetFilletPoint1( FilletPoint(nTaxiItemID1,pSeg1->IsPositiveDir()?-1.0 : 1.0 ) );
	matchfilet.SetFilletPoint2( FilletPoint(nTaxiItemID2,pSeg2->IsPositiveDir()?1.0:-1.0 ) );

	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(matchfilet) )
		{
			return &thefilet;
		}
	}
	return NULL;
}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( RunwayDirectSegInSim * pSeg1, TaxiwayDirectSegInSim* pSeg2 )
{
	FilletTaxiway matchfilet(pSeg1->GetExitNode()->GetNodeInput());
	int runwayid1 = pSeg1->GetRunwaySeg()->GetRunwayID();
	int taxiwayid2 = pSeg2->GetTaxiwaySeg()->GetTaxiwayID();

	int rumwayItemId = -1;
	RunwayIntersectItem* pRunwayItem2 = m_nodeinput.GetRunwayItem(runwayid1);
	if(pRunwayItem2)
		rumwayItemId = pRunwayItem2->GetUID();

	int taxiItemId = -1;
	TaxiwayIntersectItem* pTaxiItem1 = m_nodeinput.GetTaxiwayItem(taxiwayid2);
	if(pTaxiItem1)
		taxiItemId = pTaxiItem1->GetUID();

	matchfilet.SetFilletPoint1( FilletPoint(rumwayItemId,pSeg1->IsPositiveDir()?-1.0 : 1.0 ) );
	matchfilet.SetFilletPoint2( FilletPoint(taxiItemId,pSeg2->IsPositiveDir()?1.0:-1.0 ) );

	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(matchfilet) )
		{
			return &thefilet;
		}
	}
	return NULL;
}

FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( TaxiwayDirectSegInSim * pSeg1, TaxiwayDirectSegInSim* pSeg2 )
{
	FilletTaxiway matchfilet(pSeg1->GetExitNode()->GetNodeInput());
	int taxiid1 = pSeg1->GetTaxiwaySeg()->GetTaxiwayID();
	int taxiid2 = pSeg2->GetTaxiwaySeg()->GetTaxiwayID();

	int nTaxiItemID1 = -1;
	TaxiwayIntersectItem* pTaxiItem1 = m_nodeinput.GetTaxiwayItem(taxiid1);
	if(pTaxiItem1)
		nTaxiItemID1 = pTaxiItem1->GetUID();
	int nTaxiItemID2 = -1;
	TaxiwayIntersectItem* pTaxiItem2 = m_nodeinput.GetTaxiwayItem(taxiid2);
	if(pTaxiItem2)
		nTaxiItemID2 = pTaxiItem2->GetUID();


	matchfilet.SetFilletPoint1( FilletPoint(nTaxiItemID1,pSeg1->IsPositiveDir()?-1.0 : 1.0 ) );
	matchfilet.SetFilletPoint2( FilletPoint(nTaxiItemID2,pSeg2->IsPositiveDir()?1.0:-1.0 ) );
	DistanceUnit segDist = pSeg1->GetLength();
	for(int i=0;i< (int)m_vFilletTaxiways.size();i++)
	{
		FilletTaxiway& thefilet = m_vFilletTaxiways[i];
		if(thefilet.IsOverLapWith(matchfilet) )
		{								
			return &thefilet;
		}
	}
	return NULL;
}
FilletTaxiway* IntersectionNodeInSim::GetFilletFromTo( FlightGroundRouteDirectSegInSim* pSeg1, FlightGroundRouteDirectSegInSim* pSeg2 )
{
	if(pSeg1->GetType()==AirsideResource::ResType_RunwayDirSeg && pSeg2->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		return GetFilletFromTo((RunwayDirectSegInSim*)pSeg1,(TaxiwayDirectSegInSim*)pSeg2);
	}
	if(pSeg2->GetType() == AirsideResource::ResType_RunwayDirSeg&& pSeg1->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
	{
		return GetFilletFromTo((TaxiwayDirectSegInSim*)pSeg1,(RunwayDirectSegInSim*)pSeg2);
	}
	if(pSeg1->GetType() == AirsideResource::ResType_TaxiwayDirSeg && pSeg2->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		//ASSERT(FALSE);
		return GetFilletFromTo((TaxiwayDirectSegInSim*)pSeg1, (TaxiwayDirectSegInSim*)pSeg2);
	}
	return NULL;
}

bool IntersectionNodeInSim::IsNoParking() const
{
	if(!m_bNoParking)
		return false;

	if(GetNodeInput().GetRunwayIntersectItemList().empty())
	{
		if( m_vFilletTaxiways.size()<3 )
			return false;
	}


	int nValidIntersect = GetNodeInput().GetRunwayIntersectItemList().size()+GetNodeInput().GetTaxiwayIntersectItemList().size();
	if(nValidIntersect>=2)
	{
		return m_bNoParking;
	}
	return false;
}

ElapsedTime IntersectionNodeInSim::GetAvailableEntryTime( AirsideFlightInSim* pFlight, const ElapsedTime& entryTime, const ElapsedTime& exitTime )
{
	//int iEntryTimeidx = 0;  //available entry time at table index 
	//int iExitTimeidx = 0;   //available exit time at table index
	ElapsedTime stayTime = exitTime - entryTime;
	ASSERT( stayTime >= ElapsedTime(0L) );
	stayTime = max(stayTime, ElapsedTime(0L) );
	if( m_vOccupancyTable.size() ==0 )
	{
		return entryTime;
	}
	
	//from time begin to first occupy entry time	
	{
		OccupancyInstance& theFirst = *m_vOccupancyTable.begin();
		ElapsedTime freeTimeBegin = ElapsedTime(0L);
		ElapsedTime freeTimeEnd = theFirst.GetEnterTime();
		if( freeTimeEnd - freeTimeBegin > stayTime)
		{
			if( entryTime > freeTimeBegin && exitTime < freeTimeEnd )
			{
				return entryTime;
			}
			if( entryTime <= freeTimeBegin && stayTime < (freeTimeEnd -freeTimeBegin) )
			{
				return freeTimeBegin;
			}
		}
	}

	ElapsedTime maxFreeTBegin(0L);
	for(int i=0;i< (int)m_vOccupancyTable.size()-1;i++)
	{
		OccupancyInstance& theFirstOcyInstance = m_vOccupancyTable[i];
		OccupancyInstance& theSecondOcyInstance = m_vOccupancyTable[i+1];
		maxFreeTBegin = max(theFirstOcyInstance.GetExitTime(),maxFreeTBegin);

		ElapsedTime freeTimeBegin = maxFreeTBegin;//theFirstOcyInstance.GetExitTime() ;
		ElapsedTime freeTimeEnd = theSecondOcyInstance.GetEnterTime();


		if( freeTimeEnd - freeTimeBegin < stayTime  )
		{
			continue;
		}
		else
		{
			if( entryTime > freeTimeBegin && exitTime < freeTimeEnd )
			{
				return entryTime;
			}
			if( entryTime <= freeTimeBegin && stayTime < (freeTimeEnd -freeTimeBegin) )
			{
				return freeTimeBegin;
			}
		}
	}
	//last 
	{
		OccupancyInstance& theLast = *m_vOccupancyTable.rbegin();
		maxFreeTBegin = max(theLast.GetExitTime(),maxFreeTBegin);
		ElapsedTime freeTimeBegin = maxFreeTBegin;//theLast.GetExitTime();
		ElapsedTime freeTimeEnd = ElapsedTime::Max();

		if( freeTimeEnd - freeTimeBegin > stayTime)
		{
			if( entryTime > freeTimeBegin && exitTime < freeTimeEnd )
			{
				return entryTime;
			}
			if( entryTime <= freeTimeBegin && stayTime < (freeTimeEnd -freeTimeBegin) )
			{
				return freeTimeBegin;
			}
		}

	}
	return entryTime;
}

bool IntersectionNodeInSim::IsActiveFor( ARCMobileElement* pmob,const RouteDirPath* pPath ) const
{
	if(m_pLockFlight && m_pLockFlight!= pmob)
		return false;
	return true;
}


void IntersectionNodeInSim::OnFlightEnter( AirsideFlightInSim* pflt, const ElapsedTime&t )
{
	//ASSERT( !(m_pLockFlight&& m_pLockFlight!=pflt) );
	if(m_pLockFlight && m_pLockFlight!=pflt)
		OnMobielExit(t);
	m_pLockFlight = pflt;
	OnMobileEnter(t);
}

void IntersectionNodeInSim::OnFlightExit( AirsideFlightInSim* pflt, const ElapsedTime& t )
{
	//ASSERT(m_pLockFlight == pflt);
	if(m_pLockFlight)
		m_pLockFlight = NULL;
	OnMobielExit(t);
}

bool IntersectionNodeInSim::bLocked( ARCMobileElement* pmob, const RouteDirPath* pPath ) const
{
	return m_pLockFlight && m_pLockFlight!=pmob;
}

bool IntersectionNodeInSim::bMobileInResource( ARCMobileElement* pmob ) const
{
	return m_pLockFlight==pmob;
}

#include "..\..\Results\AirsideflightEventLog.h"
void IntersectionNodeInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetID();
	resDesc.resType = GetType();
	resDesc.strRes=PrintResource();
}

void IntersectionNodeInSim::SetNoParking( bool b )
{
	m_bNoParking = b;
}

ElapsedTime IntersectionNodeInSim::GetLastOcyTime(  ) const
{
	if(m_vOccupancyTable.empty())
		return ElapsedTime(0L);
	return m_vOccupancyTable.back().GetExitTime();
}

ElapsedTime IntersectionNodeInSim::GetLastLandingTakeoffTime() const
{
	OccupancyTable::const_reverse_iterator ritr; 
	for(ritr = m_vOccupancyTable.rbegin(); ritr!=m_vOccupancyTable.rend();++ritr)
	{
		if( ritr->GetOccupyType()== OnLanding || ritr->GetOccupyType()== OnTakeoff)
		{
			return ritr->GetExitTime();
		}
	}
	return ElapsedTime(0L);
}

bool IntersectionNodeInSim::IsNeedToCheckHold() const
{
	return m_bNoParking;
}
