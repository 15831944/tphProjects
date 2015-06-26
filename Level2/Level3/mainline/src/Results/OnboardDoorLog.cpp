#include "StdAfx.h"
#include ".\onboarddoorlog.h"

OnboardDoorLog::OnboardDoorLog(void)
{
}

OnboardDoorLog::~OnboardDoorLog(void)
{
}

void OnboardDoorLog::SetFlightID( const CString& sFlightID )
{
	m_sFlightID = sFlightID;
}

const CString& OnboardDoorLog::GetFlightID() const
{
	return m_sFlightID;
}

void OnboardDoorLog::SetDoorID( int nDoorID )
{
	m_nDoorID = nDoorID;
}

int OnboardDoorLog::GetDoorID() const
{
	return m_nDoorID;
}

void OnboardDoorLog::SetStartTime( const ElapsedTime& tTime )
{
	m_tStartTime = tTime;
}

const ElapsedTime& OnboardDoorLog::GetStartTime() const
{
	return m_tStartTime;
}

void OnboardDoorLog::SetEndTime( const ElapsedTime& tTime )
{
	m_tEndTime = tTime;
}

const ElapsedTime& OnboardDoorLog::GetEndTime() const
{
	return m_tEndTime;
}

void OnboardDoorLog::load(ArctermFile& outFile)
{
	long varInt;

	outFile.getInteger(varInt);
	m_tStartTime.setPrecisely(varInt);

	outFile.getInteger(varInt);
	m_tEndTime.setPrecisely(varInt);

	outFile.getInteger(varInt);
	m_nDoorID = varInt;

	outFile.getField(m_sFlightID.GetBuffer(),1024);
	m_sFlightID.ReleaseBuffer();

	outFile.getLine();
}

void OnboardDoorLog::write(ArctermFile& outFile)
{
	outFile.writeInt((long)m_tStartTime);
	outFile.writeInt((long)m_tEndTime);

	outFile.writeInt(m_nDoorID);

	outFile.writeField(m_sFlightID.GetBuffer());
	m_sFlightID.ReleaseBuffer();
	
	outFile.writeLine();
}


OnboardDoorsLog::OnboardDoorsLog( void )
{

}

OnboardDoorsLog::~OnboardDoorsLog( void )
{
	ClearData();
}

void OnboardDoorsLog::writeData( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeField("Door Log");
	m_LogFile.writeLine();
	for(int i=0;i<(int)m_vLogs.size();++i)
	{
		OnboardDoorLog* pLog = m_vLogs.at(i);
		pLog->write(m_LogFile);
	}

	m_LogFile.writeLine();
	m_LogFile.closeOut();
}

void OnboardDoorsLog::readData( const CString& _csFullFileName )
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
		OnboardDoorLog* pLog = new OnboardDoorLog;
		if(pLog)
		{
			pLog->load(m_LogFile);
			m_vLogs.push_back(pLog);
		}
	}

	m_LogFile.closeIn();
}

void OnboardDoorsLog::ClearData()
{
	for(int i=0;i<(int)m_vLogs.size(); ++i )
	{
		delete m_vLogs.at(i);
	}
	m_vLogs.clear();
}

bool OnboardDoorsLog::CreateLogFile( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeLine();
	m_LogFile.closeOut();
	return true;
}

void OnboardDoorsLog::WriteLog( OnboardDoorLog* pLog )
{
	if (pLog)
	{
		pLog->write(m_LogFile);
	}
}

int OnboardDoorsLog::GetItemCount() const
{
	return (int)m_vLogs.size();
}

OnboardDoorLog* OnboardDoorsLog::GetItem( int idx )
{
	if (idx >= 0 && idx < GetItemCount())
	{
		return m_vLogs[idx];
	}
	return NULL;
}

void OnboardDoorsLog::AddItem( OnboardDoorLog* pLog )
{
	if (pLog)
	{
		m_vLogs.push_back(pLog);
	}
}