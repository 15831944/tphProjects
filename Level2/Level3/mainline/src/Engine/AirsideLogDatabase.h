#pragma once

#include "..\Common\AIRSIDE_BIN.h"
//log type
enum LogType
{
	LogType_FlightLog = 0,
	UnknownLog
};
class CAirsideLogDatabase
{
public:
	CAirsideLogDatabase(void);
	~CAirsideLogDatabase(void);
public:
	/************************************************************************
	FUNCTION: Write log description to database
	IN		: nProjID, the id the project
			  pLogDesc,the data need to insert into database
			  LogType,log type,see enum logtype
    RETURN	:if insert successful ,return true ;otherwise return false
	/************************************************************************/
	bool WriteLogDesc(int nProjID,void* pLogDesc,  LogType  logType);
	/************************************************************************
	FUNCTION: Write log Event to database
	IN		: nProjID, the id of the project
			  pLogDesc,the description data of the event
			  pLog,the event need to insert into database
			  LogType,log type,see enum logtype
    RETURN	:if insert successful ,return true ;otherwise return false
	/************************************************************************/
	bool WriteLog(int nProjID,void* pLogDesc,  void* pLog,  LogType  logType);
	/************************************************************************
	FUNCTION: Delete log description from database
	IN		: nProjID, the id of the project
			  LogType,log type,see enum logtype
    RETURN	:if delete successful ,return true ;otherwise return false
	/************************************************************************/
	bool DeleteLogDesc(int nProjID,  LogType  logType);
	/************************************************************************
	FUNCTION: Delete log description from database
	IN		: nProjID, the id of the project
			  LogType,log type,see enum logtype
    RETURN	:if delete successful ,return true ;otherwise return false
	/************************************************************************/
	bool DeleteLog(int nProjID,  LogType  logType);
private:
	/************************************************************************
	FUNCTION: get the table name of the log description
	IN		: LogType,log type,see enum logtype
    OUT		:lpszName,the name of the table
	RETURN	:if get the name successful ,return true ;otherwise return false
	/************************************************************************/
	bool GetDescTableName(LogType logType, char* lpszName);
	/************************************************************************
	FUNCTION: get the table name of the log Event
	IN		: LogType,log type,see enum logtype
    OUT		:lpszName,the name of the table
	RETURN	:if get the name successful ,return true ;otherwise return false
	/************************************************************************/
	bool GetEventTableName(LogType logType, char* lpszName);
	/************************************************************************
	FUNCTION: execute SQL statement
	IN		: strSQL,sql statement
    OUT		:
	RETURN	:if execute the statement successful ,return true ;otherwise return false
	/************************************************************************/
	bool ExecuteSQL(char* strSQL);

};
