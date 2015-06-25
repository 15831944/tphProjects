#pragma once
#include "reports/ReportType.h"

#include <map>


class CMultiReportAndReportManager
{
public:
	CMultiReportAndReportManager(const CString& strProjPath);
	virtual ~CMultiReportAndReportManager(void);


public:
	void InitMultiReportFilePath(const CString& strProjPath);

	CString GetReportParamFile(ENUM_REPORT_TYPE enumrpType);
	CString GetMultiReportHostFile();


protected:
	std::map<ENUM_REPORT_TYPE,CString> m_mapReporTypeToName;
	CString m_strProjPath;
	CString m_strMultiReportPath;

};
