// SimAutoReportPara.h: interface for the CSimAutoReportPara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMAUTOREPORTPARA_H__9020B7F4_940E_43EA_A5AD_43FC45C413CB__INCLUDED_)
#define AFX_SIMAUTOREPORTPARA_H__9020B7F4_940E_43EA_A5AD_43FC45C413CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ReportType.h"
#include "SimGeneralPara.h"
#include<vector>
class CSimAutoReportPara : public CSimGeneralPara  
{
	std::vector<ENUM_REPORT_TYPE>m_vReports;
public:
	CSimAutoReportPara( CStartDate _startDate );
	virtual ~CSimAutoReportPara();
public:
	ENUM_REPORT_TYPE operator [] ( int _iIdx );
	ENUM_REPORT_TYPE At ( int _iIdx ) const;

	void AddReport( ENUM_REPORT_TYPE _enReport ){ m_vReports.push_back( _enReport );	};
	bool IfExist( ENUM_REPORT_TYPE _enReport) ;

	int GetReportCount ()const { return m_vReports.size();	};
	virtual void WriteData( ArctermFile& _file );
	virtual void ReadData( ArctermFile& _file ,InputTerminal* _interm );
	virtual CString GetClassType()const { return "AUTOREPORT";	};
	


};

#endif // !defined(AFX_SIMAUTOREPORTPARA_H__9020B7F4_940E_43EA_A5AD_43FC45C413CB__INCLUDED_)
