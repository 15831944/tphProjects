// ComparativeThroughputReport.h: interface for the CComparativeThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_)
#define AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComparativeReportResult.h"

enum ThroughPutSubType
{
	/* detail */
	TR_DETAIL = 0,

	/* summary */
	TOTAL_PAX,
	AVG_PAX,
	TOTAL_HOUR,
	AVG_HOUR
};

class CmpThroughputDetailData
{
public:
	CmpThroughputDetailData(){ clear(); }

	ElapsedTime	m_startTime;
	ElapsedTime m_endTime;
	int m_nPaxServed;

public:
	void clear()
	{
		m_startTime = 0L;
		m_endTime = 0L;
		m_nPaxServed = 0;
	}
};
typedef std::vector<CmpThroughputDetailData> OneCmpThroughputDetailVector;

class CmpThroughputSummaryData
{
public:
	CmpThroughputSummaryData(){ clear(); }

	int	m_totalPax;			// Total Pax
	int m_avgPax;			// Avg Pax
	int m_totalPerHour;		// Total / Hour
	int m_avgPerHour;		// Avg / Hour

public:
	void clear()
	{
		m_totalPax = m_avgPax = m_totalPerHour = m_avgPerHour = 0;
	}
	
	int GetData(int nSubType) const
	{
		switch(nSubType)
		{
		case TOTAL_PAX:
			return m_totalPax;
			break;
		case AVG_PAX:
			return m_avgPax;
			break;
		case TOTAL_HOUR:
			return m_totalPerHour;
			break;
		case AVG_HOUR:
			return m_avgPerHour;
			break;
		default:
			return -1;
			break;
		}
	}
};

class CComparativeThroughputReport : public CCmpBaseReport  
{
public:
	CComparativeThroughputReport();
	virtual ~CComparativeThroughputReport();

protected:
	std::vector<OneCmpThroughputDetailVector> m_vDetail;
	std::vector<CmpThroughputSummaryData> m_vSummary;
public:

public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return ThroughtputReport;}
	const std::vector<OneCmpThroughputDetailVector>& GetDetailResult() const{ return m_vDetail; }
	const std::vector<CmpThroughputSummaryData>& GetSummaryResult() const{ return m_vSummary; }
private:
	void MergeDetailSample(const ElapsedTime& tInteval);
	void MergeSummarySample(const ElapsedTime& tInteval);
};

#endif // !defined(AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_)
