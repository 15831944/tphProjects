// ComparativeThroughputReport.h: interface for the CComparativeThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_)
#define AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComparativeReportResult.h"

typedef struct tagCompThroughtputData
{
	ElapsedTime	etStart;
	ElapsedTime etEnd;
	std::vector<int>	vPaxServed;

	tagCompThroughtputData()
	{
		clear();
	}

	void clear()
	{
		etStart = 0L;
		etEnd = 0L;
		vPaxServed.clear();
	}
}CompThroughputData;

class CComparativeThroughputReport : public CCmpBaseReport  
{
public:
	CComparativeThroughputReport();
	virtual ~CComparativeThroughputReport();

protected:
	std::vector<CompThroughputData>		m_vThoughputData;

public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return ThroughtputReport;}
	const std::vector<CompThroughputData>& GetResult() const{ return m_vThoughputData; }
};

#endif // !defined(AFX_COMPARATIVETHROUGHPUTREPORT_H__B409953B_4AF4_4830_BF37_092424A45919__INCLUDED_)
