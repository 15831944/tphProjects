#include "StdAfx.h"
#include ".\landsidevehicledelaylog.h"


const CString LandsideVehicleDelayLog::EnumDelayReasonName[] = 
{
	_T("Traffic Light"),
	_T("Yeild Sign"),
	_T("Forehead Car"),
	_T("Crosswalk"),
	_T("No Delay"),

	_T("Count")

};
const CString LandsideVehicleDelayLog::EnumDelayOperationName[] = 
{
	_T("Stop"),
	_T("SlowDown"),
	_T("Normal"),

	_T("Count")
};




LandsideVehicleDelayLog::LandsideVehicleDelayLog(void)
{
}

LandsideVehicleDelayLog::~LandsideVehicleDelayLog(void)
{
}
bool LandsideVehicleDelayLog::CreateLogFile( const CString& _csFullFileName)
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeLine();
	m_LogFile.closeOut();
	return true;
}

void LandsideVehicleDelayLog::ClearData()
{
	for(int i=0;i<(int)m_vDelays.size(); ++i )
	{
		delete m_vDelays.at(i);
	}
	m_vDelays.clear();
}

void LandsideVehicleDelayLog::LoadData( const CString& _csFullFileName )
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
		LandsideVehicleDelayItem* theDelay = LandsideVehicleDelayItem::ReadLog(m_LogFile);
		if(theDelay)
			m_vDelays.push_back(theDelay);
	}

	m_LogFile.closeIn();

}

void LandsideVehicleDelayLog::SaveData( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeField("Delay Log");
	m_LogFile.writeLine();
	for(int i=0;i<(int)m_vDelays.size();++i)
	{
		LandsideVehicleDelayItem* theDelay = m_vDelays.at(i);
		theDelay->WriteLog(m_LogFile);
	}

	m_LogFile.writeLine();
	m_LogFile.closeOut();
}

void LandsideVehicleDelayLog::WriteDelay( LandsideVehicleDelayItem* pDelay )
{
	if(pDelay)
	{
		pDelay->WriteLog(m_LogFile);
	}
}



//////////////////////////////////////////////////////////////////////////
//LandsideVehicleDelayItem
LandsideVehicleDelayItem::LandsideVehicleDelayItem()
{
	//vehicle
	m_nVehicleID = -1;
	m_nVehicleType = -1;

	//Resource
	m_nResourceID = -1;

	//Reason
	m_enumReason = LandsideVehicleDelayLog::DR_TrafficLight;

	//Operation
	m_enumOperation = LandsideVehicleDelayLog::DO_Stop;

}

LandsideVehicleDelayItem::~LandsideVehicleDelayItem()
{

}
////////vehicle
//////int m_nVehicleID;
//////int m_nVehicleType;
//////
////////delay at the time start and end
//////ElapsedTime m_eStartTime;
//////ElapsedTime m_eEndTime;
//////
//////
////////the time delayed
//////ElapsedTime m_eDelayTime;
//////
////////Resource
//////int m_nResourceID;
//////CString m_strResName;
//////
////////Reason
//////EnumDelayReason m_enumReason;
//////
////////Operation
//////EnumDelayOperation m_enumOperation;
//////
////////Description
//////CString m_strDesc;
LandsideVehicleDelayItem* LandsideVehicleDelayItem::ReadLog( ArctermFile& inFile )
{
	LandsideVehicleDelayItem *pItem = new LandsideVehicleDelayItem;
	ASSERT(pItem != NULL);
	
	inFile.getInteger(pItem->m_nVehicleID);
	inFile.getInteger(pItem->m_nVehicleType);

	inFile.getTime(pItem->m_eStartTime);
	inFile.getTime(pItem->m_eEndTime);
	
	long ldTime = 0;
	inFile.getInteger(ldTime );
	pItem->m_eDelayTime.setPrecisely(ldTime);

	inFile.getInteger(pItem->m_nResourceID);
	inFile.getField(pItem->m_strResName.GetBuffer(1024),1024);
	pItem->m_strResName.ReleaseBuffer();


	int nReason = 0;
	inFile.getInteger(nReason);
	if(nReason >= LandsideVehicleDelayLog::DR_TrafficLight && nReason < LandsideVehicleDelayLog::DR_Count)
		pItem->m_enumReason = (LandsideVehicleDelayLog::EnumDelayReason)nReason;

	int nOperation = 0;
	inFile.getInteger(nOperation);
	if(nOperation >= LandsideVehicleDelayLog::DO_Stop && nOperation < LandsideVehicleDelayLog::DO_Count)
		pItem->m_enumOperation = (LandsideVehicleDelayLog::EnumDelayOperation)nOperation;


	inFile.getField(pItem->m_strDesc.GetBuffer(1024),1024);
	pItem->m_strDesc.ReleaseBuffer();

	inFile.getLine();
	
	return pItem;
}

void LandsideVehicleDelayItem::WriteLog( ArctermFile& outFile )
{

	outFile.writeInt(m_nVehicleID);
	outFile.writeInt(m_nVehicleType);

	outFile.writeTime(m_eStartTime,TRUE);
	outFile.writeTime(m_eEndTime,TRUE);

	outFile.writeInt(m_eDelayTime.getPrecisely());


	outFile.writeInt(m_nResourceID);
	outFile.writeField(m_strResName);
	

	outFile.writeInt(m_enumReason);

	outFile.writeInt(m_enumOperation);

	outFile.writeField(m_strDesc);

	outFile.writeLine();

}


















