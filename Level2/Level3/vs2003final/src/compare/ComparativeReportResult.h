// ComparativeReportResult.h: interface for the CComparativeReportResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVEREPORTRESULT_H__DC538420_6819_405C_85E8_CB6EDC3E8A49__INCLUDED_)
#define AFX_COMPARATIVEREPORTRESULT_H__DC538420_6819_405C_85E8_CB6EDC3E8A49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "../Common/elaptime.h"

enum ComparativeReportType
{
	QueueTimeReport,
	QueueLengthReport,
	ThroughtputReport,
	SpaceDensityReport,
	PaxCountReport,
	AcOperationReport,
	TimeTerminalReport,
	DistanceTravelReport
};

class CComparativeReportResult  
{
protected:
	std::vector<std::string> m_vSampleRepPaths;

public:
	ElapsedTime m_ReportStartTime, m_ReportEndTime;
public:
	CComparativeReportResult();
	virtual ~CComparativeReportResult();
public:
	void AddSamplePath(const std::vector<std::string>& _vSamplePaths);
	virtual void MergeSample(const ElapsedTime& tInteval) = 0;
	virtual void MergeSample(const long& tInteval){ASSERT(0);}
	virtual bool SaveReport(const std::string& _sPath) const = 0;
	virtual bool LoadReport(const std::string& _sPath) = 0;
	virtual int	 GetReportType() const = 0;
	int GetSampleCount() const { return m_vSampleRepPaths.size() ;}
};

#endif // !defined(AFX_COMPARATIVEREPORTRESULT_H__DC538420_6819_405C_85E8_CB6EDC3E8A49__INCLUDED_)
