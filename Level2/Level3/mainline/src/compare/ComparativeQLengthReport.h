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

class CComparativeQLengthReport : public CCmpBaseReport  
{
protected:
	QLengthMap m_mapQLength;
public:
	CComparativeQLengthReport();
	virtual ~CComparativeQLengthReport();
public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int  GetReportType() const { return QueueLengthReport; }
	const QLengthMap& GetResult() const{ return m_mapQLength; }
};

#endif // !defined(AFX_COMPARATIVEQLENGTHREPORT_H__58D6D942_440E_46A6_9BF0_0072C9B3E23A__INCLUDED_)
