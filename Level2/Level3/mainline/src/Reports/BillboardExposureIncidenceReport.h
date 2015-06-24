#pragma once
#include "basereport.h"
#include "BillboardThroughputResult.h"

class CBillboardExposureIncidenceReport :public CBaseReport
{
public:
	CBillboardExposureIncidenceReport(Terminal* _pTerm, const CString& _csProjPath);
	~CBillboardExposureIncidenceReport(void);

public:
	virtual int GetReportFileType (void) const { return BillboardExposureIncidenceFile ; };

	virtual void InitParameter( const CReportParameter* _pPara );

protected:
	ElapsedTime m_curTime;

	// given processorid and get all the resuld into _result.
	void GetResult( CBillboardThroughputResult& _result,  ProcessorID* _pProcID );

	// given processor/group id, write the result to file.
	void calculateThroughput( ProcessorID* _pProcID, ArctermFile& p_file );


	long m_lPassengersServed;
	long m_lTotalPaxServed;
	int m_nCount;

	void clear (void) { m_lPassengersServed = m_lTotalPaxServed = m_nCount = 0; };

	virtual int GetSpecFileType (void) const { return BillboardExposureIncidenceSpecFile; };
	virtual const char *GetTitle (void) const { return "Billboard Exposure Incidence Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );


	void getAverageThroughput (const ProcessorID *id);
	void writeAverage (const ProcessorID *id, ArctermFile& p_file) const;

	void writeEntry (const char *p_id, ArctermFile& p_file) const;

	void calculateThroughput (ProcLogEntry& logEntry);
};
