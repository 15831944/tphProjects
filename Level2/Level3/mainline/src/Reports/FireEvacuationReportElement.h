// FireEvacuationReportElement.h: interface for the FireEvacuationReportElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIREEVACUATIONREPORTELEMENT_H__143F5006_AA2B_47B9_A12C_C25339292454__INCLUDED_)
#define AFX_FIREEVACUATIONREPORTELEMENT_H__143F5006_AA2B_47B9_A12C_C25339292454__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reports\rep_pax.h"
#include "common\termfile.h"

class FireEvacuationReportElement : public ReportPaxEntry  
{
protected:
	long	m_lProcessorIndex;	
	long	m_lExitTime;
	long	m_lExitDuration;
	
public:
	FireEvacuationReportElement();
	virtual ~FireEvacuationReportElement();

	// get ...
	long getProcIndex( void ) const { return m_lProcessorIndex;	}
	long getExitTime( void ) const { return m_lExitTime; }
	long getExitDuration( void ) const { return m_lExitDuration;}

	// get data from log
	bool getDataFromPaxLog( void );
	
	// write data to report file
	void writeEntry( ArctermFile& p_file, Terminal* _pTerm ) const;
};

#endif // !defined(AFX_FIREEVACUATIONREPORTELEMENT_H__143F5006_AA2B_47B9_A12C_C25339292454__INCLUDED_)
