#include "StdAfx.h"
#include ".\followmecarservicemeetingpoints.h"
#include "..\Database\ADODatabase.h"

FollowMeCarServiceMeetingPoints::FollowMeCarServiceMeetingPoints(void)
{
	m_nID= -1;
}

FollowMeCarServiceMeetingPoints::~FollowMeCarServiceMeetingPoints(void)
{
}

int FollowMeCarServiceMeetingPoints::GetServiceMeetingPointCount()
{
	return (int)m_vMeetingPoints.size();
}

ALTObjectID FollowMeCarServiceMeetingPoints::GetServiceMeetingPoint( int idx )
{
	if (idx >-1 && idx < (int)m_vMeetingPoints.size())
		return m_vMeetingPoints.at(idx);

	ALTObjectID objName;

	return objName;
}

void FollowMeCarServiceMeetingPoints::ReadData( int nPoolsID )
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM AS_FOLLOWMECARSERVICEMEETINGPOINTS\
					 WHERE (PARENTID = %d)"),nPoolsID);

	CString strMeetingPoints = "";

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("MEETINGPOINTS"),strMeetingPoints);		
	}

	int nIdx =0;	
	CString strName;
	while(!strMeetingPoints.IsEmpty())
	{
		nIdx = strMeetingPoints.Find(',');
		strName = strMeetingPoints.Left(nIdx);
		strMeetingPoints = strMeetingPoints.Mid(nIdx+1);

		ALTObjectID objName(strName);
		m_vMeetingPoints.push_back(objName);
		
	}	
}

void FollowMeCarServiceMeetingPoints::SaveData( int nPoolsID )
{
	CString strMeetingPoints = "";	
	CString strName;
	int nCount = (int)m_vMeetingPoints.size();
	for (int i =0; i < nCount; i++)
	{
		//if (i < nCount-1)
			strName.Format("%s,", m_vMeetingPoints.at(i).GetIDString());
		//else
		//	strName = m_vMeetingPoints.at(i).GetIDString();

		strMeetingPoints += strName;
	}

	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO AS_FOLLOWMECARSERVICEMEETINGPOINTS\
						 (PARENTID,MEETINGPOINTS)\
						 VALUES (%d, '%s')"),nPoolsID, strMeetingPoints);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
		UpdateData(nPoolsID, strMeetingPoints);
}

void FollowMeCarServiceMeetingPoints::UpdateData(int nParentID ,const CString& strMeetingPoints)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AS_FOLLOWMECARSERVICEMEETINGPOINTS\
					 SET PARENTID =%d, MEETINGPOINTS = '%s'\
					 WHERE (ID = %d)"), nParentID,  strMeetingPoints, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FollowMeCarServiceMeetingPoints::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AS_FOLLOWMECARSERVICEMEETINGPOINTS\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

bool FollowMeCarServiceMeetingPoints::AddMeetingPoint( const ALTObjectID& objName )
{
	if (std::find(m_vMeetingPoints.begin(),m_vMeetingPoints.end(),objName) != m_vMeetingPoints.end())
		return false;

	m_vMeetingPoints.push_back(objName);
	return true;
}

void FollowMeCarServiceMeetingPoints::DelMeetingPoint( const ALTObjectID& objName )
{
	std::vector<ALTObjectID>::iterator iter = std::find(m_vMeetingPoints.begin(),m_vMeetingPoints.end(),objName);
	if (iter != m_vMeetingPoints.end())
	{
		m_vMeetingPoints.erase(iter);
	}
}
