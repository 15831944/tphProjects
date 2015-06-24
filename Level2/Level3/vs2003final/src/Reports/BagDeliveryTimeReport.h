// BagDeliveryTimeReport.h: interface for the CBagDeliveryTimeReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGDELIVERYTIMEREPORT_H__CD02CCCA_0D5F_409A_8D64_09D6B3328C33__INCLUDED_)
#define AFX_BAGDELIVERYTIMEREPORT_H__CD02CCCA_0D5F_409A_8D64_09D6B3328C33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
class CBagDeliveryTimeReport : public CBaseReport  
{
public:
	CBagDeliveryTimeReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CBagDeliveryTimeReport();

    virtual int GetReportFileType (void) const { return BagDeliveryTimeReportFile; };
	void ReadSpecifications(const CString& _csProjPath);

protected:

    virtual int GetSpecFileType (void) const { return BagDeliveryTimeSpecFile; };
    virtual const char *GetTitle (void) const { return "Baggage_Delivery_Time_Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );

};

#endif // !defined(AFX_BAGDELIVERYTIMEREPORT_H__CD02CCCA_0D5F_409A_8D64_09D6B3328C33__INCLUDED_)
