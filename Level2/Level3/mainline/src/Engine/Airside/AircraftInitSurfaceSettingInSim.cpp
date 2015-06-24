#include "stdafx.h"
#include ".\AircraftInitSurfaceSettingInSim.h"
#include "AirsideFlightInSim.h"

CAircraftInitSurfaceSettingInSim::CAircraftInitSurfaceSettingInSim( CAirportDatabase* pArpDb ) : m_settingInput(pArpDb)
{
	m_settingInput.ReadData();
}

CAircraftInitSurfaceSettingInSim::~CAircraftInitSurfaceSettingInSim()
{

}


AircraftSurfacecondition CAircraftInitSurfaceSettingInSim::getFlightSurfaceCondition( AirsideFlightInSim* pFlight, const ElapsedTime& time )
{
	CAircraftSurfaceSetting::TY_DATA* pData = m_settingInput.GetData();

	//get all in time data 
	CAircraftSurfaceSetting::TY_DATA inTimeData;
	for(int i=0;i< (int)pData->size(); i++)
	{
		CSurfacesInitalEvent* pEvent = (*pData).at(i);
		if( pEvent->GetStartTime() <= time && time < pEvent->GetEndTime() )
		{
			inTimeData.push_back(pEvent);
		}
	}
	//find the most suitable data 
	for(int i=0;i<(int)inTimeData.size();i++)
	{
		CSurfacesInitalEvent* pEvent = inTimeData.at(i);
		CSurfacesInitalEvent::TY_DATA * pConditionData = pEvent->GetConditions();
		for(int cIdx = 0;cIdx < (int)pConditionData->size(); cIdx++)
		{
			CConditionItem* pCondition = pConditionData->at(i);
			if( pFlight->fits(*pCondition->GetFlightType()) )
			{
				return CAircraftSurfacecondition::GetStringType( pCondition->GetCondition() );
			}
		}
	}
	
	return TY_Clear;
}