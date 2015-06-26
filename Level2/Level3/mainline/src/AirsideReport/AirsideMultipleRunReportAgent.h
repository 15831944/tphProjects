#pragma once
#include<map>
#include "AirsideReport/TypeDefs.h"
#include "AirsideBaseReport.h"
#include "Engine/Terminal.h"

class CParameters;

class CAirsideMultipleRunReportAgent
{
public:
	CAirsideMultipleRunReportAgent(void);
	~CAirsideMultipleRunReportAgent(void);

	void InitReportPath(const CString& strReportPath);
	void AddReportWhatToGen(reportType _reportType,CParameters * parameter, Terminal* pTerm);
	void GenerateReport(reportType _reportType,CParameters * parameter);

	void SetCBGetLogFilePath(CBGetLogFilePath pFunc){ m_pGetLogFilePath = pFunc;}
	CBGetLogFilePath GetCBGetLogFilePath(){ return m_pGetLogFilePath;}

	CString GetSimResultPath(reportType _reportType);
private:
	void Clear();
private:
	CString m_strReportPath;
	CBGetLogFilePath		m_pGetLogFilePath;
	std::map<reportType,CAirsideBaseReport*>m_mapMutiRunPerformers;
};

