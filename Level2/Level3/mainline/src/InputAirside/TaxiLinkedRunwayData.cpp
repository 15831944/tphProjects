#include "StdAfx.h"
#include ".\taxilinkedrunwaydata.h"
#include "../Database/ADODatabase.h"

CTaxiLinkedRunwayData::CTaxiLinkedRunwayData(void)
:m_nID(-1)
,m_nRwyID(-1)
,m_nRwyMark(-1)
,m_dLandingHoldDist(1000)
,m_dTakeoffHoldDist(1000)
,m_tLandingHoldTime(60L)
,m_tTakeoffHoldTime(60L)
{
}

CTaxiLinkedRunwayData::~CTaxiLinkedRunwayData(void)
{
}

void CTaxiLinkedRunwayData::SaveData( int nParentID )
{
	if (m_nID >=0)
	{
		return UpdateData();
	}

	CString SQL ; 

	SQL.Format(_T("INSERT INTO AS_TAXI_LINKEDRUNWAY (PARENTID,RUNWAYID,RUNWAYMARK,LANDINGHOLDDIST,LANDINGHOLDTIME,TAKEOFFHOLDDIST, TAKEOFFHOLDTIME) \
				  VALUES (%d,%d,%d,%d,%d,%d,%d)"),
				  nParentID,m_nRwyID, m_nRwyMark, (int)m_dLandingHoldDist,m_tLandingHoldTime.asMinutes(),(int)m_dTakeoffHoldDist, m_tTakeoffHoldTime.asMinutes()) ;
	try
	{
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}

}

void CTaxiLinkedRunwayData::ReadData( CADORecordset& recordset )
{
	if (recordset.IsEOF())
		return;
	
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("RUNWAYID"), m_nRwyID);
	recordset.GetFieldValue(_T("RUNWAYMARK"), m_nRwyMark);
	int nValue;
	recordset.GetFieldValue(_T("LANDINGHOLDDIST"), nValue);
	m_dLandingHoldDist = (double)nValue;
	long lTime = 0L;
	recordset.GetFieldValue(_T("LANDINGHOLDTIME"), lTime);
	m_tLandingHoldTime = ElapsedTime(lTime*60L);

	recordset.GetFieldValue(_T("TAKEOFFHOLDDIST"), nValue);
	m_dTakeoffHoldDist = nValue;
	recordset.GetFieldValue(_T("TAKEOFFHOLDTIME"), lTime);
	m_tTakeoffHoldTime = ElapsedTime(lTime*60L);

}

void CTaxiLinkedRunwayData::UpdateData()
{
	CString SQL ; 
	SQL.Format(_T("UPDATE AS_TAXI_LINKEDRUNWAY SET RUNWAYID = %d , RUNWAYMARK = %d , LANDINGHOLDDIST = %d ,LANDINGHOLDTIME = %d ,  \
				  TAKEOFFHOLDDIST = %d , TAKEOFFHOLDTIME = %d,  WHERE ID = %d ") ,
				  m_nRwyID, m_nRwyMark, (int)m_dLandingHoldDist,m_tLandingHoldTime.asMinutes(),(int)m_dTakeoffHoldDist, 
				  m_tTakeoffHoldTime.asMinutes(),m_nID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}

void CTaxiLinkedRunwayData::DeleteData()
{
	if (m_nID <0)
		return;

	CString SQL ;
	SQL.Format(_T("DELETE * FROM AS_TAXI_LINKEDRUNWAY WHERE ID = %d") , m_nID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}

double CTaxiLinkedRunwayData::GetLandingHoldDist()
{
	return m_dLandingHoldDist;
}

double CTaxiLinkedRunwayData::GetTakeoffHoldDist()
{
	return m_dTakeoffHoldDist;
}

ElapsedTime CTaxiLinkedRunwayData::GetLandingHoldTime()
{
	return m_tLandingHoldTime;
}

ElapsedTime CTaxiLinkedRunwayData::GetTakeoffHoldTime()
{
	return m_tTakeoffHoldTime;
}

int CTaxiLinkedRunwayData::GetLinkRwyID()
{
	return m_nRwyID;
}

int CTaxiLinkedRunwayData::GetLinkRwyMark()
{
	return m_nRwyMark;
}