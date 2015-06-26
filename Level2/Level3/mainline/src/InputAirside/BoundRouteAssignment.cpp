#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\boundrouteassignment.h"
#include "..\Common\AirportDatabase.h"
#include "PostDeice_BoundRoute.h"

/////////////////////////////CTimeRangeRouteAssignment/////////////////////////////////
CTimeRangeRouteAssignment::CTimeRangeRouteAssignment()
: m_nRouteAssignmentStatus(BOUNDROUTEASSIGNMENT_SHORTPATH)
{
}

CTimeRangeRouteAssignment::~CTimeRangeRouteAssignment(void)
{
}

void CTimeRangeRouteAssignment::SetRouteType(BOUNDROUTETYPE eType)
{
	m_eType = eType;
}

BOUNDROUTETYPE CTimeRangeRouteAssignment::GetRouteType()
{
	return m_eType;
}

void CTimeRangeRouteAssignment::SetRouteAssignmentStatus(BOUNDROUTEASSIGNMENTSTATUS raStatus)
{
	m_nRouteAssignmentStatus = raStatus;
}

BOUNDROUTEASSIGNMENTSTATUS CTimeRangeRouteAssignment::GetRouteAssignmentStatus()
{
	return m_nRouteAssignmentStatus;
}

//DBElementCollection
void CTimeRangeRouteAssignment::SaveData(int nParentID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID, strSQL);

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

	for (std::vector<CIn_OutBoundRoute *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetRouteAssignmentID(m_nID);
		(*iter)->SaveData(m_nID);
	}

	for (std::vector<CIn_OutBoundRoute *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CTimeRangeRouteAssignment::DeleteItemFromIndex(size_t nIndex)
{
	if(nIndex < GetElementCount())
	{
		m_deleteddataList.push_back(*(m_dataList.begin() + nIndex));
		m_dataList.erase(m_dataList.begin() + nIndex);
	}
}

void CTimeRangeRouteAssignment::DeleteItem(const CIn_OutBoundRoute* dbElement)
{
	std::vector<CIn_OutBoundRoute *>::iterator iter = std::find(m_dataList.begin(),m_dataList.end(),dbElement);
	if (iter != m_dataList.end())
	{		
		m_deleteddataList.push_back(*iter);
		m_dataList.erase(iter);

	}
}

//DBElement
void CTimeRangeRouteAssignment::InitFromDBRecordset(CADORecordset& recordset)
{
	int nRouteAssignmentStatus;

	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("ROUTEASSIGNMENT"), nRouteAssignmentStatus);
	long lTime;
	recordset.GetFieldValue(_T("STARTTIME"), lTime);
	m_tStartTime = ElapsedTime(lTime);
	recordset.GetFieldValue(_T("ENDTIME"),lTime);
	m_tEndTime = ElapsedTime(lTime);

	m_nRouteAssignmentStatus = (BOUNDROUTEASSIGNMENTSTATUS)(nRouteAssignmentStatus);

	if (m_nRouteAssignmentStatus == BOUNDROUTEASSIGNMENT_SHORTPATH && m_eType != OUTBOUND/*make outbound an exception*/)
		return;

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * FROM IN_OUTBOUNDROUTE \
						   WHERE (OUTBOUNDROUTEASSIGNMENTID = %d AND ROUTETYPE = %d)"),
						   m_nID, (int)m_eType);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	if (m_eType == INBOUND)
	{
		while (!adoRecordset.IsEOF())
		{
			CIn_OutBoundRoute *pInboundRoute = new CIn_BoundRoute();
			pInboundRoute->SetRouteAssignmentID(m_nID);
			pInboundRoute->InitFromDBRecordset(adoRecordset);

			AddNewItem(pInboundRoute);

			adoRecordset.MoveNextData();
		}
	}

	if (m_eType == OUTBOUND)
	{
		while (!adoRecordset.IsEOF())
		{
			CIn_OutBoundRoute *pOutboundRoute = new COut_BoundRoute();
			pOutboundRoute->InitFromDBRecordset(adoRecordset);

			AddNewItem(pOutboundRoute);

			adoRecordset.MoveNextData();
		}
	}
	if(m_eType == DEICING)
	{
		while (!adoRecordset.IsEOF())
		{
			CIn_OutBoundRoute *pDeiceRoute = new CDeice_BoundRoute();
			pDeiceRoute->InitFromDBRecordset(adoRecordset);

			AddNewItem(pDeiceRoute);

			adoRecordset.MoveNextData();
		}
	}
	if(m_eType == POSTDEICING)
	{
		while (!adoRecordset.IsEOF())
		{
			CPostDeice_BoundRoute *pPostDeiceRoute = new CPostDeice_BoundRoute();
			pPostDeiceRoute->InitFromDBRecordset(adoRecordset);

			AddNewItem(pPostDeiceRoute);
			adoRecordset.MoveNextData();
		}
	}
}

void CTimeRangeRouteAssignment::GetInsertSQL(int nParentID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTEASSIGNMENT\
					 (PROJID, ROUTEASSIGNMENT,STARTTIME, ENDTIME, PARENTID,ROUTETYPE)\
					 VALUES (%d, %d, %d, %d, %d, %d)"),\
					 -1,(int)m_nRouteAssignmentStatus,m_tStartTime.asSeconds(),m_tEndTime.asSeconds(), nParentID, (int)m_eType);
}

void CTimeRangeRouteAssignment::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTEASSIGNMENT\
					 SET ROUTEASSIGNMENT = %d,STARTTIME = %d, ENDTIME = %d  \
					 WHERE (ID =%d)"),\
					 (int)m_nRouteAssignmentStatus,m_tStartTime.asSeconds(),m_tEndTime.asSeconds(), m_nID);
}

void CTimeRangeRouteAssignment::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_OUTBOUNDROUTEASSIGNMENT\
					 WHERE (ID = %d)"),m_nID);
}

void CTimeRangeRouteAssignment::SetStartTime(ElapsedTime& tStart)
{
	m_tStartTime = tStart;
}

ElapsedTime CTimeRangeRouteAssignment::GetStartTime()
{
	return m_tStartTime;
}

void CTimeRangeRouteAssignment::SetEndTime(ElapsedTime& tEnd)
{
	m_tEndTime = tEnd;
}

ElapsedTime CTimeRangeRouteAssignment::GetEndTime()
{
	return m_tEndTime;
}

void CTimeRangeRouteAssignment::DeleteData()
{
	if (m_nID < 0)
		return;

	for (std::vector<CIn_OutBoundRoute *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}
	m_dataList.clear();

	for (std::vector<CIn_OutBoundRoute *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}
	m_deleteddataList.clear();

	CString strSQL;
	GetDeleteSQL(strSQL);

	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

bool CTimeRangeRouteAssignment::IsInTimeRange(ElapsedTime tTime)
{
	return (m_tStartTime <= tTime && m_tEndTime >= tTime);
}
///////////////////////////////CFlightTypeRouteAssignment/////////////////////////////////////////////
CFlightTypeRouteAssignment::CFlightTypeRouteAssignment()
: m_pAirportDB(NULL)
{
}

CFlightTypeRouteAssignment::~CFlightTypeRouteAssignment(void)
{
}

void CFlightTypeRouteAssignment::SetAirportDB(CAirportDatabase* pDB)
{
	m_pAirportDB = pDB;
}

void CFlightTypeRouteAssignment::SetRouteType(BOUNDROUTETYPE eType)
{
	m_eType = eType;
}

BOUNDROUTETYPE CFlightTypeRouteAssignment::GetRouteType()
{
	return m_eType;
}

//DBElementCollection
void CFlightTypeRouteAssignment::SaveData(int nParentID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID, strSQL);

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

	for (std::vector<CTimeRangeRouteAssignment *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	for (std::vector<CTimeRangeRouteAssignment *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();
}

void CFlightTypeRouteAssignment::DeleteItemFromIndex(size_t nIndex)
{
	if(nIndex < GetElementCount() && nIndex >0)
	{
		m_deleteddataList.push_back(*(m_dataList.begin() + nIndex));
		m_dataList.erase(m_dataList.begin() + nIndex);
	}
}

void CFlightTypeRouteAssignment::DeleteItem(const CTimeRangeRouteAssignment* dbElement)
{
	std::vector<CTimeRangeRouteAssignment *>::iterator iter = std::find(m_dataList.begin(),m_dataList.end(),dbElement);
	if (iter != m_dataList.end())
	{		
		m_deleteddataList.push_back(*iter);
		m_dataList.erase(iter);

	}
}

//DBElement
void CFlightTypeRouteAssignment::InitFromDBRecordset(CADORecordset& recordset)
{

	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("FLIGHTTY"), m_strFltType);


	if (m_pAirportDB)
	{
		m_FltType.SetAirportDB(m_pAirportDB);
		m_FltType.setConstraintWithVersion(m_strFltType);
	}

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT ID, ROUTEASSIGNMENT,STARTTIME, ENDTIME \
						   FROM IN_OUTBOUNDROUTEASSIGNMENT \
						   WHERE PARENTID = %d AND ROUTETYPE = %d"), m_nID, (int)m_eType);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CTimeRangeRouteAssignment *pTimeRoute = new CTimeRangeRouteAssignment();
		pTimeRoute->SetRouteType(m_eType);
		pTimeRoute->InitFromDBRecordset(adoRecordset);

		AddNewItem(pTimeRoute);

		adoRecordset.MoveNextData();
	}
}

void CFlightTypeRouteAssignment::GetInsertSQL(int nProjID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO INBOUDROUTEASSIGN_FLIGHTTY( FLIGHTTY,CLASSTY,PROID)VALUES('%s',%d,%d)"),
		m_strFltType,
		(int)m_eType,
		nProjID) ;
}

void CFlightTypeRouteAssignment::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE INBOUDROUTEASSIGN_FLIGHTTY SET FLIGHTTY = '%s' WHERE ID = %d"),
		m_strFltType,
		m_nID);
}

void CFlightTypeRouteAssignment::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM INBOUDROUTEASSIGN_FLIGHTTY\
					 WHERE (ID = %d)"),m_nID);
}

void CFlightTypeRouteAssignment::SetFltType(const CString& strFltType)
{
	m_strFltType.Format(_T("%s"), strFltType);

	if (m_pAirportDB)
	{
		m_FltType.SetAirportDB(m_pAirportDB);
		m_FltType.setConstraintWithVersion(m_strFltType);
	}

}

CString& CFlightTypeRouteAssignment::GetFltTypeName()
{
	return m_strFltType;
}

FlightConstraint& CFlightTypeRouteAssignment::GetFltType()
{
	return m_FltType;
}

void CFlightTypeRouteAssignment::DeleteData()
{
	if (m_nID < 0)
		return;

	for (std::vector<CTimeRangeRouteAssignment *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
	m_dataList.clear();

	for (std::vector<CTimeRangeRouteAssignment *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
	m_deleteddataList.clear();

	CString strSQL;
	GetDeleteSQL(strSQL);

	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

//////////////////////////////////////////////CBoundRouteAssignment///////////////////////////////////
CBoundRouteAssignment::CBoundRouteAssignment(BOUNDROUTETYPE eType,CAirportDatabase* pAirportDB)
: m_eType(eType)
, m_pAirportDB(pAirportDB)
{
}

CBoundRouteAssignment::~CBoundRouteAssignment(void)
{
}

void CBoundRouteAssignment::SetRouteType(BOUNDROUTETYPE eType)
{
	m_eType = eType;
}

BOUNDROUTETYPE CBoundRouteAssignment::GetRouteType()
{
	return m_eType;
}

//DBElementCollection
void CBoundRouteAssignment::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, FLIGHTTY  \
					 FROM INBOUDROUTEASSIGN_FLIGHTTY  \
					 WHERE PROID =%d AND CLASSTY = %d"), nProjectID, (int)m_eType);
}

void CBoundRouteAssignment::ReadData(int nProjectID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nProjectID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CFlightTypeRouteAssignment *pFlightTypeRoute = new CFlightTypeRouteAssignment();
		pFlightTypeRoute->SetRouteType(m_eType);
		pFlightTypeRoute->SetAirportDB(m_pAirportDB);
		pFlightTypeRoute->InitFromDBRecordset(adoRecordset);

		AddNewItem(pFlightTypeRoute);

		adoRecordset.MoveNextData();
	}
}

void CBoundRouteAssignment::SaveData(int nProjectID)
{
	for (std::vector<CFlightTypeRouteAssignment *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SaveData(nProjectID);
	}

	for (std::vector<CFlightTypeRouteAssignment *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();
}

void CBoundRouteAssignment::DeleteItemFromIndex(size_t nIndex)
{
	if(nIndex < GetElementCount())
	{
		m_deleteddataList.push_back(*(m_dataList.begin() + nIndex));
		m_dataList.erase(m_dataList.begin() + nIndex);
	}
}

void CBoundRouteAssignment::DeleteItem(const CFlightTypeRouteAssignment* dbElement)
{
	std::vector<CFlightTypeRouteAssignment *>::iterator iter = std::find(m_dataList.begin(),m_dataList.end(),dbElement);
	if (iter != m_dataList.end())
	{		
		m_deleteddataList.push_back(*iter);
		m_dataList.erase(iter);
	}
}

