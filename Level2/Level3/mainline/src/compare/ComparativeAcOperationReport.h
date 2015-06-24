#pragma once
#include "comparativereportresult.h"
#include <map>

//AcOperation report data structure
struct CmpAcOperation 
{
	ElapsedTime totalTime;
	char		procCount[30];

	CmpAcOperation() {totalTime = 0l; procCount[0] = 0; }
};

typedef std::vector<CmpAcOperation> CmpAcOperationVector;//AcOperation report data of one model
typedef std::map<ElapsedTime, std::vector<int> > AcOperationMap;//AcOperation report data table,save all models'  AcOperation report data


class CComparativeAcOperationReport :
	public CComparativeReportResult
{
public:
	CComparativeAcOperationReport(void);
	~CComparativeAcOperationReport(void);


protected:
	AcOperationMap		m_mapAcOperation;

public:
	void MergeSample(const ElapsedTime& tInterval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return AcOperationReport;}
	const AcOperationMap& GetResult()const {return m_mapAcOperation;}

	std::vector<int>& CComparativeAcOperationReport::GetTimePos(const ElapsedTime &t, const ElapsedTime& tDuration);

};
