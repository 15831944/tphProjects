#include "StdAfx.h"
#include ".\departurepaxlandsidebehavior.h"
#include "person.h"
#include "MobElementsDiagnosis.h"
#include "pax.h"
#include "GroupLeaderInfo.h"
#include "terminal.h"
#include <common/Point2008.h>
#include <common/ARCVector4.h>
#include "Inputs/NonPaxRelativePosSpec.h"
#include "LandsideResourceManager.h"
#include "LandsideVehicleInSim.h"
#include "LandsideParkingLotInSim.h"
#include "./ARCportEngine.h"
#include "LandsideSimulation.h"
#include ".\SimEngineConfig.h"
#include "PaxTerminalBehavior.h"
#include "LandsideCurbsideInSim.h"
#include "WalkwayInSim.h"
#include "Landside/LandsideWalkway.h"
#include "Landside/FacilityBehavior.h"
#include "VisitorLandsideBehavior.h"
#include "LandsideBusStationInSim.h"
#include "LandsideResidentVehicleInSim.h"
#include "FacilityBehaviorsInSim.h"
#include "LandsideSimErrorShown.h"
#include "Landside/LandsideCurbSide.h"
#include "Landside/LandsideBusStation.h"



DeparturePaxLandsideBehavior::DeparturePaxLandsideBehavior( Person* p)
:PaxLandsideBehavior(p)
,m_nLinkTerminalFloor(0)
{	
}

DeparturePaxLandsideBehavior::~DeparturePaxLandsideBehavior(void)
{
}
//////////////////////////////////////////////////////////////////////////
int DeparturePaxLandsideBehavior::performanceMove( ElapsedTime p_time,bool bNoLog )
{
	switch( getState() )
	{
	case EntryLandside: //leave terminal 
		processEntryLandside(p_time);
		break;
	//case On_Vehicle: //stay on the vehicle
	//	processOnVehicle(p_time);
	//	break;	
	//case GetOff_Vehice: //get off the vehicle
	//	processGetOffVehicle(p_time);
	//	break;
	//case Pick_Bag: //pick bag from the vehicle
	//	processPickBag(p_time);
	//	break;
	//case GoTo_Curbside: //go to curbside  and entry to terminal..
	//	processGotoCurbside(p_time);
	//	break;
	//case ArrivalAtCubside:
	//	processArrivalAtCurbside(p_time);
		break;
	case ArrivalAtTerminalPosition:
		processArrivalAtTerminalPosition(p_time);
		break;

	case StayOnVehicle:
		{

		}
		break;
	case GetOffVehicle:
		{
			processGetOffVehicle(p_time);
		}
		break;
	case PickCarryOnFromVehicle:
		{
			processPickCarryonFromVehicle(p_time);
		}
		break;
	case LandsideLeaveQueue:
		{
			ProcessLeaveQueue(p_time);
		}
		break; 
	case  MoveToFacility:
		{
			processMoveToFacility(p_time);
		}
		break;
	case ArriveAtFacility:
		{
			processArriveAtFacility(p_time);
		}
		break;
	case  WaitingForVehicle:
		{

		}
		break;
	case MoveToVehicle:
		{
			processMoveToVehicle(p_time);
		}
		break;
	case PutCarryonToVehicle:
		{
			ProcessPutCarryonToVehicle(p_time);
		}
		break;
	case GetOnVehicle:
		{
			ProcessGetOnVehicle(p_time);
		}
		break;


	default: 
		ASSERT(FALSE);
	}

	return TRUE;
}
//entry landside just stay on vehicle
void DeparturePaxLandsideBehavior::processEntryLandside( const ElapsedTime& t )
{
	location = ARCVector3(0,0,0);

	if(m_pVehicle)
	{
		m_pVehicle->AddOnPax(this);		
		m_pPerson->setState(StayOnVehicle);
		//m_pPerson->getLogEntry().setEntryTime(t);
		//need to update all the element entry time
		UpdateEntryTime(t);
		setLocation(location);
		setDestination(location);
		WriteLogEntry(t);
	}
	else
	{
		processEntryTerminal(t, 0.0);
	}

	GenerateEvent(t);
}

void DeparturePaxLandsideBehavior::processOnVehicle( const ElapsedTime& t )
{
	//do nothing just wait for vehicle stop
}

void DeparturePaxLandsideBehavior::processGetOffVehicle( const ElapsedTime& t )
{
	//the position where the vehicle stops
	//
	setDestination(m_pVehicle->getLastState().pos);
	//setLocation(m_pVechile->getState().pos);
	WriteLogEntry(t);

	CPoint2008 nextPt(0,0,0);

	LandsideResourceInSim* pCurRes = getResource();
	if( pCurRes  )
	{
		if(pCurRes->toCurbSide() || 
			pCurRes->toBusSation())
		{
			nextPt = pCurRes->GetPaxWaitPos();
		}
		else
		{
			LandsideResourceInSim* pParkingPlace  = (LandsideResourceInSim*)m_pVehicle->getLastState().getLandsideRes();
			if(pParkingPlace->toParkLotSpot())
			{
				nextPt = pParkingPlace->GetPaxWaitPos();
			}
			else
			{
				ASSERT(0);
			}
		}

		//if (pCurbside)
		//{
		//	nextPt = pCurbside->GetPaxWaitPos();
		//}
		//LandsideParkingSpotInSim *pParkingSpot = pCurRes->toParkLotSpot();
		//if(pParkingSpot)
		//{
		//	nextPt = pParkingSpot->getEntryPos2();
		//}
	}
	//move outside vehicle
	CPoint2008 nextpos = m_pVehicle->getOffVehiclePos(nextPt);
	setDestination(nextpos);
	ElapsedTime etime = moveTime();
	//setLocation(nextpos);
	WriteLogEntry(etime + t);
	m_pVehicle->RemoveOnPax(this, t);

	//start to pick the Bag
	m_pPerson->setState(PickCarryOnFromVehicle);
	GenerateEvent(etime+t);

	//set destination


}
//not implement yet
void DeparturePaxLandsideBehavior::processPickCarryonFromVehicle( const ElapsedTime& t )
{
	//start pick the  bag
	WriteLogEntry(t);

	ElapsedTime etime = ElapsedTime(30L);
	WriteLogEntry(etime + t);

	m_pPerson->setState(MoveToFacility);
	GenerateEvent(etime + t);

	////get destination
	////check the current resource	
	//LandsideResourceInSim* pCurrentRes = getResource();
	//if(pCurrentRes)
	//{
	//	//if curbside
	//	if(pCurrentRes->toCurbSide())
	//	{
	//		setDestResource(pCurrentRes);
	//		
	//	}
	//	else 
	//	{
	//		m_pVehicle->getOffVehiclePos()
	//		 if(pCurrentRes->toParkLotSpot())
	//		 {

	//		 }

	//	}
	//}
}


class WalkwaySorter
{
public:
	WalkwaySorter(const CPoint2008& p){ m_Pos = p; }
	bool operator()(CWalkwayInSim* w1,CWalkwayInSim* w2)
	{
		return getDist(w1) < getDist(w2);
	}
	DistanceUnit getDist(CWalkwayInSim* w1)
	{
		DistMap::iterator itr = m_Distmap.find(w1);
		if(itr!=m_Distmap.end()){
			return itr->second;
		}
		else
		{
			CPoint2008 ptNear = w1->GetCenterPath().getClosestPoint(m_Pos);
			DistanceUnit dist =  ptNear.distance(m_Pos);
			m_Distmap[w1] = dist;
			return dist;
		}
	}
protected:
	CPoint2008 m_Pos;
	typedef std::map<CWalkwayInSim*,DistanceUnit> DistMap;
	DistMap m_Distmap;
};
//do nothing go to terminal
void DeparturePaxLandsideBehavior::processMoveToFacility( const ElapsedTime& t )
{
	ElapsedTime eTime =t;
	m_pVehicle->RemoveOnPax(this,t);	
	m_pVehicle->Activate(eTime);
	
	if( LandsideResourceInSim* pRes = getResource())
	{
		LandsideLayoutObjectInSim* pObj = pRes->getLayoutObject();
		if(pObj)
		{
			if ( LandsideCurbSideInSim* pCurbside = pObj->toCurbSide())
			{
				ARCVector3 pos = pCurbside->GetPaxWaitPos();
				setResource(pCurbside);
				setDestination(location);
				ElapsedTime dt = moveTime();
				eTime += dt;
				WriteLogEntry(eTime);
				ASSERT(pCurbside);

				CFacilityBehavior *pBehavior = pCurbside->GetBehavior();
				if(pBehavior )
				{
					std::vector<ALTObjectID > vTermProcLinkage;
					//pBehavior->getLinkageList()->GetTerminalLinkageProcName(vTermProcLinkage);
					m_pLandsideSim->getFacilityBehaviors()->GetTerminalLinkageProcName(pBehavior->getLinkageList(),m_pPerson->GetTerminal(),pCurbside->getInput()->getName(),vTermProcLinkage);
					m_vLinkTerminalProc = vTermProcLinkage;
					
					if(vTermProcLinkage.size() > 0)
					{
						ProcessorID procID;
						procID.SetStrDict(m_pPerson->GetTerminal()->inStrDict);
						procID.setID(vTermProcLinkage.at(0).GetIDString());

						GroupIndex groupIndex = m_pPerson->GetTerminal()->procList->getGroupIndex(procID);
						if(groupIndex.start >= 0)
						{
							Processor *pTermProcConnected = m_pPerson->GetTerminal()->procList->getProcessor(groupIndex.start);

							if(pTermProcConnected)
							{
								m_nLinkTerminalFloor = pTermProcConnected->getFloor()/SCALE_FACTOR;
							}
						}
					}
				}

				//double dTermFloor = (double)pCurbside->getCurbInput()->m_nTernimalFloor;				
				processEntryTerminal(eTime,m_nLinkTerminalFloor);
				return;
			}
			else if(LandsideParkingLotInSim *pParkingLot = pObj->toParkLot())
			{
				if(pParkingLot->GetEmbedBusStationCount() > 0)//embed bus station
				{
					m_vDestBusStation.clear();


					std::vector<LandsideBusStationInSim *> vCanSelectedStation;

					int nEmbedStationCount = pParkingLot->GetEmbedBusStationCount();

					//need take shuttle bus to entry terminal
					//find the available terminal processor
					//then find the linkage bus station
					CPoint2008 startPos;
					startPos.init(getPoint().n[VX],getPoint().n[VY],getPoint().n[VZ]);

					double nTermFloor = startPos.getZ();
					TerminalMobElementBehavior* pTermPhase = new PaxTerminalBehavior((Passenger*)m_pPerson);
					std::vector<Processor *> vAvailableProc;
					pTermPhase->GetAvailbleEntryProcList(vAvailableProc);
					delete pTermPhase;

					//the embedded station
					//vector, the stations could embed station reach
					cpputil::nosort_map< LandsideBusStationInSim *, std::vector< LandsideBusStationInSim *> > mapEmbedDestStation;
					ASSERT(vAvailableProc.size() != 0);
					int nProcCount = static_cast<int>(vAvailableProc.size());
					for(int nProc = 0; nProc < nProcCount; ++ nProc )
					{
						Processor *pProc = vAvailableProc[nProc];
						ASSERT(pProc != NULL);

						CString strProc = pProc->getIDName();
						
						//get bus station could lead to terminal processor
						LandsideResourceInSim* pLandsideRes = m_pLandsideSim->getFacilityBehaviors()->GetLandsideTerminalLinkage(ALT_LBUSSTATION, *(pProc->getID()));
						if(pLandsideRes == NULL || pLandsideRes->toBusSation() == NULL)
							continue;

						LandsideBusStationInSim *pLinkedBusStation = pLandsideRes->toBusSation();
						if(pLinkedBusStation == NULL)
							continue;


						//check the bus station that this bus station could lead to
						double dShortestdist = (std::numeric_limits<double>::max)();
						for (int nEmbedBusStation = 0; nEmbedBusStation < nEmbedStationCount; ++ nEmbedBusStation)
						{
							LandsideBusStationInSim *pEmbedBusStation = pParkingLot->GetEmbedBusStation(nEmbedBusStation);
							if(pEmbedBusStation == NULL)
								continue;
							if(pEmbedBusStation->CanLeadTo(eTime, pLinkedBusStation))
							{
								mapEmbedDestStation[pEmbedBusStation].push_back(pLinkedBusStation);
								CPoint2008 ptBusStation;
								ptBusStation = pEmbedBusStation->getBusStation()->get2DCenter();
								double dDist = getPoint().DistanceTo(ptBusStation);
								if (dDist < dShortestdist)
								{
									dShortestdist = dDist;
									vCanSelectedStation.clear();
									vCanSelectedStation.push_back(pEmbedBusStation);
								}
								else if (dDist == dShortestdist)
								{
									vCanSelectedStation.push_back(pEmbedBusStation);
								}
								//vCanSelectedStation.push_back(pEmbedBusStation);
								//m_vDestBusStation.push_back(pLinkedBusStation);
							}

						}
					}

					if(mapEmbedDestStation.size() > 0)
					{
						int nCanSelCount = static_cast<int>(vCanSelectedStation.size()); 
						int nSelIndex = random(nCanSelCount);


						LandsideBusStationInSim *pSelectedBusStation = vCanSelectedStation[nSelIndex];
						m_vDestBusStation = mapEmbedDestStation[pSelectedBusStation];

						setDestResource(pSelectedBusStation);
						CPoint2008 endPos = pSelectedBusStation->GetPaxWaitPos();

						//go to bus station
						CLandsideTrafficSystem* pLandsideTrafficInSim = m_pLandsideSim->GetLandsideTrafficeManager();
						if( !pLandsideTrafficInSim->EnterTrafficSystem(eTime, ArriveAtFacility,this,startPos,endPos) )
						{
							setDestination(endPos);
							ElapsedTime dt = moveTime();
							eTime += dt;
							WriteLogEntry(eTime);

							m_pPerson->setState(ArriveAtFacility);
							GenerateEvent(eTime);

						}
						return;
					}
				}
				//move to terminal directly
				{
					m_pVehicle->Activate(t);
					m_pVehicle->RemoveOnPax(this,t);

					CPoint2008 startPos;
					startPos.init(getPoint().n[VX],getPoint().n[VY],getPoint().n[VZ]);
				
					//get passenger current floor
					//the landside floor is 1,2,3....
					//the terminal floor is 100,200, 300
					//so make it multiply 100
					double nTermFloor = m_pLandsideSim->GetLinkTerminalFloor((int)startPos.getZ());

					//get the processors linked with the parking lot in this floor
					//m_pLandsideSim->GetLandsideTerminalLinkageManager()->
					
					LandsideResourceInSim* pCurrentResource = m_pVehicle->getLastState().getLandsideRes();
					//int nParkinglotLevel = 0;
					int nParkingLotLevelID = 0;
					if(pCurrentResource && pCurrentResource->toParkLotSpot())
					{
						LandsideParkingSpotInSim *pParkingSpot = pCurrentResource->toParkLotSpot(); 
					//	nParkinglotLevel = pParkingSpot->getParkingLotLevelIndex();
						nParkingLotLevelID = pParkingSpot->GetParkingLotLevelID();
					}
					std::vector<ALTObjectID> vAltTerminalProc;
					m_pLandsideSim->getFacilityBehaviors()->GetTerminalProcLinkedWithParkingLot(pParkingLot->getName(), nParkingLotLevelID, vAltTerminalProc);
					if(vAltTerminalProc.size())
					{
						//get terminal processor which the passenger starts with
						//find walkways in parking	
						LandsideResourceManager *pResManager = m_pLandsideSim->GetResourceMannager();
						CFacilityBehaviorsInSim *pFacitlyBehaviors = m_pLandsideSim->getFacilityBehaviors();

						m_vLinkTerminalProc = vAltTerminalProc;
						
						TerminalMobElementBehavior* pTermPhase = new PaxTerminalBehavior((Passenger*)m_pPerson);
						std::vector<Processor *> vAvailableProc;
						pTermPhase->setLandsideSelectedProc(vAltTerminalProc);
						Processor *pEntryProc = pTermPhase->TryToSelectLandsideEntryProcessor(eTime);
						delete pTermPhase;

						if(pEntryProc)
						{
							Point ptServiceLocation = pEntryProc->GetServiceLocation();

							m_vLinkTerminalProc.clear();
							ALTObjectID altEntryProcID;
							altEntryProcID.FromString(pEntryProc->getID()->GetIDString());
							m_vLinkTerminalProc.push_back(altEntryProcID);
							

							CPoint2008 endPos;
							endPos.init(ptServiceLocation.getX(),ptServiceLocation.getY(),startPos.getZ());
							setDestination(endPos);

							CLandsideTrafficSystem* pLandsideTrafficInSim = m_pLandsideSim->GetLandsideTrafficeManager();
							if( !pLandsideTrafficInSim->EnterTrafficSystem(t,ArrivalAtTerminalPosition,this,startPos,endPos) )
							{
								ElapsedTime dt = moveTime();
								eTime += dt;
								WriteLogEntry(eTime);
											
								processEntryTerminal(eTime, nTermFloor);
							}
						}
						return;
					}	
					else
					{
						CString sError;
						sError.Format(_T("Can not find terminal Processors linked with %s"), pParkingLot->getName().GetIDString());
						LandsideSimErrorShown::PaxSimWarning(m_pPerson,sError,"Definition Error",t);
					}
				}
			}		
			else if(pObj->toBusSation())
			{
				//bus station
				//move to terminal
				if(m_vLinkTerminalProc.size() > 0)
				{
					ProcessorID procID;
					procID.SetStrDict(m_pPerson->GetTerminal()->inStrDict);
					procID.setID(m_vLinkTerminalProc.at(0).GetIDString());
					GroupIndex groupIndex = m_pPerson->GetTerminal()->procList->getGroupIndex(procID);
					if(groupIndex.start >= 0)
					{
						Processor* pTermProcConnected = m_pPerson->GetTerminal()->procList->getProcessor(groupIndex.start );
						if(pTermProcConnected)
						{
							m_nLinkTerminalFloor = pTermProcConnected->getFloor()/SCALE_FACTOR;
						}
					}
				}

				processEntryTerminal(eTime, m_nLinkTerminalFloor);
				return;
			}

		}
	}
	ASSERT(FALSE);
	processEntryTerminal(eTime,0.0);	

}

//void DeparturePaxLandsideBehavior::processArrivalAtCurbside( const ElapsedTime& t )
//{
//	Lansideres
//	if(LandsideCurbSideInSim* pCurb = getResource()->toCurbSide() )
//	{
//
//	}
//
//	LandsideCurbSideInSim* pCurbSide = (LandsideCurbSideInSim*)getResource();
//	pCurbSide->getlink
//	double dTermFloor = (double)pCurbSide->getCurbInput()->m_nTernimalFloor;
//	//m_pVechile->RemoveOnVehiclePax(this);	
//	m_pVehicle->Activate(t);
//	processEntryTerminal(t,dTermFloor);
//}

void DeparturePaxLandsideBehavior::processArrivalAtTerminalPosition( const ElapsedTime& t )
{
	double dTermFloor = (double)getPoint().z ;
	processEntryTerminal(t,m_nLinkTerminalFloor);
}

void DeparturePaxLandsideBehavior::processEntryTerminal( ElapsedTime p_time, double ifloor)
{	

	if( !simEngineConfig()->isSimTerminalMode() )
	{
		flushLog(p_time);
		return;
	}

	// arriving passenger.
	if(m_pPerson->getLogEntry().isArriving())
	{
		flushLog( p_time );
		return;
	}

	// not departing and turn around, impossible, just flush log.
	if(m_pPerson->getLogEntry().isArriving()&& !m_pPerson->getLogEntry().isTurnaround())
	{
		flushLog( p_time );
		return;
	}

	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pPerson->getLogEntry().GetMobileType() == 0)
	{
		m_pPerson->setState(Birth);
		m_pPerson->setBehavior( new PaxTerminalBehavior((Passenger*)m_pPerson) );
		TerminalMobElementBehavior* pTermPhase = m_pPerson->getTerminalBehavior();
		Point ptTerm(location[VX],location[VY],ifloor*SCALE_FACTOR);
		pTermPhase->setLocation( ptTerm  );	
		pTermPhase->setDestination(ptTerm);
        pTermPhase->SetFromLandSide(TRUE);
	//	pTermPhase->SetWalkOnBridge(TRUE);
		pTermPhase->setLandsideSelectedProc(m_vLinkTerminalProc);

		WriteLogEntry(p_time);

		//Follower 
		CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
		if(pGroupLeaderInfo->isInGroup() && !pGroupLeaderInfo->IsFollower())
		{
			// write other member of group
			MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
			int nFollowerCount = elemList.getCount();

			for( int i=0; i< nFollowerCount; i++ )
			{
				Person* _pFollower =(Person*) elemList.getItem( i );
				_pFollower->setBehavior( new TerminalMobElementBehavior(_pFollower) );

				PaxLandsideBehavior* spFollowLandsideBehavior = (PaxLandsideBehavior*) _pFollower->getLandsideBehavior();
				TerminalMobElementBehavior* spFollowTerminalBehavior = (TerminalMobElementBehavior*) _pFollower->getTerminalBehavior();
				
				
				Point ptTermLoc(spFollowLandsideBehavior->getPoint()[VX],spFollowLandsideBehavior->getPoint()[VY],ifloor*SCALE_FACTOR);
				spFollowTerminalBehavior->setLocation(ptTermLoc);

				Point ptTermDest(spFollowLandsideBehavior->getDest()[VX],spFollowLandsideBehavior->getDest()[VY],ifloor*SCALE_FACTOR);
				spFollowTerminalBehavior->setDestination(ptTermDest);
			
			}
		}

		//visitor
		int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
		for( int i=nCount-1; i>=0; i-- )
		{
			PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
			if (pVis == NULL)
				continue;
			pVis->setBehavior(new VisitorTerminalBehavior(pVis));

			VisitorTerminalBehavior *pVisitorTerminalBehavior =(VisitorTerminalBehavior *)pVis->getTerminalBehavior();
			VisitorLandsideBehavior *pVisitorLandsideBehavior =(VisitorLandsideBehavior *)pVis->getLandsideBehavior();

			ASSERT(pVisitorLandsideBehavior);
			if(pVisitorTerminalBehavior && pVisitorLandsideBehavior)
			{
				Point ptVisitorTermLoc(pVisitorLandsideBehavior->getPoint()[VX],pVisitorLandsideBehavior->getPoint()[VY],ifloor*SCALE_FACTOR);
				pVisitorTerminalBehavior->setLocation(ptVisitorTermLoc);

				Point ptVisitorTermDest(pVisitorLandsideBehavior->getDest()[VX],pVisitorLandsideBehavior->getDest()[VY],ifloor*SCALE_FACTOR);
				pVisitorTerminalBehavior->setDestination(ptVisitorTermDest);

				CGroupLeaderInfo* pVisGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
				if(pVisGroupLeaderInfo->isInGroup() && !pVisGroupLeaderInfo->IsFollower())
				{
					MobileElementList &visElemList = pVisGroupLeaderInfo->GetFollowerList();
					int nFollowerCount = visElemList.getCount();

					for( int i=0; i< nFollowerCount; i++ )
					{
						Person* _pFollower =(Person*) visElemList.getItem( i );;
						_pFollower->setBehavior(new VisitorTerminalBehavior((PaxVisitor *)_pFollower));
						VisitorLandsideBehavior* spFollowLandsideBehavior = (VisitorLandsideBehavior*) _pFollower->getLandsideBehavior();
						VisitorTerminalBehavior* spFollowTerminalBehavior = (VisitorTerminalBehavior*) _pFollower->getTerminalBehavior();

						Point ptTermLoc(spFollowLandsideBehavior->getPoint()[VX],spFollowLandsideBehavior->getPoint()[VY],ifloor*SCALE_FACTOR);
						spFollowTerminalBehavior->setLocation(ptTermLoc);

						Point ptTermDest(spFollowLandsideBehavior->getDest()[VX],spFollowLandsideBehavior->getDest()[VY],ifloor*SCALE_FACTOR);
						spFollowTerminalBehavior->setDestination(ptTermDest);

					}
				}

			}
			
		}
		GenerateEvent( p_time);
		return;//to Airside mode.

	}
	else
	{
		flushLog(p_time);
		return;
	}

	ASSERT(FALSE);
	return;
}

void DeparturePaxLandsideBehavior::InitializeBehavior()
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(pGroupLeaderInfo->isInGroup() && !pGroupLeaderInfo->IsFollower())
	{
		MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
		int nFollowerCount = elemList.getCount();						

		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* _pFollower = (Person*) elemList.getItem( i );
			if(_pFollower == NULL)
				continue;  

			DeparturePaxLandsideBehavior* pBehavior = new DeparturePaxLandsideBehavior(_pFollower);						
			_pFollower->setBehavior(pBehavior);
			_pFollower->setState(EntryLandside);					

		}

	}
	InitializeVisitorBehavior();
}

bool DeparturePaxLandsideBehavior::IsGetOff( LandsideResourceInSim* pres )
{
	return true;
}

bool DeparturePaxLandsideBehavior::OnVehicleParkAtPlace(LandsideVehicleInSim* pVehicle , LandsideResourceInSim* pRes,const ElapsedTime& t)
{
	if(getState() != StayOnVehicle)
	{
		return false;
	}
	ASSERT(m_pVehicle && m_pVehicle->IsPaxOnVehicle(GetPersonID()));
	setState(GetOffVehicle);
	setResource(pRes);
	GenerateEvent(t);
	return true;
}

void DeparturePaxLandsideBehavior::processArriveAtFacility( const ElapsedTime& eEventTime )
{
	LandsideResourceInSim * pDestResource = getDestResource();
	if(pDestResource->toBusSation())
	{
		LandsideBusStationInSim* pBusStation = pDestResource->toBusSation();
		pBusStation->PassengerMoveInto(this, eEventTime);
	}
	else
	{
		ASSERT(0);
	}

}

bool DeparturePaxLandsideBehavior::CanPaxTakeThisBus( LandsideResidentVehicleInSim *pResidentVehicle, LandsideSimulation *pSimulation )
{
	//get the bus stations that the passenger could lead to

	for (int nDestStation = 0; nDestStation < (int)m_vDestBusStation.size(); ++ nDestStation)
	{
		LandsideBusStationInSim *pBusStation = m_vDestBusStation[nDestStation];
		if(pResidentVehicle->CouldTakeToStation(pBusStation->getInput()->getName()))
		{
			return true;
		}
	}
	return false;
}

bool DeparturePaxLandsideBehavior::CanPaxTkeOffThisBusAtStation( LandsideBusStationInSim* pBusStation, LandsideSimulation *pSimulation )
{
	std::vector<LandsideBusStationInSim *>::iterator iterFind = std::find(m_vDestBusStation.begin(), m_vDestBusStation.end(), pBusStation);

	if(iterFind != m_vDestBusStation.end())
	{
		//here need to specify the terminal processor which linked to this bus station
		
		CFacilityBehaviorLinkageList* pLinkageList = pBusStation->GetBehavior()->getLinkageList();
		std::vector<CFacilityBehaviorLinkage *> vLinkage;
		pLinkageList->GetLinkageList(CFacilityBehaviorLinkage::enumLinkType_Terminal,vLinkage );
		for (int nLink = 0; nLink < (int)vLinkage.size(); ++ nLink)
		{
			CFacilityBehaviorLinkage * pLinkage = vLinkage[nLink];
			ALTObjectID altTermProc;
			pLinkage->getName(altTermProc);
			m_vLinkTerminalProc.push_back(altTermProc);
		}
		return true;
	}

	return false;
}

void DeparturePaxLandsideBehavior::processMoveToVehicle( const ElapsedTime& eEventTime )
{
	WriteLogEntry(eEventTime);
	//get on to vehicle
	ARCVector3 vehiclepos = m_pVehicle->getState(eEventTime).pos;
	setDestination(vehiclepos);
	ElapsedTime eMovetime = moveTime();

	ElapsedTime eCurTime = eMovetime + eEventTime;
	//setLocation(vehiclepos);	
	WriteLogEntry(eCurTime);
	m_pPerson->setState(PutCarryonToVehicle);
	GenerateEvent(eCurTime);
}
void DeparturePaxLandsideBehavior::ProcessPutCarryonToVehicle( const ElapsedTime& eEventTime )
{
	WriteLogEntry(eEventTime);

	////go to the parking 
	//ARCVector3 bagPos = m_pVehicle->getLastState().pos;
	//setDestination(bagPos);
	//ElapsedTime dNextTime = eEventTime;
	//dNextTime += moveTime();
	////setLocation(bagPos);		
	//WriteLogEntry(dNextTime);

	////write bag log
	//Passenger* pPassenger = (Passenger*)m_pPerson;
	//int nCount = pPassenger->m_pVisitorList.size();
	//for( int i=nCount-1; i>=0; i-- )
	//{
	//	PaxVisitor* pVis = pPassenger->m_pVisitorList[i];
	//	if (pVis == NULL)
	//		continue;
	//	VisitorLandsideBehavior* pCarryonBehavior = (VisitorLandsideBehavior*)pVis->getLandsideBehavior();
	//	ASSERT(pCarryonBehavior);
	//	//non passenger move to ground
	//	if(pCarryonBehavior)
	//	{
	//		pCarryonBehavior->setDestination(bagPos);

	//		ElapsedTime eMoveTime = moveTime();
	//		ElapsedTime eCurTime = dNextTime + eMoveTime;

	//		pCarryonBehavior->setLocation( bagPos);
	//		pCarryonBehavior->WriteLogEntry(eCurTime, i);
	//	}

	//	//detach to passenger
	//	pPassenger->m_pVisitorList[i] = NULL;		
	//}

	setState(GetOnVehicle);
	GenerateEvent(eEventTime);
}

void DeparturePaxLandsideBehavior::ProcessGetOnVehicle( const ElapsedTime& eEventTime )
{
	WriteLogEntry(eEventTime);

	setState(StayOnVehicle);
	WriteLogEntry(eEventTime);

	//Notify vehicle that one passenger has moved into
	//
	LandsideVehicleInSim* pVehicle = getVehicle();
	if(pVehicle)
		pVehicle->NotifyPaxMoveInto(m_pPerson->getEngine(),this, eEventTime);
}

void DeparturePaxLandsideBehavior::ProcessLeaveQueue( const ElapsedTime& eTime )
{
	LandsideResourceInSim *pLandsideResource = getResource();
	if(pLandsideResource == NULL)
		return;

	LandsideBusStationInSim *pBusStation = pLandsideResource->toBusSation();
	if(pBusStation && pBusStation->PaxTakeOnBus(this, m_pLandsideSim, eTime))
	{
		//the passenger could take the bus
		//
		pBusStation->PaxLeaveQueue(eTime);
		setState(MoveToVehicle);
		GenerateEvent(eTime);

	}
}

void DeparturePaxLandsideBehavior::ChooseTakeOffResource( LandsideLayoutObjectInSim*pLayout ,LandsideSimulation* pSimulation )
{
	m_vLinkTerminalProc.clear();
	CFacilityBehaviorLinkageList* pLinkageList = pLayout->GetBehavior()->getLinkageList();
	std::vector<CFacilityBehaviorLinkage *> vLinkage;
	pLinkageList->GetLinkageList(CFacilityBehaviorLinkage::enumLinkType_Terminal,vLinkage );
	for (int nLink = 0; nLink < (int)vLinkage.size(); ++ nLink)
	{
		CFacilityBehaviorLinkage * pLinkage = vLinkage[nLink];
		ALTObjectID altTermProc;
		pLinkage->getName(altTermProc);
		m_vLinkTerminalProc.push_back(altTermProc);
	}
}

bool DeparturePaxLandsideBehavior::CanPaxTakeOffThisResource( LandsideResourceInSim*pResource ,LandsideSimulation* pSimulation ) const
{

	TerminalMobElementBehavior* pTermPhase = new PaxTerminalBehavior((Passenger*)m_pPerson);
	std::vector<Processor *> vAvailableProc;
	
	//int nTermFloor= 0;
	//if(LandsideCurbSideInSim* pCurb = pResource->toCurbSide())
	//{
	//	nTermFloor = pCurb->getCurbInput()->m_nTernimalFloor;
	//}
	pTermPhase->GetAvailbleEntryProcList(vAvailableProc);
	int nProcCount=(int)vAvailableProc.size();
	for(int nProc = 0; nProc < nProcCount; ++ nProc )
	{
		Processor *pProc = vAvailableProc[nProc];
		ASSERT(pProc != NULL);

		//get bus station could lead to terminal processor
		LandsideResourceInSim* pLandsideRes = m_pLandsideSim->getFacilityBehaviors()->GetLandsideTerminalLinkage( *(pProc->getID()));
		if(pLandsideRes == pResource)
			return true;	
	}
	return false;
}




















