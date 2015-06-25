#include "StdAfx.h"
#include ".\airsideflightdelaybaseresult.h"

CAirsideFlightDelayBaseResult::CAirsideFlightDelayBaseResult(void)
{
}

CAirsideFlightDelayBaseResult::~CAirsideFlightDelayBaseResult(void)
{
}

long CAirsideFlightDelayBaseResult::ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval)
{
	if (eMinValue == eMaxTime)
	{
		eMinValue = ElapsedTime(0L);
	}
	bool bInSecond = false;
	int _nMinNumCol = 3;
	ElapsedTime timeDiff = eMaxTime - eMinValue;

	ElapsedTime eUnitTimeOnX;

	int nTimeDiffAsHours = timeDiff.asHours();
	int nTimeDiffAsMin = timeDiff.asMinutes();
	int nTimeDiffAsSec = timeDiff.asSeconds();

	if( nTimeDiffAsHours > _nMinNumCol )
	{
		// if hh  > _nMinNumCol, display column up to 2*_nMinNumCol
		int nMinHours = nTimeDiffAsHours / (2*_nMinNumCol) + 1;
		eUnitTimeOnX.set( nMinHours, 0, 0 );
		eMinValue.SetMinute( 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsHours > (2*_nMinNumCol) )
			_nMinNumCol = (2*_nMinNumCol);
		else
			_nMinNumCol = nTimeDiffAsHours;
	}
	else if( nTimeDiffAsMin > _nMinNumCol )
	{
		// else if hhmm > nNumCol min, so the column unit should be hhmm % 2 nNumCol min
		int nMinMinutes = nTimeDiffAsMin / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( nMinMinutes / 60, nMinMinutes % 60, 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsMin > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsMin;
	}
	else if( nTimeDiffAsSec > _nMinNumCol )
	{
		// be hhmmss % 2 nNumCol sec.
		int mMinSeconds = nTimeDiffAsSec / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( mMinSeconds / 3600, mMinSeconds / 60, mMinSeconds % 60 );
		if( nTimeDiffAsSec > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsSec;
		bInSecond = true;
	}

	eInterval = eUnitTimeOnX;
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60L);
	}

	return _nMinNumCol;
}

long CAirsideFlightDelayBaseResult::GetMinDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	vector<CAirsideFlightDelayReport::FltTypeDelayItem>::iterator iter = fltTypeDelayData.begin();

	vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iterItem = (*iter).m_vDelayData.begin();

	long lMinDelayTime = 0;

	bool bInitFlag = true;
	for (; iter!=fltTypeDelayData.end(); iter++)
	{
		iterItem = (*iter).m_vDelayData.begin();

		for (; iterItem!=(*iter).m_vDelayData.end(); iterItem++)
		{
			if (bInitFlag)
			{
				bInitFlag = false;
				lMinDelayTime = (*iterItem).fltDesc.startTime;
			}
			else
			{
				if ((*iterItem).fltDesc.startTime < lMinDelayTime)
				{
					lMinDelayTime = (*iterItem).fltDesc.startTime;
				}
			}
		}
	}

	return lMinDelayTime;
}

long CAirsideFlightDelayBaseResult::GetMaxDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	vector<CAirsideFlightDelayReport::FltTypeDelayItem>::iterator iter = fltTypeDelayData.begin();

	vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iterItem = (*iter).m_vDelayData.begin();

	long lMaxDelayTime = 0;

	bool bInitFlag = true;
	for (; iter!=fltTypeDelayData.end(); iter++)
	{
		iterItem = (*iter).m_vDelayData.begin();

		for (; iterItem!=(*iter).m_vDelayData.end(); iterItem++)
		{
			if (bInitFlag)
			{
				bInitFlag = false;
				lMaxDelayTime = (*iterItem).fltDesc.endTime;
			}
			else
			{
				if ((*iterItem).fltDesc.endTime > lMaxDelayTime)
				{
					lMaxDelayTime = (*iterItem).fltDesc.endTime;
				}
			}
		}
	}

	return lMaxDelayTime;
}