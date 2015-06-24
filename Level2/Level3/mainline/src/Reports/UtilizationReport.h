// UtilizationReport.h: interface for the CUtilizationReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILIZATIONREPORT_H__6C0CDC9F_5190_420B_8EEC_2E8FFCC5EECB__INCLUDED_)
#define AFX_UTILIZATIONREPORT_H__6C0CDC9F_5190_420B_8EEC_2E8FFCC5EECB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "reportsdll.h"

class REPORTS_TRANSFER CUtilizationReport : public CBaseReport  
{
public:
	CUtilizationReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CUtilizationReport();

    virtual int GetReportFileType (void) const { return UtilizationReportFile; };
protected:

//	ElapsedTime m_scheduledTime;
//	ElapsedTime m_overTime;
//	ElapsedTime m_actualTime;
//	ElapsedTime m_serviceTime;
//	ElapsedTime m_idleTime;

	double m_dScheduledTime;
	double m_dOverTime;
	double m_dActualTime;
	double m_dServiceTime;
	double m_dIdleTime;

	int m_count;

    virtual int GetSpecFileType (void) const { return UtilizationSpecFile; };
    virtual const char *GetTitle (void) const { return "Utilization_Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );

    void Clear();

    void CalculateUtilization( ProcLogEntry& logEntry );
    void WriteEntries( const char *p_id, ArctermFile& p_file ) const;

	void GetAverageUtilization (const ProcessorID *id);
	void WriteAverages (const ProcessorID *id, ArctermFile& p_file) const;

};

#endif // !defined(AFX_UTILIZATIONREPORT_H__6C0CDC9F_5190_420B_8EEC_2E8FFCC5EECB__INCLUDED_)
