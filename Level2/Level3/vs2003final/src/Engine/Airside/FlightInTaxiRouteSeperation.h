#pragma once

enum ConflictionTYPE{  NONE =0, LEAD_FLIGTH, OPPOSITE_FLIGHT, INTESECTION_NODE };
//the separations between the lead flight
class TaxiRouteSeperationWithLeadFlight // check separation with lead flight
{
public:
	static ConflictionTYPE GetType(){ return LEAD_FLIGTH; }
	TaxiRouteSeperationWithLeadFlight(TaxiRouteInSim& _theRout, AirsideFlightInSim* _pFlight,const DistanceUnit& dCurDistInRoute)
		:theRoute(_theRout){
			pFlight = _pFlight;	dFltCurDistInRoute = dCurDistInRoute;
		}

		bool DoCheck(){

			Clear();
			int nBeginItemIdx;
			DistanceUnit dDistInItem;
			theRoute.GetItemIndexAndDistInRoute(dFltCurDistInRoute,nBeginItemIdx,dDistInItem);

			DistanceUnit distToLeadFlight = 0;			

			if( nBeginItemIdx >= 0 )
			{
				TaxiRouteItemInSim& atItem = theRoute.ItemAt(nBeginItemIdx);
				AirsideResource * thisResource = atItem.GetResource();
				AirsideFlightInSim *pLeadFlight = thisResource->GetFlightAhead(pFlight); 
				if( pLeadFlight && pLeadFlight != pFlight )
					m_pLeadFlight = pLeadFlight;
			}
			//continue searching next route items
			if(!m_pLeadFlight)
			{	
				for(int i=nBeginItemIdx+1;i<theRoute.GetItemCount();i++)
				{
					TaxiRouteItemInSim& theItem = theRoute.ItemAt(i);
					AirsideResource* theResource = theItem.GetResource();
					AirsideFlightInSim *pLeadFlight = theResource->GetLastInResourceFlight();	
					if( pLeadFlight&&pLeadFlight!=pFlight )
					{
						m_pLeadFlight = pLeadFlight;						
						break;
					}					
				}
			}

			if( m_pLeadFlight )
			{
				int nLeadFlightAtItemIndex = theRoute.GetItemIndex( m_pLeadFlight->GetResource() );
				ASSERT( nLeadFlightAtItemIndex >= nBeginItemIdx );
				m_dLeadFlightDistInRoute = theRoute.GetDistInRoute(nLeadFlightAtItemIndex, m_pLeadFlight->GetDistInResource() );

				DistanceUnit dSeperation = pFlight->GetLength()*0.5 + m_pLeadFlight->GetLength()*0.5;

				if (pFlight->GetMode() == OnExitRunway || pFlight->GetMode() == OnTaxiToStand)		//inbound
					dSeperation += pFlight->GetPerformance()->getTaxiInboundSeparationDistInQ(pFlight);
				else
					dSeperation += pFlight->GetPerformance()->getTaxiOutboundSeparationDistInQ(pFlight);	//outbound

				m_dSafeDistInRoute = m_dLeadFlightDistInRoute - dSeperation;
				return true;
			}			

			return false;
		};

		void Clear(){  m_pLeadFlight = NULL; m_dSafeDistInRoute = ARCMath::DISTANCE_INFINITE; m_dLeadFlightDistInRoute = 0; }
public:	
	DistanceUnit m_dSafeDistInRoute;
	AirsideFlightInSim* m_pLeadFlight;
	DistanceUnit m_dLeadFlightDistInRoute;

protected:
	TaxiRouteInSim& theRoute;
	AirsideFlightInSim* pFlight;
	DistanceUnit dFltCurDistInRoute;
	//ClearanceItem lastItem;
};


//the separations with the opposite flight in the route
class TaxiRouteSeperationWithOppositeFlight //check the separation with opposite flight
{
public:
	static ConflictionTYPE GetType(){ return OPPOSITE_FLIGHT; }
	TaxiRouteSeperationWithOppositeFlight(TaxiRouteInSim& _theRout, AirsideFlightInSim* _pFlight,const DistanceUnit& curDistInRoute,double taxispd)
		:theRoute(_theRout)
	{
		pFlight = _pFlight;	
		dFltCurDistInRoute = curDistInRoute; 
		dTaxiSpd = taxispd;
		Clear();
	}
	bool DoCheck()
	{
		Clear();
		int nBeginItemIdx;DistanceUnit distInItem;
		theRoute.GetItemIndexAndDistInRoute(dFltCurDistInRoute,nBeginItemIdx,distInItem);

		if( theRoute.IsLockDirectionOfWholeRoute() && nBeginItemIdx < 0)  // flight going to exit stand,try to lock the direction of the whole route
		{
			

			for(int i = 0 ;i<theRoute.GetItemCount();i++)
			{
				AirsideResource * thisRes = theRoute.ItemAt(i).GetResource();
				if( thisRes && thisRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
				{
					TaxiwayDirectSegInSim* pDirSeg = (TaxiwayDirectSegInSim*) thisRes;
					if(pDirSeg->GetDirectionLock()->HasFlight(pFlight))
						continue;

					TaxiwayDirLockQueue& dirLockQueue = pDirSeg->GetDirectionLock()->m_lockQueue;
					bool bStopbyQueue = false;
					if(dirLockQueue.GetCount() )
					{
						TaxiwayDirLockQueue::QueueItem queueItem  = *dirLockQueue.m_vItems.begin();
						bStopbyQueue = (pFlight!=queueItem.m_pFlight && queueItem.m_dir!=pDirSeg->GetDir() );
					}
					if(!pDirSeg->TryLockDir(pFlight) || bStopbyQueue )
					{
						m_pDirectionLock = pDirSeg->GetTaxiwaySeg()->GetDirectionLock();		
						m_dSafeDistInRoute = -1;
						return true;						
					}
				}				
			}	
			//lock direction of all the route			
			for(int i= 0; i< theRoute.GetItemCount();i++)
			{
				AirsideResource*thisRes = theRoute.ItemAt(i).GetResource();
				if( thisRes && thisRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
				{
					TaxiwayDirectSegInSim* pDirSeg = (TaxiwayDirectSegInSim*) thisRes;
					pDirSeg->GetDirLock(pFlight);
				}
			}
			return true;
		}

		if(!theRoute.IsLockDirectionOfWholeRoute()){//lock directions of  part of the route
			//			
			for(int i=nBeginItemIdx+1;i<theRoute.GetItemCount();i++)
			{
				TaxiRouteItemInSim& theItem = theRoute.ItemAt(i);
				AirsideResource * theRes = theItem.GetResource();
				if(theRes && theRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
				{
					TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim*)theRes;						
					if(pDirSeg->GetDirectionLock()->HasFlight(pFlight))
						continue;
					TaxiwayDirLockQueue& dirLockQueue = pDirSeg->GetDirectionLock()->m_lockQueue;
					bool bStopbyQueue = false;
					if(dirLockQueue.GetCount() )
					{
						TaxiwayDirLockQueue::QueueItem queueItem  = *dirLockQueue.m_vItems.begin();
						bStopbyQueue = (pFlight!=queueItem.m_pFlight && queueItem.m_dir!=pDirSeg->GetDir() );
					}

					if(pDirSeg->TryLockDir(pFlight)&&!bStopbyQueue )
					{
						pDirSeg->GetDirLock(pFlight);					
					}
					else  //can not lock the segment, register to the queue of the segment
					{
						if(!m_pDirectionLock)
						{
							m_pDirectionLock = pDirSeg->GetTaxiwaySeg()->GetDirectionLock();						
							HoldInTaxiRoute* pEntryNodeHold = theRoute.GetHoldList().GetEntryHold(pDirSeg->GetEntryNode());
							if(pEntryNodeHold){
								m_dSafeDistInRoute = (theRoute,nBeginItemIdx,pEntryNodeHold->m_dDistInRoute);
							}
							else{
								ASSERT(FALSE);
								m_dSafeDistInRoute = theRoute.GetDistInRoute(nBeginItemIdx,pDirSeg->GetLength())-pFlight->GetLength()*0.5;
							}						
						}
						DistanceUnit dTravlDist = theRoute.GetDistInRoute(i,0) - dFltCurDistInRoute;
						ElapsedTime dT = ElapsedTime(dTravlDist/dTaxiSpd); 						
						pDirSeg->GetDirectionLock()->m_lockQueue.AddToQueue(pFlight,pDirSeg->GetDir(), pFlight->GetTime() + dT );
						return true;
					}
				}
			}			
		}
		return false;
	}

	void Clear(){  m_pDirectionLock = NULL; m_dSafeDistInRoute = ARCMath::DISTANCE_INFINITE;}
public:
	//int m_nConflictItemIndex;
	//AirsideFlightInSim* m_pOppositeFlight;	
	DistanceUnit m_dSafeDistInRoute;
	TaxiwaySegDirectionLock * m_pDirectionLock;

protected:
	TaxiRouteInSim& theRoute;
	AirsideFlightInSim* pFlight;	
	DistanceUnit dFltCurDistInRoute;
	double dTaxiSpd;
};


//the separations in the node
class TaxiRouteSeperationWithIntersectionNodes
{
public:
	static ConflictionTYPE GetType(){ return INTESECTION_NODE; }
	TaxiRouteSeperationWithIntersectionNodes(TaxiRouteInSim& _theRout, AirsideFlightInSim* _pFlight,const DistanceUnit& fltDistInRoute)
		:theRoute(_theRout)
	{
		pFlight = _pFlight;dFltCurDistInRoute = fltDistInRoute;				
	}

	bool DoCheck()
	{
		Clear();			

		std::vector<HoldInTaxiRoute> vNodeHoldList= theRoute.GetHoldList().GetNextRouteNodeEntryHoldList(dFltCurDistInRoute);		

		for(int i=0;i< (int)vNodeHoldList.size();i++)
		{
			HoldInTaxiRoute& TheNodeHold = vNodeHoldList[i];
			AirsideFlightInSim* pInNodeFlight = TheNodeHold.m_pNode->GetLastInResourceFlight();
			if( pInNodeFlight && pInNodeFlight!=pFlight  )
			{
				m_pConflictNode = TheNodeHold.m_pNode;
				m_pInNodeFlight = pInNodeFlight;	
				m_dSafeDistInRoute = TheNodeHold.m_dDistInRoute;				
				return true;
			}
		}		
		return false;
	}

	void Clear(){  m_pInNodeFlight = NULL; m_pConflictNode = NULL; m_dSafeDistInRoute = ARCMath::DISTANCE_INFINITE;}
public:
	AirsideFlightInSim* m_pInNodeFlight;
	IntersectionNodeInSim* m_pConflictNode;	
	DistanceUnit m_dSafeDistInRoute;


protected:
	TaxiRouteInSim& theRoute;
	AirsideFlightInSim* pFlight;	
	DistanceUnit dFltCurDistInRoute;
};




