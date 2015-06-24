// PassengerDensityReport.h: interface for the CPassengerDensityReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSENGERDENSITYREPORT_H__089441D4_9767_41BC_9428_14D1E744BE12__INCLUDED_)
#define AFX_PASSENGERDENSITYREPORT_H__089441D4_9767_41BC_9428_14D1E744BE12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "..\common\pollygon.h"

class CPassengerDensityReport : public CBaseReport  
{
public:
	void ReadSpecifications(const CString& _csProjPath);
	CPassengerDensityReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CPassengerDensityReport();

    virtual int GetReportFileType (void) const { return PassengerDensityReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
protected:
    ElapsedTime m_curTime;
	int m_nIntervalCount;
    long m_nPaxCount;
	double m_dArea;
	Pollygon m_region;

    virtual int GetSpecFileType (void) const { return PassengerDensitySpecFile; };
    virtual const char *GetTitle (void) const { return "Passenger_Density_Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );

    void calculatePassengerDensity (const CMobileElemConstraint& paxType,
        ArctermFile& p_file);

	void getCountAtTime (const CMobileElemConstraint& paxType);

};

#endif // !defined(AFX_PASSENGERDENSITYREPORT_H__089441D4_9767_41BC_9428_14D1E744BE12__INCLUDED_)
