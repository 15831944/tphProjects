#pragma once
#include<map>
#include "AirsideReport/TypeDefs.h"
#include "AirsideBaseReport.h"
#include "Engine/Terminal.h"

class CParameters;

typedef std::vector<CAirsideBaseReport*> AirsideReportList;

class CAirsideMultipleRunReportAgent
{
public:
	CAirsideMultipleRunReportAgent(void);
	~CAirsideMultipleRunReportAgent(void);

	void InitReportPath(const CString& strReportPath);
	CAirsideBaseReport* AddReportWhatToGen(reportType _reportType,CParameters * parameter, Terminal* pTerm);
	void GenerateReport(CAirsideBaseReport* pReport,CParameters * parameter);

	void SetCBGetLogFilePath(CBGetLogFilePath pFunc){ m_pGetLogFilePath = pFunc;}
	CBGetLogFilePath GetCBGetLogFilePath(){ return m_pGetLogFilePath;}

	CString GetSimResultPath(reportType _reportType);

private:
	CString m_strReportPath;
	CBGetLogFilePath		m_pGetLogFilePath;
};

