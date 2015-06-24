// ReportSpecs.h: interface for the CReportSpecs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTSPECS_H__91B9E13F_E5BA_4071_A66C_B3904BBB6110__INCLUDED_)
#define AFX_REPORTSPECS_H__91B9E13F_E5BA_4071_A66C_B3904BBB6110__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reportsdll.h"
//#include "inputs\pax_cnst.h"
#include "inputs\MultiMobConstraint.h"

class REPORTS_TRANSFER CReportSpecs  
{
public:
	CReportSpecs();
	virtual ~CReportSpecs();

public:
	int m_nAreaPortalIdx;
	CMultiMobConstraint m_paxConst;
	COleDateTime m_oleStartTime;
	COleDateTime m_oleEndTime;
	COleDateTime m_oleInterval;
	CString m_strProcessor;
	int		m_nRepType;//Detailed or Summary
	//Added by Xie Bo 2002.4.19
	//For threshold
	BOOL m_bUsedThreshhold;//Used or not used threshold
	float m_fThreshold;//the value of threshold
};

#endif // !defined(AFX_REPORTSPECS_H__91B9E13F_E5BA_4071_A66C_B3904BBB6110__INCLUDED_)
