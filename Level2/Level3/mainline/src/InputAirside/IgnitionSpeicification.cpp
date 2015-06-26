#include "StdAfx.h"
#include ".\ignitionspeicification.h"
#include "../Database/ADODatabase.h"
#include "../Common/ALTObjectID.h"
#include "../Common/AirportDatabase.h"

FlightIgnitionData::FlightIgnitionData(void)
:m_nID(-1)
,m_nDistance(0)
{
}

FlightIgnitionData::~FlightIgnitionData(void)
{
}

void FlightIgnitionData::ReadData(CADORecordset& recordset, CAirportDatabase* pAirportDB)
{
	if (recordset.IsEOF())
		return;

	CString strFlightType;
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("FLTTYPE",strFlightType);
	m_FltType.SetAirportDB(pAirportDB);
	m_FltType.setConstraintWithVersion(strFlightType);
	int nType;
	recordset.GetFieldValue("ENGINESTARTTYPE", nType);
	recordset.GetFieldValue("DISTANCE", m_nDistance);
	m_eEngineStartType = (EngineStartType)nType;
}

void FlightIgnitionData::SaveData(int nParentID)
{
	if (m_nID >0)
	{
		UpdateData();
	}
	else
	{
		CString strSQL = _T("");
		char szFltType[1024] = {0};
		m_FltType.WriteSyntaxStringWithVersion(szFltType);
		strSQL.Format(_T("INSERT INTO AS_FLIGHTIGNITIONDATA(PARENTID,FLTTYPE,ENGINESTARTTYPE,DISTANCE) VALUES (%d, '%s', %d, %d)"),		\
			nParentID, szFltType,(int)m_eEngineStartType, m_nDistance);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

}

void FlightIgnitionData::UpdateData()
{
	CString strSQL = _T("");
	char szFltType[1024] = {0};
	m_FltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("UPDATE AS_FLIGHTIGNITIONDATA SET FLTTYPE ='%s', ENGINESTARTTYPE = %d, DISTANCE = %d WHERE ID = %d"),	\
		szFltType,(int)m_eEngineStartType,m_nDistance, m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FlightIgnitionData::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AS_FLIGHTIGNITIONDATA WHERE ID = %d"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

/////////////////////////////////////////////////////////////////////////////
IgnitionData::IgnitionData(int nPrjID)
:m_nID(-1)
,m_nPrjID(nPrjID)
{
	m_vDataList.clear();
	m_vDelDataList.clear();
}

IgnitionData::~IgnitionData(void)
{
	std::vector<FlightIgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter != m_vDataList.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vDataList.clear();

	iter = m_vDelDataList.begin();
	for (; iter != m_vDelDataList.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vDelDataList.clear();
}

const FlightIgnitionData* IgnitionData::GetData(int nIdx)
{
	if (nIdx <0 || nIdx > GetDataCount() -1)
		return NULL;

	return m_vDataList.at(nIdx);
}

void IgnitionData::ReadData(CADORecordset& recordset, CAirportDatabase* pAirportDB)
{
	if (recordset.IsEOF())
		return;

	recordset.GetFieldValue("ID", m_nID);
	CString strStandName;
	recordset.GetFieldValue("STANDNAME",strStandName);
	ALTObjectID standID(strStandName.GetString());
	m_StandGroup.setName(standID);

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AS_FLIGHTIGNITIONDATA WHERE PARENTID = %d;"),m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		FlightIgnitionData* pData = new FlightIgnitionData;
		pData->ReadData(adoRecordset, pAirportDB);
		m_vDataList.push_back(pData);
		adoRecordset.MoveNextData();
	}
}

void IgnitionData::SaveData()
{
	if (m_nID >0)
	{
		UpdateData();
	}
	else
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO AS_IGNITIONSPECIFICATION(STANDNAME) VALUES ('%s')"),m_StandGroup.getName().GetIDString());
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<FlightIgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter !=m_vDataList.end(); iter++)
	{
		(*iter)->SaveData(m_nID);	
	}

	iter = m_vDelDataList.begin();
	for (; iter !=m_vDelDataList.end(); iter++)
	{
		if ((*iter)->getID()>=0)
			(*iter)->DeleteData();

		delete *iter;
		*iter = NULL;

	}
	m_vDelDataList.clear();
}

void IgnitionData::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AS_IGNITIONSPECIFICATION SET STANDNAME ='%s'WHERE ID = %d"),\
		m_StandGroup.getName().GetIDString(),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void IgnitionData::DeleteData()
{
	std::vector<FlightIgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter !=m_vDataList.end(); iter++)
	{
		(*iter)->DeleteData();
	}

	iter = m_vDelDataList.begin();
	for (; iter !=m_vDelDataList.end(); iter++)
	{
		if ((*iter)->getID()>=0)
			(*iter)->DeleteData();

		delete *iter;
		*iter = NULL;

	}
	m_vDelDataList.clear();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AS_IGNITIONSPECIFICATION WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void IgnitionData::DelData(FlightIgnitionData* pData)
{
	if (pData == NULL)
		return;

	std::vector<FlightIgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter != m_vDataList.end(); iter++)
	{
		if (*iter == pData)
		{
			m_vDelDataList.push_back(*iter);
			m_vDataList.erase(iter);
			return;
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
IgnitionSpeicification::IgnitionSpeicification(void)
{
	m_vDataList.clear();
	m_vDelDataList.clear();
}

IgnitionSpeicification::~IgnitionSpeicification(void)
{
	std::vector<IgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter != m_vDataList.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vDataList.clear();

	iter = m_vDelDataList.begin();
	for (; iter != m_vDelDataList.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vDelDataList.clear();
}

void IgnitionSpeicification::ReadData(int nPrjID, CAirportDatabase* pAirportDB)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AS_IGNITIONSPECIFICATION;"));

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		IgnitionData* pData = new IgnitionData(nPrjID);
		pData->ReadData(adoRecordset, pAirportDB);
		m_vDataList.push_back(pData);
		adoRecordset.MoveNextData();
	}
}

void IgnitionSpeicification::SaveData()
{
	std::vector<IgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter !=m_vDataList.end(); iter++)
	{
		(*iter)->SaveData();	
	}

	iter = m_vDelDataList.begin();
	for (; iter !=m_vDelDataList.end(); iter++)
	{
		if ((*iter)->getID()>=0)
			(*iter)->DeleteData();

		delete *iter;
		*iter = NULL;

	}
	m_vDelDataList.clear();
}

void IgnitionSpeicification::DelData(IgnitionData* pData)
{
	if (pData == NULL)
		return;

	std::vector<IgnitionData*>::iterator iter = m_vDataList.begin();
	for (; iter != m_vDataList.end(); iter++)
	{
		if (*iter == pData)
		{
			m_vDelDataList.push_back(*iter);
			m_vDataList.erase(iter);
			return;
		}
	}
}

const IgnitionData* IgnitionSpeicification::GetData(int nIdx)
{
	if (nIdx <0 || nIdx > GetDataCount() -1)
		return NULL;

	return m_vDataList.at(nIdx);
}