#include "StdAfx.h"
#include ".\airsideflightdelaylog.h"
#include "../Engine/terminal.h"
#include "../Engine/Airside/AirsideFlightDelay.h"

bool CAirsideFlightDelayLog::CreateLogFile( const CString& _csFullFileName)
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeLine();
	m_LogFile.closeOut();
	return true;
}

void CAirsideFlightDelayLog::ClearData()
{
	for(int i=0;i<(int)m_vDelays.size(); ++i )
	{
		delete m_vDelays.at(i);
	}
	m_vDelays.clear();
}

void CAirsideFlightDelayLog::LoadData( const CString& _csFullFileName )
{
	ClearData();
	try{
		m_LogFile.openFile(_csFullFileName, READ);
	}catch(...)
	{
		return ;
	}

	while(!m_LogFile.isBlank())
	{
		AirsideFlightDelay* theDelay = AirsideFlightDelay::ReadLog(m_LogFile);
		if(theDelay)
			m_vDelays.push_back(theDelay);
	}

	m_LogFile.closeIn();
	
}

void CAirsideFlightDelayLog::SaveData( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeField("Delay Log");
	m_LogFile.writeLine();
	for(int i=0;i<(int)m_vDelays.size();++i)
	{
		AirsideFlightDelay* theDelay = m_vDelays.at(i);
		theDelay->WriteLog(m_LogFile);
	}

	m_LogFile.writeLine();
	m_LogFile.closeOut();
}

void CAirsideFlightDelayLog::WriteDelay( AirsideFlightDelay* pDelay )
{
	if(pDelay)
	{
		pDelay->WriteLog(m_LogFile);
	}
}



