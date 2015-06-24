// BagDistReport.h: interface for the CBagDistReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGDISTREPORT_H__CC47CD6C_B2EB_4CFF_A7DD_7DB8779811D0__INCLUDED_)
#define AFX_BAGDISTREPORT_H__CC47CD6C_B2EB_4CFF_A7DD_7DB8779811D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"


class _mapCount
{
public:
	long arrCount;
	long depCount;
	long index;
//	long depFlightID;
//	long arrFlightID;
	
};

class CBagDistReport : public CBaseReport  
{
public:
	CBagDistReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CBagDistReport();
	void ReadSpecifications(const CString& _csProjPath);
	virtual int GetReportFileType (void) const { return BagDistReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
	
protected:
	virtual int GetSpecFileType(void) const {return BagDistSpecFile;}
	virtual const char *GetTitle(void) const {return "Baggage_Distribution_Report";}

	virtual void GenerateSummary(ArctermFile& p_file) {}
	virtual void GenerateDetailed(ArctermFile& p_file);
};

#endif // !defined(AFX_BAGDISTREPORT_H__CC47CD6C_B2EB_4CFF_A7DD_7DB8779811D0__INCLUDED_)
