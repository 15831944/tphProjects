#include "StdAfx.h"
#include ".\detaildelayresult.h"
#include "DetailFlightDelayData.h"
#include "AirsideFlightDelayParam.h"
#include "DetailFlightDelayData.h"
#include "AirsideFlightDelayReport.h"

CDetailDelayResult::CDetailDelayResult(void)
: m_estStartTime(0l)
, m_estEndTime(0l)
{
}

CDetailDelayResult::~CDetailDelayResult(void)
{
}

void CDetailDelayResult::ClearAllData()
{
	for (int i=0; i<(int)m_vFlightData.size(); i++)
	{
		delete m_vFlightData[i];
	}

	m_vFlightData.clear();
	m_vTimeRange.clear();
}

void CDetailDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	ClearAllData();

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	//delay time error
	if (lMaxDelayTime < lMinDelayTime)
	{
		return;
	}

	m_estStartTime = estMinDelayTime;
	m_estEndTime   = estMaxDelayTime;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < lUserIntervalTime)
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (lUserIntervalTime);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(lUserIntervalTime)));

	//	lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	bool bSetTimeRange = false;
	for (int j=0; j<(int)pParameter->getFlightConstraintCount(); j++)
	{
		FlightConstraint flightConstraint = pParameter->getFlightConstraint(j);

		CDetailFlightDelayData *pDetailFlightDelayData = new CDetailFlightDelayData;
		pDetailFlightDelayData->m_fltCons = flightConstraint;

		for (long i=0; i<lDelayTimeSegmentCount; i++)
		{
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

			if (!bSetTimeRange)
			{
				CString strTimeRange = _T("");
				//strTimeRange.Format(_T("%s-%s"), estTempMinDelayTime.printTime(), estTempMaxDelayTime.printTime());
				strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), estTempMinDelayTime.asHours(), estTempMinDelayTime.GetMinute(), estTempMaxDelayTime.asHours(), estTempMaxDelayTime.GetMinute());
				m_vTimeRange.push_back(strTimeRange);
			}			

			pDetailFlightDelayData->m_vData.push_back(GetFlightCountInIntervalTime(flightConstraint, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);  //second
		}

		bSetTimeRange = true;

		m_vFlightData.push_back(pDetailFlightDelayData);
	}
}