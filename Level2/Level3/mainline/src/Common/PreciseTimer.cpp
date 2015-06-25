#include "StdAfx.h"
#include "PreciseTimer.h"


//windows 32 implementation
#include "windows.h"


LARGE_INTEGER GetFrequence()
{
	LARGE_INTEGER litmp;
	::QueryPerformanceFrequency(&litmp);
	return litmp;
}
const LARGE_INTEGER Win32PreciseTimer::dfFrequence = GetFrequence();


//

void Win32PreciseTimer::Start()
{
	if(bRunning)
	{
		return;
	}
	else
	{
		::QueryPerformanceCounter(&m_tStartTime); 
		bRunning = true;
	}	
}

void Win32PreciseTimer::Stop()
{
	if(bRunning)
	{
		LARGE_INTEGER curTime;
		::QueryPerformanceCounter(&curTime);
		m_tElasped.QuadPart += (curTime.QuadPart - m_tStartTime.QuadPart);
		bRunning = false;
	}
}

void Win32PreciseTimer::Reset()
{
	m_tStartTime.QuadPart = 0;
	m_tElasped.QuadPart = 0;
}

double Win32PreciseTimer::getElapsedSeconds() const
{
	LARGE_INTEGER curTime;
	::QueryPerformanceCounter(&curTime);
	return (double)(curTime.QuadPart - m_tStartTime.QuadPart + m_tElasped.QuadPart)/dfFrequence.QuadPart;
}

//////////////////////////////////////////////////////////////////////////
using namespace std;


void TextFileLog::FuncBegin( const CString& funcName )
{
	outfile.open(strFile.GetString(),ios::app);	
	outfile << "function " << funcName.GetString() << "start"<<endl;
	m_time.Reset();
	m_time.Start();
}

void TextFileLog::FuncEnd()
{
	m_time.Stop();
	outfile << "function return , use time " << m_time.getElapsedSeconds() << "s" << endl; 
	outfile.close();
}