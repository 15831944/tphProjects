#include "StdAfx.h"
#include ".\airsidecircuitflightinsim.h"
#include "..\..\InputAirside\TrainingFlightContainer.h"
#include "AirsideSimulation.h"
#include "StandAssignmentRegisterInSim.h"
#include "FlightBirthEvent.h"
#include "AirsideTrainFlightTypeManagerInSim.h"
#include "..\..\InputAirside\CTrainFlightsManage.h"
#include "IFlightProcess.h"
#include "Common\CPPUtil.h"
#include "AirsideCircuitFlightProcess2.h"


AirsideCircuitFlightInSim::AirsideCircuitFlightInSim(CARCportEngine *pARCPortEngine,Flight *pFlight,int nid)
:AirsideFlightInSim(pARCPortEngine,pFlight,nid)
,m_emType(LowAndOver_Operation)
,m_state(START_PROCESS)
,m_dTouchSpeed(0.0)
,m_dLowHigh(0.0)
,m_pTrainFlightTypeManager(NULL)
,m_nLowOver(0)
,m_nTouchGo(0)
,m_nStopGo(0)
,m_nForceGo(0)
{
	m_bCircuitRouteLanding = false;
	m_bCircuitRouteTakeoff = false;
	m_intensionState = false;
	m_process = new AirsideCircuitFlightProcess2(this);
}

AirsideCircuitFlightInSim::~AirsideCircuitFlightInSim(void)
{
	cpputil::autoPtrReset(m_pflightInput);
	cpputil::autoPtrReset(m_pTrainFlightTypeManager);
	cpputil::autoPtrReset(m_process);
}

int AirsideCircuitFlightInSim::fits( const FlightConstraint& _fltcons ) const
{
	return 1;
}

void AirsideCircuitFlightInSim::SetFltConstraint( const FlightConstraint& fltType )
{

}

void AirsideCircuitFlightInSim::SetEntryStart( const ALTObjectID& objID )
{
	m_entryStart = objID;
}

void AirsideCircuitFlightInSim::SetExitSID( const ALTObjectID& objID )
{
	m_exitSID = objID;
}

const ALTObjectID& AirsideCircuitFlightInSim::GetEntryStart() const
{
	return m_entryStart;
}

const ALTObjectID& AirsideCircuitFlightInSim::GetExitSID() const
{
	return m_exitSID;
}

RunwayExitInSim* AirsideCircuitFlightInSim::GetTakeoffPosition()
{
	RunwayResourceManager * pRunwayRes = m_pATC->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource();
	LogicRunwayInSim * pRunway =NULL;

	if(NULL == pRunwayRes)
		return NULL;

	if (m_pTrainFlightTypeManager == NULL)
		return NULL;
	
	if(pRunwayRes->GetRunwayCount())
	{
		RunwayExitInSim* pTakeoff = m_pTrainFlightTypeManager->GetTakeoffPosition(this,pRunwayRes);
		return pTakeoff;
	}
	return NULL;
}

RunwayExitInSim* AirsideCircuitFlightInSim::GetAndAssignTakeoffPosition()
{
	return GetTakeoffPosition();
}
void AirsideCircuitFlightInSim::InitCircuitFlight( CTrainingFlightData *pFlightData )
{
	m_dTouchSpeed = pFlightData->GetTouchValue()*100.0;
	m_dLowHigh = pFlightData->GetLowValue();
	m_nLowOver = pFlightData->GetLowFlight();
	m_nTouchGo = pFlightData->GetTouchFlight();
	m_nStopGo = pFlightData->GetStopFlight();
	m_nForceGo = pFlightData->GetForceGo();
	double dRandValue = pFlightData->GetStopValue().GetProbDistribution()->getRandomValue();
	m_eStopTime = ElapsedTime(dRandValue*60);
}

void AirsideCircuitFlightInSim::getACType( char *p_str ) const
{
	strcpy (p_str, m_pflightInput->getLogEntry().acType); 
}

AirsideCircuitFlightInSim::LandingOperation AirsideCircuitFlightInSim::GetStartLandingOperation() const
{
	if (m_nLowOver)
		return LowAndOver_Operation;
	
	if (m_nTouchGo)
		return TouchAndGo_Operaiton;
	
	if (m_nStopGo)
		return StopAndGo_Operaiton;

	if (m_nForceGo)
		return ForceAndGo_Operation;
	
	return LowAndOver_Operation;
}

void AirsideCircuitFlightInSim::SetTrainFlightManager( CTrainingFlightTypeManage* pTrainFlightTypeManager )
{
	m_pTrainFlightTypeManager = new AirsideTrainFlightTypeManagerInSim(pTrainFlightTypeManager);

}

void AirsideCircuitFlightInSim::GetNextClearance( ClearanceItem& lastItem, Clearance& newClearance )
{
	if(m_process)
	{
		bool btillEnd  = m_process->Process(lastItem,newClearance);
		if(btillEnd)
			return;
	}
	
	lastItem.SetMode(OnTerminate);
	lastItem.SetResource(NULL);
	newClearance.AddItem(lastItem);
	return;
	
}

void AirsideCircuitFlightInSim::ReleaseIntension( LandingOperation op )
{
	switch(op)
	{
	case LowAndOver_Operation:
		m_nLowOver--;
		break;
	case StopAndGo_Operaiton:
		m_nStopGo--;
		break;
	case ForceAndGo_Operation:
		m_nForceGo--;
		break;
	case TouchAndGo_Operaiton:
		m_nTouchGo--;
		break;
	default:NULL;
	}
}

void AirsideCircuitFlightInSim::SetStartIntension(bool bStart)
{
	if (m_intensionState == false)
	{
		m_intensionState = true;
	}
}

bool AirsideCircuitFlightInSim::IntensionOn()const
{
	return m_intensionState;
}

bool AirsideCircuitFlightInSim::isAllIntensionDone() const
{
	return LowOverDone() && ForceGoDone() && StopGoDone() && TouchGoDone();
}

AirsideCircuitFlightInSim::LandingOperation AirsideCircuitFlightInSim::GetRandomLandingOperation() const
{
	return GetStartLandingOperation();
}

AirsideCircuiteFlightInSimGenerator::AirsideCircuiteFlightInSimGenerator(InputTerminal* pInputTerminal,AirsideSimulation *pAirsideSimulation,int nid)
:m_pInputTerminal(pInputTerminal)
,m_pAirsideSimulation(pAirsideSimulation)
,m_nFlightID(nid)
{

}

AirsideCircuiteFlightInSimGenerator::~AirsideCircuiteFlightInSimGenerator()
{

}

void AirsideCircuiteFlightInSimGenerator::GenerateCircuiteFlight( CARCportEngine *pARCPortEngine,CTrainingFlightsManage* pTrainFlightManager )
{
	CTrainingFlightContainer trainingFlightCon;
	trainingFlightCon.ReadData();

	for (size_t i = 0; i < trainingFlightCon.GetElementCount(); i++)
	{
		CTrainingFlightData* pTrainingFlightData = trainingFlightCon.GetItem(i);
		CTrainingFlightTypeManage* pTrainFlightTypeManager = pTrainFlightManager->FindFlightTypeItem(*pTrainingFlightData);
		InitializeFlight(pARCPortEngine,pTrainingFlightData,pTrainFlightTypeManager);
	}
}

void AirsideCircuiteFlightInSimGenerator::InitializeFlight( CARCportEngine *pARCPortEngine,CTrainingFlightData *pFlightData,CTrainingFlightTypeManage* pTrainFlightTypeManager )
{
	int nIdx = 0;
	for (int i = 0; i < pFlightData->GetNumFlight(); i++)
	{
		Flight *pFlight = new Flight((Terminal *)m_pInputTerminal);
		CString strAirline = pFlightData->GetCarrierCode();
		pFlight->setAirline(strAirline);
		CString strArrID;
		strArrID.Format(_T("%d"),2*nIdx+1);
		CString strDepID;
		strDepID.Format(_T("%d"),2*nIdx+2);
		pFlight->setArrID(strArrID);
		pFlight->setDepID(strDepID);
		TimeRange timeRange = pFlightData->GetTimeRange();
		ElapsedTime eArrTime = timeRange.GetStartTime();
		ElapsedTime eIntervalTime = pFlightData->GetDistribution().GetProbDistribution()->getRandomValue()*60;
		eArrTime += eIntervalTime*long(i);
		pFlight->setArrTime(eArrTime);
		pFlight->setDepTime(timeRange.GetEndTime());
		pFlight->setACType(pFlightData->GetAcType());
		pFlight->setServiceTime(ElapsedTime(30*60L));
		//set arrival stand
		ALTObjectID entryStart;
		if (pFlightData->GetStartType())
		{
			//pFlight->setArrStand(pFlightData->GetStartPoint());
		}
		else
			entryStart = pFlightData->GetStartPoint();
		//set departure stand
		ALTObjectID exitSID;
		if (pFlightData->GetEndType())
		{
			//pFlight->setDepStand(pFlightData->GetEndPoint());
		}
		else
			exitSID = pFlightData->GetEndPoint();

		AirsideCircuitFlightInSim *pFlightInSim = new AirsideCircuitFlightInSim(pARCPortEngine,pFlight,++m_nFlightID);
		pFlightInSim->SetOutput(m_pAirsideSimulation->m_pOuput);		
		pFlightInSim->SetAirTrafficController(m_pAirsideSimulation->GetAirTrafficController());
		pFlightInSim->SetPerformance(m_pAirsideSimulation->GetFlightPerformanceManager());
		pFlightInSim->SetEntryStart(entryStart);
		pFlightInSim->SetExitSID(exitSID);
		pFlightInSim->InitCircuitFlight(pFlightData);
		pFlightInSim->SetTrainFlightManager(pTrainFlightTypeManager);
		pFlightInSim->SetArrTime(pFlight->getArrTime());

		std::vector<StandInSim*> vArrStands;
		StandInSim* pArrStand = NULL;
		m_pAirsideSimulation->GetAirsideResourceManager()->GetAirportResource()->getStandResource()->GetStandListByStandFamilyName(entryStart,vArrStands);	
		int nArrStandCount = (int)vArrStands.size();
		if ( nIdx < nArrStandCount)
		{
			pArrStand = vArrStands[nIdx];
		}
		if (pFlightInSim->IsArrival() && pArrStand)
		{
			pFlight->setArrStand(pArrStand->GetStandInput()->getName());
			pArrStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim, ARR_PARKING,
				pFlightInSim->getArrTime(), pFlightInSim->getArrTime()+pFlightInSim->getServiceTime(ARR_PARKING));
		}
		std::vector<StandInSim*> vDepStands;
		StandInSim* pDepStand = NULL;
		m_pAirsideSimulation->GetAirsideResourceManager()->GetAirportResource()->getStandResource()->GetStandListByStandFamilyName(exitSID,vDepStands);	
		int nDepStandCount = (int)vDepStands.size();
		if (nIdx < nDepStandCount)
		{
			pDepStand = vDepStands[nIdx];
		}

		if (pFlightInSim->IsDeparture() && pDepStand)
		{
			pFlight->setDepStand(pDepStand->GetStandInput()->getName());
			ElapsedTime tStart = pFlightInSim->getDepTime() - pFlightInSim->getServiceTime(DEP_PARKING);			
			pDepStand->GetStandAssignmentRegister()->StandAssignmentRecordInit(pFlightInSim,DEP_PARKING, 
				tStart > 0L? tStart: 0L, pFlightInSim->getDepTime());
		}
		
		pFlightInSim->SetPerformance(m_pAirsideSimulation->GetFlightPerformanceManager());

		if (m_pAirsideSimulation&&pFlightInSim->IsArrival())
		{
			//Time is out of range
			if (!(m_pAirsideSimulation->GetSimStartTime() <= pFlightInSim->getArrTime()
				&& pFlightInSim->getArrTime() <= m_pAirsideSimulation->GetSimEndTime()))
			{			
				delete pFlightInSim;
				continue;
			}
		}
		else if (m_pAirsideSimulation&&pFlightInSim->IsDeparture())
		{
			//Time is out of range
			if (!(m_pAirsideSimulation->GetSimStartTime() <= pFlightInSim->getDepTime()
				&& pFlightInSim->getDepTime() <=  m_pAirsideSimulation->GetSimEndTime()))
			{
				delete pFlightInSim;
				continue;
			}
		}
		else
		{	
			delete pFlightInSim;
			continue;
		}

		m_pAirsideSimulation->GetAirsideFlightList().push_back(pFlightInSim);
		FlightBirthEvent *fltbirth = new FlightBirthEvent(pFlightInSim);		
		fltbirth->addEvent();
		nIdx++;
	}
}

