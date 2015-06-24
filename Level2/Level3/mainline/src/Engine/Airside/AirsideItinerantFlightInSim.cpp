#include "StdAfx.h"
#include ".\airsideitinerantflightinsim.h"
#include "InputAirside/ItinerantFlight.h"
#include "common/ACTypesManager.h"
#include "common/AirlineManager.h"
#include "VehicleRequestDispatcher.h"
#include "VehiclePoolsManagerInSim.h"
#include "FlightServiceManagerInSim.h"
#include "FlightServiceInformation.h"
#include "../../InputAirside/ALTObjectGroup.h"
#include "../../InputAirside/FlightTypeDetailsItem.h"
#include "../../InputAirside/FlightTypeDetailsManager.h"
#include "StandAssignmentRegisterInSim.h"
#include "AirsideSimulation.h"
#include "../../Results/OutputAirside.h"
#include "FlightBirthEvent.h"
#include "../../common/AirportDatabase.h"
#include "../../inputs/IN_TERM.H"
#include "../../InputAirside/ItinerantFlightSchedule.h"
#include "AirspaceResourceManager.h"




AirsideItinerantFlightInSim::AirsideItinerantFlightInSim(CARCportEngine *pARCPortEngine,Flight *pFlight,int nid)
:AirsideFlightInSim(pARCPortEngine,pFlight,nid)
,m_nEntryWaypointID(-1)
,m_nExitWaypointID(-1)
,m_pEnroute(NULL)
,m_bEnroute(FALSE)
{
}

AirsideItinerantFlightInSim::~AirsideItinerantFlightInSim(void)
{
	delete m_pflightInput;
}



//AirsideItinerantFlightInSim* AirsideItinerantFlightInSim::InitializeAirsideItinerantFlight(Flight *pFlt)
//{
//	//Flight * pFlt =  m_pInterm->flightSchedule->getItem(i);
//
//	////log entry
//	char strbuf[256];
//	AirsideFlightDescStruct desc;
//	memset((char*)&desc, 0, sizeof(AirsideFlightDescStruct));
//
//	//set info
//	desc._depID = pFlt->getLogEntry().depID;
//	desc._acType = pFlt->getLogEntry().acType;
//	desc._airline= pFlt->getLogEntry().airline;
//	desc._arrID = pFlt->getLogEntry().arrID;
//	desc._depStopover = pFlt->getLogEntry().depStopover;
//	desc._arrStopover = pFlt->getLogEntry().arrStopover;
//	desc._origin = pFlt->getLogEntry().origin;
//	desc._destination = pFlt->getLogEntry().destination;
//
//	//	
//	pFlt->getArrID(strbuf); 	strcpy(desc.arrID,strbuf );
//	pFlt->getDepID(strbuf);	strcpy(desc.depID, strbuf);
//	pFlt->getFullID(strbuf); strcpy(desc.flightID, strbuf);
//	pFlt->getACType(strbuf);	strcpy(desc.acType, strbuf);
//	//pCFlight->isArriving()?pCFlight->getArrGate().printID(strbuf):pCFlight->getDepGate().printID(strbuf);
//	pFlt->getOrigin(strbuf); strcpy(desc.origin,strbuf);
//	pFlt->getDestination(strbuf); strcpy(desc.dest,strbuf);
//	pFlt->getAirline(strbuf);  desc.airlineNO =(short) CAirlinesManager::GetCode(strbuf);
//	desc.userCode = (long)pFlt;		
//
//	return NULL;
//}

int AirsideItinerantFlightInSim::fits(const FlightConstraint& _fltcons) const
{
	return _fltcons.fits(m_fltCons);
}
void AirsideItinerantFlightInSim::SetFltConstraint(const FlightConstraint& fltType)
{
//	m_fltCons.SetAirportDB(pDatabase);
//	m_fltCons.setConstraint(_fltcons,VERSION_CONSTRAINT_CURRENT);
	m_fltCons = fltType;
}
void AirsideItinerantFlightInSim::getACType (char *p_str) const 
{
	m_fltCons.getACType(p_str);
}

FlightRouteInSim* AirsideItinerantFlightInSim::GetEnroute(AirRouteNetworkInSim* pRouteNetwork)
{
	if (!m_bEnroute)
		return NULL;

	if (m_pEnroute == NULL)
	{
		m_pEnroute = pRouteNetwork->GetFlightEnroute(this, m_nEntryWaypointID,m_nExitWaypointID);
	}

	return m_pEnroute;
}
//////////////////////////////////////////////////////////////////////////
//class AirsideItinerantFlightInSimGenerator
AirsideItinerantFlightInSimGenerator::AirsideItinerantFlightInSimGenerator(int nProjID,InputTerminal *pInputTerminal,int nid,AirsideSimulation *pAirsideSimulation)
:m_nProjID(nProjID)
,m_pInputTerminal(pInputTerminal)
,m_nFlightID(nid)
,m_pAirsideSimulation(pAirsideSimulation)
{

}
AirsideItinerantFlightInSimGenerator::~AirsideItinerantFlightInSimGenerator()
{

}

void AirsideItinerantFlightInSimGenerator::GenerateFlight(CARCportEngine *pARCPortEngine)
{

	//ItinerantFlightList lstItinerantFlight;
	//lstItinerantFlight.SetAirportDB(m_pInputTerminal->m_pAirportDB);
	//lstItinerantFlight.ReadData(m_nProjID);


	//size_t nCount = lstItinerantFlight.GetElementCount();
	//
	//int nFlightID = m_nFlightID;

	//for (size_t i =0; i < nCount; ++i)
	//{
	//	 
	//	ItinerantFlight *pItiFlight = lstItinerantFlight.GetItem(i);

	//	Flight *pTerminalFlight = InitializeTerminalFlight(pItiFlight,nFlightID);

	//	//AirsideItinerantFlightInSim* pAirsideItiFlight = new AirsideItinerantFlightInSim(pTerminalFlight,m_nFlightID +i +1);
	//	//pAirsideItiFlight->InitializeAirsideItinerantFlight(pTerminalFlight);
	//


	//}


	ItinerantFlightSchedule itinerantFltSchedule;
	itinerantFltSchedule.SetAirportDB(m_pInputTerminal->m_pAirportDB);
	itinerantFltSchedule.ReadData(m_nProjID);

	size_t nCount = itinerantFltSchedule.GetElementCount();

	for (size_t i =0; i < nCount ;++ i)
	{
		ItinerantFlightScheduleItem *pItiFlt = itinerantFltSchedule.GetItem(i);
		//initialize the itinerant Flight 
		InitializeFlightFromSchedule(pARCPortEngine, pItiFlt,m_nFlightID + i);

	}

}
void AirsideItinerantFlightInSimGenerator::InitializeFlightFromSchedule(CARCportEngine *pARCPortEngine,ItinerantFlightScheduleItem *pItiFltSch,int nFltID)
{

	Flight *pFlt = new Flight((Terminal *)m_pInputTerminal);
	pFlt->setServiceTime(ElapsedTime(30*60L));
	AirsideItinerantFlightInSim *pFlightInSim = new AirsideItinerantFlightInSim(pARCPortEngine,pFlt,nFltID);


	if( pItiFltSch->isArriving() )
	{
		pFlightInSim->SetFltConstraint(pItiFltSch->getType('A'));
	}
	else if( pItiFltSch->isDeparting() )
	{
		pFlightInSim->SetFltConstraint(pItiFltSch->getType('D'));
	}

	if (pItiFltSch->GetEnRoute())
	{
		pFlightInSim->SetThroughOut();
	}

//	pFlightInSim->SetFltConstraint(pItiFltSch->getType());
	////log entry
	char strbuf[256];
	AirsideFlightDescStruct desc;
	memset((char*)&desc, 0, sizeof(AirsideFlightDescStruct));
	
	//set Flight ID
	if (pItiFltSch->GetEntryFlag() == ALT_WAYPOINT)
	{
		CString strArrID;
		strArrID.Format(_T("Arr%d"),nFltID);
		desc._arrID = strArrID;
		pFlt->setArrID(strArrID);
		desc._depID = pFlt->getLogEntry().depID;
		pFlightInSim->SetEntryWaypointID(pItiFltSch->GetEntryID());
	}
	if (pItiFltSch->GetExitFlag() == ALT_WAYPOINT)
	{
		CString strDepID;
		strDepID.Format(_T("Dep%d"),nFltID);
		desc._depID = strDepID;
		pFlt->setDepID(strDepID);
		desc._arrID = pFlt->getLogEntry().arrID;
		pFlightInSim->SetExitWaypointID(pItiFltSch->GetExitID());
	}

	//pFlightInSim->InitStatus();
	char szACType[100];
	pItiFltSch->getACType(szACType);
	desc._acType = szACType;
	desc._airline= pFlt->getLogEntry().airline;
	//desc._arrID = pFlt->getLogEntry().arrID;
	desc._depStopover = pFlt->getLogEntry().depStopover;
	desc._arrStopover = pFlt->getLogEntry().arrStopover;
	desc._origin = pFlt->getLogEntry().origin;
	desc._destination = pFlt->getLogEntry().destination;


	pFlt->getArrID(strbuf); 	strcpy(desc.arrID,strbuf );
	pFlt->getDepID(strbuf);	strcpy(desc.depID, strbuf);
	pFlt->getFullID(strbuf); strcpy(desc.flightID, strbuf);
	pFlt->getACType(strbuf);	strcpy(desc.acType, _T("320"));
	//pCFlight->isArriving()?pCFlight->getArrGate().printID(strbuf):pCFlight->getDepGate().printID(strbuf);
	pFlt->getOrigin(strbuf); strcpy(desc.origin,strbuf);
	pFlt->getDestination(strbuf); strcpy(desc.dest,strbuf);
	pFlt->getAirline(strbuf);  desc.airlineNO =(short) CAirlinesManager::GetCode(strbuf);
	desc.userCode = (long)pFlt;	

	desc.arrloadFactor = 0.0;//pFlt->getLogEntry().loadFactor;
	desc.deploadFactor = 0.0;
	desc.nCapacity = 100;//pFlt->getLogEntry().capacity;
	desc.gateOccupancy  = 30*60*100;
	desc.arrTime = -1;
	desc.depTime = -1;

	if (pFlightInSim->IsThroughOut())
	{	
		ElapsedTime  eArrTime = pItiFltSch->getArrTime();
		pFlightInSim->SetArrTime(eArrTime);
		pFlt->setArrTime(eArrTime);
		desc.arrTime = eArrTime;

		pFlightInSim->SetOutput(m_pAirsideSimulation->m_pOuput);		
		pFlightInSim->SetAirTrafficController(m_pAirsideSimulation->GetAirTrafficController());
		pFlightInSim->SetPerformance(m_pAirsideSimulation->GetFlightPerformanceManager());

		m_pAirsideSimulation->GetAirsideFlightList().push_back(pFlightInSim);		
		FlightBirthEvent *fltbirth = new FlightBirthEvent(pFlightInSim);		
		fltbirth->addEvent();
		return;
	}	
	//
	int  nStandID = -1, nDepStandID = -1;
	if(pFlightInSim->IsArrival())
	{
		ElapsedTime  eArrTime = pItiFltSch->getArrTime();
		pFlightInSim->SetArrTime(eArrTime);
		pFlt->setArrTime(eArrTime);
		desc.arrTime = eArrTime;

	//	nStandID = pItiFltSch->GetExitID();
		if (!pItiFltSch->getArrStand().IsBlank())
		{
			std::vector<ALTObject> vObject;
			ALTObject::GetObjectList(ALT_STAND,m_nProjID,vObject);
			for (int i = 0; i < (int)vObject.size();i++)
			{
				if (vObject.at(i).GetObjectName() == pItiFltSch->getArrStand())
				{
					nStandID = vObject.at(i).getID();
					break;
				}
			}
		}
	}
	if(pFlightInSim->IsDeparture())
	{
		ElapsedTime eDepTime = pItiFltSch->getArrTime();

		pFlightInSim->SetDepTime(eDepTime);
		pFlt->setDepTime(eDepTime);	
		desc.depTime = eDepTime;
	
	//	nStandID = pItiFltSch->GetEntryID();
		if (!pItiFltSch->getDepStand().IsBlank())
		{
			std::vector<ALTObject> vObject;
			ALTObject::GetObjectList(ALT_STAND,m_nProjID,vObject);
			for (int i = 0; i < (int)vObject.size();i++)
			{
				if (vObject.at(i).GetObjectName() == pItiFltSch->getDepStand())
				{
					nDepStandID = vObject.at(i).getID();
					break;
				}
			}
		}
	}

	if (nStandID == -1 && nDepStandID == -1)
	{
		delete pFlightInSim;
		return;
	}
	pFlightInSim->SetOutput(m_pAirsideSimulation->m_pOuput);		
	pFlightInSim->SetAirTrafficController(m_pAirsideSimulation->GetAirTrafficController());

	CFlightServiceInformation* pServiceInfo = new CFlightServiceInformation(pFlightInSim);
	FlightTypeDetailsManager *pManager = m_pAirsideSimulation->GetFlightServiceManager()->GetFlightServiceManager();
	FlightTypeDetailsItem *pItem = pManager->GetFlightDetailItem(desc);
	if (pItem)
	{
		pServiceInfo->SetFltServiceLocations(pItem->GetRelativeLocations());
		pServiceInfo->SetRingRoute(pItem->GetRingRoad());
	}
	pFlightInSim->SetFlightServiceInformation(pServiceInfo);

	StandInSim * pArrStand = m_pAirsideSimulation->GetAirsideResourceManager()->GetAirportResource()->getStandResource()->GetStandByID(nStandID);	
	StandInSim * pDepStand = m_pAirsideSimulation->GetAirsideResourceManager()->GetAirportResource()->getStandResource()->GetStandByID(nDepStandID);	

	
	{
		if (pFlightInSim->IsArrival() && pArrStand)
		{
			pArrStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim, ARR_PARKING,
				pFlightInSim->getArrTime(), pFlightInSim->getArrTime()+pFlightInSim->getServiceTime(ARR_PARKING));
		}
		if (pFlightInSim->IsDeparture() && pDepStand)
		{
			ElapsedTime tStart = pFlightInSim->getDepTime() - pFlightInSim->getServiceTime(DEP_PARKING);			
			pDepStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim,DEP_PARKING, 
				tStart > 0L? tStart: 0L, pFlightInSim->getDepTime());
		}
	}

	pFlt->setArrStand(pItiFltSch->getArrStand());
	pFlt->setDepStand(pItiFltSch->getDepStand());

	pFlightInSim->SetServiceRequestDispatcher(m_pAirsideSimulation->GetVehicleRequestDispatcher());
	pFlightInSim->SetPerformance(m_pAirsideSimulation->GetFlightPerformanceManager());

	//add by Peter 2007.12.20
	if (m_pAirsideSimulation&&pFlightInSim->IsArrival())
	{
		//Time is out of range
		if (!(m_pAirsideSimulation->GetSimStartTime() <= pFlightInSim->getArrTime()
			&& pFlightInSim->getArrTime() <= m_pAirsideSimulation->GetSimEndTime()))
		{			
			delete pFlightInSim;
			return;
		}
	}
	else if (m_pAirsideSimulation&&pFlightInSim->IsDeparture())
	{
		//Time is out of range
		if (!(m_pAirsideSimulation->GetSimStartTime() <= pFlightInSim->getDepTime()
			&& pFlightInSim->getDepTime() <=  m_pAirsideSimulation->GetSimEndTime()))
		{
			delete pFlightInSim;
			return;
		}
	}
	else
	{	
		delete pFlightInSim;
		return;
	}
	//End add by Peter 2007.12.20

	m_pAirsideSimulation->GetAirsideFlightList().push_back(pFlightInSim);
	FlightBirthEvent *fltbirth = new FlightBirthEvent(pFlightInSim);		
	fltbirth->addEvent();

}






























//Flight* AirsideItinerantFlightInSimGenerator::InitializeTerminalFlight(ItinerantFlight *pItiFlight,int& nFltID)
//{
//
//	if (pItiFlight->GetEntryFlag() == pItiFlight->GetExitFlag())
//		return NULL;
//
//	//logEntry = aFlight.logEntry;
//	//standGate = aFlight.standGate;
//	//arrGate = aFlight.arrGate;
//	//depGate = aFlight.depGate;
//	//serviceTime = aFlight.serviceTime;
//	//curPax = aFlight.curPax;
//	//delay = aFlight.delay;
//	//m_mapLastCallOfEveryStage = aFlight.m_mapLastCallOfEveryStage;
//	//lastCall = aFlight.lastCall;
//	//baggageDevice = aFlight.baggageDevice;
//	//m_pTerm = aFlight.m_pTerm;
//	CString strName = pItiFlight->GetName();
//	ElapsedTime eStartTime = *(pItiFlight->GetStartTime());
//	ElapsedTime eEndTime = *(pItiFlight->GetEndTime());
//
//	ProbabilityDistribution *pProbDist = pItiFlight->GetProbDistribution();
//	double dInteValue = pProbDist->getRandomValue() *60;
//
//	int nCount = pItiFlight->GetFltCount();
//	
//	int nEntryID = pItiFlight->GetEntryID();
//
//	std::vector<ALTObject> vEntryObject;
//	if(pItiFlight->GetEntryFlag() == ALT_WAYPOINT)//way point
//	{
//
//	}
//	else//stand
//	{
//		ALTObjectGroup objectGroup;
//		objectGroup.ReadData(nEntryID);
//		objectGroup.GetObjects(vEntryObject);
//		if (vEntryObject.size() == 0)
//			return NULL;
//	}
//
//	//exit id
//
//	int nExitID = pItiFlight->GetExitID();
//	std::vector<ALTObject> vExitObject;
//
//	if (pItiFlight->GetExitFlag() == ALT_WAYPOINT)
//	{
//
//	}
//	else
//	{
//		ALTObjectGroup objectGroup;
//		objectGroup.ReadData(nExitID);
//		objectGroup.GetObjects(vExitObject);
//		if (vExitObject.size() == 0)
//			return NULL;
//	}
//
//
//	//generate flight
//	for (int i =0 ; i< nCount ;++i)
//	{
//		nFltID += 1;
//		Flight *pFlt = new Flight((Terminal *)m_pInputTerminal);
//		pFlt->setServiceTime(ElapsedTime(30*60L));
//		AirsideItinerantFlightInSim *pFlightInSim = new AirsideItinerantFlightInSim(pFlt,nFltID);
//
////		pFlightInSim->SetFltConstraint(m_pInputTerminal->m_pAirportDB,pItiFlight->GetFltType());
//		pFlightInSim->SetFltConstraint(pItiFlight->GetFltType());
//		////log entry
//		char strbuf[256];
//		AirsideFlightDescStruct desc;
//		memset((char*)&desc, 0, sizeof(AirsideFlightDescStruct));
//
//		//set info
//		if (pItiFlight->GetEntryFlag() == ALT_WAYPOINT)
//		{
//			CString strArrID;
//			strArrID.Format(_T("Arr%d"),nFltID);
//			desc._arrID = strArrID;
//			pFlt->setArrID(strArrID);
//			desc._depID = pFlt->getLogEntry().depID;
//			pFlightInSim->SetEntryWaypointID(nEntryID);
//		}
//		else
//		{
//			CString strDepID;
//			strDepID.Format(_T("Dep%d"),nFltID);
//			desc._depID = strDepID;
//			pFlt->setDepID(strDepID);
//			desc._arrID = pFlt->getLogEntry().arrID;
//			pFlightInSim->SetExitWaypointID(nExitID);
//		}
//
//		//desc._depID = pFlt->getLogEntry().depID;
//		//desc._acType = pFlt->getLogEntry().acType;
//		char szACType[100];
//		 pItiFlight->GetFltType().getACType(szACType);
//		 desc._acType = szACType;
//		desc._airline= pFlt->getLogEntry().airline;
//		//desc._arrID = pFlt->getLogEntry().arrID;
//		desc._depStopover = pFlt->getLogEntry().depStopover;
//		desc._arrStopover = pFlt->getLogEntry().arrStopover;
//		desc._origin = pFlt->getLogEntry().origin;
//		desc._destination = pFlt->getLogEntry().destination;
//
//		//	
//		pFlt->getArrID(strbuf); 	strcpy(desc.arrID,strbuf );
//		pFlt->getDepID(strbuf);	strcpy(desc.depID, strbuf);
//		pFlt->getFullID(strbuf); strcpy(desc.flightID, strbuf);
//		pFlt->getACType(strbuf);	strcpy(desc.acType, _T("320"));
//		//pCFlight->isArriving()?pCFlight->getArrGate().printID(strbuf):pCFlight->getDepGate().printID(strbuf);
//		pFlt->getOrigin(strbuf); strcpy(desc.origin,strbuf);
//		pFlt->getDestination(strbuf); strcpy(desc.dest,strbuf);
//		pFlt->getAirline(strbuf);  desc.airlineNO =(short) CAirlinesManager::GetCode(strbuf);
//		desc.userCode = (long)pFlt;	
//
//		desc.loadFactor = 0.0;//pFlt->getLogEntry().loadFactor;
//		desc.nCapacity = 100;//pFlt->getLogEntry().capacity;
//		desc.gateOccupancy  = 30*60*100;
//		desc.arrTime = -1;
//		desc.depTime = -1;
//		//arr time / dep time
//		
//		ALTObjectID standID;
//		if(pFlightInSim->IsArrival())
//		{
//			size_t nCount = vExitObject.size();
//			if (nCount > 0)
//			{
//				standID = vExitObject[rand()%nCount].GetObjectName();
//			}
//			ElapsedTime  eArrTime = eStartTime + ElapsedTime(dInteValue)*(long(i+1));
//			pFlightInSim->SetArrTime(eArrTime);
//			pFlt->setArrTime(eArrTime);
//			
//			desc.arrTime = eArrTime;
//
//		}
//		if(pFlightInSim->IsDeparture())
//		{
//			size_t nCount = vEntryObject.size();
//			if (nCount > 0)
//			{
//				standID = vEntryObject[rand()%nCount].GetObjectName();
//			}
//
//			ElapsedTime eDepTime = eEndTime + ElapsedTime(dInteValue)*(long(i+1));
//
//			pFlightInSim->SetDepTime(eDepTime);
//			pFlt->setDepTime(eDepTime);	
//			desc.depTime = eDepTime;
//		}
//
//		if( m_pAirsideSimulation&&m_pAirsideSimulation->m_pOuput)
//			m_pAirsideSimulation->m_pOuput->LogFlightEntry(pFlt,desc);
//		
//		
//		pFlightInSim->SetOutput(m_pAirsideSimulation->m_pOuput);		
//		pFlightInSim->SetAirTrafficController(m_pAirsideSimulation->GetAirTrafficController());
//
//		CFlightServiceInformation* pServiceInfo = new CFlightServiceInformation(pFlightInSim);
//		FlightTypeDetailsManager *pManager = m_pAirsideSimulation->GetFlightServiceManager()->GetFlightServiceManager();
//		FlightTypeDetailsItem *pItem = pManager->GetFlightDetailItem(desc);
//		if (pItem)
//		{
//			pServiceInfo->SetFltServiceLocations(pItem->GetRelativeLocations());
//			pServiceInfo->SetRingRoute(pItem->GetRingRoad());
//		}
//		pFlightInSim->SetFlightServiceInformation(pServiceInfo);
//
//		//ALTObjectID standID = pFlightInSim->GetFlightInput()->getStand();
//		StandInSim * pStand = m_pAirsideSimulation->GetAirsideResourceManager()->GetAirportResource()->getStandResource()->GetStandByName(standID);	
//		if (pStand)
//			pStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim);
//
//		pFlt->setStand(standID);
//
//		pFlightInSim->SetServiceRequestDispatcher(m_pAirsideSimulation->GetVehicleRequestDispatcher());
//		pFlightInSim->SetPerformance(m_pAirsideSimulation->GetFlightPerformanceManager());
//	
//		
//		m_pAirsideSimulation->GetAirsideFlightList().push_back(pFlightInSim);
//
//
//		FlightBirthEvent *fltbirth = new FlightBirthEvent(pFlightInSim);		
//		fltbirth->addEvent();
//
//	}
//
//
//	return NULL;
//}



















