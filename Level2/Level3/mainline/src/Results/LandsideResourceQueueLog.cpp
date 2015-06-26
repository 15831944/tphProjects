#include "StdAfx.h"
#include ".\landsideresourcequeuelog.h"

LandsideResourceQueueLog::LandsideResourceQueueLog(void)
{
}

LandsideResourceQueueLog::~LandsideResourceQueueLog(void)
{
}
bool LandsideResourceQueueLog::CreateLogFile( const CString& _csFullFileName)
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeLine();
	m_LogFile.closeOut();
	return true;
}

void LandsideResourceQueueLog::ClearData()
{
	for(int i=0;i<(int)m_vItems.size(); ++i )
	{
		delete m_vItems.at(i);
	}
	m_vItems.clear();
}

void LandsideResourceQueueLog::LoadData( const CString& _csFullFileName )
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
		LandsideResourceQueueItem* theItem = LandsideResourceQueueItem::ReadLog(m_LogFile);
		if(theItem)
			m_vItems.push_back(theItem);
	}

	m_LogFile.closeIn();

}

void LandsideResourceQueueLog::SaveData( const CString& _csFullFileName )
{
	m_LogFile.openFile(_csFullFileName, WRITE);
	m_LogFile.writeField("Landside Resource Queue Log");
	m_LogFile.writeLine();
	for(int i=0;i<(int)m_vItems.size();++i)
	{
		LandsideResourceQueueItem* theDelay = m_vItems.at(i);
		theDelay->WriteLog(m_LogFile);
	}

	m_LogFile.writeLine();
	m_LogFile.closeOut();
}

void LandsideResourceQueueLog::Write( LandsideResourceQueueItem* pDelay )
{
	if(pDelay)
	{
		pDelay->WriteLog(m_LogFile);
	}
}




//////////////////////////////////////////////////////////////////////////
//LandsideResourceQueueItem
LandsideResourceQueueItem::LandsideResourceQueueItem()
{
	//vehicle
	m_nVehicleID = -1;
//	m_nVehicleType = -1;

	//Resource
	m_nResourceID = -1;

	//Operation
	m_enumOperation = QO_Entry;
}

LandsideResourceQueueItem::~LandsideResourceQueueItem()
{

}

LandsideResourceQueueItem* LandsideResourceQueueItem::ReadLog( ArctermFile& inFile )
{
	LandsideResourceQueueItem *pItem = new LandsideResourceQueueItem;
	ASSERT(pItem != NULL);

	inFile.getInteger(pItem->m_nResourceID);
	inFile.getField(pItem->m_strResName.GetBuffer(1024),1024);
	pItem->m_strResName.ReleaseBuffer();

	inFile.getInteger(pItem->m_nVehicleID);
//	inFile.getInteger(pItem->m_nVehicleType);
	inFile.getField(pItem->m_strVehicleType.GetBuffer(1024),1024);
	pItem->m_strVehicleType.ReleaseBuffer();

	inFile.getTime(pItem->m_eTime);



	int nOperation = 0;
	inFile.getInteger(nOperation);
	if(nOperation >= QO_Entry && nOperation < QO_Count)
		pItem->m_enumOperation = (QueueOperation)nOperation;


	inFile.getLine();

	return pItem;
}

void LandsideResourceQueueItem::WriteLog( ArctermFile& outFile )
{

	outFile.writeInt(m_nResourceID);
	outFile.writeField(m_strResName);

	outFile.writeInt(m_nVehicleID);
//	outFile.writeInt(m_nVehicleType);
	outFile.writeField(m_strVehicleType);

	outFile.writeTime(m_eTime,TRUE);
	outFile.writeInt(m_enumOperation);

	outFile.writeLine();
}



















