#include "stdafx.h"
#include <iomanip>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include "elaptime.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ElapsedTime ElapsedTime::operator + (const ElapsedTime& t) const
{
    ElapsedTime work;
	work.m_time = m_time + t.m_time;
	return work;
}

ElapsedTime ElapsedTime::operator - (const ElapsedTime& t) const
{
    ElapsedTime work;
	work.m_time = m_time - t.m_time;
	return work;
}

ElapsedTime ElapsedTime::operator + (long seconds) const
{
    ElapsedTime work;
	work.m_time = (m_time + seconds*TimePrecision);
	return work;
}

ElapsedTime ElapsedTime::operator - (long seconds) const
{
    ElapsedTime work;
	work.m_time = (m_time - seconds*TimePrecision);
	return work;
}

ElapsedTime ElapsedTime::operator * (long seconds) const
{
    ElapsedTime work;
    work.m_time = (m_time * seconds);
	return work;
}

ElapsedTime ElapsedTime::operator / (long seconds) const
{
    ElapsedTime work;
    work.m_time = (seconds)? (m_time / seconds): 0l;
	return work;
}

void ElapsedTime::set (int hour, int minute, int second)
{
	m_time =  ((long)3600*hour + (long)60*minute +
							(long)second) * TimePrecision;
}

void ElapsedTime::set (const COleDateTime& oleDateTime)
{
	double secTime = oleDateTime.m_dt*24*3600;
	long seconds = (long)floor(secTime);
	if ( (secTime - seconds) >= 0.5 )
	{
		seconds++;
	}
	m_time =  seconds * TimePrecision;
}

void ElapsedTime::printTime (char *str, int printSeconds) const
{
	
    long seconds;
	seconds= m_time/ TimePrecision;
	if(m_time<0) seconds= -m_time / TimePrecision;


    short hours = (short) (seconds / 3600L);
    short min = (short) ((seconds - (hours * 3600L)) / 60L);
    short sec = (short) (seconds - (hours * 3600L) - (min * 60L));

	if(m_time<0)
	{
		char strHour[4];
		_itoa(hours,strHour,10);
		strcpy(str,"-");
		strcat(str,strHour);
	}
	else
	{
		_itoa (hours, str, 10);
	}
    strcat (str, ":");
    if (min < 10 && min>=0)
        strcat (str, "0");
    _itoa (min, str + strlen(str), 10);

	if (printSeconds)
	{
		strcat (str, ":");
		if (sec < 10)
			strcat (str, "0");
		_itoa (sec, str + strlen(str), 10);
	}
}

CString ElapsedTime::printTime( int printSeconds/* = 1*/ ) const
{
	char szBuf[32] = {0};
	printTime( szBuf, printSeconds );

	return CString( szBuf );
}

CString ElapsedTime::PrintDateTime() const
{
	CString strDateTime;
	strDateTime.Format(_T("Day%d %02d:%02d:%02d"), GetDay(),GetHour(),GetMinute(),GetSecond());
	return strDateTime;
}

stdostream& operator << (stdostream& s, const ElapsedTime& t)
{
	char str[16] = {0};
    t.printTime(str);

    return s << str;
}

/*
Possible Format:

"-"  --> minus

If got two ":",  hh:mm:ss / h:m:s

else if got one ":",  hh:mm / h:m

else if length is 6:   hhmmss

else if length is 4:   hhmm

else wrong format, return false;

*/


bool ElapsedTime::SetTime( const char* _strTime )
{
	const int nStrLen = 12;
	char strTime[nStrLen];
	bool bMinus = false;
	// "-"  --> minus
	if( _strTime[0] == '-' )
	{
		strcpy( strTime, _strTime+1 );
		bMinus = true;
	}
	else
	{
		strcpy( strTime, _strTime );
	}

	// count ":"
	int nIdx1 = -1;
	int nIdx2 = -1;
	for( int i=0; i<nStrLen; i++ )
	{
		if( strTime[i] == ':' )
		{
			if( nIdx1 == -1 )
				nIdx1 = i;
			else if( nIdx2 == -1 )
				nIdx2 = i;
		}
	}

	// If got two ":",  hh:mm:ss
	if( nIdx2 != -1 )
	{
		int nSeconds = atoi( strTime + nIdx2 + 1 );
		strTime[nIdx2] = 0;
		int nMinutes = atoi( strTime + nIdx1 + 1 );
		strTime[nIdx1] = 0;
		int nHours = atoi( strTime );
		set( nHours, nMinutes, nSeconds );
		if( bMinus )
			m_time = -m_time;
		return true;
	}

	// else if got one ":",  hh:mm / h:m
	if( nIdx1 != -1 )
	{
		int nMinutes = atoi( strTime + nIdx1 + 1 );
		strTime[nIdx1] = 0;
		int nHours = atoi( strTime );
		set( nHours, nMinutes, 0 );
		if( bMinus )
			m_time = -m_time;
		return true;
	}


	// else if length is 6:   hhmmss
	if( strlen( strTime) == 6 )
	{
		strTime[6] = 0;
		int nSeconds = atoi( strTime + 4 );
		strTime[4] = 0;
		int nMinutes = atoi( strTime + 2 );
		strTime[2] = 0;
		int nHours = atoi( strTime );
		set( nHours, nMinutes, nSeconds );
		if( bMinus )
			m_time = -m_time;
		return true;
	}

	// else if length is 4:   hhmm
	if( strlen( strTime) == 4 )
	{
		strTime[4] = 0;
		int nMinutes = atoi( strTime + 2 );
		strTime[2] = 0;
		int nHours = atoi( strTime );
		set( nHours, nMinutes, 0 );
		if( bMinus )
			m_time = -m_time;
		return true;
	}

	// else if length is 3:   hhmm
	if( strlen( strTime) == 3 )
	{
		strTime[3] = 0;
		int nMinutes = atoi( strTime + 1 );
		strTime[1] = 0;
		int nHours = atoi( strTime );
		set( nHours, nMinutes, 0 );
		if( bMinus )
			m_time = -m_time;
		return true;
	}

	// else wrong format, return false;
	return false;
		
}

stdistream& operator >> (stdistream& s, ElapsedTime& t)
{
    short hours, minutes;
    char c;

    s >> hours >> c >> minutes;
    t.set (hours, minutes, 0);

    return s;
}

void ElapsedTime::getSystemTime (void)
{
    SYSTEMTIME t;
    GetLocalTime (&t);  
	m_time =  (3600*t.wHour + 60*t.wMinute + t.wSecond) * TimePrecision + t.wMilliseconds/10;
}


//void ElapsedTime::setSystemTime (void) const
//{
//	_SYSTEMTIME t;
//    GetLocalTime (&t);  
//
//    long seconds = m_time / TimePrecision;
//
//    t.wHour = (WORD)(seconds / 3600l);
//    t.wMinute = (WORD)((seconds - (t.wHour * 3600l)) / 60l);
//    t.wSecond =  (WORD)(seconds - (t.wHour * 3600l) - (t.wMinute * 60l));
//    t.wMilliseconds = (WORD)(m_time % 100);
//    SetSystemTime (&t);
//}

ElapsedTime ElapsedTime::Max()
{
	return (std::numeric_limits<long>::max)()/TimePrecision;
}

int ElapsedTime::GetDay(void) const
{
	int nDay = asHours()/24 + 1;

	return nDay;
}

void ElapsedTime::SetDay(int nDay)
{
	m_time += (nDay - GetDay())*24*60*60*TimePrecision;
}

int ElapsedTime::GetHour(void) const
{
	int nHour = asHours()%24;

	return nHour;
}

void ElapsedTime::SetHour(int nHour)
{
	m_time = m_time - GetHour()*60*60*TimePrecision
		            + nHour*60*60*TimePrecision;
}

int ElapsedTime::GetMinute(void) const
{
	int nMinute = asMinutes()%60;

	return nMinute;
}

void ElapsedTime::SetMinute(int nMinute)
{
	m_time = m_time - GetMinute()*60*TimePrecision
		            + nMinute*60*TimePrecision;
}

int ElapsedTime::GetSecond(void) const
{
	int nSecond = asSeconds()%60;

	return nSecond;
}

void ElapsedTime::SetSecond(int nSecond)
{
	m_time = m_time - GetSecond()*TimePrecision + nSecond*TimePrecision;
}

COleDateTime ElapsedTime::ToOleDateTime()
{
	COleDateTime dateTime;
	dateTime.m_dt = ((double)m_time)/(TimePrecision*60*60*24);
	return dateTime;
}