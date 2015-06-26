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

enum SpaceDensitySubType
{
	/* detail */
	SpaceDensity_Count,
	SpaceDensity_PaxToM,
	SpaceDensity_MtoPax
};

struct SpaceDensigyGroup 
{
	SpaceDensigyGroup()
	{
		m_iCount = 0;
		m_iPaxToM = 0.0;
		m_iMToPax = 0.0;
	}

	double GetValue(int nSubType)const
	{
		switch (nSubType)
		{
		case 0:
			return static_cast<double>(m_iCount);
		case 1:
			return m_iPaxToM;
		case 2:
			return m_iMToPax;
		default:
			break;
		}
		return static_cast<double>(m_iCount);
	}

	int m_iCount;
	double m_iPaxToM;
	double m_iMToPax;
};

typedef std::map< ElapsedTime, std::vector<SpaceDensigyGroup> > PaxDensityMap;

class CComparativeSpaceDensityReport : public CCmpBaseReport  
{
protected:
	PaxDensityMap		m_mapPaxDens;

public:
	CComparativeSpaceDensityReport();
	virtual ~CComparativeSpaceDensityReport();

public:
	void MergeSample(const ElapsedTime& tInteval);
	bool SaveReport(const std::string& _sPath) const;
	bool LoadReport(const std::string& _sPath);
	int GetReportType() const{return SpaceDensityReport;}
	const PaxDensityMap& GetResult() const{return m_mapPaxDens;}

	CString GetFooter(int iSubType)const;
};

#endif // !defined(AFX_COMPARATIVESPACEDENSITYREPORT_H__63CC9911_B23A_4399_A79A_A08BEB9B5E5A__INCLUDED_)
