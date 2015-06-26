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
#include "ReportParamToCompare.h"

enum ComparativeReportType
{
	QueueTimeReport,
	QueueLengthReport,
	ThroughtputReport,
	SpaceDensityReport,
	PaxCountReport,
	AcOperationReport,
	TimeTerminalReport,
	DistanceTravelReport,
    ProcessorUtilizationReport
};

class CCmpBaseReport  
{
protected:
	std::vector<std::string> m_vSampleRepPaths;
	CString m_cmpReportName;
	std::vector<CString> m_vSimName;
	
public:
	ElapsedTime m_ReportStartTime, m_ReportEndTime;
	CReportParamToCompare m_cmpParam;
public:
	CCmpBaseReport();
	virtual ~CCmpBaseReport();
public:
	void AddSamplePath(const std::vector<std::string>& _vSamplePaths);
	virtual void MergeSample(const ElapsedTime& tInteval) = 0;
	virtual void MergeSample(const long& tInteval){ASSERT(0);}
	virtual bool SaveReport(const std::string& _sPath) const = 0;
	virtual bool LoadReport(const std::string& _sPath) = 0;
	virtual int	 GetReportType() const = 0;
	int GetSampleCount() const { return m_vSampleRepPaths.size() ;}
	CString GetCmpReportName(){ return m_cmpReportName; }
	void SetCmpReportName(CString name){ m_cmpReportName = name; }
	std::vector<CString>& GetSimNameList() { return m_vSimName; }
	void AddSimName(CString simName){  m_vSimName.push_back(simName); }
	void ClearSimName(){ m_vSimName.clear(); }
};

#endif // !defined(AFX_COMPARATIVEREPORTRESULT_H__DC538420_6819_405C_85E8_CB6EDC3E8A49__INCLUDED_)
