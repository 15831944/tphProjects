#include "StdAfx.h"
#include ".\flightbirthevent.h"
#include "AirsideFlightInSim.h"
#include "Engine\ARCportEngine.h"
#include "Common\ARCException.h"
#include "Common\ARCExceptionEx.h"
#include "Engine\SimulationDiagnoseDelivery.h"

FlightBirthEvent::FlightBirthEvent(AirsideFlightInSim* flt, CARCportEngine * pEngine)
:CAirsideMobileElementEvent(flt)
,m_pARCPortEngine(pEngine)
{
	m_pCFlight = flt;
	setTime(m_pCFlight->GetBirthTime());
}

FlightBirthEvent::~FlightBirthEvent(void)
{
}

int FlightBirthEvent::Process()
{	
	//start logging
	m_pCFlight->StartLogging();
	m_pCFlight->InitStatus();


	m_pCFlight->SetTime(time);
	m_pCFlight->WakeUp(time);

	if (m_pARCPortEngine && m_pARCPortEngine->IsOnboardSel() && m_pCFlight->IsArrivingOperation())
	{
		try
		{
			PaxGeneration();
		}
		catch (ARCSimEngineException* sim_exception)
		{
			m_pARCPortEngine->SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
			delete sim_exception;
		}
		catch (OnBoardSimEngineException* onboard_exeption)
		{
			OnBoardDiagnose* pDiagnose = 
				(OnBoardDiagnose*)onboard_exeption->GetDiagnose();
			DiagnoseDelivery()->DeliveryOnBoardDiagnose(pDiagnose);
			delete onboard_exeption;
		}

	}

	return TRUE;

}

void FlightBirthEvent::PaxGeneration()
{
	std::vector<Person*> vPaxList;
	CPaxGenerator paxgenerator(m_pARCPortEngine) ;
	paxgenerator.GenerateDelayMobileElement(m_pCFlight->GetFlightInput()->getFlightIndex(), time, vPaxList,false, -1) ;
}