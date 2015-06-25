#include "StdAfx.h"
#include "FileOutPut.h"
#include <afxpriv.h>
#include "StackWalker.h"
#include "MyStackWalker.h"
#include "RunTimeDebug.h"
//
//CExcuteTimeLog CExcuteTimeLog::classInstance;
//
//CTimeLogInstance::CTimeLogInstance()
//{
//	m_tAccTime = 0;
//	m_tLastAdd = 0;
//	m_nLastCalltime =0;
//	m_nCallTime = 0;
//}
//
//void CTimeLogInstance::Flush( std::ofstream& filestream )
//{
//	//if(nCalltime == 0 )
//	//	return;
//
//	double dfFreq = CExcuteTimeLog::GetInstance().m_dfFreq;
//
//	double dfTime   =  (double)m_tAccTime /dfFreq;
//	double dAddTime = (double) m_tLastAdd /dfFreq;
//
//	filestream <<"("<< m_nLastCalltime << ", "<<dAddTime<< "s), ";
//	filestream <<"("<< m_nCallTime << "," << dfTime << "s),";
//	filestream << m_strExtraInfo << ", ";
//
//	ClearCallHistotry();
//}
//
//CExcuteTimeLog::CExcuteTimeLog()
//{
//	LARGE_INTEGER litmp;
//	QueryPerformanceFrequency(&litmp);
//	m_dfFreq = (double) litmp.QuadPart;
//
//	m_filestream.open("C:\\ArcportDebug\\RunTime.txt");
//	
//}
//
//CExcuteTimeLog::~CExcuteTimeLog()
//{	
//	FlushLogs();
//	m_filestream.close();
//}
//
//void CExcuteTimeLog::AddLog( const CTimeLogInstance::Key& theKey, const LARGE_INTEGER& tBegin, const LARGE_INTEGER& tEnd)
//{
//	CTimeLogInstance& theInstance = m_tMap[theKey];
//	theInstance.AddCall(  tEnd.QuadPart - tBegin.QuadPart);
//}
//
//
//void CExcuteTimeLog::FlushLogs(const CString& strLog)
//{
//	m_filestream<<std::endl;
//	m_filestream << strLog << std::endl;
//	for(TMap::iterator itr = m_tMap.begin();itr!= m_tMap.end();itr++)
//	{
//		CTimeLogInstance& theInstance = itr->second;
//		CTimeLogInstance::Key theKey = itr->first;
//		if(theInstance.m_nLastCalltime == 0)
//			continue;
//
//		m_filestream<<theKey<<": ";
//		theInstance.Flush(m_filestream);
//		m_filestream<<std::endl;
//	}
//	m_filestream.flush();
//}
//
//CExcuteTimeLog& CExcuteTimeLog::GetInstance()
//{
//	return classInstance;
//}
//
//CExcuteTimeLogger::CExcuteTimeLogger( const CTimeLogInstance::Key& thekey, const CString& extraInfo /*= _T("")*/ )
//{	
//	GetBeginTime();
//	m_key = thekey;
//	CExcuteTimeLog::GetInstance().SetExtraInfo(m_key,extraInfo);	
//}
//
//CExcuteTimeLogger::~CExcuteTimeLogger()
//{
//	GetEndTime();	
//	CExcuteTimeLog::GetInstance().AddLog(m_key, m_tBegin, m_tExit);
//	m_filestream.close();
//}
//
//void CExcuteTimeLogger::GetBeginTime()
//{
//	QueryPerformanceCounter(&m_tBegin);	
//}
//
//void CExcuteTimeLogger::GetEndTime()
//{
//	QueryPerformanceCounter(&m_tExit);
//}



static CString debugfilePath;

MyStackWalker stackwalker;


RunTimeDebugLog::RunTimeDebugLog()
	:CFileOutput(GetFilePath())
{

}
#ifdef _DEBUG
#define  LOG_FILE _T("runtimeD.log")
#else
#define	LOG_FILE _T("runtimeR.log")
#endif

CString& RunTimeDebugLog::GetFilePath()
{
	if(debugfilePath.IsEmpty())
	{
		CString modulePath;
		AfxGetModuleFileName(NULL, modulePath);
		debugfilePath = modulePath.Left(modulePath.ReverseFind('\\')+1) + LOG_FILE;
		CFileOutput::Clear(debugfilePath);
	}
	return debugfilePath;
}

void RunTimeDebugLog::LogStack( int nDepth )
{
	if(nDepth>0)
		stackwalker.PrintStack(*this,nDepth);
}



