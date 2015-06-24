// QueueReport.h: interface for the CQueueReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUEREPORT_H__EED24E1C_6899_4BEB_A35D_7AF15B0670B6__INCLUDED_)
#define AFX_QUEUEREPORT_H__EED24E1C_6899_4BEB_A35D_7AF15B0670B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reportsdll.h"
#include "BaseReport.h"
#include "StatisticalTools.h"
class CQueueReport : public CBaseReport  
{
public:
	void getAverageQueueTime (const CMobileElemConstraint& aConst,CStatisticalTools<ElapsedTime>& _tools);
	void GenerateSummary(ArctermFile &p_file);
	void GenerateDetailed(ArctermFile &p_file);
	virtual int GetReportFileType (void) const { return QueueTimeReportFile; };
	virtual int GetSpecFileType (void) const { return QueueTimeSpecFile; };
	virtual const char *GetTitle (void) const { return "Queue_Time_Report"; };
	CQueueReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CQueueReport();
	virtual int getSpecFileType (void) const { return QueueTimeSpecFile; };
    virtual int getReportFileType (void) const { return QueueTimeReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
	
	// summary statistics
    ElapsedTime minimum;
    ElapsedTime average;
    ElapsedTime maximum;

	int count;

};

#endif // !defined(AFX_QUEUEREPORT_H__EED24E1C_6899_4BEB_A35D_7AF15B0670B6__INCLUDED_)
