#include "StdAfx.h"
#include ".\multireportandreportmanager.h"
#include "common\mutilreport.h"
#include "Common/fileman.h"

extern CString arrayReortName[];
#define  REPORTTYPENUM 28


CMultiReportAndReportManager::CMultiReportAndReportManager(const CString& strProjPath)
:m_strProjPath(strProjPath)
{
	InitMultiReportFilePath(strProjPath);
}

CMultiReportAndReportManager::~CMultiReportAndReportManager(void)
{
}

void CMultiReportAndReportManager::InitMultiReportFilePath(const CString& strProjPath)
{
	CString m_strDirectoryName = _T("\\SimResult\\MultiReport");
	
	m_strMultiReportPath = strProjPath + m_strDirectoryName;
	FileManager::MakePath( m_strMultiReportPath );

	for( int i=0; i<REPORTTYPENUM; i++ )
	{
		CString strReportName	= arrayReortName[i];
		CString strDir			= m_strMultiReportPath + "\\" + strReportName;
		CString strPara			= strDir + "\\" + strReportParaFile;
		//CString strResult		= strDir + "\\" + strReportName + ".rep";

		FileManager::MakePath( strDir );

		m_mapReporTypeToName[ (ENUM_REPORT_TYPE)i ] = strPara;
	}

}
CString CMultiReportAndReportManager::GetReportParamFile(ENUM_REPORT_TYPE enumrpType)
{
	if (enumrpType>=ENUM_PAXLOG_REP && enumrpType < REPORTTYPENUM)
	{
		return m_mapReporTypeToName[enumrpType];
	}

	ASSERT(FALSE);
	return _T("");
}
CString CMultiReportAndReportManager::GetMultiReportHostFile()
{
	return m_strMultiReportPath +_T("HostInfo.hos") ;
}