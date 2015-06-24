// ComparativeSpaceDensityReport.h: interface for the CComparativeSpaceDensityReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVESPACEDENSITYREPORT_H__63CC9911_B23A_4399_A79A_A08BEB9B5E5A__INCLUDED_)
#define AFX_COMPARATIVESPACEDENSITYREPORT_H__63CC9911_B23A_4399_A79A_A08BEB9B5E5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComparativeReportResult.h"
#include <map>

typedef std::map< ElapsedTime, std::vector<int> > PaxDensMap;

class CComparativeSpaceDensityReport : public CComparativeReportResult  
{
protected:
	PaxDensMap		m_mapPaxDens;

public:
	CComparativeSpaceDensityReport();
	virtual ~CComparativeSpaceDensityReport();

public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return SpaceDensityReport;}
	const PaxDensMap& GetResult() const{return m_mapPaxDens;}
};

#endif // !defined(AFX_COMPARATIVESPACEDENSITYREPORT_H__63CC9911_B23A_4399_A79A_A08BEB9B5E5A__INCLUDED_)
