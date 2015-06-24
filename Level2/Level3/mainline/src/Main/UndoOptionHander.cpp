#include "StdAfx.h"
#include ".\undooptionhander.h"

CUndoOptionHander::CUndoOptionHander(void)
{
}

CUndoOptionHander::~CUndoOptionHander(void)
{
}
BOOL CUndoOptionHander::ReadData(int& _length , int& _time)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_PROJECT_OPTION ")) ;
	CADORecordset DataSet ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,DataSet,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return FALSE;
	}
	while(!DataSet.IsEOF())
	{
		DataSet.GetFieldValue(_T("UNDO_LENGTH"),_length) ;
		DataSet.GetFieldValue(_T("UNDO_TIME"),_time) ;
		return TRUE ;
	}
	return FALSE ;
}
BOOL CUndoOptionHander::SaveData(int _length , int _time)
{

	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_PROJECT_OPTION")) ;
		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	    SQL.Format(_T("INSERT INTO TB_PROJECT_OPTION (UNDO_LENGTH,UNDO_TIME) VALUES(%d,%d)"),_length,_time) ;
		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	return TRUE ;
}