#include "StdAfx.h"
#include ".\baggagecartservicestandtevent.h"
#include "AirsideBaggageTrainInSim.h"
#include "Airside\AirsideFlightInSim.h"


CBaggageCartServiceStandtEvent::CBaggageCartServiceStandtEvent( AirsideBaggageTrainInSim *pBaggageTrain, ElapsedTime time )
:CAirsideMobileElementEvent(pBaggageTrain)
,m_pBaggageTrain(pBaggageTrain)
{
	setTime(time);
}

CBaggageCartServiceStandtEvent::~CBaggageCartServiceStandtEvent(void)
{
}

int CBaggageCartServiceStandtEvent::Process()
{
	ASSERT(m_pBaggageTrain);
	if(m_pBaggageTrain  == NULL)
		return 1;

	if(m_pBaggageTrain->getFlightOperation() == ARRIVAL_OPERATION)
	{
		m_pBaggageTrain->LoadBagFromFlight(time);

	}
	else
	{
		m_pBaggageTrain->UnloadBaggageFromCart(time);

	}

	return 1;

}
