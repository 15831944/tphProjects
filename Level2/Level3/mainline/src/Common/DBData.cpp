#include "StdAfx.h"
#include ".\dbdata.h"
#include "../Database/ADODatabase.h"

void CDBData::SaveData( int nParentID )
{
	try
	{
		//CADODatabase::BeginTransaction() ;
		CString strSQL;
		if (m_nID < 0)
		{
			GetInsertSQL(nParentID,strSQL);

			if (strSQL.IsEmpty())
				return;

			m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}
		else
		{
			GetUpdateSQL(strSQL);
			if (strSQL.IsEmpty())
				return;

			CADODatabase::ExecuteSQLStatement(strSQL);
		}
		//CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		//CADODatabase::RollBackTransation() ;
		CString s = e.ErrorMessage();
	}
	
}

void CDBData::DeleteData()
{
	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;
   try
   {
	  // CADODatabase::BeginTransaction() ;
	   CADODatabase::ExecuteSQLStatement(strSQL);
	  // CADODatabase::CommitTransaction() ;
   }
   catch (CADOException e)
   {
	  // CADODatabase::RollBackTransation() ;
	   CString s = e.ErrorMessage();
   }
	
	m_nID = -1;
}

void CDBData::ReadData( int nID )
{
	CString strSelectSQL;
	GetSelectSQL(nID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		GetDataFromDBRecordset(adoRecordset);	
	}
}


void CDBData::GetDeleteSQL( CString& strSQL ) const
{	
	strSQL.Format(_T("DELETE FROM %s WHERE ID = %d"), GetTableName().GetString(), GetID() );	
}

void CDBData::GetSelectSQL( int nID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM  %s \
					 WHERE (ID = %d);"), GetTableName().GetString(), nID);
}
