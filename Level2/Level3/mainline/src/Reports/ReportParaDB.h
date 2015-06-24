// ReportParaDB.h: interface for the CReportParaDB class.
//
#pragma once

#include "common\dataset.h"
#include "ReportType.h"

class CReportParameter;

class CReportParaDB : public DataSet  
{
public:
	CReportParaDB();
	virtual ~CReportParaDB();
	CReportParameter* operator [] ( int _iIdx );
	CReportParameter* At( int _iIdx );
	CReportParameter* GetReportPara( ENUM_REPORT_TYPE _enReportCategory );

	CReportParameter* GetParameterbyName(const CString& _strParaName ) const;
	CReportParameter* GetParameterbyType(ENUM_REPORT_TYPE _reportType ) const;

	void Add( CReportParameter* _pNewPara ){ m_vAllReportPara.push_back( _pNewPara );}
	void clear();
	virtual void readData( ArctermFile& p_file );
	virtual void writeData( ArctermFile& p_file ) const;
    
	int GetSize()const { return m_vAllReportPara.size();	}
	virtual const char *getTitle () const{ return " Report parameter database";	};
	virtual const char *getHeaders () const{ return "parameter name, parameter data";	};
	virtual void readObsoleteData ( ArctermFile& p_file );

	CReportParameter* BuildReportParam( ENUM_REPORT_TYPE _enumReportType );

private:
	void InitDefaultReportData();
	virtual void initDefaultValues (void);

	std::vector<CReportParameter*> m_vAllReportPara;
};
