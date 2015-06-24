#include "StdAfx.h"
#include ".\arrivalpaxlandsidebehavior.h"
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
#include "..\Landside\LandsideCurbSide.h"
#include "PaxTerminalBehavior.h"
#include "VisitorLandsideBehavior.h"
#include "LandsideCurbsideInSim.h"
#include "LandsideBusStationInSim.h"
#include "Landside/LandsideBusStation.h"
#include "Landside/VehicleItemDetail.h"
#include "Landside/CLandSidePublicVehicleType.h"
#include "LandsideSimErrorShown.h"
#include "LandsideResidentVehicleInSim.h"
#include "LandsideCellPhoneProcess.h"
#include "LandsideTaxiQueueInSim.h"
#include "FacilityBehaviorsInSim.h"


//the procession flow of arrival passenger
//EntryLandside
//  MoveToFacility
//	ArriveAtFacility
//	LeaveFacility

//		//Vehicle Operation
//		WaitForVehicle
//		PutCarryonToVehicle
//		GetOnVehicle
//		StayOnVehicle
//		GetOffVehicle
//		PickCarryOnFromVehicle
//  

//Vehicle could be Public Vehicle and Private Vehicle

//the objects that act with
//		Facility
//		Passenger
//		Vehicle



ArrivalPaxLandsideBehavior::ArrivalPaxLandsideBehavior( Person* p ) 
:PaxLandsideBehavior(p)
{
	
}
ArrivalPaxLandsideBehavior::~ArrivalPaxLandsideBehavior(void)
{
}

int ArrivalPaxLandsideBehavior::performanceMove( ElapsedTime p_time,bool bNoLog )
{
	switch( getState() )
	{
	case EntryLandside: //leave terminal 
		processEntryLandisde(p_time);
		break;
	//case WaitFor_Vehicle: //wait for vehicle park at the place
	//	processWaitForVehicle_old(p_time);
	//	break;
	////case GoTo_VehicleParkingPlace: //go the where the vehicle parked(curbside, bus station, taxi station, parking lot)
	////	processGoToVehicleParking(p_time);
	////	break;
	//case Put_Bag: //put bag to vehicle
	//	processPutBag(p_time);
	//	break;
	//case GetOn_Vehicle: //get on to the vehicle
	//	processGetOnVehicle_old(p_time);
	//	break;
	//case On_Vehicle: //stay on vehicle to the end.
	//	processOnVehicle(p_time);
	//	break;
	//case GetOff_Vehice:
	//	{
	//		ProcessGetOffVehicle(p_time);
	//	}
	//	break;
	case LandsideLeaveQueue:
		{
			ProcessLeaveQueue(p_time);
		}
		break; 

	case MoveToFacility:
		{
			ProcessMoveToFacility(p_time);
		}
		break;
	case ArriveAtFacility:
		{
			ProcessArriveAtFacility(p_time);
		}
		break;
	case LeaveFacility:
		{

		}
		break;
		//Vehicle Operation
	case WaitingForVehicle:
		{

		}
		break;
	case MoveToVehicle:
		{
			ProcessMoveToVehicle(p_time);
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
	case StayOnVehicle:
		{
			ProcessStayOnVehicle(p_time);
		}
		break;
	case GetOffVehicle:
		{
			ProcessGetOffVehicle(p_time);
		}
		break;
	case PickCarryOnFromVehicle:
		{
			ProcessPickCarryOnFromVehicle(p_time);

		}
		break;
	case Death:
		{
			terminate(p_time);
		}
		break;
	default: 
		ASSERT(FALSE);
	}

	return TRUE;
}
void ArrivalPaxLandsideBehavior::processEntryLandisde( const ElapsedTime& eEventTime )
{
	if(!m_pPerson)
		return;

	InitializeBehavior();
	SetEnterLandsideLocation(location);

	if(simEngineConfig()->isSimTerminalMode())
	{	
		//WriteLogEntry(t);
		Processor* pProc=  m_pPerson->getTerminalBehavior()->getLastTerminalProc();
		ASSERT(pProc);
		if(pProc)//find 
		{
			CString strProcName = pProc->getIDName();
			CFacilityBehaviorsInSim *pFacilityBehavior = m_pPerson->getEngine()->GetLandsideSimulation()->getFacilityBehaviors();
			LandsideResourceInSim* pLandsideLinkageObject = pFacilityBehavior->GetLandsideTerminalLinkage(*pProc->getID());
			if(pLandsideLinkageObject)
			{
				//it could be curbside, bus station ...
				pLandsideLinkageObject->PassengerMoveInto(this, eEventTime);
				return;
			}
			else //the last processor is not link to any 
			{
				//check parking spot
				LandsideResourceInSim *pDestResource = NULL;
				LandsideVehicleInSim* pVehicle = m_pPerson->getEngine()->GetLandsideSimulation()->FindPaxVehicle(m_pPerson->getID());
				if(pVehicle)
				{
					setVehicle(pVehicle);
				}
				else  //vehicle not entry simulation?
				{
					LandsideSimErrorShown::PaxSimWarning(m_pPerson,_T("Pick Vehicle is not entry simulation yet"),_T("Pax Runtime Error"),eEventTime);
					terminate(eEventTime);
					return;
				}

				if(LandsideResourceInSim* pDestResource = pVehicle->getLastState().getLandsideRes() )
				{
					setResource(pDestResource);
					//destination is parking lot
					setDestResource(pDestResource);
					setState(MoveToFacility);
					GenerateEvent(eEventTime + ElapsedTime(2L)); //@CodeWarn ?2sec what for
					return;
				}
				else 
				{
					LandsideSimErrorShown::PaxSimWarning(m_pPerson,_T("Pax vehicle is not in any resource"),_T("Pax Runtime Error"),eEventTime);
					terminate(eEventTime);
					return;
				}
			}			
		}
		else  //no proc?
		{
			LandsideSimErrorShown::PaxSimWarning(m_pPerson,_T("No Last Processor"),_T("Pax Runtime Error"),eEventTime);
			terminate(eEventTime);
			return;
		}
	}
	else
	{
		if(!m_pVehicle)
		{
			LandsideVehicleInSim* pVehicle = m_pPerson->getEngine()->GetLandsideSimulation()->FindPaxVehicle(m_pPerson->getID());
			if(pVehicle)
			{
				setVehicle(pVehicle);
			}
			else
			{
				ASSERT(FALSE);
				terminate(eEventTime);
				return;
			}
		}

		UpdateEntryTime(eEventTime);
		//get on to the vehicle straight
		CPoint2008 pos= m_pVehicle->getOffVehiclePos( CPoint2008(getPoint()) );
		LandsideResourceInSim* pRes= m_pVehicle->getState(eEventTime).getLandsideRes();
		setResource(pRes);
		setLocation(pos);
		setDestination(pos);
		WriteLogEntry(eEventTime);

		m_pPerson->setState(MoveToVehicle);
		GenerateEvent(eEventTime);
	}
	

	
	
}
void ArrivalPaxLandsideBehavior::processPutBag( const ElapsedTime& t )
{
	WriteLogEntry(t);

	//go to the pak
	ARCVector3 bagPos = m_pVehicle->getLastState().pos;
	setDestination(bagPos);
	ElapsedTime dNextTime = t;
	dNextTime += moveTime();
	//setLocation(bagPos);		
	WriteLogEntry(dNextTime);

	//write bag log
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		VisitorLandsideBehavior* pCarryonBehavior = (VisitorLandsideBehavior*)pVis->getLandsideBehavior();
		ASSERT(pCarryonBehavior);
		//non passenger move to ground
		if(pCarryonBehavior)
		{
			pCarryonBehavior->setDestination(bagPos);			
			ElapsedTime eTime = moveTime();
			pCarryonBehavior->setLocation(bagPos);
			pCarryonBehavior->WriteLogEntry(eTime+t, i);
		}

		//detach to passenger
		pPassenger->m_pVisitorList[i] = NULL;		
	}

	setState(GetOn_Vehicle);
	GenerateEvent(dNextTime);

}

void ArrivalPaxLandsideBehavior::processGetOnVehicle_old( const ElapsedTime& t )
{
	WriteLogEntry(t);
	//get on to vehicle
	ARCVector3 vehiclepos = m_pVehicle->getState(t).pos;
	setDestination(vehiclepos);
	ElapsedTime etime = moveTime();
	//setLocation(vehiclepos);	
	WriteLogEntry(etime + t);
	m_pPerson->setState(On_Vehicle);
	GenerateEvent(etime+t);
}

void ArrivalPaxLandsideBehavior::processOnVehicle( const ElapsedTime& t )
{
	//do nothing , wait for vehicle flush its log
	WriteLogEntry(t);
	if(m_pVehicle)
	{
		m_pVehicle->Activate(t);
		m_pVehicle->AddOnPax(this);
	}
	return;
}

void ArrivalPaxLandsideBehavior::InitializeBehavior()
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

			ArrivalPaxLandsideBehavior* pBehavior = new ArrivalPaxLandsideBehavior(_pFollower);						
			_pFollower->setBehavior(pBehavior);
			_pFollower->setState(EntryLandside);					

		}
	}

	InitializeVisitorBehavior();
}


bool ArrivalPaxLandsideBehavior::OnVehicleParkAtPlace( LandsideVehicleInSim* pVehicle, LandsideResourceInSim* pRes,const ElapsedTime& eTime )
{
	m_pVehicle =  pVehicle;
	if( getState() == WaitingForVehicle && pRes == getResource() )
	{
		setState(MoveToVehicle);
		GenerateEvent(eTime);
		return true;
	}
	return false;
}

void ArrivalPaxLandsideBehavior::ProcessLeaveQueue( const ElapsedTime& eTime )
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
	
	LandsideTaxiQueueInSim *pTaxiQ = pLandsideResource->toTaxiQueue();
	if(pTaxiQ && pTaxiQ->PaxTakeOnBus(this, m_pLandsideSim, eTime))
	{
		//the passenger could take the bus
		//
		pTaxiQ->PaxLeaveQueue(eTime);
		setState(MoveToVehicle);
		GenerateEvent(eTime);

	}


}

void ArrivalPaxLandsideBehavior::ProcessMoveToVehicle( const ElapsedTime& eEventTime )
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

void ArrivalPaxLandsideBehavior::ProcessPutCarryonToVehicle( const ElapsedTime& eEventTime )
{
	ElapsedTime serviceT = ElapsedTime(0L);
	LandsideResourceInSim* pRes = getResource();
	LandsideVehicleInSim* pV = getVehicle();
	if(pRes && pV)
	{
		serviceT = pRes->getLayoutObject()->GetServiceTime(pV->getName());
	}
	ElapsedTime nextT = eEventTime+serviceT;
	WriteLogEntry(nextT);

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
	GenerateEvent(nextT);
}

void ArrivalPaxLandsideBehavior::ProcessGetOnVehicle( const ElapsedTime& eEventTime )
{
	WriteLogEntry(eEventTime);

	setState(StayOnVehicle);
	WriteLogEntry(eEventTime);

	//Notify vehicle that one passenger has moved into
	//
	LandsideVehicleInSim* pVehicle = getVehicle();
	if(pVehicle)
	{	
		if(LandsideResourceInSim* pRes = getResource())
		{
			pRes->PassengerMoveOut(this,eEventTime);
		}
		pVehicle->AddOnPax(this);
		pVehicle->NotifyPaxMoveInto(m_pPerson->getEngine(),this, eEventTime);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void ArrivalPaxLandsideBehavior::ProcessStayOnVehicle( const ElapsedTime& eEventTime )
{
	//do nothing
	ASSERT(0);
}
void ArrivalPaxLandsideBehavior::ProcessGetOffVehicle( const ElapsedTime& eEventTime )
{
	//get vehicle

	//get
	//the position where the vehicle stops
	//
	WriteLogEntry(eEventTime);

	setState(GetOffVehicle);
	setDestination(m_pVehicle->getLastState().pos);
	//setLocation(m_pVechile->getState().pos);
	WriteLogEntry(eEventTime);

	CPoint2008 nextPt(0,0,0);

	LandsideBusStationInSim*  pBusStation = NULL;
	if( LandsideResourceInSim* pRes = getResource())
	{
		pBusStation =  pRes->toBusSation();
		if ( pBusStation)
		{
			nextPt = pBusStation->GetPaxWaitPos();
		}
	}
	//move outside vehicle
	CPoint2008 nextpos = m_pVehicle->getOffVehiclePos(nextPt);
	setDestination(nextpos);
	ElapsedTime eMovetime = moveTime();
	ElapsedTime eCurTime = eMovetime + eEventTime;

	//setLocation(nextpos);
	WriteLogEntry(eCurTime);

	m_pVehicle->RemoveOnPax(this, eCurTime);

	//start to pick the Bag
	
	m_pPerson->setState(PickCarryOnFromVehicle);
	GenerateEvent(eCurTime);


}

void ArrivalPaxLandsideBehavior::ProcessPickCarryOnFromVehicle( const ElapsedTime& eEventTime )
{
	//pick up carry ons

	// move to destination 
	LandsideResourceInSim *pDestResource = NULL;
	LandsideVehicleInSim* pVehicle = m_pPerson->getEngine()->GetLandsideSimulation()->FindPaxVehicle(m_pPerson->getID());
	if(pVehicle)
	{
		setVehicle(pVehicle);
		pDestResource = pVehicle->getLastState().getLandsideRes();
	}

	if(pDestResource == NULL)
	{
		ASSERT(FALSE);
		terminate(eEventTime);
		return;
	}
	
	
	//destination is parking lot
	setDestResource(pDestResource);
	setState(MoveToFacility);
	GenerateEvent(eEventTime + ElapsedTime(2L));


}

void ArrivalPaxLandsideBehavior::ProcessMoveToFacility( const ElapsedTime& eEventTime )
{
	LandsideResourceInSim *pDestResource = getDestResource();
	if(pDestResource->toParkLotSpot())
	{
		LandsideParkingSpotInSim* pParkSpot = pDestResource->toParkLotSpot();

		ARCVector3 nextPos = pParkSpot->getPos();
		CPoint2008 endPos;
		endPos.init(nextPos.n[VX],nextPos.n[VY],nextPos.n[VZ]);

		CLandsideTrafficSystem* pLandsideTrafficInSim = m_pLandsideSim->GetLandsideTrafficeManager();
		LandsideResourceInSim* pStartLandsideResInSim = getResource();
		LandsideResourceInSim* pEndLandsideResInSim = pParkSpot->getParkingLot();

		CPoint2008 startPos;
		startPos.init(getPoint().n[VX],getPoint().n[VY],getPoint().n[VZ]);

		if( !pLandsideTrafficInSim->EnterTrafficSystem(eEventTime,ArriveAtFacility,this,startPos,endPos))
		{
			ElapsedTime dNextTime = eEventTime;
			setDestination(endPos);
			ElapsedTime etime = moveTime();
			//setLocation(nextPos);	
			dNextTime += etime;
			WriteLogEntry(dNextTime);
			m_pPerson->setState(ArriveAtFacility);
			GenerateEvent(dNextTime);
			return;
		}
	}
	else
	{
		ASSERT(FALSE);
		terminate(eEventTime);
	}

}


void ArrivalPaxLandsideBehavior::ProcessArriveAtFacility( const ElapsedTime& eEventTime )
{
	//same resource of destination
	LandsideResourceInSim *pDestResource = getDestResource();
	if(pDestResource->toParkLotSpot())
	{
		LandsideParkingSpotInSim* pParkSpot = pDestResource->toParkLotSpot();
		pDestResource->PassengerMoveInto(this, eEventTime);
	}
	else
	{
		ASSERT(FALSE);
		terminate(eEventTime);
	}
}

void ArrivalPaxLandsideBehavior::ProcessWaitForVehicle( const ElapsedTime& eEventTime )
{
	//do nothing

}

bool ArrivalPaxLandsideBehavior::CanPaxTakeThisBus( LandsideResidentVehicleInSim *pResidentVehicle, LandsideSimulation *pSimulation )
{
	LandsideIndividualArrivalPaxVehicleConvergerManager *pConverageManager = pSimulation->GetLandsideConvergerManager();
	if(pConverageManager)
	{
		LandsideObjectLinkageItemInSim objNameLinkaged;
		pConverageManager->GetLinkageObject(GetPersonID(), objNameLinkaged);
		//get passenger stop station
		if(pResidentVehicle->CouldTakeToStation(objNameLinkaged.getObject()))
		{
			return true;
		}
	}
	return false;
}

bool ArrivalPaxLandsideBehavior::CanPaxTkeOffThisBusAtStation( LandsideBusStationInSim* pBusStation, LandsideSimulation *pSimulation )
{
	LandsideIndividualArrivalPaxVehicleConvergerManager *pConverageManager = pSimulation->GetLandsideConvergerManager();
	if(pConverageManager)
	{
		LandsideObjectLinkageItemInSim objNameLinkaged;
		pConverageManager->GetLinkageObject(GetPersonID(), objNameLinkaged);
		if(pBusStation->getInput()->getName() == objNameLinkaged.getObject())
		{
			return true;
		}

		LandsideParkingLotInSim *pLinkedPakringLot = pBusStation->getEmbeddedParkingLotInSim();
		if(pLinkedPakringLot)
		{
			if(pLinkedPakringLot->getInput()->getName() == objNameLinkaged.getObject())
			{		
				return true;
			}
		}
	}
	return false;
}

bool ArrivalPaxLandsideBehavior::CanBoardingOrBoarding( LandsideResourceInSim* pRes )
{
	if(getState()==WaitingForVehicle && pRes == getResource() )
	{
		return true;
	}
	if(getState()>=MoveToVehicle && getState()< StayOnVehicle)
	{
		return true;
	}
	return false;
}





























