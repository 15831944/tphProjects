#include "stdafx.h"
#include "StandBufferItem.h"

StandBufferItem::StandBufferItem()
 : m_nID(-1)
 , m_nArrivalBuf(0)
 , m_nDepartureBuf(0)
{
	m_nStandBuffID = -1;
}

StandBufferItem::~StandBufferItem()
{
}

void StandBufferItem::SaveData()
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	m_standGrop.SaveData(m_standGrop.GetProjID());

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_STANDBUFFERDATA \
		(STANDBUFID, STANDGROUPID, ARRBUF, DEPARTBUF) \
		VALUES (%d,%d,%d,%d)"),m_nStandBuffID, m_standGrop.getID(),m_nArrivalBuf,m_nDepartureBuf);


	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void StandBufferItem::DeleteData()
{
	if (m_nID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_STANDBUFFERDATA \
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void StandBufferItem::UpdateData()
{
	if (m_nID == -1)
		return;

	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE IN_STANDBUFFERDATA \
		SET STANDGROUPID =%d, ARRBUF =%d, DEPARTBUF =%d \
		WHERE (ID = %d)"),m_standGrop.getID(),m_nArrivalBuf,m_nDepartureBuf,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
//ID, STANDBUFID, STANDGROUPID, ARRBUF, DEPARTBUF
void StandBufferItem::ReadData(CADORecordset& adoDatabase)
{
	adoDatabase.GetFieldValue(_T("ID"),m_nID);
	adoDatabase.GetFieldValue(_T("STANDBUFID"),m_nStandBuffID);
	int nStandGroupID = -1;
	adoDatabase.GetFieldValue(_T("STANDGROUPID"),nStandGroupID);
	adoDatabase.GetFieldValue(_T("ARRBUF"),m_nArrivalBuf);
	adoDatabase.GetFieldValue(_T("DEPARTBUF"),m_nDepartureBuf);

	m_standGrop.ReadData(nStandGroupID);

}












