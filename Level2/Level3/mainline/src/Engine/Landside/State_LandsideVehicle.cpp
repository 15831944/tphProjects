#include "stdafx.h"
#include "State_LandsideVehicle.h"
#include "..\LandsideVehicleInSim.h"

ElapsedTime IState_LandsideVehicle::curTime() 
{
	return SAgent::curTime();
}

IStateMachine* IState_LandsideVehicle::getMachine() const
{
	return getVehicle();
}


//////////////////////////////////////////////////////////////////////////
void State_ServiceTimer::doProcess( CARCportEngine* pEngine )
{
	//CFileOutput outfile("D:\\state.log");
	SetEnd(true);
	if(m_pState)
	{
		//outfile.Log(m_pState->getDesc()).Log("{");
		m_pState->Execute(pEngine); 
		//outfile.Log("}").Line();
	}
}

void State_ServiceTimer::StartTimer( IState* pState, const ElapsedTime& endT )
{
	SetEnd(false);
	removeFromEventList();
	setTime(endT);
	m_pState = pState;	
	addEvent();
}

State_ServiceTimer::State_ServiceTimer()
{
	m_bEnd = true;
	m_pState= NULL;
}


//////////////////////////////////////////////////////////////////////////
//void State_OnBirthLandsideVehicle::Entry( CARCportEngine* pEngine )
//{
//	m_pOwner->InitBirth(pEngine);
//}
