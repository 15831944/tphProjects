// ThroughtputReport.h: interface for the CThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THROUGHTPUTREPORT_H__684AFAC8_3278_4A08_B936_C4BFA34C2751__INCLUDED_)
#define AFX_THROUGHTPUTREPORT_H__684AFAC8_3278_4A08_B936_C4BFA34C2751__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h" 
#include "SpaceThroughtoutResult.h"
#include "ReportParameter.h"
class CThroughputReport : public CBaseReport  
{
public:
	CThroughputReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CThroughputReport();

    virtual int GetReportFileType (void) const { return ThroughputReportFile; };

	virtual void InitParameter( const CReportParameter* _pPara );
	
protected:
	ElapsedTime m_curTime;

	// given processorid and get all the resuld into _result.
	void GetResult( CSpaceThroughtputResult& _result,  ProcessorID* _pProcID );
		
	// given processor/group id, write the result to file.
	void calculateThroughput( ProcessorID* _pProcID, ArctermFile& p_file );
		
	
	long m_lPassengersServed;
	int m_nCount;

	void clear (void) { m_lPassengersServed = m_nCount = 0; };

    virtual int GetSpecFileType (void) const { return ThroughputSpecFile; };
    virtual const char *GetTitle (void) const { return "Throughput_Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );


    void getAverageThroughput (const ProcessorID *id);
    void writeAverage (const ProcessorID *id, ArctermFile& p_file) const;

    void writeEntry (const char *p_id, ArctermFile& p_file) const;

    void calculateThroughput (ProcLogEntry& logEntry);
};

#endif // !defined(AFX_THROUGHTPUTREPORT_H__684AFAC8_3278_4A08_B936_C4BFA34C2751__INCLUDED_)
