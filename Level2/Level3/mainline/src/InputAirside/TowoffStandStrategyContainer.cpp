#include "StdAfx.h"
#include ".\towoffstandstrategycontainer.h"
#include "Database\ADODatabase.h"
TowoffStandStrategy::TowoffStandStrategy()
:m_towoffStandAssignList(ContentNonNull)
{

}

TowoffStandStrategy::~TowoffStandStrategy()
{

}

void TowoffStandStrategy::SetStrategyName( const CString& sName )
{
	m_strName = sName;
}

const CString& TowoffStandStrategy::GetStrategyName() const
{
	return m_strName;
}

CString TowoffStandStrategy::GetTableName() const
{
	return _T("TOWOFF_STANDASSIGN_DATA");
}

void TowoffStandStrategy::GetDataFromDBRecordset( CADORecordset& recordset )
{
	if (recordset.IsEOF())
		return;
	
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("STRATEGYNAME"),m_strName);

	m_towoffStandAssignList.ReadData(m_nID);
}

bool TowoffStandStrategy::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO TOWOFF_STANDASSIGN_DATA (STRATEGYNAME) VALUES ('%s')"),m_strName);
	return true;
}

bool TowoffStandStrategy::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE TOWOFF_STANDASSIGN_DATA SET STRATEGYNAME = '%s' WHERE ID = %d"),
		m_strName,m_nID);
	return true;
}

void TowoffStandStrategy::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM TOWOFF_STANDASSIGN_DATA WHERE ID = %d"),m_nID);
}

void TowoffStandStrategy::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(FALSE);
}

void TowoffStandStrategy::SaveData( int nParentID )
{
	CDBData::SaveData(nParentID);
	m_towoffStandAssignList.SaveData(m_nID);
}

void TowoffStandStrategy::DeleteData()
{
	m_towoffStandAssignList.DeleteData();
	CDBData::DeleteData();
}

CTowOffStandAssignmentDataList* TowoffStandStrategy::GetTowoffStandDataList()
{
	return &m_towoffStandAssignList;
}

void TowoffStandStrategy::SetAirportDatabase( CAirportDatabase* pAirportDB )
{
	m_towoffStandAssignList.SetAirportDatabase(pAirportDB);
}

TowoffStandStrategyContainer::TowoffStandStrategyContainer(void)
:m_nSelectStrategy(0)
,m_pAirportDatabase(NULL)
{
}

TowoffStandStrategyContainer::~TowoffStandStrategyContainer(void)
{
}

void TowoffStandStrategyContainer::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM TOWOFF_STANDASSIGN_DATA"));
}

void TowoffStandStrategyContainer::ReadData( int nParentID )
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM TOWOFF_STANDASSIGN_STRATEGY"));

	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if (!rs.IsEOF())
	{
		rs.GetFieldValue(_T("STRATEGYID"),m_nSelectStrategy);
	}

	CDBDataCollection<TowoffStandStrategy>::ReadData(nParentID);
	int nCount = GetItemCount();
	for (int i=0;i<nCount;i++)
	{
		GetItem(i)->SetAirportDatabase(m_pAirportDatabase);
	}
}

TowoffStandStrategy* TowoffStandStrategyContainer::GetActiveStrategy()
{
	int nCount = GetItemCount();

	if (m_nSelectStrategy < 0 || m_nSelectStrategy >= nCount)
	{
		return NULL;
	}

	return GetItem(m_nSelectStrategy);
}

void TowoffStandStrategyContainer::SaveData( int nParentID )
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM TOWOFF_STANDASSIGN_STRATEGY"));

	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if (!rs.IsEOF())
	{
		CString strUpdateSQL;
		strUpdateSQL.Format(_T("UPDATE TOWOFF_STANDASSIGN_STRATEGY SET STRATEGYID = %d"),m_nSelectStrategy);
		CADODatabase::ExecuteSQLStatement(strUpdateSQL);
	}
	else
	{
		CString strInsertSQL;
		strInsertSQL.Format(_T("INSERT INTO TOWOFF_STANDASSIGN_STRATEGY (STRATEGYID) VALUES (%d)"),m_nSelectStrategy);
		CADODatabase::ExecuteSQLStatement(strInsertSQL);
	}
	

	CDBDataCollection<TowoffStandStrategy>::SaveData(nParentID);
}

void TowoffStandStrategyContainer::SetSelectStrategy( int Sel )
{
	m_nSelectStrategy = Sel;
}

int TowoffStandStrategyContainer::GetSelectStrategy() const
{
	return m_nSelectStrategy;
}

void TowoffStandStrategyContainer::SetAirportDatabase( CAirportDatabase* pAirportDB )
{	
	m_pAirportDatabase = pAirportDB;
}
