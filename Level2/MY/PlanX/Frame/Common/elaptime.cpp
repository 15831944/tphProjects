 #include "elaptime.h"

void ElapsedTime::PrintTime(char *str, bool printSeconds) const
{}
// 	if(str == 0) return;
//     long seconds = m_seconds;
// 	if(m_seconds<0) seconds= -m_seconds;
// 
//     int hours = seconds / 3600l;
//     int min = (seconds%3600l) / 60l;
//     int sec = seconds%60l;
// 
// 	if(m_seconds<0)
// 	{
// 		char strHour[4];
// 		_itoa(hours,strHour,10);
// 		str[0] = '-';
// 		strcat(str,strHour);
// 	}
// 	else
// 	{
// 		_itoa (hours, str, 10);
// 	}
//     strcat(str, ":");
//     if (min < 10 && min>=0)
//         strcat (str, "0");
//     _itoa(min, str + strlen(str), 10);
// 
//     if (printSeconds)
//     {
//         strcat (str, ":");
//         if (sec < 10)
//             strcat (str, "0");
//         _itoa (sec, str + strlen(str), 10);
//     }
// }
// 
bool ElapsedTime::SetByString( const char* _strTime )
{
	return false;
}

// 	const int nStrLen = 12;
// 	char strTime[nStrLen];
// 	bool bMinus = false;
// 	// "-"  --> minus
// 	if( _strTime[0] == '-' )
// 	{
// 		strcpy( strTime, _strTime+1 );
// 		bMinus = true;
// 	}
// 	else
// 	{
// 		strcpy( strTime, _strTime );
// 	}
// 
// 	// count ":"
// 	int nIdx1 = -1;
// 	int nIdx2 = -1;
// 	for( int i=0; i<nStrLen; i++ )
// 	{
// 		if( strTime[i] == ':' )
// 		{
// 			if( nIdx1 == -1 )
// 				nIdx1 = i;
// 			else if( nIdx2 == -1 )
// 				nIdx2 = i;
// 		}
// 	}
// 
// 	// If got two ":",  hh:mm:ss
// 	if( nIdx2 != -1 )
// 	{
// 		int nSeconds = atoi( strTime + nIdx2 + 1 );
// 		strTime[nIdx2] = 0;
// 		int nMinutes = atoi( strTime + nIdx1 + 1 );
// 		strTime[nIdx1] = 0;
// 		int nHours = atoi( strTime );
// 		SetByHourMinuteSecond( nHours, nMinutes, nSeconds );
// 		if( bMinus )
// 			m_seconds = -m_seconds;
// 		return true;
// 	}
// 
// 	// else if got one ":",  hh:mm / h:m
// 	if( nIdx1 != -1 )
// 	{
// 		int nMinutes = atoi( strTime + nIdx1 + 1 );
// 		strTime[nIdx1] = 0;
// 		int nHours = atoi( strTime );
// 		SetByHourMinuteSecond( nHours, nMinutes, 0 );
// 		if( bMinus )
// 			m_seconds = -m_seconds;
// 		return true;
// 	}
// 
// 
// 	// else if length is 6:   hhmmss
// 	if( strlen( strTime) == 6 )
// 	{
// 		strTime[6] = 0;
// 		int nSeconds = atoi( strTime + 4 );
// 		strTime[4] = 0;
// 		int nMinutes = atoi( strTime + 2 );
// 		strTime[2] = 0;
// 		int nHours = atoi( strTime );
// 		SetByHourMinuteSecond( nHours, nMinutes, nSeconds );
// 		if( bMinus )
// 			m_seconds = -m_seconds;
// 		return true;
// 	}
// 
// 	// else if length is 4:   hhmm
// 	if( strlen( strTime) == 4 )
// 	{
// 		strTime[4] = 0;
// 		int nMinutes = atoi( strTime + 2 );
// 		strTime[2] = 0;
// 		int nHours = atoi( strTime );
// 		SetByHourMinuteSecond( nHours, nMinutes, 0 );
// 		if( bMinus )
// 			m_seconds = -m_seconds;
// 		return true;
// 	}
// 
// 	// else if length is 3:hhmm
// 	if( strlen( strTime) == 3 )
// 	{
// 		strTime[3] = 0;
// 		int nMinutes = atoi( strTime + 1 );
// 		strTime[1] = 0;
// 		int nHours = atoi( strTime );
// 		SetByHourMinuteSecond( nHours, nMinutes, 0 );
// 		if( bMinus )
// 			m_seconds = -m_seconds;
// 		return true;
// 	}
// 
// 	// else wrong format, return false;
// 	return false;
// 		
// }
// 
int main()
{
	float xxx = -1.5f;
	long xxx2 = xxx;
	ElapsedTime time(100.01f);
	ElapsedTime time2; 
	time2 = time++;
	short milli = time.GetMilliSecond();
	long second = time.GetAsSecond();
	return 0;
}