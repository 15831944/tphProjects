#include "stdafx.h"
#include "./weatherInsim.h"
Precipitationtype WeatherInSim::getPrecipitaionType( const ElapsedTime& t ) const
{
	const CWeatherScripts::TY_DATA * dataList = m_weatherScript.GetData();
	if( dataList && dataList->size() )
	{
		for(int i=0;i<(int)dataList->size();i++)
		{
			CPrecipitationTypeEventItem* pItem = dataList->at(i);
			if( pItem->GetStartTime() <= t && pItem->GetEndTime()>=t )
			{
				return CPrecipitationtype::GetStringType(pItem->GetType());
			}
		}
	}
	return TY_Nonfreezing;
}

WeatherInSim::WeatherInSim()
{
	m_weatherScript.ReadData();
}