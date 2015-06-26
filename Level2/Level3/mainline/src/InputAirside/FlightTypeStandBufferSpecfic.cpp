#include "stdafx.h"
#include "FlightTypeStandBufferSpecfic.h"
#include "StandBufferItem.h"

#include <algorithm>

FlightTypeStandBufferSpecfic::FlightTypeStandBufferSpecfic(int nProjID)
:m_nProjectID(nProjID)
,m_nID(-1)
{
}

FlightTypeStandBufferSpecfic::~FlightTypeStandBufferSpecfic(void)
{
	RemoveAll();
}
//ID, PROJID, FLTTYPE
void FlightTypeStandBufferSpecfic::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjectID);
	adoRecordset.GetFieldValue(_T("FLTTYPE"),m_strFltType);

}
void FlightTypeStandBufferSpecfic::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, STANDBUFID, STANDGROUPID, ARRBUF, DEPARTBUF \
		FROM IN_STANDBUFFERDATA \
		WHERE (STANDBUFID = %d)"),m_nID);


	long nRecordCount;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while(!adoRecordset.IsEOF())
	{
		StandBufferItem *pItem = new StandBufferItem;
		pItem->ReadData(adoRecordset);

		m_vectStandBuf.push_back(pItem);

		adoRecordset.MoveNextData();
	}

}

void FlightTypeStandBufferSpecfic::SaveData()
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_STANDBUFFER \
		(PROJID, FLTTYPE) \
		VALUES (%d,'%s')"),m_nProjectID,m_strFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	
	
	SaveStandBuffList();

}
void FlightTypeStandBufferSpecfic::DeleteData()
{
	if (m_nID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_STANDBUFFER \
		WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	DeleteStandBuffList();
}

void FlightTypeStandBufferSpecfic::SaveStandBuffList()
{
	StandBufIter iter = m_vectStandBuf.begin();
	StandBufIter iterEnd = m_vectStandBuf.end();
	for (;iter != iterEnd;++iter)
	{
		(*iter)->SaveData();
	}
}
void FlightTypeStandBufferSpecfic::DeleteStandBuffList()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_STANDBUFFERDATA \
		 WHERE (STANDBUFID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void FlightTypeStandBufferSpecfic::UpdateData()
{
	if (m_nID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_STANDBUFFER \
		SET PROJID =%d, FLTTYPE ='%s' \
		 WHERE (ID = %d)"),m_nProjectID,m_strFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
	SaveStandBuffList();
}

int FlightTypeStandBufferSpecfic::GetStandBufItemCount() const
{
	return (int)m_vectStandBuf.size();
}

StandBufferItem* FlightTypeStandBufferSpecfic::GetStandBufItem(int nIndex) const
{
	ASSERT(nIndex >=0 && nIndex < (int)m_vectStandBuf.size());
	return m_vectStandBuf[nIndex];
}

void FlightTypeStandBufferSpecfic::AddStandBufItem(StandBufferItem* pItem)
{
	pItem->setStandBufferID(m_nID);
	m_vectStandBuf.push_back(pItem);	
}

void FlightTypeStandBufferSpecfic::DeleteStandBufItem(StandBufferItem* pItem)
{
	StandBufIter iter = 
		std::find(m_vectStandBuf.begin(), m_vectStandBuf.end(), pItem);
	if(iter == m_vectStandBuf.end())
		return;
	m_vectNeedtoDel.push_back(*iter);
	m_vectStandBuf.erase(iter);
}

void FlightTypeStandBufferSpecfic::RemoveAll()
{
	for (StandBufIter iter = m_vectStandBuf.begin();
		iter != m_vectStandBuf.end(); iter++)
	{
		delete(*iter);
		(*iter) = NULL;
	}
	m_vectStandBuf.clear();
}
