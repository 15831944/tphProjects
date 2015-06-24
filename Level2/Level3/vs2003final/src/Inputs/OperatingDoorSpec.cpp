#include "StdAfx.h"
#include ".\operatingdoorspec.h"
#include "../Database/ADODatabase.h"
#include "../InputAirside/ALTObjectGroup.h"


StandOperatingDoorData::StandOperatingDoorData( void )
:m_nID(-1)
,m_nStandID(-2)
,m_pAirportDB(NULL)
{
	m_vOpDoors.clear();
}

StandOperatingDoorData::~StandOperatingDoorData( void )
{

}

int StandOperatingDoorData::GetID()
{
	return m_nID;
}

void StandOperatingDoorData::SetID( int nID )
{
	m_nID = nID;
}

const ALTObjectID& StandOperatingDoorData::GetStandName()
{
	return m_standName;
}

int StandOperatingDoorData::GetOpDoorCount()
{
	return m_vOpDoors.size();
}

int StandOperatingDoorData::GetOpDoorID(int nIdx)
{
	if(nIdx<0 || nIdx >= GetOpDoorCount())
		return -1;

	return m_vOpDoors.at(nIdx);
}

void StandOperatingDoorData::AddOpDoor( int nID )
{
	m_vOpDoors.push_back(nID);
}

void StandOperatingDoorData::ReadData( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nParentID;
	recordset.GetFieldValue(_T("PARENTID"),nParentID);
	recordset.GetFieldValue(_T("STANDID"),m_nStandID);
	CString strDoors;
	recordset.GetFieldValue(_T("OPDOORS"),strDoors);

	ALTObjectGroup standGroup;
	standGroup.ReadData(m_nStandID);
	m_standName = standGroup.getName();

	int nDoorID;
	int nIdx0 =0;
	int nIdx =0;
	while(nIdx >=0 && nIdx <strDoors.GetLength()-1)
	{
		nIdx = strDoors.Find(',',nIdx0);
		nDoorID = atoi(strDoors.Mid(nIdx0,nIdx));
		if (nDoorID >=0)
			m_vOpDoors.push_back(nDoorID);
		nIdx0 = nIdx+1;
	}
}

void StandOperatingDoorData::SaveData( int nParentID )
{
	ALTObjectGroup standGroup;

	if (m_nStandID >=0)
	{
		standGroup.ReadData(m_nStandID);
		standGroup.DeleteData();
	}
		
	standGroup.setName(m_standName);
	standGroup.setID(-1);
	standGroup.SaveData(1);
	m_nStandID = standGroup.getID();

	CString strDoors = "";	
	CString strName;
	int nCount = m_vOpDoors.size();
	for (int i =0; i < nCount; i++)
	{
		strName.Format("%d,", m_vOpDoors.at(i));
		strDoors += strName;
	}

	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO TB_STANDDOOROPERATING\
						 (PARENTID,STANDID,OPDOORS)\
						 VALUES (%d,%d,'%s')"),nParentID,m_nStandID,strDoors);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
		UpdateData(nParentID, strDoors);
}

void StandOperatingDoorData::UpdateData( int nParentID ,const CString& strDoors)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TB_STANDDOOROPERATING\
					 SET PARENTID =%d, STANDID = %d,OPDOORS = '%s'\
					 WHERE (ID = %d)"), nParentID,  m_nStandID, strDoors, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void StandOperatingDoorData::DeleteData()
{
	ALTObjectGroup standGroup;

	if (m_nStandID >=0)
	{
		standGroup.ReadData(m_nStandID);
		standGroup.DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TB_STANDDOOROPERATING\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

int StandOperatingDoorData::GetStandID()
{
	return m_nStandID;
}

void StandOperatingDoorData::SetStandName( const ALTObjectID& altName )
{
	m_standName = altName;
}

void StandOperatingDoorData::DelOpDoor( int nID )
{
	std::vector<int>::iterator iter = std::find(m_vOpDoors.begin(),m_vOpDoors.end(),nID);
	if (iter != m_vOpDoors.end())
		m_vOpDoors.erase(iter);
	
}

void StandOperatingDoorData::GetOpDoors( std::vector<int>& vDoors )
{
	vDoors.assign(m_vOpDoors.begin(),m_vOpDoors.end());
}
///////////////////////////////////////////////////////////////////
FltOperatingDoorData::FltOperatingDoorData( CAirportDatabase* pAirportDatabase )
:m_nID(-1)
,m_pAirportDB(pAirportDatabase)
{
	m_vDelStandOpDoorData.clear();
	m_vStandOpDoorData.clear();
}

FltOperatingDoorData::~FltOperatingDoorData( void )
{
	std::vector<StandOperatingDoorData*>::iterator iter = m_vStandOpDoorData.begin();
	while(iter != m_vStandOpDoorData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vStandOpDoorData.clear();

	iter = m_vDelStandOpDoorData.begin();
	while(iter != m_vDelStandOpDoorData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vDelStandOpDoorData.clear();
}

int FltOperatingDoorData::GetID()
{
	return m_nID;
}

void FltOperatingDoorData::SetID( int nID )
{
	m_nID = nID;
}

const FlightConstraint& FltOperatingDoorData::GetFltType()
{
	return m_FltType;
}

void FltOperatingDoorData::SetFltType( const FlightConstraint& fltType )
{
	m_FltType = fltType;
}

int FltOperatingDoorData::GetStandDoorCount()
{
	return m_vStandOpDoorData.size();
}

StandOperatingDoorData* FltOperatingDoorData::GetStandDoorData( int nIdx )
{
	if(nIdx <0 || nIdx >= GetStandDoorCount())
		return NULL;

	return m_vStandOpDoorData.at(nIdx);
}

void FltOperatingDoorData::AddStandDoor( StandOperatingDoorData* pData )
{
	m_vStandOpDoorData.push_back(pData);
}

void FltOperatingDoorData::ReadData( CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	CString strFltType;
	adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);

	if (m_pAirportDB)
	{
		m_FltType.SetAirportDB(m_pAirportDB);
		m_FltType.setConstraintWithVersion(strFltType);
	}

	CString strSQL;
	strSQL.Format(_T("SELECT *  FROM  TB_STANDDOOROPERATING\
					 WHERE (PARENTID = %d)"),m_nID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			StandOperatingDoorData* pData = new StandOperatingDoorData;
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vStandOpDoorData.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void FltOperatingDoorData::SaveData()
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_FltType.WriteSyntaxStringWithVersion(szFltType);
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO TB_FLTDOOROPERATING ( FLTTYPE )   \
						 VALUES ('%s')"),szFltType);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
		UpdateData();

	std::vector<StandOperatingDoorData*>::iterator iter = m_vStandOpDoorData.begin();

	for (;iter != m_vStandOpDoorData.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	for(iter = m_vDelStandOpDoorData.begin(); iter != m_vDelStandOpDoorData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vDelStandOpDoorData.clear();
}

void FltOperatingDoorData::UpdateData()
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_FltType.WriteSyntaxStringWithVersion(szFltType);

	strSQL = _T("");
	strSQL.Format(_T("UPDATE TB_FLTDOOROPERATING\
					 SET FLTTYPE = '%s' WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}

void FltOperatingDoorData::DeleteData()
{
	std::vector<StandOperatingDoorData*>::iterator iter  = m_vStandOpDoorData.begin();
	for (; iter != m_vStandOpDoorData.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vDelStandOpDoorData.begin(); iter != m_vDelStandOpDoorData.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TB_FLTDOOROPERATING\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FltOperatingDoorData::DelStandData( StandOperatingDoorData* pData )
{
	std::vector<StandOperatingDoorData*>::iterator iter = std::find(m_vStandOpDoorData.begin(),m_vStandOpDoorData.end(),pData);
	if (iter != m_vStandOpDoorData.end())
	{
		m_vDelStandOpDoorData.push_back(*iter);
		m_vStandOpDoorData.erase(iter);
	}
}
///////////////////////////////////////////////////////////////////
OperatingDoorSpec::OperatingDoorSpec(void)
:m_pAirportDB(NULL)
{
}

OperatingDoorSpec::~OperatingDoorSpec(void)
{
	std::vector<FltOperatingDoorData*>::iterator iter = m_vFltData.begin();
	while(iter != m_vFltData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vFltData.clear();

	iter = m_vDelFltData.begin();
	while(iter != m_vDelFltData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vDelFltData.clear();
}

int OperatingDoorSpec::GetFltDataCount()
{
	return m_vFltData.size();
}

FltOperatingDoorData* OperatingDoorSpec::GetFltDoorData( int nIdx )
{
	if (nIdx <0 || nIdx >= GetFltDataCount())
		return NULL;
		
	return m_vFltData.at(nIdx);
}

void OperatingDoorSpec::AddFlightOpDoor( FltOperatingDoorData* pData )
{
	m_vFltData.push_back(pData);
}

void OperatingDoorSpec::ReadData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM TB_FLTDOOROPERATING"));
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			FltOperatingDoorData* pData = new FltOperatingDoorData(m_pAirportDB);
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vFltData.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void OperatingDoorSpec::SaveData()
{
	std::vector<FltOperatingDoorData*>::iterator iter  = m_vFltData.begin();
	for (; iter != m_vFltData.end(); ++iter)
	{
		(*iter)->SaveData();
	}

	for (iter = m_vDelFltData.begin(); iter != m_vDelFltData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vDelFltData.clear();
}

void OperatingDoorSpec::DelFltData( FltOperatingDoorData* pData )
{
	std::vector<FltOperatingDoorData*>::iterator iter = std::find(m_vFltData.begin(),m_vFltData.end(),pData);
	if (iter != m_vFltData.end())
	{
		m_vDelFltData.push_back(*iter);
		m_vFltData.erase(iter);
	}
}