#pragma once
#include "comparativereportresult.h"
#include "Common\elaptime.h"
#include <vector>
#include <map>
#include "ComparativeQTimeReport.h"

class CComparativeTimeTerminalReport :
	public CComparativeReportResult
{
public:
	CComparativeTimeTerminalReport(void);
	~CComparativeTimeTerminalReport(void);

public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int  GetReportType() const { return TimeTerminalReport; }
	const QTimeMap& GetResult() const{return m_mapTimeTerminal;}
private:

	QTimeMap		m_mapTimeTerminal;//save the TimeTerminal report data
	std::vector<int>& GetTimePos(const ElapsedTime& t, const ElapsedTime& tDuration);

};
