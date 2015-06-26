#pragma once

#include "ComparativeReportResult.h"
#include "Common\elaptime.h"
#include <map>

class CmpProcUtilizationDetailData
{
public:
    CmpProcUtilizationDetailData();
    ~CmpProcUtilizationDetailData();
public:
    void ReadData(ArctermFile& file);
    void WriteData(ArctermFile& file) const;
public:
    CString m_strProc;
    ElapsedTime m_dScheduledTime;
    ElapsedTime m_dOverTime;
    ElapsedTime m_dActualTime;
    ElapsedTime m_dServiceTime;
    ElapsedTime m_dIdleTime;
    float m_fUtilization;
};

class CmpProcUtilizationSummaryData : public CmpProcUtilizationDetailData
{
public:
    CmpProcUtilizationSummaryData();
    ~CmpProcUtilizationSummaryData();
public:
    void ReadData(ArctermFile& file);
    void WriteData(ArctermFile& file) const;
public:
    int m_nProcCount;
};

//              <strSim, vector<processor utilization data>>
typedef std::map<CString, std::vector<CmpProcUtilizationDetailData>> mapProcUtilizationDetail;

//              <strSim, vector<processor utilization data>>
typedef std::map<CString, std::vector<CmpProcUtilizationSummaryData>> mapProcUtilizationSummary;

class CComparativeProcUtilizationReport : public CCmpBaseReport
{
public:
	CComparativeProcUtilizationReport();
	virtual ~CComparativeProcUtilizationReport();

	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath)const;
	bool LoadReport(const std::string& _sPath);
	int  GetReportType() const { return ProcessorUtilizationReport; }

    const mapProcUtilizationDetail& GetMapDetailResult() { return m_mapDetail; }
    const mapProcUtilizationSummary& GetMapSummaryResult() { return m_mapSummary; }

	CString GetFooter(int iSubType)const;
protected:
	void MergeSampleDetail(const ElapsedTime& tInteval);
	bool SaveReportDetail(ArctermFile& file) const;
	bool LoadReportDetail(ArctermFile& file);

	void MergeSampleSummary(const ElapsedTime& tInteval);
	bool SaveReportSummary(ArctermFile& file) const;
	bool LoadReportSummary(ArctermFile& file);

protected:
    mapProcUtilizationDetail m_mapDetail;
    mapProcUtilizationSummary m_mapSummary;
};

