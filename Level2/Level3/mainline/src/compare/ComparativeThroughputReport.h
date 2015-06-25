// ComparativeThroughputReport.h: interface for the CComparativeThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_)
#define AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComparativeReportResult.h"

class CmpThroughputData
{
public:
	CmpThroughputData(){ clear(); }

	ElapsedTime	m_startTime;
	ElapsedTime m_endTime;

	// detail
	std::vector<int> m_vPaxServed;

	// summary
	std::vector<int> m_v1;
	std::vector<int> m_v2;
	std::vector<int> m_v3;
	std::vector<int> m_v4;

public:
	void clear()
	{
		m_startTime = 0L;
		m_endTime = 0L;
		m_vPaxServed.clear();
		m_v1.clear();
		m_v2.clear();
		m_v3.clear();
		m_v4.clear();
	}
};

class CComparativeThroughputReport : public CCmpBaseReport  
{
public:
	CComparativeThroughputReport();
	virtual ~CComparativeThroughputReport();

protected:
	std::vector<CmpThroughputData> m_vThoughputData;
public:
	enum ThroughPutSubType
	{
		/* detail */
		MIN_QLENGTH = 0,

		/* summary */
		AVA_QLENGTH,
		MAX_QLENGTH,
		TOTAL_QLENGTH,
		QUEUELENGTH_TYPE
	};
public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return ThroughtputReport;}
	const std::vector<CmpThroughputData>& GetResult() const{ return m_vThoughputData; }

private:
	void MergeDetailSample(const ElapsedTime& tInteval);
	void MergeSummarySample(const ElapsedTime& tInteval);
};

#endif // !defined(AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_)
