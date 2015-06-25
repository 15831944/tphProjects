#include "stdafx.h"
#include "TimeRange.h"
#include "Common/ARCMathCommon.h"

TimeRange::TimeRange()
{
}


TimeRange::TimeRange(ElapsedTime etStart, ElapsedTime etEnd)
:m_etStart(etStart), m_etEnd(etEnd)
{
}

TimeRange::~TimeRange(void)
{
}


//CString TimeRange::GetString()const
//{
//	//int nStartDay, nEndDay;
//	//COleDateTime dtStart, dtEnd;
//	//ConvertElapsedTime(nStartDay, dtStart, m_etStart);
//	//ConvertElapsedTime(nEndDay, dtEnd, m_etEnd);
//	//	strRet.Format("Day:%d %d %d - Day:%d %d %d", 
//	//		dtStart.GetDay(),dtStart.GetHour(),dtStart.GetMinute(),
//	//		dtEnd.GetDay(),dtEnd.GetHour(),dtEnd.GetMinute());
//	
//	CString strRet;
//
//	strRet.Format("%s - %s", m_etStart.printTime(), m_etEnd.printTime());
//
//	return strRet;
//}

void TimeRange::GetString( CString& strText ) const
{
	strText.Format("%s - %s", m_etStart.printTime(), m_etEnd.printTime());
}

CString TimeRange::GetString() const
{
	CString strText;
	strText.Format("%s - %s", m_etStart.printTime(), m_etEnd.printTime());
	return strText;
}


CString TimeRange::MakeDatabaseString() const
{
	CString strData;
	strData.Format(_T("%d,%d"),m_etStart.asSeconds(),m_etEnd.asSeconds());
	return strData;
}

void TimeRange::ParseDatabaseString(const CString& strDatabaseString)
{
	int nPos = strDatabaseString.Find(',');
	if (nPos != -1)
	{
		CString strLeft;
		CString strRight;
		strLeft = strDatabaseString.Left(nPos);
		m_etStart.set(atoi(strLeft));
		int nLengh = strDatabaseString.GetLength();
		strRight = strDatabaseString.Right(nLengh - nPos - 1);
		m_etEnd.set(atoi(strRight));
	}
}

CString TimeRange::ScreenPrint() const
{
	CString strRet;
	strRet.Format("Day%d %d:%d:%d - Day%d %d:%d:%d", 
			m_etStart.GetDay(),m_etStart.GetHour(),m_etStart.GetMinute(),m_etStart.GetSecond(),
			m_etEnd.GetDay(),m_etEnd.GetHour(),m_etEnd.GetMinute(),m_etEnd.GetSecond());
	return strRet;
}

bool TimeRange::isTimeInRange( const ElapsedTime& t ) const
{
	return m_etStart<=t && t <= m_etEnd;
}

ElapsedTime TimeRange::GetRandTime() const
{
	ElapsedTime dT = GetEndTime() - GetStartTime();
	double drand = ARCMath::Randf();
	return m_etStart * (1-drand) + m_etEnd * (drand);
		
}

bool TimeRange::IsOverlapped( const TimeRange& passedRange ) const
{
	if(passedRange.GetStartTime()> m_etStart && passedRange.GetStartTime() < m_etEnd)
		return true;
	if(passedRange.GetEndTime()> m_etStart && passedRange.GetEndTime() < m_etEnd)
		return true;
	if(m_etStart > passedRange.GetStartTime() && m_etStart < passedRange.GetEndTime())
		return true;
	if(m_etEnd > passedRange.GetStartTime() && m_etEnd < passedRange.GetEndTime())
		return true;


	return false;

}

//void TimeRange::ConvertElapsedTime(int& nDay, COleDateTime& time, const ElapsedTime& elapsedTime)
//{
//	int nTotalSeconds = elapsedTime.asSeconds();
//	nDay = (int) nTotalSeconds / 86400;
//	int nHours = int((nTotalSeconds - nDay * 86400) / 3600);
//	int nMinutes = int((nTotalSeconds - nDay * 86400 - nHours * 3600) / 60);
//	int nSeconds = nTotalSeconds - nDay * 86400 - nHours * 3600 - nMinutes * 60;
//	time.SetTime(nHours, nMinutes, nSeconds);
//}