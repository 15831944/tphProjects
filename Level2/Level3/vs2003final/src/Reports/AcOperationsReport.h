// AcOperationsReport.h: interface for the CAcOperationsReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACOPERATIONSREPORT_H__03500361_6252_4CC0_9972_B8D4BB3EB3A4__INCLUDED_)
#define AFX_ACOPERATIONSREPORT_H__03500361_6252_4CC0_9972_B8D4BB3EB3A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"

class CAcOperationsReport : public CBaseReport  
{
public:
	CAcOperationsReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CAcOperationsReport();
	void ReadSpecifications(const CString& _csProjPath);
	virtual int GetReportFileType (void) const { return AcOperationsReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
protected:
	bool FilterFlight(ElapsedTime& etArrTime,ElapsedTime& etDepTime, char* sType );
	virtual int GetSpecFileType(void) const {return AcOperationsSpecFile;}
	virtual const char *GetTitle(void) const {return "Aircraft_Operations_Report";}	
	virtual void GenerateSummary(ArctermFile& p_file);
	virtual void GenerateDetailed(ArctermFile& p_file) {};


};

#endif // !defined(AFX_ACOPERATIONSREPORT_H__03500361_6252_4CC0_9972_B8D4BB3EB3A4__INCLUDED_)
