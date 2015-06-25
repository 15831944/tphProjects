// ComparativeQLengthReport.h: interface for the CComparativeQLengthReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVEQLENGTHREPORT_H__58D6D942_440E_46A6_9BF0_0072C9B3E23A__INCLUDED_)
#define AFX_COMPARATIVEQLENGTHREPORT_H__58D6D942_440E_46A6_9BF0_0072C9B3E23A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include <map>
#include "ComparativeReportResult.h"
#include "Common\elaptime.h"

typedef std::map<ElapsedTime, std::vector<int> > QLengthMap;//QueueLength report data table,save all models'  QueueLength report data
struct QueueLengthGroup 
{
	QueueLengthGroup()
	{
		m_nMinValue = 0;
		m_nMaxValue = 0;
		m_nAvaValue = 0;
		m_nTotalValue = 0;
	}
	CString m_strModelName;
	int GetValue(int nSubType)const
	{
		switch (nSubType)
		{
		case 0:
			return m_nMinValue;
		case 1:
			return m_nAvaValue;
		case 2:
			return m_nMaxValue;
		case 3:
			return m_nTotalValue;
		default:
			break;
		}
		return m_nMinValue;
	}
	int m_nMinValue;
	int m_nMaxValue;
	int m_nAvaValue;
	int m_nTotalValue;
};

//typedef std::map<ElapsedTime,std::vector<QueueLengthGroup>>QLengthSummaryMap;
//struct QLengthSummary
//{
//	ElapsedTime m_tTime;
//	
//	QueueLengthGroup m_queueLengthGroup;
//};

typedef std::map<ElapsedTime,std::vector<QueueLengthGroup>> QLengthSummaryMap;

class CComparativeQLengthReport : public CCmpBaseReport  
{
protected:
	QLengthMap m_mapQLength;
	QLengthSummaryMap m_mapSummaryQLength;
public:
	enum QSummaryLengthSubType
	{
		MIN_QLENGTH,
		AVA_QLENGTH,
		MAX_QLENGTH,
		TOTAL_QLENGTH
	};

	enum QDetailLengthSubType
	{
		QUEUELENGTH_TYPE,
	};

	CComparativeQLengthReport();
	virtual ~CComparativeQLengthReport();
public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int  GetReportType() const { return QueueLengthReport; }
	const QLengthMap& GetResult() const{ return m_mapQLength; }
	const QLengthSummaryMap& GetSummaryResult()const {return m_mapSummaryQLength;}
private:
	void MergeDetailSample(const ElapsedTime& tInteval);
	void MergeSummarySample(const ElapsedTime& tInteval);

	bool SaveDetailReport(const std::string& _sPath)const;
	bool SaveSummaryReport(const std::string& _sPath)const;

	bool LoadDetailReport(const std::string& _sPath);
	bool LoadSummaryReport(const std::string& _sPath);
};

#endif // !defined(AFX_COMPARATIVEQLENGTHREPORT_H__58D6D942_440E_46A6_9BF0_0072C9B3E23A__INCLUDED_)
