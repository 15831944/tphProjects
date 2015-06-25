// StartDate.cpp: implementation of the CStartDate class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StartDate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStartDate::CStartDate()
{
	m_bAbsDate = false;
	m_dtDate = COleDateTime::GetCurrentTime();
}
/*
CStartDate::CStartDate(const COleDateTime& _date)
{
	m_dtDate.SetDate( _date.GetYear(), _date.GetMonth(), _date.GetDay() );
}
*/
CStartDate::~CStartDate()
{

}


void CStartDate::SetDate(int _year, int _month, int _day)
{
	m_dtDate.SetDate(_year, _month, _day);
}

CStartDate& CStartDate::operator =(const CStartDate& _rhs)
{
	m_dtDate = _rhs.m_dtDate;
	m_bAbsDate = _rhs.m_bAbsDate;
	return (*this);
}



// convert Elapsed time to the real date / time.
void CStartDate::GetDateTime(const ElapsedTime& _t,		// input:
							 bool& _bAbsDate,			// output: tell if _date valid
							 COleDateTime& _date,		// output: give absolute date
							 int& _nDtIdx,				// output: date index
							 COleDateTime& _time )	const	// output: time part.
{
	ElapsedTime eTime = _t;
	if( eTime >= ElapsedTime(0l) )
	{
		int nAdjDays = eTime.asSeconds() / WholeDay;
		eTime -= (long)nAdjDays * WholeDay;

		int hours = eTime.asHours(), minutes = eTime.asMinutes() % 60 , seconds = eTime.asSeconds() % 60;
		if( eTime.asHours() > 23 )
		{
			hours = 23;
			minutes = 59;
			seconds = 59;
		}
		_time.SetTime( hours, minutes, seconds);

//		_time.SetTime( eTime.asHours(), eTime.asMinutes() % 60, eTime.asSeconds() % 60 );

		// decide _bAbsDate
		_bAbsDate = m_bAbsDate;
		if( m_bAbsDate )
			_date = m_dtDate + COleDateTimeSpan( nAdjDays );
		else
			_nDtIdx = nAdjDays;
	}
	else
	{
		int nAdjDays = eTime.asSeconds() / WholeDay;
		nAdjDays = -nAdjDays;
		nAdjDays += 1;
		eTime = (long)nAdjDays * WholeDay + (long)eTime.asSeconds();
		int hours = eTime.asHours(), minutes = eTime.asMinutes() % 60 , seconds = eTime.asSeconds() % 60;
		if( eTime.asHours() > 23 )
		{
			hours = 23;
			minutes = 59;
			seconds = 59;
		}
		_time.SetTime( hours, minutes, seconds);

		// decide _bAbsDate
		_bAbsDate = m_bAbsDate;
		if( m_bAbsDate )
			_date = m_dtDate - COleDateTimeSpan( nAdjDays );
		else
			_nDtIdx = nAdjDays;
		//else
		//	ASSERT( false );
	}
}
	
// read start date from string
void CStartDate::ReadStartDate( CString& _strDate )
{
	if( _strDate.IsEmpty() )
		m_bAbsDate = false;
	else
	{
		int nPrevPos = 0;
		int nPos = 0;
		int	nNum[3];
		int i = 0;
		while (((nPos = _strDate.Find('-', nPos + 1)) != -1) && (i < 3))
		{
			//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
			nNum[i++] = atoi( _strDate.Mid(nPrevPos, nPos - nPrevPos));
			nPrevPos = nPos + 1;
		}
		
		//CString strTemp = strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos); 
		nNum[i] = atoi(_strDate.Mid(nPrevPos, _strDate.GetLength() - nPrevPos));

		m_bAbsDate = true;
		m_dtDate.SetDate( nNum[0], nNum[1], nNum[2] );
	}	
}

// write start date to the str
CString CStartDate::WriteStartDate() const
{
	CString str;
	if( m_bAbsDate )
	{
		str = m_dtDate.Format(_T("%Y-%m-%d"));
	}
	else
	{
		str.Empty();
	}
	return str;
}


void CStartDate::GetRelativeTime( const COleDateTime& _inDate, const ElapsedTime& _inTime, ElapsedTime& _outTime) const
{
	COleDateTime dtTemp( _inDate );
	COleDateTimeSpan dtSpan( _inTime.asHours()/24, _inTime.asHours()%24, _inTime.asMinutes() % 60, _inTime.asSeconds() % 60);
	dtTemp += dtSpan;

	GetRelativeTime(dtTemp, _outTime);
}


void CStartDate::GetRelativeTime(const COleDateTime &_dt, ElapsedTime &_outTime) const
{
	if ( m_bAbsDate )
	{
		COleDateTimeSpan dtsTemp = _dt - m_dtDate;
		_outTime.set((long)dtsTemp.GetTotalSeconds());
	}
	else
		_outTime.set(_dt.GetHour(), _dt.GetMinute(), _dt.GetSecond());
}



void CStartDate::GetElapsedTime(const bool _bAbsDate,		// input: tell if _date valid
							const COleDateTime& _date,		// input: give absolute date
							const int _nDtIdx,				// input: date index
							const COleDateTime& _time,		// input: time part.
							ElapsedTime& _t					// output:
						) const
{
	COleDateTime tmptime;
	COleDateTimeSpan tmptimespan;	
	if(_bAbsDate)
	{
//		tmptime = _date - m_dtDate + _time;	
		tmptime = _time;
		tmptimespan = _date - m_dtDate;
		/*tmptime += tmptimespan;

		long lSeconds = tmptime.GetDay() * 24l;
		lSeconds += tmptime.GetHour();
		lSeconds *= 60l;
		lSeconds += tmptime.GetMinute();
		lSeconds *= 60l;
		lSeconds += tmptime.GetSecond();*/
		

		_t.set( _time.GetHour(), _time.GetMinute(), _time.GetSecond() );
		_t += (long)tmptimespan.GetTotalSeconds();
	}
	else
	{
//		tmptime = _nDtIdx * 24 + _time;
		tmptime = _time;	

		long lSeconds = _nDtIdx * 24l;
		lSeconds += tmptime.GetHour();
		lSeconds *= 60l;
		lSeconds += tmptime.GetMinute();
		lSeconds *= 60l;
		lSeconds += tmptime.GetSecond();

		_t.set( lSeconds );
		//_t.set(tmptime.GetHour(), tmptime.GetMinute(), tmptime.GetSecond());
	}	
}
/*
CString CStartDate::PrintDate()
{
	COleDateTime dtInvalid;
	if (m_dtDate != dtInvalid)
		return m_dtDate.Format(_T("%Y-%m-%d"));
	else
		return _T("");
}
const COleDateTime& CStartDate::GetDate() const
{
	return m_dtDate;
}


BOOL CStartDate::GetRegularDateTime(const ElapsedTime &_inTime, CStartDate &_outDate, ElapsedTime &_outTime)
{
	if (!IsValid())
	{
		_outTime = _inTime;
		return FALSE;
	}

	COleDateTime dtTemp = m_dtDate;
	COleDateTimeSpan dtsTemp(_inTime.asHours() / 24, _inTime.asHours() % 24, _inTime.asMinutes() % 60, _inTime.asSeconds() % 60);
	dtTemp += dtsTemp;
	_outDate.SetDate(dtTemp.GetYear(), dtTemp.GetMonth(), dtTemp.GetDay());
	_outTime.set(dtTemp.GetHour(), dtTemp.GetMinute(), dtTemp.GetSecond());

	return TRUE;
}


int CStartDate::GetYear() const
{
	return m_dtDate.GetYear();
}

int CStartDate::GetMonth() const
{
	return m_dtDate.GetMonth();
}

int CStartDate::GetDay() const
{
	return m_dtDate.GetDay();
}

COleDateTime CStartDate::GetDateTime()
{
	COleDateTime dt = m_dtDate;
	return dt;
}
*/