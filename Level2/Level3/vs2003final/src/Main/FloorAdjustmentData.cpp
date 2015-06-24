#include "StdAfx.h"
#include ".\flooradjustmentdata.h"
#include "../Database/ADODatabase.h"

double CFloorAdjustmentData::ReadDataFromDataBase()
{
	CString SQL ;
	double val = 0 ;
	SQL.Format(_T("SELECT * FROM TB_FLOORADJUSTMENT")) ;
	CADORecordset recordset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
	}
	while(!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("SCALEVALUE"),val) ;
		break ;
	}
	return val ;
}
void CFloorAdjustmentData::WriteDataToDataBase(double _val)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_FLOORADJUSTMENT")) ;
	try
	{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
		SQL.Format(_T("INSERT INTO TB_FLOORADJUSTMENT (SCALEVALUE) VALUES(%0.2f)"),_val) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation() ;
		e.ErrorMessage() ;
	}

}