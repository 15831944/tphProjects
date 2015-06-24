// MutiRunReportAgent.h: interface for the CMutiRunReportAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTIRUNREPORTAGENT_H__46DB4CC5_8565_479A_B7DF_65365519EE60__INCLUDED_)
#define AFX_MUTIRUNREPORTAGENT_H__46DB4CC5_8565_479A_B7DF_65365519EE60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<map>
#include<vector>
#include "ReportType.h"
class CBaseReport;
class CReportParameter;
class Terminal;
class CMutiRunReportAgent  
{
	std::map<ENUM_REPORT_TYPE,CBaseReport*>m_mapMutiRunPerformers;
	typedef std::pair<ENUM_REPORT_TYPE,CReportParameter*>Pair_Report_Para;
	std::vector<Pair_Report_Para>m_vReportParameters;
	CString m_csParentDirectory;
	Terminal* m_pTerm;
public:
	CMutiRunReportAgent();
	virtual ~CMutiRunReportAgent();
public:

	void Init( const CString& _csParentDirectory, Terminal* _pTerm );
	//add new report which you want to generate
	void AddReportWhatToGen(ENUM_REPORT_TYPE _reportType, CReportParameter* _pReportPara , int _iFloorCount=0 );
	void Clear();
	void GenerateAll();
private:
	bool DefaultIsSummary( ENUM_REPORT_TYPE _reportType,CReportParameter* _pReportPara );
};

#endif // !defined(AFX_MUTIRUNREPORTAGENT_H__46DB4CC5_8565_479A_B7DF_65365519EE60__INCLUDED_)
