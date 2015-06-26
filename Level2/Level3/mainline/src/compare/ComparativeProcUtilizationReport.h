#pragma once

#include "ComparativeReportResult.h"
#include "Common\elaptime.h"
#include <map>

class CmpProcUtilizationData
{
public:
    CmpProcUtilizationData();
    ~CmpProcUtilizationData();
public:
    void ReadData(ArctermFile& file);
    void WriteData(ArctermFile& file);
public:
    CString m_strProc;
    ElapsedTime m_dScheduledTime;
    ElapsedTime m_dOverTime;
    ElapsedTime m_dActualTime;
    ElapsedTime m_dServiceTime;
    ElapsedTime m_dActualTime_m_dServiceTime;
};

//              <strSim, processor utilization list>
typedef std::map<CString, std::vector<CmpProcUtilizationData>> mapProcUtilization;

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
    mapProcUtilization m_mapProcUtilization;
};

