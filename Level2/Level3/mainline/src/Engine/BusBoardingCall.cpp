#include "StdAfx.h"
#include ".\busboardingcall.h"
#include "process.h"
#include "../Inputs/MobileElemConstraint.h"
#include "hold.h"
CBusBoardingCall::CBusBoardingCall(Terminal* p_terminal):m_pterminal(p_terminal)
{
}
CBusBoardingCall::~CBusBoardingCall()
{

}
int CBusBoardingCall::BoardingCallHoldingArea(Flight* pFlight , ElapsedTime time ,int pax_num,CAirsidePaxBusInSim* _bus) 
{
	ProcessorArray vHoldingAreas;
	m_pterminal->procList->getProcessorsOfType (HoldAreaProc, vHoldingAreas);
	int iHoldAreaCount = vHoldingAreas.getCount();

	CMobileElemConstraint paxType(m_pterminal);
	HoldingArea* p_holdArea = NULL ;
	int actual_pax = 0 ;
	for (int i = 0 ; i < iHoldAreaCount ; ++i)
	{
		
		p_holdArea =(HoldingArea*) vHoldingAreas.getItem(i) ;
		FlightConstraint fltcnst = pFlight->getType ('D');
		fltcnst.SetAirportDB(paxType.GetAirportDB());
		FlightConstraint& paxCnst = paxType;
		paxCnst = fltcnst;
		p_holdArea->RegisterServerBus(_bus) ;
		actual_pax = actual_pax + p_holdArea->releasePaxToAirside(paxType , pax_num ,time ) ;
	}
	return actual_pax ;
}