#pragma once
#include "comparativereportresult.h"
#include "Common/TimeRange.h"
#include <map>

typedef std::map< TimeRange, std::vector<int> > PaxThroughputMap;
class CComparativSpaceThroughputReport :
	public CCmpBaseReport
{
public:
	CComparativSpaceThroughputReport(void);
	~CComparativSpaceThroughputReport(void);

	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return SpaceThroughputReport;}
	const PaxThroughputMap& GetResult() const{return m_mapPaxThroughput;}

protected:
	PaxThroughputMap		m_mapPaxThroughput;
};

