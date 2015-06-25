#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include "FileOutPut.h"

//class CTimeLogInstance
//{
//public:
//	CTimeLogInstance();
//	typedef CString Key;
//	typedef LONGLONG Val;
//	Val m_tAccTime;
//	int m_nCallTime;
//
//	int m_nLastCalltime;
//	Val m_tLastAdd;
//
//	void ClearCallHistotry(){ m_nLastCalltime = 0; m_tLastAdd = 0; }
//
//	void inline AddCall(const Val& tTime ){ 
//		m_nCallTime++; m_nLastCalltime++; 
//		m_tLastAdd += tTime; m_tAccTime+=tTime; }
//	
//	CString m_strExtraInfo;
//
//	void Flush(std::ofstream& filestream);
//};
//
//
//class CExcuteTimeLog
//{
//public:
//	static CExcuteTimeLog classInstance;	
//	static CExcuteTimeLog& GetInstance();
//	double m_dfFreq;
//
//protected:
//	typedef std::map<CTimeLogInstance::Key, CTimeLogInstance>  TMap;
//	TMap m_tMap;
//	std::ofstream m_filestream;
//
//private:
//	CExcuteTimeLog();
//	~CExcuteTimeLog();
//
//public:
//	void SetExtraInfo(const CTimeLogInstance::Key& theKey, const CString& extraInfo){
//		CTimeLogInstance& theInstance = m_tMap[theKey];
//		theInstance.m_strExtraInfo = extraInfo;
//	}
//	void AddLog(const CTimeLogInstance::Key& theKey, const LARGE_INTEGER& tBegin, const LARGE_INTEGER& tEnd);
//	void FlushLogs(const CString& strLog = _T(""));
//};
//
//class CExcuteTimeLogger
//{
//public:
//	CExcuteTimeLogger(const CTimeLogInstance::Key& thekey, const CString& extraInfo = _T(""));
//	~CExcuteTimeLogger();
//	
//	void GetBeginTime();
//	void GetEndTime();
//	void DoNothing(){}
//protected:
//	CTimeLogInstance::Key m_key;	
//	LARGE_INTEGER m_tBegin;
//	LARGE_INTEGER m_tExit;
//	std::ofstream m_filestream; //detail log
//};
//
//#define BEGIN_LOGTIME(x) {CExcuteTimeLogger timeLogger(x);
//#define BEGIN_LOGTIME_EX(x,y) {CExcuteTimeLogger timeLogger(x,y);
//#define END_LOGTIME  timeLogger.DoNothing();}
//#define FLUSH_TIMELOG(x) CExcuteTimeLog::GetInstance().FlushLogs(x);

class RunTimeDebugLog : public CFileOutput
{
public: 
	RunTimeDebugLog();
	static CString& GetFilePath();
	void LogStack(int nDepth);	
};

#define RUNTIME_DEBUG 0

#if RUNTIME_DEBUG
#define RUNTIMELOG_BEGIN(stack) { RunTimeDebugLog __runtimedebuglog; __runtimedebuglog.LogSourceLine(__FILE__,__LINE__).Line(); __runtimedebuglog.LogStack(stack);
#define RUNTIMELOG(x)  __runtimedebuglog.LogItem(x);
#define RUNTIMELOG_END __runtimedebuglog.Line(); }
#else
#define RUNTIMELOG_BEGIN
#define RUNTIMELOG(x)  
#define RUNTIMELOG_END } 
#endif

#define RUNTIME_LINE(stack) RUNTIMELOG_BEGIN(stack) RUNTIMELOG_END