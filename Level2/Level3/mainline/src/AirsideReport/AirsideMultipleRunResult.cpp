#include "stdafx.h"
#include "AirsideMultipleRunResult.h"
#include "AirsideBaseReport.h"


CAirsideMultipleRunResult::CAirsideMultipleRunResult(void)
{
}


CAirsideMultipleRunResult::~CAirsideMultipleRunResult(void)
{
	ClearSimReport();
}

void CAirsideMultipleRunResult::AddSimReport( const CString& strSimResult,  CAirsideBaseReport* pReport )
{
	m_mapSimReport[strSimResult] = pReport;
}

void CAirsideMultipleRunResult::ClearSimReport()
{
	mapSimReport::iterator iter = m_mapSimReport.begin();
	for (; iter != m_mapSimReport.end(); ++iter)
	{
		delete iter->second;
	}
	m_mapSimReport.clear();
}
 
long CAirsideMultipleRunResult::GetMapMinValue( mapLoadResult mapData )
{
	bool bInitFlag = true;
	long iMin = 0;
	mapLoadResult::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		int iValue = GetMinValue(iter->second);
		if (bInitFlag)
		{
			iMin = iValue;
			bInitFlag = false;
		}
		else if (iMin > iValue)
		{
			iMin = iValue;
		}
	}
	return iMin;
}
 
long CAirsideMultipleRunResult::GetMapMaxValue( mapLoadResult mapData )
{
	bool bInitFlag = true;
	long iMax = 0;
	mapLoadResult::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		int iValue = GetMaxValue(iter->second);
		if (bInitFlag)
		{
			iMax = iValue;
			bInitFlag = false;
		}
		else if (iMax < iValue)
		{
			iMax = iValue;
		}
	}
	return iMax;
}

long CAirsideMultipleRunResult::GetMinValue( std::vector<long> vData ) const
{
	bool bInitFlag = true;
	long iMin = 0;
	for (size_t i = 0; i < vData.size(); i++)
	{
		int iValue = vData.at(i);
		if (bInitFlag)
		{
			iMin = iValue;
			bInitFlag = false;
		}
		else if (iMin > iValue)
		{
			iMin = iValue;
		}
	}
	return iMin;
}

long CAirsideMultipleRunResult::GetMaxValue( std::vector<long> vData ) const
{
	bool bInitFlag = true;
	long iMax = 0;
	for (size_t i = 0; i < vData.size(); i++)
	{
		int iValue = vData.at(i);
		if (bInitFlag)
		{
			iMax = iValue;
			bInitFlag = false;
		}
		else if (iMax < iValue)
		{
			iMax = iValue;
		}
	}
	return iMax;
}

int CAirsideMultipleRunResult::GetIntervalCount( long iStart, long iEnd, std::vector<long> vData,long iIgnore /*= 0*/ ) const
{
	int iCount = 0;
	for (unsigned i = 0; i < vData.size(); i++)
	{
		long iData = vData.at(i);
		if (iData < iIgnore)//ignore necessary secs
			continue;

		if (iData >= iStart && iData < iEnd)
		{
			iCount++;
		}
	}
	return iCount;
}

long CAirsideMultipleRunResult::ClacTimeRange( ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval )
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

CString CAirsideMultipleRunResult::GetMultipleRunReportString()
{
	CString strMultipleRun;
	strMultipleRun = _T("(");
	mapSimReport::iterator iter = m_mapSimReport.begin();
	for (int i = 0; iter != m_mapSimReport.end(); ++iter,i++)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strRun;
		strRun.Format(_T("Run%d"),nCurSimResult+1);

		if(i == 0)
		{
			strMultipleRun += strRun;
		}
		else 
		{
			strMultipleRun += _T(",");
			strMultipleRun += strRun;
		}
	}
	 
	strMultipleRun += _T(")");
	return strMultipleRun;
}
