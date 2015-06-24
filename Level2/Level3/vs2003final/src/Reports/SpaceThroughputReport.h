// SpaceThroughputReport.h: interface for the CSpaceThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPACETHROUGHPUTREPORT_H__032D0904_940F_45C5_90E1_0A1BC024541C__INCLUDED_)
#define AFX_SPACETHROUGHPUTREPORT_H__032D0904_940F_45C5_90E1_0A1BC024541C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "..\common\pollygon.h"
#include <map>
#include <vector>
#include "results\outpax.h"
#include "SpaceThroughtoutResult.h"
#include "rep_pax.h"
typedef std::vector<PaxEvent> PointVector;
class REPORTS_TRANSFER CSpaceThroughputReport : public CBaseReport  
{
public:
	CSpaceThroughputReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CSpaceThroughputReport();
	void ReadSpecifications(const CString& _csProjPath);
	
	virtual int GetReportFileType (void) const { return SpaceThroughputReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
protected:
	ElapsedTime m_curTime;
	int m_nIntervalCount;
	long m_nPaxCount;
	double m_dPortal;
	Point m_pointList[2];
	
	virtual int GetSpecFileType(void) const {return SpaceThroughputSpecFile;}
	virtual const char *GetTitle(void) const {return "Space_Throughput_Report";}

	virtual void GenerateSummary(ArctermFile& p_file) {};
	virtual void GenerateDetailed(ArctermFile& p_file);

	void calculateSpaceThroughReport (const CMobileElemConstraint& paxType, ArctermFile& p_file);
	//void getCountAtTime (const PassengerConstraint& paxType);
	//void PaxTrack();
	//int CrossPortalNum(Point *PointList, long num);
	void GetResult( CSpaceThroughtputResult& _result ,const CMobileElemConstraint& paxType );
	void ComputerSinglePaxResult( CSpaceThroughtputResult& _result ,ReportPaxEntry& _element );
	void ComputerSinglePaxCrossPortal( CSpaceThroughtputResult& _result , PointVector& _trackVector );

	
};

#endif // !defined(AFX_SPACETHROUGHPUTREPORT_H__032D0904_940F_45C5_90E1_0A1BC024541C__INCLUDED_)
