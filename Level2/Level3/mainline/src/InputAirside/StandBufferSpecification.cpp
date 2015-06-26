#include "StdAfx.h"
#include "StandBufferSpecification.h"
#include "FlightTypeStandBufferSpecfic.h"
#include <algorithm>
#include "..\Database\ADODatabase.h"

StandBuffers::StandBuffers(int nProjID)
 : m_nProjID(nProjID)
{
}

StandBuffers::~StandBuffers()
{
	RemoveAll();
}

void StandBuffers::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, PROJID, FLTTYPE \
		FROM IN_STANDBUFFER \
		WHERE (PROJID = %d)"),m_nProjID);

	long nRecordCount;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while(!adoRecordset.IsEOF())
	{
		FlightTypeStandBufferSpecfic *pItem = new FlightTypeStandBufferSpecfic(m_nProjID);
		pItem->ReadData(adoRecordset);
		pItem->ReadData();

		m_vectFltTypeStandBuf.push_back(pItem);

		adoRecordset.MoveNextData();
	}
}

void StandBuffers::SaveData()
{
	for (FltTypeStandBufIter iter = m_vectFltTypeStandBuf.begin();
		iter != m_vectFltTypeStandBuf.end(); iter++)
	{
		(*iter)->SaveData();
	}
}

int StandBuffers::GeItemsCount()
{
	return (int)m_vectFltTypeStandBuf.size();
}

FlightTypeStandBufferSpecfic* StandBuffers::GetItem(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex<(int)m_vectFltTypeStandBuf.size());
	return m_vectFltTypeStandBuf[nIndex];
}

void StandBuffers::AddFltTypeStandBufferSpecific(FlightTypeStandBufferSpecfic* pItem)
{
	m_vectFltTypeStandBuf.push_back(pItem);
}

void StandBuffers::DeleteltTypeStandBufferSpecific(FlightTypeStandBufferSpecfic* pItem)
{
	FltTypeStandBufIter iter = std::find(m_vectFltTypeStandBuf.begin(), m_vectFltTypeStandBuf.end(), pItem);
	if (iter == m_vectFltTypeStandBuf.end())
		return;

	m_vectNeedToDel.push_back(*iter);
	m_vectFltTypeStandBuf.erase(iter);
}

void StandBuffers::RemoveAll()
{
	for (FltTypeStandBufIter iter = m_vectFltTypeStandBuf.begin();
		iter != m_vectFltTypeStandBuf.end(); iter++)
	{
		delete (*iter);
		(*iter) = NULL;
	}

	m_vectFltTypeStandBuf.clear();
}