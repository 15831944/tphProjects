#include "StdAfx.h"
#include ".\airsidesimulation.h"
#include "../../Engine/terminal.h"
#include "FlightBirthEvent.h"
#include "../../Common/AIRSIDE_BIN.h"
#include "../../Common/AirlineManager.h"
#include "../../Results/OutputAirside.h"
#include "AirsideFlightInSim.h"
#include "StandAssignmentRegisterInSim.h"
#include "AirsideVehicleInSim.h"
#include "VehicleBirthEvent.h"
#include "VehicleRequestDispatcher.h"
#include "VehiclePoolsManagerInSim.h"
#include "FlightServiceManagerInSim.h"
#include "FlightServiceInformation.h"
#include "../../InputAirside/FlightTypeDetailsItem.h"
#include "../../InputAirside/FlightTypeDetailsManager.h"
#include "AirsideItinerantFlightInSim.h"
#include "../../Inputs/GateAssignmentConstraints.h"
#include "../../InputAirside/SimSettingClosure.h"
#include "./FlightBirthTerminalEvent.h"
#include "../../InputAirside/VehicleSpecifications.h"
#include "AirsidePaxBusInSim.h"
#include "../../Inputs/Stand2GateConstraint.h"
#include "../../Inputs/IntermediateStandAssign.h"
#include "../SimulationDiagnoseDelivery.h"
#include "SimulationErrorShow.h"
#include "../../Inputs/FlightPriorityInfo.h"
#include "AirsideTowTruckInSim.h"
#include "SimulationErrorShow.h"
#include "../../Common/ARCTracker.h"
#include "../FltOperatingDoorSpecInSim.h"
#include "AirsideFollowMeCarInSim.h"
#include "AircraftClassificationManager.h"
#include "Engine/ARCportEngine.h"
#include <Results/AirsideFlightEventLog.h>
#include "../OnboardSimulation.h"
#include "../OnboardFlightInSim.h"
#include "AirsideCircuitFlightInSim.h"
#include "../../InputAirside/CTrainFlightsManage.h"
#include "../AirsideBaggageTrainInSim.h"


AirsideSimulation::AirsideSimulation( int nPrjID, CARCportEngine *_pEngine,const CString& strProjPath,FltOperatingDoorSpecInSim* pFltOperatingDoorspec)
{
	m_nPrjID = nPrjID;
	m_pOuput = NULL;
	//m_pInterm = pInterm;
	m_pEngine = _pEngine;
	m_bInitialized = false;

	m_pVehicleRequestDispatcher = new VehicleRequestDispatcher(_pEngine->getTerminal()->m_pAirportDB, _pEngine->getTerminal()->GetProcessorList());
	m_pVehiclePoolsDeployment = new VehiclePoolsManagerInSim;
	m_pVehiclePoolsDeployment->Init(nPrjID, _pEngine->getTerminal()->m_pAirportDB);

	m_pFlightServiceManager = new CFlightServiceManagerInSim(nPrjID,_pEngine->getTerminal());
	m_pAircraftClassification = new AircraftClassificationManager;
	
	m_pVehicleSpecifications = NULL;

	m_pStand2gateconstraint = new CStand2GateConstraint(nPrjID);
	m_strProjPath = strProjPath;
	m_pStand2gateconstraint->SetInputTerminal(_pEngine->getTerminal());
	m_pStand2gateconstraint->loadDataSet(strProjPath);
	m_AirTrafficController.SetFltOperatingDoorSpec(pFltOperatingDoorspec);

	m_pTrainFlightsManger = new CTrainingFlightsManage();
	m_pTrainFlightsManger->ReadData();
	m_bAllowCyclicGroundRoute = false;	
	
}
AirsideSimulation::~AirsideSimulation(void)
{
	//delete flights
	for(int i =0 ;i<(int)m_vFlights.size(); i++)
	{
		delete m_vFlights.at(i);
	}
	for (int i =0; i < int(m_vVehicles.size());i++)
	{
		delete m_vVehicles.at(i);
	}
	delete m_pVehiclePoolsDeployment;
	m_pVehiclePoolsDeployment = NULL;

	delete m_pVehicleRequestDispatcher;
	m_pVehicleRequestDispatcher = NULL;

	delete m_pFlightServiceManager;
	m_pFlightServiceManager = NULL;

	delete m_pVehicleSpecifications;
	m_pVehicleSpecifications = NULL;

	delete m_pStand2gateconstraint;
	m_pStand2gateconstraint = NULL;

	delete m_pAircraftClassification;
	m_pAircraftClassification = NULL;

	if (m_pTrainFlightsManger)
	{
		delete m_pTrainFlightsManger;
		m_pTrainFlightsManger = NULL;
	}
}
#include <boost/bind.hpp>
void UpdateFlightWingSpanInfo(std::vector<AirsideFlightInSim*>& vFlights)
{
	if(!vFlights.size())
		return ;

	AirsideFlightInSim* pFirst = *vFlights.begin();
	AirsideFlightInSim* pSecond = *vFlights.begin();
	
	for(int i=1;i<(int)vFlights.size();i++)
	{
		AirsideFlightInSim* pFlt = vFlights.at(i);
		if(pFirst->GetWingspan() < pFlt->GetWingspan() )
		{
			pFirst = pFlt;
		}
		else if(pSecond->GetWingspan() < pFlt->GetWingspan() )
		{ 
			pSecond = pFlt;
		}		
	}
	for(int i=0;i<(int)vFlights.size();i++)
	{
		AirsideFlightInSim* pFlt = vFlights.at(i);
		pFlt->SetOtherMaxWingSpan(pFlt==pFirst?pSecond->GetWingspan():pFirst->GetWingspan());
	}

}


int AirsideSimulation::AirsideEventListInitialize( EventList* _eventlist)
{
	PLACE_METHOD_TRACK_STRING();
	if(!m_bInitialized) return FALSE;


	//flight init time 
	CArrivalETAOffsetList vETAoffsetSet;
	vETAoffsetSet.SetAirportDatabase(m_pEngine->getTerminal()->m_pAirportDB);
	vETAoffsetSet.ReadData(m_nPrjID);

	ElapsedTime tMinTime  = ElapsedTime::Max();
	for (int i = 0; i < m_pEngine->m_simFlightSchedule.getCount();i++) //it should user the m_simFlightSchedule, it's the flight which has been filted 
	{
		Flight * pFlt =  m_pEngine->m_simFlightSchedule.getItem(i);		

		pFlt->SetFlightIndex(i) ; // set the flight id to index , make the flight id is equal with the log entry. 
		//add birth event to list
		AirsideFlightInSim * pFlightInSim = new AirsideFlightInSim(m_pEngine,pFlt, i);
		m_vFlights.push_back(pFlightInSim);
		pFlightInSim->SetOutput(m_pOuput);		
		pFlightInSim->SetAirTrafficController(&m_AirTrafficController);
		pFlightInSim->SetPerformance(&m_FlightPerformanceManager);	
		pFlightInSim->InitScheduleEndTime(m_pEngine->getTerminal()->flightSchedule->GetFlightScheduleEndTime());

		//init vehicle service information 
		CFlightServiceInformation* pServiceInfo = new CFlightServiceInformation(pFlightInSim);
		FlightTypeDetailsManager *pManager = m_pFlightServiceManager->GetFlightServiceManager();
		FlightTypeDetailsItem *pItem = pManager->GetFlightDetailItem( pFlightInSim->GetLogEntry().GetAirsideDesc() );
		if (pItem)
		{
			pServiceInfo->SetFltServiceLocations(pItem->GetRelativeLocations());
			pServiceInfo->SetRingRoute(pItem->GetRingRoad());
		}
		pFlightInSim->SetFlightServiceInformation(pServiceInfo);	
		pFlightInSim->SetServiceRequestDispatcher(m_pVehicleRequestDispatcher);
		//

		//init arrival time
		pFlightInSim->InitBirth(vETAoffsetSet,m_simConfig);

		tMinTime = min( pFlightInSim->GetBirthTime(),tMinTime);


		CAirsideMobileElementEvent *fltbirth = NULL;
		if( m_simConfig.bOnlySimStand) 
		{
			fltbirth = new FlightBirthTerminalEvent(pFlightInSim, 
				m_Resource.GetAirportResource()->getStandResource());		
		}
		else
		{
			if (m_pEngine->IsOnboardSel())
				fltbirth = new FlightBirthEvent(pFlightInSim,m_pEngine);
			else
				fltbirth = new FlightBirthEvent(pFlightInSim);
		}
		fltbirth->setTime(pFlightInSim->GetBirthTime());
		fltbirth->addEvent();	

		//generate onboard flight close door event
		OnboardFlightGeneration(pFlightInSim);
		
		//Init Stand assignment register
		ALTObjectID standID;
		{
			StandInSim* pArrStand = NULL;
			StandInSim* pDepStand = NULL;
			StandInSim* pIntStand = NULL;
			ElapsedTime tArrOnTime;
			ElapsedTime tArrOffTime;
			ElapsedTime tDepOnTime;
			ElapsedTime tDepOffTime;
			ElapsedTime tIntOnTime;
			ElapsedTime tIntOffTime;

			if (pFlightInSim->IsArrival())
			{
				standID = pFlightInSim->getArrStand();
				pArrStand = m_Resource.GetAirportResource()->getStandResource()->GetStandByName(standID);	
				if (pArrStand)
				{
					tArrOnTime = pFlightInSim->getArrTime();
					tArrOffTime = tArrOnTime + pFlightInSim->getServiceTime(ARR_PARKING);
					pArrStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim, ARR_PARKING, tArrOnTime, tArrOffTime);
					if (!pFlightInSim->IsDeparture())
					{
						WriteStandPlanLog(pFlightInSim, pArrStand, ARR_PARKING, tArrOnTime, tArrOffTime);
					}
				}
			}
			if (pFlightInSim->IsDeparture())
			{
				standID = pFlightInSim->getDepStand();
				pDepStand = m_Resource.GetAirportResource()->getStandResource()->GetStandByName(standID);	
				if (pDepStand)
				{
					tDepOffTime = pFlightInSim->getDepTime();
					tDepOnTime = tDepOffTime - pFlightInSim->getServiceTime(DEP_PARKING);
					pDepStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim, DEP_PARKING, tDepOnTime, tDepOffTime);
					if (!pFlightInSim->IsArrival())
					{
						WriteStandPlanLog(pFlightInSim, pDepStand, DEP_PARKING, tDepOnTime, tDepOffTime);
					}
				}
			}

			if (!pFlightInSim->GetFlightInput()->getIntermediateStand().IsBlank())
			{
				standID = pFlightInSim->GetFlightInput()->getIntermediateStand();
				pIntStand = m_Resource.GetAirportResource()->getStandResource()->GetStandByName(standID);
				if (pIntStand)
				{
					ElapsedTime tIntOnTime = pFlightInSim->getArrTime() + pFlightInSim->getServiceTime(ARR_PARKING);
					ElapsedTime tIntOffTime = tIntOnTime + pFlightInSim->getServiceTime(INT_PARKING);
					pIntStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim, INT_PARKING, tIntOnTime, tIntOffTime);
					WriteStandPlanLog(pFlightInSim, pIntStand, INT_PARKING, tIntOnTime, tIntOffTime);
				}
			}

			if (pFlightInSim->IsArrival() && pFlightInSim->IsDeparture())
			{
				if (pIntStand || pArrStand != pDepStand)
				{
					if (pArrStand)
						WriteStandPlanLog(pFlightInSim, pArrStand, ARR_PARKING, tArrOnTime, tArrOffTime);
					if (pDepStand)
						WriteStandPlanLog(pFlightInSim, pDepStand, DEP_PARKING, tDepOnTime, tDepOffTime);
				}
				else
				{
					if(pArrStand)
						WriteStandPlanLog(pFlightInSim, pArrStand, ARR_PARKING, tArrOnTime, tDepOffTime);
				}
			}
		}			
	}
	if(m_simConfig.bOnlySimStand)
	{
		InitAirsideFlightExtraInfomation();
		return TRUE;
	}
	
	CAirsideSimSettingClosure simSetting(m_nPrjID);
	simSetting.ReadData();

	if (simSetting.IsItinerantFlight())
	{
		AirsideItinerantFlightInSimGenerator itiFltGenrator(m_nPrjID,
			m_pEngine->getTerminal(),
			m_pEngine->getTerminal()->flightSchedule->getCount(),
			this);

		itiFltGenrator.GenerateFlight(m_pEngine);
	}
	

	int nGeneratorCount = (int)m_vFlights.size();

	if(simSetting.IsTrainingFlight())
	{
		AirsideCircuiteFlightInSimGenerator circuiteFlightGenerator(m_pEngine->getTerminal(),this,nGeneratorCount);
		circuiteFlightGenerator.GenerateCircuiteFlight(m_pEngine,m_pTrainFlightsManger);
	}

	InitAirsideFlightExtraInfomation();

	//call this after all flight's info is prepared
	m_AirTrafficController.SetFlightList(m_vFlights);

	UpdateFlightWingSpanInfo(m_vFlights);


	m_bAllowCyclicGroundRoute = simSetting.AllowCyclicGroundRoute();

	bool bVehicleService = simSetting.IsVehicleService();

	m_pVehicleRequestDispatcher->SetVehicleService(bVehicleService);

//	int nVehicleBeginCount = 0;
	int nNextVehicleUnqiueID = 0;
	if (bVehicleService)
	{
		if(m_pVehicleSpecifications != NULL)
			delete m_pVehicleSpecifications;

		m_pVehicleSpecifications = new CVehicleSpecifications();
		m_pVehicleSpecifications->ReadData(m_nPrjID);

		VehiclePoolResourceManager* pPoolRes = m_Resource.GetAirportResource()->getVehicleResource();

		for (int i =0; i < pPoolRes->GetVehiclePoolCount(); i++)
		{
			VehiclePoolInSim* pPool = pPoolRes->GetVehiclePoolByIdx(i);
			if (pPool)
			{
				pPool->SetVehicleRequestDispatcher(m_pVehicleRequestDispatcher);
				pPool->SetVehiclePoolDeployment(m_pVehiclePoolsDeployment);

				int nPoolId = pPool->GetVehiclePoolID();
				for (int j =0; j < m_pVehiclePoolsDeployment->GetVehicleTypeCount(); j++)
				{
					int nVehicleTypeID =m_pVehiclePoolsDeployment->GetVehicleTypeIDByIdx(j);
					int nCount = m_pVehiclePoolsDeployment->GetVehicleQuantityInPool(nVehicleTypeID,nPoolId);
					if (nCount > 0)
					{
						CVehicleSpecificationItem* pVehicleSpecItem = m_pVehicleSpecifications->GetVehicleItemByID(nVehicleTypeID);
						VehicleGeneration(nNextVehicleUnqiueID,nCount,pVehicleSpecItem,nPoolId,tMinTime,m_pVehicleSpecifications);
					}
				}
			}

		}
	}

	return TRUE;

}
void  AirsideSimulation::InitAirsideFlightExtraInfomation()
{
	std::vector<AirsideFlightInSim* > ::iterator iter = m_vFlights.begin();
	for (;iter != m_vFlights.end(); ++ iter)
	{
		(*iter)->InitFlightInfo(m_pEngine->getTerminal()->m_pAirportDB);
	}

}
//generate itinerant flight
void AirsideSimulation::ItinerantFlightGeneration()
{


}
void AirsideSimulation::VehicleGeneration(int& nNextVehicleUnqiueID,int nCount, CVehicleSpecificationItem* pVehicleSpecItem, int nPoolId, ElapsedTime tBirthTime, CVehicleSpecifications *pVehicleSpecifications)
{
	
	VehiclePoolResourceManager* pPoolRes = m_Resource.GetAirportResource()->getVehicleResource();
	VehiclePoolInSim* pPool = pPoolRes->GetVehiclePool(nPoolId);

	for (int i =0; i < nCount; i++)
	{
		AirsideVehicleInSim* pVehicle = GenerateBaseTypeVehicle(nNextVehicleUnqiueID,m_nPrjID,pVehicleSpecItem, pVehicleSpecifications);//new AirsideVehicleInSim(desc,i,m_nPrjID,pVehicleSpecItem);
		pVehicle->InitLogEntry(pPool,m_pOuput);

		CPoint2008 pos;
		CPoint2008 dir;
		pPool->BirthOnPool(pVehicle, pos,dir);
		pVehicle->SetPosition(pos);
		pVehicle->SetBirthTime( tBirthTime);	//birth time

		VehicleRouteResourceManager* pRouteRes = m_Resource.GetAirportResource()->getVehicleRouteRes();
		pVehicle->SetResource(pPool);
		pVehicle->SetTime(tBirthTime);

		pVehicle->WirteLog(pos,pVehicle->GetSpeed(),tBirthTime);
		pVehicle->WirteLog(pos+ dir, pVehicle->GetSpeed(), tBirthTime);

		pVehicle->SetRouteResourceManager(pRouteRes);
		m_vVehicles.push_back(pVehicle);
		pPool->AddVehicleInPool(pVehicle);

		VehicleBirthEvent *vehiclebirth = new VehicleBirthEvent(pVehicle);	
		vehiclebirth->setTime(pVehicle->GetBirthTime());
		pVehicle->GenerateNextEvent(vehiclebirth);
		//vehiclebirth->addEvent();
	}
}

#include "DeiceVehicleInSim.h"
AirsideVehicleInSim* AirsideSimulation::GenerateBaseTypeVehicle(int& nNextVehicleUnqiueID,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem,CVehicleSpecifications *pVehicleSpecifications)
{
	AirsideVehicleInSim *pVehicle = NULL;
	enumVehicleBaseType vehicleBaseType = pVehicleSpecItem->getBaseType();
	
	switch (vehicleBaseType)
	{
	case VehicleType_PaxTruck:
		{
			CPaxBusParkingResourceManager *pPaxParkingManager =  m_Resource.GetAirportResource()->getPaxParkingResouceManager();

			pVehicle = new CAirsidePaxBusInSim(nNextVehicleUnqiueID,nPrjID,pVehicleSpecItem,pPaxParkingManager,m_pEngine);
			nNextVehicleUnqiueID += 1;
		}
		break;
	case VehicleType_DeicingTruck:
		{
			 pVehicle = new DeiceVehicleInSim(nNextVehicleUnqiueID,nPrjID,pVehicleSpecItem);
			 nNextVehicleUnqiueID += 1;
		}
		break;
	case VehicleType_TowTruck:
		{
			pVehicle = new AirsideTowTruckInSim(nNextVehicleUnqiueID,nPrjID,pVehicleSpecItem);
			nNextVehicleUnqiueID += 1;
		}
		break;
	case VehicleType_FollowMeCar:
		{
			pVehicle = new AirsideFollowMeCarInSim(nNextVehicleUnqiueID,nPrjID,pVehicleSpecItem);
			nNextVehicleUnqiueID += 1;
		}
		break;
	case VehicleType_BaggageTug:
		{
			//baggage train is a baggage tug
			//but combining with several baggage carts
			std::vector<int> vIDs;
			pVehicleSpecifications->GetVehicleIDByBaseType(VehicleType_JointBagTug, vIDs);
			CVehicleSpecificationItem * pBagCartsSpecItem = NULL;
			if(vIDs.size())
			{
				pBagCartsSpecItem = pVehicleSpecifications->GetVehicleItemByID(vIDs.at(0));
			}
			AirsideBaggageTrainInSim *pBagTrain =  new AirsideBaggageTrainInSim(nNextVehicleUnqiueID,nPrjID,pVehicleSpecItem, pBagCartsSpecItem, m_pEngine);
			nNextVehicleUnqiueID += 1;
			pVehicle = pBagTrain;

			pBagTrain->Initialize(nNextVehicleUnqiueID);



		}
		break;
	case  VehicleType_JointBagTug:
		{//baggage cart would be generated while creating  AirsideBaggageTrainInSim
			//based on the vehicle specification
			//the max number of carts could be leaded by the tug 

		}
		break;
	default:
		{
			pVehicle = new AirsideVehicleInSim(nNextVehicleUnqiueID,nPrjID,pVehicleSpecItem);
			nNextVehicleUnqiueID += 1;
		}


	}
	return pVehicle;
}

void AirsideSimulation::SetOutputAirside( OutputAirside* pOutput )
{
	m_pOuput = pOutput;
}

void AirsideSimulation::SetGateAssignmentConstraints(GateAssignmentConstraintList* pConstraints)
{
	ASSERT(pConstraints != NULL);
	m_pGateAssignConstraints = pConstraints;
}

bool AirsideSimulation::Initialize( int nPrjID,const AirsideSimConfig& simconf )
{
	PLACE_METHOD_TRACK_STRING();
	m_simConfig = simconf;
	m_FlightPerformanceManager.Init(nPrjID,m_pEngine->getTerminal()->m_pAirportDB);
	m_pAircraftClassification->Init(m_pEngine->getTerminal()->m_pAirportDB->getAcMan(),nPrjID);


	if( ! m_Resource.Init(nPrjID,m_pEngine->getTerminal()->m_pAirportDB, m_pAircraftClassification, simconf) )
		return false;	


	if(simconf.bOnlySimStand)
	{
		m_AirTrafficController.SetResourceManager(&m_Resource);
		m_bInitialized = true;		
		//m_AirTrafficController.Init
		return true;
	}
	if( ! m_AirTrafficController.Init(nPrjID,&m_Resource,m_pAircraftClassification, m_pEngine->getTerminal()->m_pAirportDB,m_pStand2gateconstraint,m_pOuput)) 
		return false;

	m_Resource.GetAirportResource()->getStandResource()->InitStandCriteriaAssignment(nPrjID,m_pEngine->getTerminal()->m_pAirportDB);
	m_AirTrafficController.SetGateAssignmentConstraints( m_pGateAssignConstraints );
	m_AirTrafficController.SetStandAssignmentPriority(m_pEngine->getTerminal()->flightPriorityInfo);

	m_pVehicleRequestDispatcher->SetVehiclePoolsDeployment(m_pVehiclePoolsDeployment);
	m_pVehicleRequestDispatcher->SetVehiclePoolResourceManager(m_Resource.GetAirportResource()->getVehicleResource());
	m_pVehicleRequestDispatcher->SetFlightServiceRequirement(m_AirTrafficController.GetFlightServiceRequirement());
	CPaxBusParkingResourceManager *pPaxParkingManager =  m_Resource.GetAirportResource()->getPaxParkingResouceManager();
	ASSERT(pPaxParkingManager);
	m_pVehicleRequestDispatcher->SetPaxBusParkingResourceManager(pPaxParkingManager);
	m_pVehicleRequestDispatcher->SetBagCartsParkingSpotResourceManager( m_Resource.GetAirportResource()->getBagCartsParkingSpotResManager());
	m_bInitialized = true;
	return true;
}

void AirsideSimulation::FlushLog()
{
	PLACE_METHOD_TRACK_STRING();
	//Get Max End Time
	ElapsedTime maxTime = (long)0;

	for (int i =0 ;i<(int)m_vFlights.size();	i++)
	{
		if (maxTime < m_vFlights.at(i)->GetTime())
		{
			maxTime = m_vFlights.at(i)->GetTime();
		}
	}

	for (int i =0 ;i<(int)m_vVehicles.size();	i++)
	{
		if (maxTime < m_vVehicles.at(i)->GetTime())
		{
			maxTime = m_vVehicles.at(i)->GetTime();
		}
	}

	for(int i =0 ;i<(int)m_vFlights.size();	i++)
	{
		m_vFlights.at(i)->FlushLog(maxTime);
	}

	for(int i =0 ;i<(int)m_vVehicles.size();i++)
	{
		m_vVehicles.at(i)->FlushLog(maxTime);
	}
}
AirsideFlightInSim* AirsideSimulation::GetAirsideFlight(int FlightIndex)
{
	std::vector<AirsideFlightInSim* >::iterator iter = m_vFlights.begin() ;
	for ( ; iter != m_vFlights.end() ; ++iter)
	{
        if((*iter)->GetFlightInput()->getFlightIndex() == FlightIndex)
			return *iter ;
	}
	return NULL ;
}

void AirsideSimulation::WriteStandPlanLog( AirsideFlightInSim* pFlight, StandInSim* pStand,
										  FLIGHT_PARKINGOP_TYPE eParkingOpType, ElapsedTime tStart, ElapsedTime tEnd )
{
	//{
	//	AirsideFlightStandOperationLog* pStandOnLog =
	//		new AirsideFlightStandOperationLog(tStart.getPrecisely(), pFlight,
	//		pStand->GetID(), pStand->GetStandInput()->GetObjectName(),
	//		AirsideFlightStandOperationLog::PlanedStandOn);
	//	pStandOnLog->m_eParkingOpType = eParkingOpType;
	//	pFlight->LogEventItem(pStandOnLog);
	//}

	//{
	//	AirsideFlightStandOperationLog* pStandOffLog =
	//		new AirsideFlightStandOperationLog(tEnd.getPrecisely(), pFlight,
	//		pStand->GetID(), pStand->GetStandInput()->GetObjectName(),
	//		AirsideFlightStandOperationLog::PlanedStandOff);
	//	pStandOffLog->m_eParkingOpType = eParkingOpType;
	//	pFlight->LogEventItem(pStandOffLog);
	//}
}

void AirsideSimulation::OnboardFlightGeneration(AirsideFlightInSim * pFlightInSim)
{
	OnboardSimulation* pOnboardSim = m_pEngine->GetOnboardSimulation();
	if (pOnboardSim == NULL)
		return;
	//onboard arrival flight
	if (pFlightInSim->IsArrival())
	{
		OnboardFlightInSim* pArrFlightInSim = pOnboardSim->GetOnboardFlightInSim(pFlightInSim,true);
		if (pArrFlightInSim)
		{
			ElapsedTime& tArrTime = pFlightInSim->GetArrTime();
			pArrFlightInSim->GenerateCloseDoorEvent(tArrTime);
		}
	}
	//onboard departure flight
	if(pFlightInSim->IsDeparture())
	{
		OnboardFlightInSim* pDepFlightInSim = pOnboardSim->GetOnboardFlightInSim(pFlightInSim,true);
		if (pDepFlightInSim)
		{
			ElapsedTime& tDepTime = pFlightInSim->GetDepTime();
			pDepFlightInSim->GenerateCloseDoorEvent(tDepTime);
		}
	}
}