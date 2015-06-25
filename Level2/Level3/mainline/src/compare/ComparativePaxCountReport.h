#pragma once
#include "comparativereportresult.h"
#include <map>

typedef std::map< ElapsedTime, std::vector<int> > PaxCountMap;//PaxCount report data table,save all models'  PaxCount report data

class CComparativePaxCountReport :
	public CCmpBaseReport
{

protected:
	PaxCountMap		m_mapPaxCount;

public:
	CComparativePaxCountReport(void);
	~CComparativePaxCountReport(void);


public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return PaxCountReport;}
	const PaxCountMap& GetResult()const {return m_mapPaxCount;}
};
