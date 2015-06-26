#include "StdAfx.h"
#include ".\airsiderunwayoperationlog.h"


//////////////////////////////////////////////////////////////////////////
//AirsideRunwayOperationLog
AirsideRunwayOperationLog::RunwayLogItem::RunwayLogItem()
{
	//time
	m_eTime = ElapsedTime(0L);
	//runway ID in input
	m_nRunwayID = -1;

	//runway mark index, first or
	//	RUNWAYMARK_FIRST = 0,
	//	RUNWAYMARK_SECOND,
	m_enumRunwayMark = RUNWAYMARK_FIRST;

	//runway mark name
	m_strMarkName = _T("");

	//flight index
	m_nFlightIndex = -1;

	//flight ID
	m_strFlightID = _T("");

	//flight type
	m_strFlightType = _T("");

	//runway operation
	m_enumOperation = enumOperation_Landing;

	//entry or exit runway
	m_enumEntryOrExit = enumEntryOrExit_Entry;

	//possible exit ID, name
	m_nPossibleExitID = -1;;
	m_strPossibleExitName = _T("");

	//possible exit time, it is minimum occupy time using max breaking 
	m_ePossibleTime = ElapsedTime(0L);


}

AirsideRunwayOperationLog::RunwayLogItem::~RunwayLogItem()
{

}

void AirsideRunwayOperationLog::RunwayLogItem::WriteLog( ArctermFile& outFile )
{
	//time
	outFile.writeTime(m_eTime,TRUE);

	//runway id
	outFile.writeInt(m_nRunwayID);
	outFile.writeInt((int)m_enumRunwayMark);
	outFile.writeField(m_strMarkName);

	//entry or exit
	outFile.writeInt((int)m_enumEntryOrExit);

	//operation
	outFile.writeInt((int)m_enumOperation);// landing,takeoff or approach

	//flight
	outFile.writeInt(m_nFlightIndex);
	outFile.writeField(m_strFlightID);
	outFile.writeField(m_strFlightType);//ac type
	
	outFile.writeInt(m_nPossibleExitID);
	outFile.writeField(m_strPossibleExitName);

	outFile.writeTime(m_ePossibleTime,TRUE);

	//airroute
	outFile.writeInt((int)m_vAirRoute.size());
	
	for (int nAirRoute = 0; nAirRoute < (int)m_vAirRoute.size(); ++nAirRoute)
	{
		outFile.writeInt(m_vAirRoute[nAirRoute].m_nRouteID);
		outFile.writeField(m_vAirRoute[nAirRoute].m_strRouteName);
	}

	outFile.writeLine();


}

AirsideRunwayOperationLog::RunwayLogItem* AirsideRunwayOperationLog::RunwayLogItem::ReadLog( ArctermFile& inFile )
{
	AirsideRunwayOperationLog::RunwayLogItem *pItem = new AirsideRunwayOperationLog::RunwayLogItem();
	
	//time
	ElapsedTime eTime;
	inFile.getTime(eTime,TRUE);
	pItem->SetTime(eTime);

	//runway
	int nRunwayID = -1;
	RUNWAY_MARK runwayMark = RUNWAYMARK_FIRST;
	CString strMarkName = _T("");
	inFile.getInteger(nRunwayID);
	int nMark = 0;
	inFile.getInteger(nMark);
	if(nMark == RUNWAYMARK_SECOND)
		runwayMark = RUNWAYMARK_SECOND;

	inFile.getField(strMarkName.GetBuffer(1024),1024);
	strMarkName.ReleaseBuffer();

	pItem->SetRunway(nRunwayID,runwayMark,strMarkName);

	//entry or exit
	int nEntryOrExit = 0;
	inFile.getInteger(nEntryOrExit);

	if(nEntryOrExit == enumEntryOrExit_Entry)
		pItem->SetEnumEntryOrExit(enumEntryOrExit_Entry);
	else
		pItem->SetEnumEntryOrExit(enumEntryOrExit_Exit);

	//operation
	pItem->SetEnumOperation(enumOperation_Landing);
	int nOperation = 0;
	inFile.getInteger(nOperation);
	if(nOperation >= enumOperation_Landing && nOperation < enumOperation_Cross)
	{
		pItem->SetEnumOperation((enumOperation)nOperation);
	}

	//flight
	int nFlightIndex = -1;
	CString strFlightID;
	CString strACType;
	inFile.getInteger(nFlightIndex);
	inFile.getField(strFlightID.GetBuffer(1024),1024);
	strFlightID.ReleaseBuffer();

	inFile.getField(strACType.GetBuffer(1024),1024);
	strACType.ReleaseBuffer();

	pItem->SetFlightType(strACType);
	pItem->SetFlightID(strFlightID);
	pItem->SetFlightIndex(nFlightIndex);
	
	//possible exit

	int nPExitID = -1;
	CString nPExitName = _T("");
	inFile.getInteger(nPExitID);
	inFile.getField(nPExitName.GetBuffer(1024),1024);
	nPExitName.ReleaseBuffer();
	
	ElapsedTime ePossibleExitTime;
	inFile.getTime(ePossibleExitTime,TRUE);

	pItem->SetPossibleExitInformation(nPExitID, nPExitName, ePossibleExitTime);

	int nAirRouteCount;
	inFile.getInteger(nAirRouteCount);
	for (int nAirRoute = 0; nAirRoute < nAirRouteCount; ++nAirRoute)
	{
		RunwayLogAirRoute logAirRoute;
		inFile.getInteger(logAirRoute.m_nRouteID);
		inFile.getField(logAirRoute.m_strRouteName.GetBuffer(1024),1024);
		logAirRoute.m_strRouteName.ReleaseBuffer();
		pItem->AddAirRoute(logAirRoute);
	}

	
	
	
	inFile.getLine();





	return pItem;
}

void AirsideRunwayOperationLog::RunwayLogItem::SetPossibleExitInformation( int nExitID, const CString& strExitName, ElapsedTime eExitTime )
{
	m_nPossibleExitID = nExitID;
	m_strPossibleExitName = strExitName;
	m_ePossibleTime = eExitTime;
}

void AirsideRunwayOperationLog::RunwayLogItem::AddAirRoute( const RunwayLogAirRoute& airRoute )
{
	m_vAirRoute.push_back(airRoute);
}

CString AirsideRunwayOperationLog::RunwayLogItem::GetAirRouteNames()
{
	CString strName;
	for (int nAirRoute = 0; nAirRoute < (int)m_vAirRoute.size(); ++nAirRoute)
	{
		strName += m_vAirRoute[nAirRoute].m_strRouteName;
	}
	return strName;
}






//////////////////////////////////////////////////////////////////////////
//AirsideRunwayOperationLog
AirsideRunwayOperationLog::AirsideRunwayOperationLog(void)
{
}

AirsideRunwayOperationLog::~AirsideRunwayOperationLog(void)
{
}


bool AirsideRunwayOperationLog::CreateLogFile( const CString& _csFullFileName)
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeLine();
	m_LogFile.closeOut();
	return true;
}

void AirsideRunwayOperationLog::ClearData()
{
	for(int i=0;i<(int)m_vLogItem.size(); ++i )
	{
		delete m_vLogItem.at(i);
	}
	m_vLogItem.clear();
}


void AirsideRunwayOperationLog::LoadData( const CString& _csFullFileName )
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
		RunwayLogItem* theItem = RunwayLogItem::ReadLog(m_LogFile);
		if(theItem)
			m_vLogItem.push_back(theItem);
	}

	m_LogFile.closeIn();

}

void AirsideRunwayOperationLog::SaveData( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeField("Delay Log");
	m_LogFile.writeLine();
	for(int i=0;i<(int)m_vLogItem.size();++i)
	{
		RunwayLogItem* theItem = m_vLogItem.at(i);
		theItem->WriteLog(m_LogFile);
	}

	m_LogFile.writeLine();
	m_LogFile.closeOut();
}

void AirsideRunwayOperationLog::WriteLogItem( RunwayLogItem* pDelay )
{
	if(pDelay)
	{
		pDelay->WriteLog(m_LogFile);
	}
}

AirsideRunwayOperationLog::RunwayLogItem* AirsideRunwayOperationLog::GetItem( int idx )
{
	return m_vLogItem.at(idx);
}

void AirsideRunwayOperationLog::AddItem( RunwayLogItem* pItem )
{
	//find the aircraft last log
	std::vector<RunwayLogItem*>::reverse_iterator iter = m_vLogItem.rbegin();
	for(; iter!=m_vLogItem.rend();iter++)
	{
		if((*iter)->m_nFlightIndex == pItem->m_nFlightIndex && 
			(*iter)->m_enumEntryOrExit == pItem->m_enumEntryOrExit &&
			(*iter)->m_enumOperation == pItem->m_enumOperation)
		{

			delete pItem;
			return;
		}
	}
	m_vLogItem.push_back(pItem);
}

int AirsideRunwayOperationLog::GetItemCount() const
{
	return (int)m_vLogItem.size();
}

