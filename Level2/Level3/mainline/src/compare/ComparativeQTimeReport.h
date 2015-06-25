// ComparativeQTimeReport.h: interface for the CComparativeQTimeReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVEQTIMEREPORT_H__A5C05991_487E_452A_B8E2_8E70E20C78BA__INCLUDED_)
#define AFX_COMPARATIVEQTIMEREPORT_H__A5C05991_487E_452A_B8E2_8E70E20C78BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComparativeReportResult.h"
#include "Common\elaptime.h"
#include <vector>
#include <map>
#include "Main\MultiRunsReportDataLoader.h"

//Queue Time report data structure 
struct CmpQTime
{
	ElapsedTime totalTime;
	int			procCount;
};

typedef std::vector<CmpQTime> CmpQTimeVector;//Q Time report data of one model
typedef std::map<ElapsedTime, std::vector<int> > QTimeMap;//QueueTime report data table,save all models'  QueueTime report data


class CComparativeQTimeReport : public CCmpBaseReport  
{
protected:
	QTimeMap		m_mapQTime;
	std::vector<int>& GetTimePos(const ElapsedTime& t, const ElapsedTime& tDuration);
	
public:
	CComparativeQTimeReport();
	virtual ~CComparativeQTimeReport();

	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath)const;
	bool LoadReport(const std::string& _sPath);
public:
	int  GetReportType() const { return QueueTimeReport; }
	//const std::vector<CmpQTimeVector>& GetResult() const { return m_vMultiQTimeReports; }
	const QTimeMap& GetResultDetail() const{return m_mapQTime;}
	const MultiRunsReport::Summary::SummaryQTimeList& GetResultSummary()const{ return summaryQTimeList; }
protected:
	void MergeSampleDetail(const ElapsedTime& tInteval);
	bool SaveReportDetail(ArctermFile& file) const;
	bool LoadReportDetail(ArctermFile& file);

	void MergeSampleSummary(const ElapsedTime& tInteval);
	bool SaveReportSummary(ArctermFile& file) const;
	bool LoadReportSummary(ArctermFile& file);

	MultiRunsReport::Summary::SummaryQTimeList summaryQTimeList;
};

#endif // !defined(AFX_COMPARATIVEQTIMEREPORT_H__A5C05991_487E_452A_B8E2_8E70E20C78BA__INCLUDED_)
