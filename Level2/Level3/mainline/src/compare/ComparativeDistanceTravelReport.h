#pragma once
#include "comparativereportresult.h"
#include "Common\elaptime.h"
#include <vector>
#include <map>

//Distance report data structure
struct CmpDistance
{
	long		totalDistance;
	int			paxCount;
};

typedef std::vector<CmpDistance> CmpDistanceVector;//Distance travel report data of one model
typedef std::map<long, std::vector<int> > DistanceMap;//Distance travel report data table,save all models'  Distance travel report data


class CComparativeDistanceTravelReport :
	public CCmpBaseReport
{
public:
	CComparativeDistanceTravelReport(void);
	~CComparativeDistanceTravelReport(void);
	
public:	
	DistanceMap		m_mapDistance;
	std::vector<int>& GetDistancePos(const long& t, const long& tDuration);
public:
	void MergeSample(const ElapsedTime& tInteval);
	void MergeSample(const long& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int  GetReportType() const { return DistanceTravelReport; }
	const DistanceMap& GetResult() const{return m_mapDistance;}
};
