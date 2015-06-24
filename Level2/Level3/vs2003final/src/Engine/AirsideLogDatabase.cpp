#include "StdAfx.h"
#include ".\airsidelogdatabase.h"
#include "../Database/ARCportDatabaseConnection.h"
using namespace ADODB;
CAirsideLogDatabase::CAirsideLogDatabase(void)
{
}
CAirsideLogDatabase::~CAirsideLogDatabase(void)
{
}

/************************************************************************
FUNCTION: Write log description to database
IN		: nProjID, the id the project
pLogDesc,the data need to insert into database
LogType,log type,see enum logtype
RETURN	:if insert successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::WriteLogDesc(int nProjID,void* pLogDesc,  LogType  logType)
{
	if(pLogDesc == NULL)
		return FALSE;

	char lpszText[1024] = {0};
	char tablename[100] = {0};
	GetDescTableName(logType, tablename);

	switch(logType)
	{	 
	case LogType_FlightLog:
		{
			AirsideFlightDescStruct desc;		
			memcpy((void*)&desc, pLogDesc, sizeof(AirsideFlightDescStruct));
			sprintf(lpszText, 
				"INSERT INTO %s(ID, ICAOCode, FlightID, ArrID, DepID, \
				ACType, Origin, Dest, StartTime, EndTime, StartPos, EndPos, \
				ACCategory, WeightCat, SpeedCat, AirlineNO, UserCode,ProjID) \
				VALUES(%d , '%s', '%s', '%s', '%s', '%s', '%s', '%s', \
				%d, %d, %d, %d, '%s', '%s', '%s', %d, %d, %d)", tablename,
				desc.id, desc.icao_code, desc.flightID, desc.arrID, desc.depID, 
				desc.acType, desc.origin, desc.dest, desc.startTime, desc.endTime,
				desc.startPos, desc.endPos, desc.acCategory, desc.weightCat, 
				desc.speedCat, desc.airlineNO, desc.userCode,nProjID);
		}
		break;
	case UnknownLog:
		return false;
	}

	if(!ExecuteSQL(lpszText))
		return false;

	return true;
}
/************************************************************************
FUNCTION: Write log Event to database
IN		: nProjID, the id of the project
pLogDesc,the description data of the event
pLog,the event need to insert into database
LogType,log type,see enum logtype
RETURN	:if insert successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::WriteLog(int nProjID,void* pLogDesc,  void* pLog,  LogType  logType)
{
	if(pLogDesc == NULL || pLog == NULL)
		return false;

	char lpszText[1024] = {0};
	char tablename[100] = {0};
	GetEventTableName(logType, tablename);
	switch(logType)
	{	
	case LogType_FlightLog:
		{
			AirsideFlightDescStruct desc;
			memcpy((void*)&desc, pLogDesc, sizeof(AirsideFlightDescStruct));
			AirsideFlightEventStruct evt;
			memcpy((void*)&evt, pLog, sizeof(AirsideFlightEventStruct));
			if( '\0' == evt.mode )
			{
				evt.mode = 'x';
			}


			sprintf(lpszText, 
				"INSERT INTO %s(DescID, ETime, EState, ENodeNO, EPosX, EPosY, EPosZ, \
				ECode, ERunwayNO, EMode, ESegmentNO, EGateNO, EDelayTime, EInterval, EIsBackUp, ProjID) \
				VALUES(%d, %d, \'%c\', %d, %f, %f, %f, \'%c\', %d, \'%c\', %d, %d, %d, %d, %d, %d)", tablename,
				desc.id, evt.time, evt.state, evt.nodeNO, evt.x, evt.y, evt.z,
				evt.eventCode,	evt.runwayNO, evt.mode, evt.segmentNO, evt.gateNO, 
				evt.delayTime, evt.interval, evt.IsBackUp, nProjID);
		}
		break;
	case UnknownLog:
		return false;
	}

	if(!ExecuteSQL(lpszText))
		return false;

	return true;
}
/************************************************************************
FUNCTION: Delete log description from database
IN		: nProjID, the id of the project
LogType,log type,see enum logtype
RETURN	:if delete successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::DeleteLogDesc(int nProjID,  LogType  logType)
{
	char lpszText[1024] = {0};
	char tablename[100] = {0};
	GetDescTableName(logType, tablename);

	switch(logType)
	{	 
	case LogType_FlightLog:
		{
			sprintf(lpszText, "DELETE FROM %s WHERE PROJID = %d",tablename,nProjID);
		}
		break;
	case UnknownLog:
		return false;
	}
	if(!ExecuteSQL(lpszText))
		return false;

	return true;
}
/************************************************************************
FUNCTION: Delete log description from database
IN		: nProjID, the id of the project
LogType,log type,see enum logtype
RETURN	:if delete successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::DeleteLog(int nProjID,  LogType  logType)
{
	char lpszText[1024] = {0};
	char tablename[100] = {0};
	GetEventTableName(logType, tablename);
	switch(logType)
	{	
	case LogType_FlightLog:
		{
			sprintf(lpszText, "DELETE FROM %s WHERE PROJID = %d",tablename,nProjID);
		}
		break;
	case UnknownLog:
		return false;
	}

	if(!ExecuteSQL(lpszText))
		return false;

	return true;
}
/************************************************************************
FUNCTION: get the table name of the log description
IN		: LogType,log type,see enum logtype
OUT		:lpszName,the name of the table
RETURN	:if get the name successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::GetDescTableName(LogType logType, char* lpszName)
{
	lpszName[0] = 0;
	if(lpszName == NULL)
		return false;
	switch(logType)
	{	
	case LogType_FlightLog:
		strcpy(lpszName, "FlightDesc");
		break;
	case UnknownLog:
		return false;
	}
	return true;
}
/************************************************************************
FUNCTION: get the table name of the log Event
IN		: LogType,log type,see enum logtype
OUT		:lpszName,the name of the table
RETURN	:if get the name successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::GetEventTableName(LogType logType, char* lpszName)
{
	if(lpszName == NULL)
		return false;
	switch(logType)
	{	
	case LogType_FlightLog:
		strcpy(lpszName, "FlightEvent");
		break;
	case UnknownLog:
		return false;
	}
	return true;
}
/************************************************************************
FUNCTION: execute SQL statement
IN		: strSQL,sql statement
OUT		:
RETURN	:if execute the statement successful ,return true ;otherwise return false
/************************************************************************/
bool CAirsideLogDatabase::ExecuteSQL(char* strSQL)
{
	if( strSQL == "")
		return false;
	try
	{
		DATABASECONNECTION.GetConnection()->Execute(_bstr_t(strSQL), NULL, adCmdText);
		return true;
	}
	catch( _com_error &e)
	{
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());
	}
	return false;
}