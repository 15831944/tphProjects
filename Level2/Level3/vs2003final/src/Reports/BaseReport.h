// BaseReport.h: interface for the CBaseReport class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BASEREPORT_H__
#define __BASEREPORT_H__

#pragma once

#include "ProgressWnd.h"
#include "common\elaptime.h"
#include "inputs\pax_cnst.h"
#include <CommonData/procid.h>
#include "inputs\MultiMobConstraint.h"
#include "results\proclog.h"
#include <Inputs/PROCIDList.h>

class CReportParameter;


class CBaseReport  
{
public:
	CBaseReport( Terminal* _pTerm, const CString& _csProjPath );
	virtual ~CBaseReport();

	virtual void GenerateReport( const CString& _csProjPath, bool bSummary = true );
	virtual void ReadSpecifications(const CString& _csProjPath);

    virtual int GetReportFileType (void) const = 0;
	virtual void InitParameter( const CReportParameter* _pPara );
	int getProcType (int p_ndx);

protected:

    ElapsedTime m_startTime;
    ElapsedTime m_endTime;
    ElapsedTime m_interval;
    

	// passenger and processor types of interest
	CMultiMobConstraint m_multiConst;
	ProcessorIDList m_procIDlist;
    ProcLogEntry* m_procList;

	Terminal* m_pTerm;
public:
	int m_usesInterval;
	void InitDefaultSpecs();
	void InitProcList();

    // simple test to determine whether a processor fits specifications
	int ProcFitsSpecs(int p_ndx,ProcessorID& id);
	int ProcFitsSpecs(ProcLogEntry p_entry,ProcessorID& id);
    int ProcFitsSpecs (int p_ndx);
    int ProcFitsSpecs (ProcLogEntry p_entry);

    virtual void GenerateDetailed( ArctermFile& p_file ) = 0;
    virtual void GenerateSummary( ArctermFile& p_file ) = 0;

    virtual int GetSpecFileType (void) const = 0;
    virtual const char* GetTitle (void) const = 0;

};


#endif // include guard
