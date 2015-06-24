// ConveyorWaitTimeReport.h: interface for the CConveyorWaitTimeReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVEYORWAITTIMEREPORT_H__5D482C7F_849E_4A21_83EF_C6A8615813E8__INCLUDED_)
#define AFX_CONVEYORWAITTIMEREPORT_H__5D482C7F_849E_4A21_83EF_C6A8615813E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "StatisticalTools.h"
class CConveyorWaitTimeReport : public CBaseReport  
{
public:
	CConveyorWaitTimeReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CConveyorWaitTimeReport();
public:
	virtual int GetReportFileType (void) const { return ConveyorWaitTimeReport; };
	virtual int GetSpecFileType (void) const { return ConveyorWaitTimeSpecFile; };
    virtual const char *GetTitle (void) const { return "Conveyor Wait Time Report"; };
	virtual void InitParameter( const CReportParameter* _pPara );
	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
private:
	void getAverageQueueTime (const CMobileElemConstraint& aConst,CStatisticalTools<ElapsedTime>& _tools);
	// summary statistics
    ElapsedTime minimum;
    ElapsedTime average;
    ElapsedTime maximum;
	
	int count;

};

#endif // !defined(AFX_CONVEYORWAITTIMEREPORT_H__5D482C7F_849E_4A21_83EF_C6A8615813E8__INCLUDED_)
