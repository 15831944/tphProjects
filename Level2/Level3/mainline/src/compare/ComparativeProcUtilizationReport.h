#pragma once

#include "ComparativeReportResult.h"
#include "Common\elaptime.h"
#include <vector>
#include <map>
#include "Main\MultiRunsReportDataLoader.h"

class CmpProcUtilizationData
{
public:
    CmpProcUtilizationData():
        m_dScheduledTime(0L),
        m_dOverTime(0L),
        m_dActualTime(0L),
        m_dServiceTime(0L),
        m_dActualTime_m_dServiceTime(0L)
        {}

    ~CmpProcUtilizationData(){}

public:
    CString m_strModel;
    CString m_strSim;
    CString m_strProc;
    ElapsedTime m_dScheduledTime;
    ElapsedTime m_dOverTime;
    ElapsedTime m_dActualTime;
    ElapsedTime m_dServiceTime;
    ElapsedTime m_dActualTime_m_dServiceTime;
};

class CComparativeProcUtilizationReport : public CCmpBaseReport
{
public:
	CComparativeProcUtilizationReport();
	virtual ~CComparativeProcUtilizationReport();

	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath)const;
	bool LoadReport(const std::string& _sPath);
	int  GetReportType() const { return ProcessorUtilizationReport; }
protected:
	void MergeSampleDetail(const ElapsedTime& tInteval);
	bool SaveReportDetail(ArctermFile& file) const;
	bool LoadReportDetail(ArctermFile& file);

	void MergeSampleSummary(const ElapsedTime& tInteval);
	bool SaveReportSummary(ArctermFile& file) const;
	bool LoadReportSummary(ArctermFile& file);
};

