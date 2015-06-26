#include "StdAfx.h"
#include ".\airsideflightstairlog.h"
#include "../Engine/Airside/AirsideFlightStairsLog.h"

AirsideFlightStairLogs::AirsideFlightStairLogs(void)
{
}

AirsideFlightStairLogs::~AirsideFlightStairLogs(void)
{
	ClearData();
}

void AirsideFlightStairLogs::readData( const CString& _csFullFileName )
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
		AirsideFlightStairsLog* pLog = new AirsideFlightStairsLog;
		if(pLog)
		{
			pLog->ReadLog(m_LogFile);
			m_vLogs.push_back(pLog);
		}
	}

	m_LogFile.closeIn();

}

void AirsideFlightStairLogs::writeData( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeField("Stair Log");
	m_LogFile.writeLine();
	for(int i=0;i<(int)m_vLogs.size();++i)
	{
		AirsideFlightStairsLog* pLog = m_vLogs.at(i);
		pLog->WriteLog(m_LogFile);
	}

	m_LogFile.writeLine();
	m_LogFile.closeOut();
}

void AirsideFlightStairLogs::ClearData()
{
	for(int i=0;i<(int)m_vLogs.size(); ++i )
	{
		delete m_vLogs.at(i);
	}
	m_vLogs.clear();
}

bool AirsideFlightStairLogs::CreateLogFile( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeLine();
	m_LogFile.closeOut();
	return true;
}

void AirsideFlightStairLogs::WriteLog( AirsideFlightStairsLog* pLog )
{
	if (pLog)
	{
		pLog->WriteLog(m_LogFile);
	}
}

int AirsideFlightStairLogs::GetItemCount() const
{
	return (int)m_vLogs.size();
}

AirsideFlightStairsLog* AirsideFlightStairLogs::GetItem( int idx )
{
	return m_vLogs.at(idx);
}

void AirsideFlightStairLogs::AddItem( AirsideFlightStairsLog* pLog )
{
	//int nCount = m_vLogs.size();
	//for (int i =0; i < nCount; i++)
	//{
	//	AirsideFlightStairsLog* pExistLog = m_vLogs.at(i);
	//	if (pExistLog->m_tEndTime < 0L && pExistLog->m_nFlightId == pLog->m_nFlightId
	//		pExistLog->m_cFltStatus == pLog->m_cFltStatus && pExistLog->m_nStairId == pLog->m_nStairId)
	//	{
	//		pExistLog->m_tEndTime = pLog->m_tStartTime;

	//		delete pLog;
	//		pLog = NULL;

	//		return;
	//	}
	//}
	m_vLogs.push_back(pLog);
}